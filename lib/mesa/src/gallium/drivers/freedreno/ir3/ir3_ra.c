/* -*- mode: C; c-file-style: "k&r"; tab-width 4; indent-tabs-mode: t; -*- */

/*
 * Copyright (C) 2014 Rob Clark <robclark@freedesktop.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Authors:
 *    Rob Clark <robclark@freedesktop.org>
 */

#include "util/u_math.h"
#include "util/register_allocate.h"
#include "util/ralloc.h"
#include "util/bitset.h"

#include "ir3.h"
#include "ir3_compiler.h"

/*
 * Register Assignment:
 *
 * Uses the register_allocate util, which implements graph coloring
 * algo with interference classes.  To handle the cases where we need
 * consecutive registers (for example, texture sample instructions),
 * we model these as larger (double/quad/etc) registers which conflict
 * with the corresponding registers in other classes.
 *
 * Additionally we create additional classes for half-regs, which
 * do not conflict with the full-reg classes.  We do need at least
 * sizes 1-4 (to deal w/ texture sample instructions output to half-
 * reg).  At the moment we don't create the higher order half-reg
 * classes as half-reg frequently does not have enough precision
 * for texture coords at higher resolutions.
 *
 * There are some additional cases that we need to handle specially,
 * as the graph coloring algo doesn't understand "partial writes".
 * For example, a sequence like:
 *
 *   add r0.z, ...
 *   sam (f32)(xy)r0.x, ...
 *   ...
 *   sam (f32)(xyzw)r0.w, r0.x, ...  ; 3d texture, so r0.xyz are coord
 *
 * In this scenario, we treat r0.xyz as class size 3, which is written
 * (from a use/def perspective) at the 'add' instruction and ignore the
 * subsequent partial writes to r0.xy.  So the 'add r0.z, ...' is the
 * defining instruction, as it is the first to partially write r0.xyz.
 *
 * Note i965 has a similar scenario, which they solve with a virtual
 * LOAD_PAYLOAD instruction which gets turned into multiple MOV's after
 * register assignment.  But for us that is horrible from a scheduling
 * standpoint.  Instead what we do is use idea of 'definer' instruction.
 * Ie. the first instruction (lowest ip) to write to the array is the
 * one we consider from use/def perspective when building interference
 * graph.  (Other instructions which write other array elements just
 * define the variable some more.)
 */

static const unsigned class_sizes[] = {
	1, 2, 3, 4,
	4 + 4, /* txd + 1d/2d */
	4 + 6, /* txd + 3d */
	/* temporary: until we can assign arrays, create classes so we
	 * can round up array to fit.  NOTE with tgsi arrays should
	 * really all be multiples of four:
	 */
	4 * 4,
	4 * 8,
	4 * 16,
	4 * 32,

};
#define class_count ARRAY_SIZE(class_sizes)

static const unsigned half_class_sizes[] = {
	1, 2, 3, 4,
};
#define half_class_count  ARRAY_SIZE(half_class_sizes)
#define total_class_count (class_count + half_class_count)

/* Below a0.x are normal regs.  RA doesn't need to assign a0.x/p0.x. */
#define NUM_REGS             (4 * (REG_A0 - 1))
/* Number of virtual regs in a given class: */
#define CLASS_REGS(i)        (NUM_REGS - (class_sizes[i] - 1))
#define HALF_CLASS_REGS(i)   (NUM_REGS - (half_class_sizes[i] - 1))

/* register-set, created one time, used for all shaders: */
struct ir3_ra_reg_set {
	struct ra_regs *regs;
	unsigned int classes[class_count];
	unsigned int half_classes[half_class_count];
	/* maps flat virtual register space to base gpr: */
	uint16_t *ra_reg_to_gpr;
	/* maps cls,gpr to flat virtual register space: */
	uint16_t **gpr_to_ra_reg;
};

/* One-time setup of RA register-set, which describes all the possible
 * "virtual" registers and their interferences.  Ie. double register
 * occupies (and conflicts with) two single registers, and so forth.
 * Since registers do not need to be aligned to their class size, they
 * can conflict with other registers in the same class too.  Ie:
 *
 *    Single (base) |  Double
 *    --------------+---------------
 *       R0         |  D0
 *       R1         |  D0 D1
 *       R2         |     D1 D2
 *       R3         |        D2
 *           .. and so on..
 *
 * (NOTE the disassembler uses notation like r0.x/y/z/w but those are
 * really just four scalar registers.  Don't let that confuse you.)
 */
