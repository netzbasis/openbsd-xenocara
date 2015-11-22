#ifndef GEN_MI_XML
#define GEN_MI_XML

/* Autogenerated file, DO NOT EDIT manually!

This file was generated by the rules-ng-ng headergen tool in this git repository:
https://github.com/olvaffe/envytools/
git clone https://github.com/olvaffe/envytools.git

Copyright (C) 2014-2015 by the following authors:
- Chia-I Wu <olvaffe@gmail.com> (olv)

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice (including the
next paragraph) shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE COPYRIGHT OWNER(S) AND/OR ITS SUPPLIERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


enum gen_mi_alu_opcode {
    GEN75_MI_ALU_NOOP					      = 0x0,
    GEN75_MI_ALU_LOAD					      = 0x80,
    GEN75_MI_ALU_LOADINV				      = 0x480,
    GEN75_MI_ALU_LOAD0					      = 0x81,
    GEN75_MI_ALU_LOAD1					      = 0x481,
    GEN75_MI_ALU_ADD					      = 0x100,
    GEN75_MI_ALU_SUB					      = 0x101,
    GEN75_MI_ALU_AND					      = 0x102,
    GEN75_MI_ALU_OR					      = 0x103,
    GEN75_MI_ALU_XOR					      = 0x104,
    GEN75_MI_ALU_STORE					      = 0x180,
    GEN75_MI_ALU_STOREINV				      = 0x580,
};

enum gen_mi_alu_operand {
    GEN75_MI_ALU_R0					      = 0x0,
    GEN75_MI_ALU_R1					      = 0x1,
    GEN75_MI_ALU_R2					      = 0x2,
    GEN75_MI_ALU_R3					      = 0x3,
    GEN75_MI_ALU_R4					      = 0x4,
    GEN75_MI_ALU_R5					      = 0x5,
    GEN75_MI_ALU_R6					      = 0x6,
    GEN75_MI_ALU_R7					      = 0x7,
    GEN75_MI_ALU_R8					      = 0x8,
    GEN75_MI_ALU_R9					      = 0x9,
    GEN75_MI_ALU_R10					      = 0xa,
    GEN75_MI_ALU_R11					      = 0xb,
    GEN75_MI_ALU_R12					      = 0xc,
    GEN75_MI_ALU_R13					      = 0xd,
    GEN75_MI_ALU_R14					      = 0xe,
    GEN75_MI_ALU_R15					      = 0xf,
    GEN75_MI_ALU_SRCA					      = 0x20,
    GEN75_MI_ALU_SRCB					      = 0x21,
    GEN75_MI_ALU_ACCU					      = 0x31,
    GEN75_MI_ALU_ZF					      = 0x32,
    GEN75_MI_ALU_CF					      = 0x33,
};

#define GEN6_MI_TYPE__MASK					0xe0000000
#define GEN6_MI_TYPE__SHIFT					29
#define GEN6_MI_TYPE_MI						(0x0 << 29)
#define GEN6_MI_OPCODE__MASK					0x1f800000
#define GEN6_MI_OPCODE__SHIFT					23
#define GEN6_MI_OPCODE_MI_NOOP					(0x0 << 23)
#define GEN75_MI_OPCODE_MI_SET_PREDICATE			(0x1 << 23)
#define GEN75_MI_OPCODE_MI_RS_CONTROL				(0x6 << 23)
#define GEN75_MI_OPCODE_MI_URB_ATOMIC_ALLOC			(0x9 << 23)
#define GEN6_MI_OPCODE_MI_BATCH_BUFFER_END			(0xa << 23)
#define GEN7_MI_OPCODE_MI_PREDICATE				(0xc << 23)
#define GEN7_MI_OPCODE_MI_URB_CLEAR				(0x19 << 23)
#define GEN75_MI_OPCODE_MI_MATH					(0x1a << 23)
#define GEN6_MI_OPCODE_MI_STORE_DATA_IMM			(0x20 << 23)
#define GEN6_MI_OPCODE_MI_LOAD_REGISTER_IMM			(0x22 << 23)
#define GEN6_MI_OPCODE_MI_STORE_REGISTER_MEM			(0x24 << 23)
#define GEN6_MI_OPCODE_MI_FLUSH_DW				(0x26 << 23)
#define GEN6_MI_OPCODE_MI_REPORT_PERF_COUNT			(0x28 << 23)
#define GEN7_MI_OPCODE_MI_LOAD_REGISTER_MEM			(0x29 << 23)
#define GEN75_MI_OPCODE_MI_LOAD_REGISTER_REG			(0x2a << 23)
#define GEN75_MI_OPCODE_MI_LOAD_URB_MEM				(0x2c << 23)
#define GEN75_MI_OPCODE_MI_STORE_URB_MEM			(0x2d << 23)
#define GEN6_MI_OPCODE_MI_BATCH_BUFFER_START			(0x31 << 23)
#define GEN6_MI_LENGTH__MASK					0x0000003f
#define GEN6_MI_LENGTH__SHIFT					0
#define GEN6_MI_NOOP__SIZE					1
#define GEN6_MI_NOOP_DW0_WRITE_NOPID				(0x1 << 22)
#define GEN6_MI_NOOP_DW0_VALUE__MASK				0x003fffff
#define GEN6_MI_NOOP_DW0_VALUE__SHIFT				0

#define GEN75_MI_SET_PREDICATE__SIZE				1
#define GEN75_MI_SET_PREDICATE_DW0_PREDICATE__MASK		0x00000003
#define GEN75_MI_SET_PREDICATE_DW0_PREDICATE__SHIFT		0
#define GEN75_MI_SET_PREDICATE_DW0_PREDICATE_ALWAYS		0x0
#define GEN75_MI_SET_PREDICATE_DW0_PREDICATE_ON_CLEAR		0x1
#define GEN75_MI_SET_PREDICATE_DW0_PREDICATE_ON_SET		0x2
#define GEN75_MI_SET_PREDICATE_DW0_PREDICATE_DISABLE		0x3

#define GEN75_MI_RS_CONTROL__SIZE				1
#define GEN75_MI_RS_CONTROL_DW0_ENABLE				(0x1 << 0)

#define GEN75_MI_URB_ATOMIC_ALLOC__SIZE				1
#define GEN75_MI_URB_ATOMIC_ALLOC_DW0_OFFSET__MASK		0x000ff000
#define GEN75_MI_URB_ATOMIC_ALLOC_DW0_OFFSET__SHIFT		12
#define GEN75_MI_URB_ATOMIC_ALLOC_DW0_SIZE__MASK		0x000001ff
#define GEN75_MI_URB_ATOMIC_ALLOC_DW0_SIZE__SHIFT		0

#define GEN6_MI_BATCH_BUFFER_END__SIZE				1

#define GEN7_MI_PREDICATE__SIZE					1
#define GEN7_MI_PREDICATE_DW0_LOADOP__MASK			0x000000c0
#define GEN7_MI_PREDICATE_DW0_LOADOP__SHIFT			6
#define GEN7_MI_PREDICATE_DW0_LOADOP_KEEP			(0x0 << 6)
#define GEN7_MI_PREDICATE_DW0_LOADOP_LOAD			(0x2 << 6)
#define GEN7_MI_PREDICATE_DW0_LOADOP_LOADINV			(0x3 << 6)
#define GEN7_MI_PREDICATE_DW0_COMBINEOP__MASK			0x00000018
#define GEN7_MI_PREDICATE_DW0_COMBINEOP__SHIFT			3
#define GEN7_MI_PREDICATE_DW0_COMBINEOP_SET			(0x0 << 3)
#define GEN7_MI_PREDICATE_DW0_COMBINEOP_AND			(0x1 << 3)
#define GEN7_MI_PREDICATE_DW0_COMBINEOP_OR			(0x2 << 3)
#define GEN7_MI_PREDICATE_DW0_COMBINEOP_XOR			(0x3 << 3)
#define GEN7_MI_PREDICATE_DW0_COMPAREOP__MASK			0x00000003
#define GEN7_MI_PREDICATE_DW0_COMPAREOP__SHIFT			0
#define GEN7_MI_PREDICATE_DW0_COMPAREOP_TRUE			0x0
#define GEN7_MI_PREDICATE_DW0_COMPAREOP_FALSE			0x1
#define GEN7_MI_PREDICATE_DW0_COMPAREOP_SRCS_EQUAL		0x2
#define GEN7_MI_PREDICATE_DW0_COMPAREOP_DELTAS_EQUAL		0x3

#define GEN7_MI_URB_CLEAR__SIZE					2

#define GEN7_MI_URB_CLEAR_DW1_LENGTH__MASK			0x3fff0000
#define GEN7_MI_URB_CLEAR_DW1_LENGTH__SHIFT			16
#define GEN7_MI_URB_CLEAR_DW1_OFFSET__MASK			0x00007fff
#define GEN7_MI_URB_CLEAR_DW1_OFFSET__SHIFT			0

#define GEN75_MI_MATH__SIZE					65

#define GEN75_MI_MATH_DW_OP__MASK				0xfff00000
#define GEN75_MI_MATH_DW_OP__SHIFT				20
#define GEN75_MI_MATH_DW_SRC1__MASK				0x000ffc00
#define GEN75_MI_MATH_DW_SRC1__SHIFT				10
#define GEN75_MI_MATH_DW_SRC2__MASK				0x000007ff
#define GEN75_MI_MATH_DW_SRC2__SHIFT				0

#define GEN6_MI_STORE_DATA_IMM__SIZE				6
#define GEN6_MI_STORE_DATA_IMM_DW0_USE_GGTT			(0x1 << 22)


#define GEN6_MI_STORE_DATA_IMM_DW2_ADDR__MASK			0xfffffffc
#define GEN6_MI_STORE_DATA_IMM_DW2_ADDR__SHIFT			2
#define GEN6_MI_STORE_DATA_IMM_DW2_ADDR__SHR			2




#define GEN6_MI_LOAD_REGISTER_IMM__SIZE				3
#define GEN6_MI_LOAD_REGISTER_IMM_DW0_WRITE_DISABLES__MASK	0x00000f00
#define GEN6_MI_LOAD_REGISTER_IMM_DW0_WRITE_DISABLES__SHIFT	8

#define GEN6_MI_LOAD_REGISTER_IMM_DW1_REG__MASK			0x007ffffc
#define GEN6_MI_LOAD_REGISTER_IMM_DW1_REG__SHIFT		2
#define GEN6_MI_LOAD_REGISTER_IMM_DW1_REG__SHR			2


#define GEN6_MI_STORE_REGISTER_MEM__SIZE			4
#define GEN6_MI_STORE_REGISTER_MEM_DW0_USE_GGTT			(0x1 << 22)
#define GEN75_MI_STORE_REGISTER_MEM_DW0_PREDICATE_ENABLE	(0x1 << 21)

#define GEN6_MI_STORE_REGISTER_MEM_DW1_REG__MASK		0x007ffffc
#define GEN6_MI_STORE_REGISTER_MEM_DW1_REG__SHIFT		2
#define GEN6_MI_STORE_REGISTER_MEM_DW1_REG__SHR			2

#define GEN6_MI_STORE_REGISTER_MEM_DW2_ADDR__MASK		0xfffffffc
#define GEN6_MI_STORE_REGISTER_MEM_DW2_ADDR__SHIFT		2
#define GEN6_MI_STORE_REGISTER_MEM_DW2_ADDR__SHR		2


#define GEN6_MI_FLUSH_DW__SIZE					4




#define GEN6_MI_REPORT_PERF_COUNT__SIZE				3

#define GEN6_MI_REPORT_PERF_COUNT_DW1_CORE_MODE_ENABLE		(0x1 << 4)
#define GEN6_MI_REPORT_PERF_COUNT_DW1_USE_GGTT			(0x1 << 0)
#define GEN6_MI_REPORT_PERF_COUNT_DW1_ADDR__MASK		0xffffffc0
#define GEN6_MI_REPORT_PERF_COUNT_DW1_ADDR__SHIFT		6
#define GEN6_MI_REPORT_PERF_COUNT_DW1_ADDR__SHR			6


#define GEN7_MI_LOAD_REGISTER_MEM__SIZE				4
#define GEN7_MI_LOAD_REGISTER_MEM_DW0_USE_GGTT			(0x1 << 22)
#define GEN7_MI_LOAD_REGISTER_MEM_DW0_ASYNC_MODE_ENABLE		(0x1 << 21)

#define GEN7_MI_LOAD_REGISTER_MEM_DW1_REG__MASK			0x007ffffc
#define GEN7_MI_LOAD_REGISTER_MEM_DW1_REG__SHIFT		2
#define GEN7_MI_LOAD_REGISTER_MEM_DW1_REG__SHR			2

#define GEN7_MI_LOAD_REGISTER_MEM_DW2_ADDR__MASK		0xfffffffc
#define GEN7_MI_LOAD_REGISTER_MEM_DW2_ADDR__SHIFT		2
#define GEN7_MI_LOAD_REGISTER_MEM_DW2_ADDR__SHR			2


#define GEN75_MI_LOAD_REGISTER_REG__SIZE			3

#define GEN75_MI_LOAD_REGISTER_REG_DW1_SRC_REG__MASK		0x007ffffc
#define GEN75_MI_LOAD_REGISTER_REG_DW1_SRC_REG__SHIFT		2
#define GEN75_MI_LOAD_REGISTER_REG_DW1_SRC_REG__SHR		2

#define GEN75_MI_LOAD_REGISTER_REG_DW2_DST_REG__MASK		0x007ffffc
#define GEN75_MI_LOAD_REGISTER_REG_DW2_DST_REG__SHIFT		2
#define GEN75_MI_LOAD_REGISTER_REG_DW2_DST_REG__SHR		2

#define GEN75_MI_LOAD_URB_MEM__SIZE				4

#define GEN75_MI_LOAD_URB_MEM_DW1_ADDR__MASK			0x00007ffc
#define GEN75_MI_LOAD_URB_MEM_DW1_ADDR__SHIFT			2
#define GEN75_MI_LOAD_URB_MEM_DW1_ADDR__SHR			2

#define GEN75_MI_LOAD_URB_MEM_DW2_ADDR__MASK			0xffffffc0
#define GEN75_MI_LOAD_URB_MEM_DW2_ADDR__SHIFT			6
#define GEN75_MI_LOAD_URB_MEM_DW2_ADDR__SHR			6


#define GEN75_MI_STORE_URB_MEM__SIZE				4

#define GEN75_MI_STORE_URB_MEM_DW1_ADDR__MASK			0x00007ffc
#define GEN75_MI_STORE_URB_MEM_DW1_ADDR__SHIFT			2
#define GEN75_MI_STORE_URB_MEM_DW1_ADDR__SHR			2

#define GEN75_MI_STORE_URB_MEM_DW2_ADDR__MASK			0xffffffc0
#define GEN75_MI_STORE_URB_MEM_DW2_ADDR__SHIFT			6
#define GEN75_MI_STORE_URB_MEM_DW2_ADDR__SHR			6


#define GEN6_MI_BATCH_BUFFER_START__SIZE			3
#define GEN75_MI_BATCH_BUFFER_START_DW0_SECOND_LEVEL		(0x1 << 22)
#define GEN75_MI_BATCH_BUFFER_START_DW0_ADD_OFFSET_ENABLE	(0x1 << 16)
#define GEN75_MI_BATCH_BUFFER_START_DW0_PREDICATION_ENABLE	(0x1 << 15)
#define GEN75_MI_BATCH_BUFFER_START_DW0_NON_PRIVILEGED		(0x1 << 13)
#define GEN6_MI_BATCH_BUFFER_START_DW0_CLEAR_COMMAND_BUFFER	(0x1 << 11)
#define GEN6_MI_BATCH_BUFFER_START_DW0_USE_PPGTT		(0x1 << 8)

#define GEN6_MI_BATCH_BUFFER_START_DW1_ADDR__MASK		0xfffffffc
#define GEN6_MI_BATCH_BUFFER_START_DW1_ADDR__SHIFT		2
#define GEN6_MI_BATCH_BUFFER_START_DW1_ADDR__SHR		2



#endif /* GEN_MI_XML */
