/**************************************************************************
 * 
 * Copyright 2008 VMware, Inc.
 * Copyright 2009-2010 VMware, Inc.
 * All Rights Reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL VMWARE AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 **************************************************************************/

#ifndef P_SHADER_TOKENS_H
#define P_SHADER_TOKENS_H

#ifdef __cplusplus
extern "C" {
#endif


struct tgsi_header
{
   unsigned HeaderSize : 8;
   unsigned BodySize   : 24;
};

struct tgsi_processor
{
   unsigned Processor  : 4;  /* PIPE_SHADER_ */
   unsigned Padding    : 28;
};

enum tgsi_token_type {
   TGSI_TOKEN_TYPE_DECLARATION,
   TGSI_TOKEN_TYPE_IMMEDIATE,
   TGSI_TOKEN_TYPE_INSTRUCTION,
   TGSI_TOKEN_TYPE_PROPERTY,
};

struct tgsi_token
{
   unsigned Type       : 4;  /**< TGSI_TOKEN_TYPE_x */
   unsigned NrTokens   : 8;  /**< UINT */
   unsigned Padding    : 20;
};

enum tgsi_file_type {
   TGSI_FILE_NULL,
   TGSI_FILE_CONSTANT,
   TGSI_FILE_INPUT,
   TGSI_FILE_OUTPUT,
   TGSI_FILE_TEMPORARY,
   TGSI_FILE_SAMPLER,
   TGSI_FILE_ADDRESS,
   TGSI_FILE_IMMEDIATE,
   TGSI_FILE_PREDICATE,
   TGSI_FILE_SYSTEM_VALUE,
   TGSI_FILE_IMAGE,
   TGSI_FILE_SAMPLER_VIEW,
   TGSI_FILE_BUFFER,
   TGSI_FILE_MEMORY,
   TGSI_FILE_COUNT,      /**< how many TGSI_FILE_ types */
};


#define TGSI_WRITEMASK_NONE     0x00
#define TGSI_WRITEMASK_X        0x01
#define TGSI_WRITEMASK_Y        0x02
#define TGSI_WRITEMASK_XY       0x03
#define TGSI_WRITEMASK_Z        0x04
#define TGSI_WRITEMASK_XZ       0x05
#define TGSI_WRITEMASK_YZ       0x06
#define TGSI_WRITEMASK_XYZ      0x07
#define TGSI_WRITEMASK_W        0x08
#define TGSI_WRITEMASK_XW       0x09
#define TGSI_WRITEMASK_YW       0x0A
#define TGSI_WRITEMASK_XYW      0x0B
#define TGSI_WRITEMASK_ZW       0x0C
#define TGSI_WRITEMASK_XZW      0x0D
#define TGSI_WRITEMASK_YZW      0x0E
#define TGSI_WRITEMASK_XYZW     0x0F

enum tgsi_interpolate_mode {
   TGSI_INTERPOLATE_CONSTANT,
   TGSI_INTERPOLATE_LINEAR,
   TGSI_INTERPOLATE_PERSPECTIVE,
   TGSI_INTERPOLATE_COLOR,          /* special color case for smooth/flat */
   TGSI_INTERPOLATE_COUNT,
};

enum tgsi_interpolate_loc {
   TGSI_INTERPOLATE_LOC_CENTER,
   TGSI_INTERPOLATE_LOC_CENTROID,
   TGSI_INTERPOLATE_LOC_SAMPLE,
   TGSI_INTERPOLATE_LOC_COUNT,
};

#define TGSI_CYLINDRICAL_WRAP_X (1 << 0)
#define TGSI_CYLINDRICAL_WRAP_Y (1 << 1)
#define TGSI_CYLINDRICAL_WRAP_Z (1 << 2)
#define TGSI_CYLINDRICAL_WRAP_W (1 << 3)

enum tgsi_memory_type {
   TGSI_MEMORY_TYPE_GLOBAL,         /* OpenCL global              */
   TGSI_MEMORY_TYPE_SHARED,         /* OpenCL local / GLSL shared */
   TGSI_MEMORY_TYPE_PRIVATE,        /* OpenCL private             */
   TGSI_MEMORY_TYPE_INPUT,          /* OpenCL kernel input params */
   TGSI_MEMORY_TYPE_COUNT,
};

struct tgsi_declaration
{
   unsigned Type        : 4;  /**< TGSI_TOKEN_TYPE_DECLARATION */
   unsigned NrTokens    : 8;  /**< UINT */
   unsigned File        : 4;  /**< one of TGSI_FILE_x */
   unsigned UsageMask   : 4;  /**< bitmask of TGSI_WRITEMASK_x flags */
   unsigned Dimension   : 1;  /**< any extra dimension info? */
   unsigned Semantic    : 1;  /**< BOOL, any semantic info? */
   unsigned Interpolate : 1;  /**< any interpolation info? */
   unsigned Invariant   : 1;  /**< invariant optimization? */
   unsigned Local       : 1;  /**< optimize as subroutine local variable? */
   unsigned Array       : 1;  /**< extra array info? */
   unsigned Atomic      : 1;  /**< atomic only? for TGSI_FILE_BUFFER */
   unsigned MemType     : 2;  /**< TGSI_MEMORY_TYPE_x for TGSI_FILE_MEMORY */
   unsigned Padding     : 3;
};

struct tgsi_declaration_range
{
   unsigned First   : 16; /**< UINT */
   unsigned Last    : 16; /**< UINT */
};

struct tgsi_declaration_dimension
{
   unsigned Index2D:16; /**< UINT */
   unsigned Padding:16;
};

struct tgsi_declaration_interp
{
   unsigned Interpolate : 4;   /**< one of TGSI_INTERPOLATE_x */
   unsigned Location    : 2;   /**< one of TGSI_INTERPOLATE_LOC_x */
   unsigned CylindricalWrap:4; /**< TGSI_CYLINDRICAL_WRAP_x flags */
   unsigned Padding     : 22;
};

enum tgsi_semantic {
   TGSI_SEMANTIC_POSITION,
   TGSI_SEMANTIC_COLOR,
   TGSI_SEMANTIC_BCOLOR,       /**< back-face color */
   TGSI_SEMANTIC_FOG,
   TGSI_SEMANTIC_PSIZE,
   TGSI_SEMANTIC_GENERIC,
   TGSI_SEMANTIC_NORMAL,
   TGSI_SEMANTIC_FACE,
   TGSI_SEMANTIC_EDGEFLAG,
   TGSI_SEMANTIC_PRIMID,
   TGSI_SEMANTIC_INSTANCEID,  /**< doesn't include start_instance */
   TGSI_SEMANTIC_VERTEXID,
   TGSI_SEMANTIC_STENCIL,
   TGSI_SEMANTIC_CLIPDIST,
   TGSI_SEMANTIC_CLIPVERTEX,
   TGSI_SEMANTIC_GRID_SIZE,   /**< grid size in blocks */
   TGSI_SEMANTIC_BLOCK_ID,    /**< id of the current block */
   TGSI_SEMANTIC_BLOCK_SIZE,  /**< block size in threads */
   TGSI_SEMANTIC_THREAD_ID,   /**< block-relative id of the current thread */
   TGSI_SEMANTIC_TEXCOORD,    /**< texture or sprite coordinates */
   TGSI_SEMANTIC_PCOORD,      /**< point sprite coordinate */
   TGSI_SEMANTIC_VIEWPORT_INDEX,  /**< viewport index */
   TGSI_SEMANTIC_LAYER,       /**< layer (rendertarget index) */
   TGSI_SEMANTIC_SAMPLEID,
   TGSI_SEMANTIC_SAMPLEPOS,
   TGSI_SEMANTIC_SAMPLEMASK,
   TGSI_SEMANTIC_INVOCATIONID,
   TGSI_SEMANTIC_VERTEXID_NOBASE,
   TGSI_SEMANTIC_BASEVERTEX,
   TGSI_SEMANTIC_PATCH,       /**< generic per-patch semantic */
   TGSI_SEMANTIC_TESSCOORD,   /**< coordinate being processed by tess */
   TGSI_SEMANTIC_TESSOUTER,   /**< outer tessellation levels */
   TGSI_SEMANTIC_TESSINNER,   /**< inner tessellation levels */
   TGSI_SEMANTIC_VERTICESIN,  /**< number of input vertices */
   TGSI_SEMANTIC_HELPER_INVOCATION,  /**< current invocation is helper */
   TGSI_SEMANTIC_BASEINSTANCE,
   TGSI_SEMANTIC_DRAWID,
   TGSI_SEMANTIC_WORK_DIM,    /**< opencl get_work_dim value */
   TGSI_SEMANTIC_COUNT,       /**< number of semantic values */
};

struct tgsi_declaration_semantic
{
   unsigned Name           : 8;  /**< one of TGSI_SEMANTIC_x */
   unsigned Index          : 16; /**< UINT */
   unsigned Padding        : 8;
};

struct tgsi_declaration_image {
   unsigned Resource    : 8; /**< one of TGSI_TEXTURE_ */
   unsigned Raw         : 1;
   unsigned Writable    : 1;
   unsigned Format      : 10; /**< one of PIPE_FORMAT_ */
   unsigned Padding     : 12;
};

enum tgsi_return_type {
   TGSI_RETURN_TYPE_UNORM = 0,
   TGSI_RETURN_TYPE_SNORM,
   TGSI_RETURN_TYPE_SINT,
   TGSI_RETURN_TYPE_UINT,
   TGSI_RETURN_TYPE_FLOAT,
   TGSI_RETURN_TYPE_COUNT
};

struct tgsi_declaration_sampler_view {
   unsigned Resource    : 8; /**< one of TGSI_TEXTURE_ */
   unsigned ReturnTypeX : 6; /**< one of enum tgsi_return_type */
   unsigned ReturnTypeY : 6; /**< one of enum tgsi_return_type */
   unsigned ReturnTypeZ : 6; /**< one of enum tgsi_return_type */
   unsigned ReturnTypeW : 6; /**< one of enum tgsi_return_type */
};

struct tgsi_declaration_array {
   unsigned ArrayID : 10;
   unsigned Padding : 22;
};

enum tgsi_imm_type {
   TGSI_IMM_FLOAT32,
   TGSI_IMM_UINT32,
   TGSI_IMM_INT32,
   TGSI_IMM_FLOAT64,
   TGSI_IMM_UINT64,
   TGSI_IMM_INT64,
};

struct tgsi_immediate
{
   unsigned Type       : 4;  /**< TGSI_TOKEN_TYPE_IMMEDIATE */
   unsigned NrTokens   : 14; /**< UINT */
   unsigned DataType   : 4;  /**< one of TGSI_IMM_x */
   unsigned Padding    : 10;
};

union tgsi_immediate_data
{
   float Float;
   unsigned Uint;
   int Int;
};

enum tgsi_property_name {
   TGSI_PROPERTY_GS_INPUT_PRIM,
   TGSI_PROPERTY_GS_OUTPUT_PRIM,
   TGSI_PROPERTY_GS_MAX_OUTPUT_VERTICES,
   TGSI_PROPERTY_FS_COORD_ORIGIN,
   TGSI_PROPERTY_FS_COORD_PIXEL_CENTER,
   TGSI_PROPERTY_FS_COLOR0_WRITES_ALL_CBUFS,
   TGSI_PROPERTY_FS_DEPTH_LAYOUT,
   TGSI_PROPERTY_VS_PROHIBIT_UCPS,
   TGSI_PROPERTY_GS_INVOCATIONS,
   TGSI_PROPERTY_VS_WINDOW_SPACE_POSITION,
   TGSI_PROPERTY_TCS_VERTICES_OUT,
   TGSI_PROPERTY_TES_PRIM_MODE,
   TGSI_PROPERTY_TES_SPACING,
   TGSI_PROPERTY_TES_VERTEX_ORDER_CW,
   TGSI_PROPERTY_TES_POINT_MODE,
   TGSI_PROPERTY_NUM_CLIPDIST_ENABLED,
   TGSI_PROPERTY_NUM_CULLDIST_ENABLED,
   TGSI_PROPERTY_FS_EARLY_DEPTH_STENCIL,
   TGSI_PROPERTY_NEXT_SHADER,
   TGSI_PROPERTY_CS_FIXED_BLOCK_WIDTH,
   TGSI_PROPERTY_CS_FIXED_BLOCK_HEIGHT,
   TGSI_PROPERTY_CS_FIXED_BLOCK_DEPTH,
   TGSI_PROPERTY_COUNT,
};

struct tgsi_property {
   unsigned Type         : 4;  /**< TGSI_TOKEN_TYPE_PROPERTY */
   unsigned NrTokens     : 8;  /**< UINT */
   unsigned PropertyName : 8;  /**< one of TGSI_PROPERTY */
   unsigned Padding      : 12;
};

enum tgsi_fs_coord_origin {
   TGSI_FS_COORD_ORIGIN_UPPER_LEFT,
   TGSI_FS_COORD_ORIGIN_LOWER_LEFT,
};

enum tgsi_fs_coord_pixcenter {
   TGSI_FS_COORD_PIXEL_CENTER_HALF_INTEGER,
   TGSI_FS_COORD_PIXEL_CENTER_INTEGER,
};

enum tgsi_fs_depth_layout {
   TGSI_FS_DEPTH_LAYOUT_NONE,
   TGSI_FS_DEPTH_LAYOUT_ANY,
   TGSI_FS_DEPTH_LAYOUT_GREATER,
   TGSI_FS_DEPTH_LAYOUT_LESS,
   TGSI_FS_DEPTH_LAYOUT_UNCHANGED,
};

struct tgsi_property_data {
   unsigned Data;
};

/* TGSI opcodes.  
 * 
 * For more information on semantics of opcodes and
 * which APIs are known to use which opcodes, see
 * gallium/docs/source/tgsi.rst
 */
#define TGSI_OPCODE_ARL                 0
#define TGSI_OPCODE_MOV                 1
#define TGSI_OPCODE_LIT                 2
#define TGSI_OPCODE_RCP                 3
#define TGSI_OPCODE_RSQ                 4
#define TGSI_OPCODE_EXP                 5
#define TGSI_OPCODE_LOG                 6
#define TGSI_OPCODE_MUL                 7
#define TGSI_OPCODE_ADD                 8
#define TGSI_OPCODE_DP3                 9
#define TGSI_OPCODE_DP4                 10
#define TGSI_OPCODE_DST                 11
#define TGSI_OPCODE_MIN                 12
#define TGSI_OPCODE_MAX                 13
#define TGSI_OPCODE_SLT                 14
#define TGSI_OPCODE_SGE                 15
#define TGSI_OPCODE_MAD                 16
#define TGSI_OPCODE_SUB                 17
#define TGSI_OPCODE_LRP                 18
#define TGSI_OPCODE_FMA                 19
#define TGSI_OPCODE_SQRT                20
#define TGSI_OPCODE_DP2A                21
#define TGSI_OPCODE_F2U64               22
#define TGSI_OPCODE_F2I64               23
#define TGSI_OPCODE_FRC                 24
#define TGSI_OPCODE_CLAMP               25
#define TGSI_OPCODE_FLR                 26
#define TGSI_OPCODE_ROUND               27
#define TGSI_OPCODE_EX2                 28
#define TGSI_OPCODE_LG2                 29
#define TGSI_OPCODE_POW                 30
#define TGSI_OPCODE_XPD                 31
#define TGSI_OPCODE_U2I64               32
#define TGSI_OPCODE_ABS                 33
#define TGSI_OPCODE_I2I64               34
#define TGSI_OPCODE_DPH                 35
#define TGSI_OPCODE_COS                 36
#define TGSI_OPCODE_DDX                 37
#define TGSI_OPCODE_DDY                 38
#define TGSI_OPCODE_KILL                39 /* unconditional */
#define TGSI_OPCODE_PK2H                40
#define TGSI_OPCODE_PK2US               41
#define TGSI_OPCODE_PK4B                42
#define TGSI_OPCODE_PK4UB               43
#define TGSI_OPCODE_D2U64               44
#define TGSI_OPCODE_SEQ                 45
#define TGSI_OPCODE_D2I64               46
#define TGSI_OPCODE_SGT                 47
#define TGSI_OPCODE_SIN                 48
#define TGSI_OPCODE_SLE                 49
#define TGSI_OPCODE_SNE                 50
#define TGSI_OPCODE_U642D               51
#define TGSI_OPCODE_TEX                 52
#define TGSI_OPCODE_TXD                 53
#define TGSI_OPCODE_TXP                 54
#define TGSI_OPCODE_UP2H                55
#define TGSI_OPCODE_UP2US               56
#define TGSI_OPCODE_UP4B                57
#define TGSI_OPCODE_UP4UB               58
#define TGSI_OPCODE_U642F               59
#define TGSI_OPCODE_I642F               60
#define TGSI_OPCODE_ARR                 61
#define TGSI_OPCODE_I642D               62
#define TGSI_OPCODE_CAL                 63
#define TGSI_OPCODE_RET                 64
#define TGSI_OPCODE_SSG                 65 /* SGN */
#define TGSI_OPCODE_CMP                 66
#define TGSI_OPCODE_SCS                 67
#define TGSI_OPCODE_TXB                 68
                                /* gap */
#define TGSI_OPCODE_DIV                 70
#define TGSI_OPCODE_DP2                 71
#define TGSI_OPCODE_TXL                 72
#define TGSI_OPCODE_BRK                 73
#define TGSI_OPCODE_IF                  74
#define TGSI_OPCODE_UIF                 75
                                /* gap */
#define TGSI_OPCODE_ELSE                77
#define TGSI_OPCODE_ENDIF               78

#define TGSI_OPCODE_DDX_FINE            79
#define TGSI_OPCODE_DDY_FINE            80

#define TGSI_OPCODE_PUSHA               81
#define TGSI_OPCODE_POPA                82
#define TGSI_OPCODE_CEIL                83
#define TGSI_OPCODE_I2F                 84
#define TGSI_OPCODE_NOT                 85
#define TGSI_OPCODE_TRUNC               86
#define TGSI_OPCODE_SHL                 87
                                /* gap */
#define TGSI_OPCODE_AND                 89
#define TGSI_OPCODE_OR                  90
#define TGSI_OPCODE_MOD                 91
#define TGSI_OPCODE_XOR                 92
#define TGSI_OPCODE_SAD                 93
#define TGSI_OPCODE_TXF                 94
#define TGSI_OPCODE_TXQ                 95
#define TGSI_OPCODE_CONT                96
#define TGSI_OPCODE_EMIT                97
#define TGSI_OPCODE_ENDPRIM             98
#define TGSI_OPCODE_BGNLOOP             99
#define TGSI_OPCODE_BGNSUB              100
#define TGSI_OPCODE_ENDLOOP             101
#define TGSI_OPCODE_ENDSUB              102
#define TGSI_OPCODE_TXQ_LZ              103 /* TXQ for mipmap level 0 */
#define TGSI_OPCODE_TXQS                104
#define TGSI_OPCODE_RESQ                105
                                /* gap */
#define TGSI_OPCODE_NOP                 107

#define TGSI_OPCODE_FSEQ                108
#define TGSI_OPCODE_FSGE                109
#define TGSI_OPCODE_FSLT                110
#define TGSI_OPCODE_FSNE                111

#define TGSI_OPCODE_MEMBAR              112
#define TGSI_OPCODE_CALLNZ              113
                                /* gap */
#define TGSI_OPCODE_BREAKC              115
#define TGSI_OPCODE_KILL_IF             116  /* conditional kill */
#define TGSI_OPCODE_END                 117  /* aka HALT */
#define TGSI_OPCODE_DFMA                118
#define TGSI_OPCODE_F2I                 119
#define TGSI_OPCODE_IDIV                120
#define TGSI_OPCODE_IMAX                121
#define TGSI_OPCODE_IMIN                122
#define TGSI_OPCODE_INEG                123
#define TGSI_OPCODE_ISGE                124
#define TGSI_OPCODE_ISHR                125
#define TGSI_OPCODE_ISLT                126
#define TGSI_OPCODE_F2U                 127
#define TGSI_OPCODE_U2F                 128
#define TGSI_OPCODE_UADD                129
#define TGSI_OPCODE_UDIV                130
#define TGSI_OPCODE_UMAD                131
#define TGSI_OPCODE_UMAX                132
#define TGSI_OPCODE_UMIN                133
#define TGSI_OPCODE_UMOD                134
#define TGSI_OPCODE_UMUL                135
#define TGSI_OPCODE_USEQ                136
#define TGSI_OPCODE_USGE                137
#define TGSI_OPCODE_USHR                138
#define TGSI_OPCODE_USLT                139
#define TGSI_OPCODE_USNE                140
#define TGSI_OPCODE_SWITCH              141
#define TGSI_OPCODE_CASE                142
#define TGSI_OPCODE_DEFAULT             143
#define TGSI_OPCODE_ENDSWITCH           144

/* resource related opcodes */
#define TGSI_OPCODE_SAMPLE              145
#define TGSI_OPCODE_SAMPLE_I            146
#define TGSI_OPCODE_SAMPLE_I_MS         147
#define TGSI_OPCODE_SAMPLE_B            148
#define TGSI_OPCODE_SAMPLE_C            149
#define TGSI_OPCODE_SAMPLE_C_LZ         150
#define TGSI_OPCODE_SAMPLE_D            151
#define TGSI_OPCODE_SAMPLE_L            152
#define TGSI_OPCODE_GATHER4             153
#define TGSI_OPCODE_SVIEWINFO           154
#define TGSI_OPCODE_SAMPLE_POS          155
#define TGSI_OPCODE_SAMPLE_INFO         156

#define TGSI_OPCODE_UARL                157
#define TGSI_OPCODE_UCMP                158
#define TGSI_OPCODE_IABS                159
#define TGSI_OPCODE_ISSG                160

#define TGSI_OPCODE_LOAD                161
#define TGSI_OPCODE_STORE               162

#define TGSI_OPCODE_MFENCE              163
#define TGSI_OPCODE_LFENCE              164
#define TGSI_OPCODE_SFENCE              165
#define TGSI_OPCODE_BARRIER             166

#define TGSI_OPCODE_ATOMUADD            167
#define TGSI_OPCODE_ATOMXCHG            168
#define TGSI_OPCODE_ATOMCAS             169
#define TGSI_OPCODE_ATOMAND             170
#define TGSI_OPCODE_ATOMOR              171
#define TGSI_OPCODE_ATOMXOR             172
#define TGSI_OPCODE_ATOMUMIN            173
#define TGSI_OPCODE_ATOMUMAX            174
#define TGSI_OPCODE_ATOMIMIN            175
#define TGSI_OPCODE_ATOMIMAX            176

/* to be used for shadow cube map compares */
#define TGSI_OPCODE_TEX2                177
#define TGSI_OPCODE_TXB2                178
#define TGSI_OPCODE_TXL2                179

#define TGSI_OPCODE_IMUL_HI             180
#define TGSI_OPCODE_UMUL_HI             181

#define TGSI_OPCODE_TG4                 182

#define TGSI_OPCODE_LODQ                183

#define TGSI_OPCODE_IBFE                184
#define TGSI_OPCODE_UBFE                185
#define TGSI_OPCODE_BFI                 186
#define TGSI_OPCODE_BREV                187
#define TGSI_OPCODE_POPC                188
#define TGSI_OPCODE_LSB                 189
#define TGSI_OPCODE_IMSB                190
#define TGSI_OPCODE_UMSB                191

#define TGSI_OPCODE_INTERP_CENTROID     192
#define TGSI_OPCODE_INTERP_SAMPLE       193
#define TGSI_OPCODE_INTERP_OFFSET       194

/* sm5 marked opcodes are supported in D3D11 optionally - also DMOV, DMOVC */
#define TGSI_OPCODE_F2D                 195 /* SM5 */
#define TGSI_OPCODE_D2F                 196
#define TGSI_OPCODE_DABS                197
#define TGSI_OPCODE_DNEG                198 /* SM5 */
#define TGSI_OPCODE_DADD                199 /* SM5 */
#define TGSI_OPCODE_DMUL                200 /* SM5 */
#define TGSI_OPCODE_DMAX                201 /* SM5 */
#define TGSI_OPCODE_DMIN                202 /* SM5 */
#define TGSI_OPCODE_DSLT                203 /* SM5 */
#define TGSI_OPCODE_DSGE                204 /* SM5 */
#define TGSI_OPCODE_DSEQ                205 /* SM5 */
#define TGSI_OPCODE_DSNE                206 /* SM5 */
#define TGSI_OPCODE_DRCP                207 /* eg, cayman */
#define TGSI_OPCODE_DSQRT               208 /* eg, cayman also has DRSQ */
#define TGSI_OPCODE_DMAD                209
#define TGSI_OPCODE_DFRAC               210 /* eg, cayman */
#define TGSI_OPCODE_DLDEXP              211 /* eg, cayman */
#define TGSI_OPCODE_DFRACEXP            212 /* eg, cayman */
#define TGSI_OPCODE_D2I                 213
#define TGSI_OPCODE_I2D                 214
#define TGSI_OPCODE_D2U                 215
#define TGSI_OPCODE_U2D                 216
#define TGSI_OPCODE_DRSQ                217 /* eg, cayman also has DRSQ */
#define TGSI_OPCODE_DTRUNC              218 /* nvc0 */
#define TGSI_OPCODE_DCEIL               219 /* nvc0 */
#define TGSI_OPCODE_DFLR                220 /* nvc0 */
#define TGSI_OPCODE_DROUND              221 /* nvc0 */
#define TGSI_OPCODE_DSSG                222

#define TGSI_OPCODE_VOTE_ANY            223
#define TGSI_OPCODE_VOTE_ALL            224
#define TGSI_OPCODE_VOTE_EQ             225

#define TGSI_OPCODE_U64SEQ              226
#define TGSI_OPCODE_U64SNE              227
#define TGSI_OPCODE_I64SLT              228
#define TGSI_OPCODE_U64SLT              229
#define TGSI_OPCODE_I64SGE              230
#define TGSI_OPCODE_U64SGE              231

#define TGSI_OPCODE_I64MIN              232
#define TGSI_OPCODE_U64MIN              233
#define TGSI_OPCODE_I64MAX              234
#define TGSI_OPCODE_U64MAX              235

#define TGSI_OPCODE_I64ABS              236
#define TGSI_OPCODE_I64SSG              237
#define TGSI_OPCODE_I64NEG              238

#define TGSI_OPCODE_U64ADD              239
#define TGSI_OPCODE_U64MUL              240
#define TGSI_OPCODE_U64SHL              241
#define TGSI_OPCODE_I64SHR              242
#define TGSI_OPCODE_U64SHR              243

#define TGSI_OPCODE_I64DIV              244
#define TGSI_OPCODE_U64DIV              245
#define TGSI_OPCODE_I64MOD              246
#define TGSI_OPCODE_U64MOD              247
#define TGSI_OPCODE_LAST                248

/**
 * Opcode is the operation code to execute. A given operation defines the
 * semantics how the source registers (if any) are interpreted and what is
 * written to the destination registers (if any) as a result of execution.
 *
 * NumDstRegs and NumSrcRegs is the number of destination and source registers,
 * respectively. For a given operation code, those numbers are fixed and are
 * present here only for convenience.
 *
 * If Predicate is TRUE, tgsi_instruction_predicate token immediately follows.
 *
 * Saturate controls how are final results in destination registers modified.
 */

struct tgsi_instruction
{
   unsigned Type       : 4;  /* TGSI_TOKEN_TYPE_INSTRUCTION */
   unsigned NrTokens   : 8;  /* UINT */
   unsigned Opcode     : 8;  /* TGSI_OPCODE_ */
   unsigned Saturate   : 1;  /* BOOL */
   unsigned NumDstRegs : 2;  /* UINT */
   unsigned NumSrcRegs : 4;  /* UINT */
   unsigned Predicate  : 1;  /* BOOL */
   unsigned Label      : 1;
   unsigned Texture    : 1;
   unsigned Memory     : 1;
   unsigned Padding    : 1;
};

/*
 * If tgsi_instruction::Label is TRUE, tgsi_instruction_label follows.
 *
 * If tgsi_instruction::Texture is TRUE, tgsi_instruction_texture follows.
 *   if texture instruction has a number of offsets,
 *   then tgsi_instruction::Texture::NumOffset of tgsi_texture_offset follow.
 * 
 * Then, tgsi_instruction::NumDstRegs of tgsi_dst_register follow.
 * 
 * Then, tgsi_instruction::NumSrcRegs of tgsi_src_register follow.
 *
 * tgsi_instruction::NrTokens contains the total number of words that make the
 * instruction, including the instruction word.
 */

enum tgsi_swizzle {
   TGSI_SWIZZLE_X,
   TGSI_SWIZZLE_Y,
   TGSI_SWIZZLE_Z,
   TGSI_SWIZZLE_W,
};

struct tgsi_instruction_label
{
   unsigned Label    : 24;   /* UINT */
   unsigned Padding  : 8;
};

enum tgsi_texture_type {
   TGSI_TEXTURE_BUFFER,
   TGSI_TEXTURE_1D,
   TGSI_TEXTURE_2D,
   TGSI_TEXTURE_3D,
   TGSI_TEXTURE_CUBE,
   TGSI_TEXTURE_RECT,
   TGSI_TEXTURE_SHADOW1D,
   TGSI_TEXTURE_SHADOW2D,
   TGSI_TEXTURE_SHADOWRECT,
   TGSI_TEXTURE_1D_ARRAY,
   TGSI_TEXTURE_2D_ARRAY,
   TGSI_TEXTURE_SHADOW1D_ARRAY,
   TGSI_TEXTURE_SHADOW2D_ARRAY,
   TGSI_TEXTURE_SHADOWCUBE,
   TGSI_TEXTURE_2D_MSAA,
   TGSI_TEXTURE_2D_ARRAY_MSAA,
   TGSI_TEXTURE_CUBE_ARRAY,
   TGSI_TEXTURE_SHADOWCUBE_ARRAY,
   TGSI_TEXTURE_UNKNOWN,
   TGSI_TEXTURE_COUNT,
};

struct tgsi_instruction_texture
{
   unsigned Texture  : 8;    /* TGSI_TEXTURE_ */
   unsigned NumOffsets : 4;
   unsigned Padding : 20;
};

/* for texture offsets in GLSL and DirectX.
 * Generally these always come from TGSI_FILE_IMMEDIATE,
 * however DX11 appears to have the capability to do
 * non-constant texture offsets.
 */
struct tgsi_texture_offset
{
   int      Index    : 16;
   unsigned File     : 4;  /**< one of TGSI_FILE_x */
   unsigned SwizzleX : 2;  /* TGSI_SWIZZLE_x */
   unsigned SwizzleY : 2;  /* TGSI_SWIZZLE_x */
   unsigned SwizzleZ : 2;  /* TGSI_SWIZZLE_x */
   unsigned Padding  : 6;
};

/*
 * For SM3, the following constraint applies.
 *   - Swizzle is either set to identity or replicate.
 */
struct tgsi_instruction_predicate
{
   int      Index    : 16; /* SINT */
   unsigned SwizzleX : 2;  /* TGSI_SWIZZLE_x */
   unsigned SwizzleY : 2;  /* TGSI_SWIZZLE_x */
   unsigned SwizzleZ : 2;  /* TGSI_SWIZZLE_x */
   unsigned SwizzleW : 2;  /* TGSI_SWIZZLE_x */
   unsigned Negate   : 1;  /* BOOL */
   unsigned Padding  : 7;
};

/**
 * File specifies the register array to access.
 *
 * Index specifies the element number of a register in the register file.
 *
 * If Indirect is TRUE, Index should be offset by the X component of the indirect
 * register that follows. The register can be now fetched into local storage
 * for further processing.
 *
 * If Negate is TRUE, all components of the fetched register are negated.
 *
 * The fetched register components are swizzled according to SwizzleX, SwizzleY,
 * SwizzleZ and SwizzleW.
 *
 */

struct tgsi_src_register
{
   unsigned File        : 4;  /* TGSI_FILE_ */
   unsigned Indirect    : 1;  /* BOOL */
   unsigned Dimension   : 1;  /* BOOL */
   int      Index       : 16; /* SINT */
   unsigned SwizzleX    : 2;  /* TGSI_SWIZZLE_ */
   unsigned SwizzleY    : 2;  /* TGSI_SWIZZLE_ */
   unsigned SwizzleZ    : 2;  /* TGSI_SWIZZLE_ */
   unsigned SwizzleW    : 2;  /* TGSI_SWIZZLE_ */
   unsigned Absolute    : 1;    /* BOOL */
   unsigned Negate      : 1;    /* BOOL */
};

/**
 * If tgsi_src_register::Indirect is TRUE, tgsi_ind_register follows.
 *
 * File, Index and Swizzle are handled the same as in tgsi_src_register.
 *
 * If ArrayID is zero the whole register file might be indirectly addressed,
 * if not only the Declaration with this ArrayID is accessed by this operand.
 *
 */

struct tgsi_ind_register
{
   unsigned File    : 4;  /* TGSI_FILE_ */
   int      Index   : 16; /* SINT */
   unsigned Swizzle : 2;  /* TGSI_SWIZZLE_ */
   unsigned ArrayID : 10; /* UINT */
};

/**
 * If tgsi_src_register::Dimension is TRUE, tgsi_dimension follows.
 */

struct tgsi_dimension
{
   unsigned Indirect    : 1;  /* BOOL */
   unsigned Dimension   : 1;  /* BOOL */
   unsigned Padding     : 14;
   int      Index       : 16; /* SINT */
};

struct tgsi_dst_register
{
   unsigned File        : 4;  /* TGSI_FILE_ */
   unsigned WriteMask   : 4;  /* TGSI_WRITEMASK_ */
   unsigned Indirect    : 1;  /* BOOL */
   unsigned Dimension   : 1;  /* BOOL */
   int      Index       : 16; /* SINT */
   unsigned Padding     : 6;
};

#define TGSI_MEMORY_COHERENT (1 << 0)
#define TGSI_MEMORY_RESTRICT (1 << 1)
#define TGSI_MEMORY_VOLATILE (1 << 2)

/**
 * Specifies the type of memory access to do for the LOAD/STORE instruction.
 */
struct tgsi_instruction_memory
{
   unsigned Qualifier : 3;  /* TGSI_MEMORY_ */
   unsigned Texture   : 8;  /* only for images: TGSI_TEXTURE_ */
   unsigned Format    : 10; /* only for images: PIPE_FORMAT_ */
   unsigned Padding   : 11;
};

#define TGSI_MEMBAR_SHADER_BUFFER (1 << 0)
#define TGSI_MEMBAR_ATOMIC_BUFFER (1 << 1)
#define TGSI_MEMBAR_SHADER_IMAGE  (1 << 2)
#define TGSI_MEMBAR_SHARED        (1 << 3)
#define TGSI_MEMBAR_THREAD_GROUP  (1 << 4)

#ifdef __cplusplus
}
#endif

#endif /* P_SHADER_TOKENS_H */