struct ir3_ra_reg_set *
ir3_ra_alloc_reg_set(void *memctx)
{
	struct ir3_ra_reg_set *set = rzalloc(memctx, struct ir3_ra_reg_set);
	unsigned ra_reg_count, reg, first_half_reg;
	unsigned int **q_values;

	/* calculate # of regs across all classes: */
	ra_reg_count = 0;
	for (unsigned i = 0; i < class_count; i++)
		ra_reg_count += CLASS_REGS(i);
	for (unsigned i = 0; i < half_class_count; i++)
		ra_reg_count += HALF_CLASS_REGS(i);

	/* allocate and populate q_values: */
	q_values = ralloc_array(set, unsigned *, total_class_count);
	for (unsigned i = 0; i < class_count; i++) {
		q_values[i] = rzalloc_array(q_values, unsigned, total_class_count);

		/* From register_allocate.c:
		 *
		 * q(B,C) (indexed by C, B is this register class) in
		 * Runeson/Nyström paper.  This is "how many registers of B could
		 * the worst choice register from C conflict with".
		 *
		 * If we just let the register allocation algorithm compute these
		 * values, is extremely expensive.  However, since all of our
		 * registers are laid out, we can very easily compute them
		 * ourselves.  View the register from C as fixed starting at GRF n
		 * somewhere in the middle, and the register from B as sliding back
		 * and forth.  Then the first register to conflict from B is the
		 * one starting at n - class_size[B] + 1 and the last register to
		 * conflict will start at n + class_size[B] - 1.  Therefore, the
		 * number of conflicts from B is class_size[B] + class_size[C] - 1.
		 *
		 *   +-+-+-+-+-+-+     +-+-+-+-+-+-+
		 * B | | | | | |n| --> | | | | | | |
		 *   +-+-+-+-+-+-+     +-+-+-+-+-+-+
		 *             +-+-+-+-+-+
		 * C           |n| | | | |
		 *             +-+-+-+-+-+
		 *
		 * (Idea copied from brw_fs_reg_allocate.cpp)
		 */
		for (unsigned j = 0; j < class_count; j++)
			q_values[i][j] = class_sizes[i] + class_sizes[j] - 1;
	}

	for (unsigned i = class_count; i < total_class_count; i++) {
		q_values[i] = ralloc_array(q_values, unsigned, total_class_count);

		/* see comment above: */
		for (unsigned j = class_count; j < total_class_count; j++) {
			q_values[i][j] = half_class_sizes[i - class_count] +
					half_class_sizes[j - class_count] - 1;
		}
	}

	/* allocate the reg-set.. */
	set->regs = ra_alloc_reg_set(set, ra_reg_count, true);
	set->ra_reg_to_gpr = ralloc_array(set, uint16_t, ra_reg_count);
	set->gpr_to_ra_reg = ralloc_array(set, uint16_t *, total_class_count);

	/* .. and classes */
	reg = 0;
	for (unsigned i = 0; i < class_count; i++) {
		set->classes[i] = ra_alloc_reg_class(set->regs);

		set->gpr_to_ra_reg[i] = ralloc_array(set, uint16_t, CLASS_REGS(i));

		for (unsigned j = 0; j < CLASS_REGS(i); j++) {
			ra_class_add_reg(set->regs, set->classes[i], reg);

			set->ra_reg_to_gpr[reg] = j;
			set->gpr_to_ra_reg[i][j] = reg;

			for (unsigned br = j; br < j + class_sizes[i]; br++)
				ra_add_transitive_reg_conflict(set->regs, br, reg);

			reg++;
		}
	}

	first_half_reg = reg;

	for (unsigned i = 0; i < half_class_count; i++) {
		set->half_classes[i] = ra_alloc_reg_class(set->regs);

		set->gpr_to_ra_reg[class_count + i] =
				ralloc_array(set, uint16_t, CLASS_REGS(i));

		for (unsigned j = 0; j < HALF_CLASS_REGS(i); j++) {
			ra_class_add_reg(set->regs, set->half_classes[i], reg);

			set->ra_reg_to_gpr[reg] = j;
			set->gpr_to_ra_reg[class_count + i][j] = reg;

			for (unsigned br = j; br < j + half_class_sizes[i]; br++)
				ra_add_transitive_reg_conflict(set->regs, br + first_half_reg, reg);

			reg++;
		}
	}

	ra_set_finalize(set->regs, q_values);

	ralloc_free(q_values);

	return set;
}

/* additional block-data (per-block) */
struct ir3_ra_block_data {
	BITSET_WORD *def;        /* variables defined before used in block */
	BITSET_WORD *use;        /* variables used before defined in block */
	BITSET_WORD *livein;     /* which defs reach entry point of block */
	BITSET_WORD *liveout;    /* which defs reach exit point of block */
};

/* additional instruction-data (per-instruction) */
struct ir3_ra_instr_data {
	/* cached instruction 'definer' info: */
	struct ir3_instruction *defn;
	int off, sz, cls;
};

/* register-assign context, per-shader */
struct ir3_ra_ctx {
	struct ir3 *ir;
	enum shader_t type;
	bool frag_face;

	struct ir3_ra_reg_set *set;
	struct ra_graph *g;
	unsigned alloc_count;
	unsigned class_alloc_count[total_class_count];
	unsigned class_base[total_class_count];
	unsigned instr_cnt;
	unsigned *def, *use;     /* def/use table */
	struct ir3_ra_instr_data *instrd;
};

static bool
is_half(struct ir3_instruction *instr)
{
	return !!(instr->regs[0]->flags & IR3_REG_HALF);
}

static int
size_to_class(unsigned sz, bool half)
{
	if (half) {
		for (unsigned i = 0; i < half_class_count; i++)
			if (half_class_sizes[i] >= sz)
				return i + class_count;
	} else {
		for (unsigned i = 0; i < class_count; i++)
			if (class_sizes[i] >= sz)
				return i;
	}
	debug_assert(0);
	return -1;
}

static bool
is_temp(struct ir3_register *reg)
{
	if (reg->flags & (IR3_REG_CONST | IR3_REG_IMMED))
		return false;
	if ((reg->num == regid(REG_A0, 0)) ||
			(reg->num == regid(REG_P0, 0)))
		return false;
	return true;
}

static bool
writes_gpr(struct ir3_instruction *instr)
{
	if (is_store(instr))
		return false;
	/* is dest a normal temp register: */
	return is_temp(instr->regs[0]);
}

static struct ir3_instruction *
get_definer(struct ir3_ra_ctx *ctx, struct ir3_instruction *instr,
		int *sz, int *off)
{
	struct ir3_ra_instr_data *id = &ctx->instrd[instr->ip];
	struct ir3_instruction *d = NULL;

	if (instr->fanin)
		return get_definer(ctx, instr->fanin, sz, off);

	if (id->defn) {
		*sz = id->sz;
		*off = id->off;
		return id->defn;
	}

	if (is_meta(instr) && (instr->opc == OPC_META_FI)) {
		/* What about the case where collect is subset of array, we
		 * need to find the distance between where actual array starts
		 * and fanin..  that probably doesn't happen currently.
		 */
		struct ir3_register *src;
		int dsz, doff;

		/* note: don't use foreach_ssa_src as this gets called once
		 * while assigning regs (which clears SSA flag)
		 */
		foreach_src_n(src, n, instr) {
			struct ir3_instruction *dd;
			if (!src->instr)
				continue;

			dd = get_definer(ctx, src->instr, &dsz, &doff);

			if ((!d) || (dd->ip < d->ip)) {
				d = dd;
				*sz = dsz;
				*off = doff - n;
			}
		}

	} else if (instr->cp.right || instr->cp.left) {
		/* covers also the meta:fo case, which ends up w/ single
		 * scalar instructions for each component:
		 */
		struct ir3_instruction *f = ir3_neighbor_first(instr);

		/* by definition, the entire sequence forms one linked list
		 * of single scalar register nodes (even if some of them may
		 * be fanouts from a texture sample (for example) instr.  We
		 * just need to walk the list finding the first element of
		 * the group defined (lowest ip)
		 */
		int cnt = 0;

		d = f;
		while (f) {
			if (f->ip < d->ip)
				d = f;
			if (f == instr)
				*off = cnt;
			f = f->cp.right;
			cnt++;
		}

		*sz = cnt;

	} else {
		/* second case is looking directly at the instruction which
		 * produces multiple values (eg, texture sample), rather
		 * than the fanout nodes that point back to that instruction.
		 * This isn't quite right, because it may be part of a larger
		 * group, such as:
		 *
		 *     sam (f32)(xyzw)r0.x, ...
		 *     add r1.x, ...
		 *     add r1.y, ...
		 *     sam (f32)(xyzw)r2.x, r0.w  <-- (r0.w, r1.x, r1.y)
		 *
		 * need to come up with a better way to handle that case.
		 */
		if (instr->address) {
			*sz = instr->regs[0]->size;
		} else {
			*sz = util_last_bit(instr->regs[0]->wrmask);
		}
		*off = 0;
		d = instr;
	}

	if (d->regs[0]->flags & IR3_REG_PHI_SRC) {
		struct ir3_instruction *phi = d->regs[0]->instr;
		struct ir3_instruction *dd;
		int dsz, doff;

		dd = get_definer(ctx, phi, &dsz, &doff);

		*sz = MAX2(*sz, dsz);
		*off = doff;

		if (dd->ip < d->ip) {
			d = dd;
		}
	}

	if (is_meta(d) && (d->opc == OPC_META_PHI)) {
		/* we have already inserted parallel-copies into
		 * the phi, so we don't need to chase definers
		 */
		struct ir3_register *src;
		struct ir3_instruction *dd = d;

		/* note: don't use foreach_ssa_src as this gets called once
		 * while assigning regs (which clears SSA flag)
		 */
		foreach_src(src, d) {
			if (!src->instr)
				continue;
			if (src->instr->ip < dd->ip)
				dd = src->instr;
		}

		d = dd;
	}

	if (is_meta(d) && (d->opc == OPC_META_FO)) {
		struct ir3_instruction *dd;
		int dsz, doff;

		dd = get_definer(ctx, d->regs[1]->instr, &dsz, &doff);

		/* by definition, should come before: */
		debug_assert(dd->ip < d->ip);

		*sz = MAX2(*sz, dsz);

		/* Fanout's are grouped, so *off should already valid */

		d = dd;
	}

	id->defn = d;
	id->sz = *sz;
	id->off = *off;

	return d;
}

static void
ra_block_find_definers(struct ir3_ra_ctx *ctx, struct ir3_block *block)
{
	list_for_each_entry (struct ir3_instruction, instr, &block->instr_list, node) {
		struct ir3_ra_instr_data *id = &ctx->instrd[instr->ip];
		if (instr->regs_count == 0)
			continue;
		/* couple special cases: */
		if (writes_addr(instr) || writes_pred(instr)) {
			id->cls = -1;
			continue;
		}
		id->defn = get_definer(ctx, instr, &id->sz, &id->off);
		id->cls = size_to_class(id->sz, is_half(id->defn));
	}
}

/* give each instruction a name (and ip), and count up the # of names
 * of each class
 */
static void
ra_block_name_instructions(struct ir3_ra_ctx *ctx, struct ir3_block *block)
{
	list_for_each_entry (struct ir3_instruction, instr, &block->instr_list, node) {
		struct ir3_ra_instr_data *id = &ctx->instrd[instr->ip];

#ifdef DEBUG
		instr->name = ~0;
#endif

		ctx->instr_cnt++;

		if (instr->regs_count == 0)
			continue;

		if (!writes_gpr(instr))
			continue;

		if (id->defn != instr)
			continue;

		/* arrays which don't fit in one of the pre-defined class
		 * sizes are pre-colored:
		 *
		 * TODO but we still need to allocate names for them, don't we??
		 */
		if (id->cls >= 0) {
			instr->name = ctx->class_alloc_count[id->cls]++;
			ctx->alloc_count++;
		}
	}
}

static void
ra_init(struct ir3_ra_ctx *ctx)
{
	unsigned n;

	ir3_clear_mark(ctx->ir);
	n = ir3_count_instructions(ctx->ir);

	ctx->instrd = rzalloc_array(NULL, struct ir3_ra_instr_data, n);

	list_for_each_entry (struct ir3_block, block, &ctx->ir->block_list, node) {
		ra_block_find_definers(ctx, block);
	}

	list_for_each_entry (struct ir3_block, block, &ctx->ir->block_list, node) {
		ra_block_name_instructions(ctx, block);
	}

	/* figure out the base register name for each class.  The
	 * actual ra name is class_base[cls] + instr->name;
	 */
	ctx->class_base[0] = 0;
	for (unsigned i = 1; i < total_class_count; i++) {
		ctx->class_base[i] = ctx->class_base[i-1] +
				ctx->class_alloc_count[i-1];
	}

	ctx->g = ra_alloc_interference_graph(ctx->set->regs, ctx->alloc_count);
	ralloc_steal(ctx->g, ctx->instrd);
	ctx->def = rzalloc_array(ctx->g, unsigned, ctx->alloc_count);
	ctx->use = rzalloc_array(ctx->g, unsigned, ctx->alloc_count);
}

static unsigned
ra_name(struct ir3_ra_ctx *ctx, int cls, struct ir3_instruction *defn)
{
	unsigned name;
	debug_assert(cls >= 0);
	name = ctx->class_base[cls] + defn->name;
	debug_assert(name < ctx->alloc_count);
	return name;
}

static void
ra_destroy(struct ir3_ra_ctx *ctx)
{
	ralloc_free(ctx->g);
}

static void
ra_block_compute_live_ranges(struct ir3_ra_ctx *ctx, struct ir3_block *block)
{
	struct ir3_ra_block_data *bd;
	unsigned bitset_words = BITSET_WORDS(ctx->alloc_count);

	bd = rzalloc(ctx->g, struct ir3_ra_block_data);

	bd->def     = rzalloc_array(bd, BITSET_WORD, bitset_words);
	bd->use     = rzalloc_array(bd, BITSET_WORD, bitset_words);
	bd->livein  = rzalloc_array(bd, BITSET_WORD, bitset_words);
	bd->liveout = rzalloc_array(bd, BITSET_WORD, bitset_words);

	block->bd = bd;

	list_for_each_entry (struct ir3_instruction, instr, &block->instr_list, node) {
		struct ir3_instruction *src;

		if (instr->regs_count == 0)
			continue;

		/* There are a couple special cases to deal with here:
		 *
		 * fanout: used to split values from a higher class to a lower
		 *     class, for example split the results of a texture fetch
		 *     into individual scalar values;  We skip over these from
		 *     a 'def' perspective, and for a 'use' we walk the chain
		 *     up to the defining instruction.
		 *
		 * fanin: used to collect values from lower class and assemble
		 *     them together into a higher class, for example arguments
		 *     to texture sample instructions;  We consider these to be
		 *     defined at the earliest fanin source.
		 *
		 * phi: used to merge values from different flow control paths
		 *     to the same reg.  Consider defined at earliest phi src,
		 *     and update all the other phi src's (which may come later
		 *     in the program) as users to extend the var's live range.
		 *
		 * Most of this, other than phi, is completely handled in the
		 * get_definer() helper.
		 *
		 * In either case, we trace the instruction back to the original
		 * definer and consider that as the def/use ip.
		 */

		if (writes_gpr(instr)) {
			struct ir3_ra_instr_data *id = &ctx->instrd[instr->ip];

			if (id->defn == instr) {
				/* arrays which don't fit in one of the pre-defined class
				 * sizes are pre-colored:
				 */
				if (id->cls >= 0) {
					unsigned name = ra_name(ctx, id->cls, id->defn);

					ctx->def[name] = id->defn->ip;
					ctx->use[name] = id->defn->ip;

					/* since we are in SSA at this point: */
					debug_assert(!BITSET_TEST(bd->use, name));

					BITSET_SET(bd->def, name);

					if (is_half(id->defn)) {
						ra_set_node_class(ctx->g, name,
								ctx->set->half_classes[id->cls - class_count]);
					} else {
						ra_set_node_class(ctx->g, name,
								ctx->set->classes[id->cls]);
					}

					/* extend the live range for phi srcs, which may come
					 * from the bottom of the loop
					 */
					if (id->defn->regs[0]->flags & IR3_REG_PHI_SRC) {
						struct ir3_instruction *phi = id->defn->regs[0]->instr;
						foreach_ssa_src(src, phi) {
							/* if src is after phi, then we need to extend
							 * the liverange to the end of src's block:
							 */
							if (src->ip > phi->ip) {
								struct ir3_instruction *last =
									list_last_entry(&src->block->instr_list,
										struct ir3_instruction, node);
								ctx->use[name] = MAX2(ctx->use[name], last->ip);
							}
						}
					}
				}
			}
		}

		foreach_ssa_src(src, instr) {
			if (writes_gpr(src)) {
				struct ir3_ra_instr_data *id = &ctx->instrd[src->ip];

				if (id->cls >= 0) {
					unsigned name = ra_name(ctx, id->cls, id->defn);
					ctx->use[name] = MAX2(ctx->use[name], instr->ip);
					if (!BITSET_TEST(bd->def, name))
						BITSET_SET(bd->use, name);
				}
			}
		}
	}
}

static bool
ra_compute_livein_liveout(struct ir3_ra_ctx *ctx)
{
	unsigned bitset_words = BITSET_WORDS(ctx->alloc_count);
	bool progress = false;

	list_for_each_entry (struct ir3_block, block, &ctx->ir->block_list, node) {
		struct ir3_ra_block_data *bd = block->bd;

		/* update livein: */
		for (unsigned i = 0; i < bitset_words; i++) {
			BITSET_WORD new_livein =
				(bd->use[i] | (bd->liveout[i] & ~bd->def[i]));

			if (new_livein & ~bd->livein[i]) {
				bd->livein[i] |= new_livein;
				progress = true;
			}
		}

		/* update liveout: */
		for (unsigned j = 0; j < ARRAY_SIZE(block->successors); j++) {
			struct ir3_block *succ = block->successors[j];
			struct ir3_ra_block_data *succ_bd;

			if (!succ)
				continue;

			succ_bd = succ->bd;

			for (unsigned i = 0; i < bitset_words; i++) {
				BITSET_WORD new_liveout =
					(succ_bd->livein[i] & ~bd->liveout[i]);

				if (new_liveout) {
					bd->liveout[i] |= new_liveout;
					progress = true;
				}
			}
		}
	}

	return progress;
}

static void
ra_add_interference(struct ir3_ra_ctx *ctx)
{
	struct ir3 *ir = ctx->ir;

	/* compute live ranges (use/def) on a block level, also updating
	 * block's def/use bitmasks (used below to calculate per-block
	 * livein/liveout):
	 */
	list_for_each_entry (struct ir3_block, block, &ir->block_list, node) {
		ra_block_compute_live_ranges(ctx, block);
	}

	/* update per-block livein/liveout: */
	while (ra_compute_livein_liveout(ctx)) {}

	/* extend start/end ranges based on livein/liveout info from cfg: */
	unsigned bitset_words = BITSET_WORDS(ctx->alloc_count);
	list_for_each_entry (struct ir3_block, block, &ir->block_list, node) {
		struct ir3_ra_block_data *bd = block->bd;

		for (unsigned i = 0; i < bitset_words; i++) {
			if (BITSET_TEST(bd->livein, i)) {
				ctx->def[i] = MIN2(ctx->def[i], block->start_ip);
				ctx->use[i] = MAX2(ctx->use[i], block->start_ip);
			}

			if (BITSET_TEST(bd->liveout, i)) {
				ctx->def[i] = MIN2(ctx->def[i], block->end_ip);
				ctx->use[i] = MAX2(ctx->use[i], block->end_ip);
			}
		}
	}

	/* need to fix things up to keep outputs live: */
	for (unsigned i = 0; i < ir->noutputs; i++) {
		struct ir3_instruction *instr = ir->outputs[i];
		struct ir3_ra_instr_data *id = &ctx->instrd[instr->ip];

		if (id->cls >= 0) {
			unsigned name = ra_name(ctx, id->cls, id->defn);
			ctx->use[name] = ctx->instr_cnt;
		}
	}

	for (unsigned i = 0; i < ctx->alloc_count; i++) {
		for (unsigned j = 0; j < ctx->alloc_count; j++) {
			if (!((ctx->def[i] >= ctx->use[j]) ||
					(ctx->def[j] >= ctx->use[i]))) {
				ra_add_node_interference(ctx->g, i, j);
			}
		}
	}
}

/* some instructions need fix-up if dst register is half precision: */
static void fixup_half_instr_dst(struct ir3_instruction *instr)
{
	switch (instr->category) {
	case 1: /* move instructions */
		instr->cat1.dst_type = half_type(instr->cat1.dst_type);
		break;
	case 3:
		switch (instr->opc) {
		case OPC_MAD_F32:
			instr->opc = OPC_MAD_F16;
			break;
		case OPC_SEL_B32:
			instr->opc = OPC_SEL_B16;
			break;
		case OPC_SEL_S32:
			instr->opc = OPC_SEL_S16;
			break;
		case OPC_SEL_F32:
			instr->opc = OPC_SEL_F16;
			break;
		case OPC_SAD_S32:
			instr->opc = OPC_SAD_S16;
			break;
		/* instructions may already be fixed up: */
		case OPC_MAD_F16:
		case OPC_SEL_B16:
		case OPC_SEL_S16:
		case OPC_SEL_F16:
		case OPC_SAD_S16:
			break;
		default:
			assert(0);
			break;
		}
		break;
	case 5:
		instr->cat5.type = half_type(instr->cat5.type);
		break;
	}
}
/* some instructions need fix-up if src register is half precision: */
static void fixup_half_instr_src(struct ir3_instruction *instr)
{
	switch (instr->category) {
	case 1: /* move instructions */
		instr->cat1.src_type = half_type(instr->cat1.src_type);
		break;
	}
}

static void
reg_assign(struct ir3_ra_ctx *ctx, struct ir3_register *reg,
		struct ir3_instruction *instr)
{
	struct ir3_ra_instr_data *id = &ctx->instrd[instr->ip];

	if (id->cls >= 0) {
		unsigned name = ra_name(ctx, id->cls, id->defn);
		unsigned r = ra_get_node_reg(ctx->g, name);
		unsigned num = ctx->set->ra_reg_to_gpr[r] + id->off;

		if (reg->flags & IR3_REG_RELATIV)
			num += reg->offset;

		reg->num = num;
		reg->flags &= ~(IR3_REG_SSA | IR3_REG_PHI_SRC);

		if (is_half(id->defn))
			reg->flags |= IR3_REG_HALF;
	}
}

static void
ra_block_alloc(struct ir3_ra_ctx *ctx, struct ir3_block *block)
{
	list_for_each_entry (struct ir3_instruction, instr, &block->instr_list, node) {
		struct ir3_register *reg;

		if (instr->regs_count == 0)
			continue;

		if (writes_gpr(instr)) {
			reg_assign(ctx, instr->regs[0], instr);
			if (instr->regs[0]->flags & IR3_REG_HALF)
				fixup_half_instr_dst(instr);
		}

		foreach_src_n(reg, n, instr) {
			struct ir3_instruction *src = reg->instr;
			if (!src)
				continue;

			reg_assign(ctx, instr->regs[n+1], src);
			if (instr->regs[n+1]->flags & IR3_REG_HALF)
				fixup_half_instr_src(instr);
		}
	}
}

static int
ra_alloc(struct ir3_ra_ctx *ctx)
{
	/* frag shader inputs get pre-assigned, since we have some
	 * constraints/unknowns about setup for some of these regs:
	 */
	if (ctx->type == SHADER_FRAGMENT) {
		struct ir3 *ir = ctx->ir;
		unsigned i = 0, j;
		if (ctx->frag_face && (i < ir->ninputs) && ir->inputs[i]) {
			struct ir3_instruction *instr = ir->inputs[i];
			int cls = size_to_class(1, true);
			unsigned name = ra_name(ctx, cls, instr);
			unsigned reg = ctx->set->gpr_to_ra_reg[cls][0];

			/* if we have frag_face, it gets hr0.x */
			ra_set_node_reg(ctx->g, name, reg);
			i += 4;
		}

		for (j = 0; i < ir->ninputs; i++) {
			struct ir3_instruction *instr = ir->inputs[i];
			if (instr) {
				struct ir3_ra_instr_data *id = &ctx->instrd[instr->ip];

				if (id->defn == instr) {
					unsigned name, reg;

					name = ra_name(ctx, id->cls, id->defn);
					reg = ctx->set->gpr_to_ra_reg[id->cls][j];

					ra_set_node_reg(ctx->g, name, reg);
					j += id->sz;
				}
			}
		}
	}

	if (!ra_allocate(ctx->g))
		return -1;

	list_for_each_entry (struct ir3_block, block, &ctx->ir->block_list, node) {
		ra_block_alloc(ctx, block);
	}

	return 0;
}

int ir3_ra(struct ir3 *ir, enum shader_t type,
		bool frag_coord, bool frag_face)
{
	struct ir3_ra_ctx ctx = {
			.ir = ir,
			.type = type,
			.frag_face = frag_face,
			.set = ir->compiler->set,
	};
	int ret;

	ra_init(&ctx);
	ra_add_interference(&ctx);
	ret = ra_alloc(&ctx);
	ra_destroy(&ctx);

	return ret;
}
