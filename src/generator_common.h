/******************************************************************************
* Copyright (c) Intel Corporation - All rights reserved.                      *
* This file is part of the LIBXSMM library.                                   *
*                                                                             *
* For information on the license, see the LICENSE file.                       *
* Further information: https://github.com/hfp/libxsmm/                        *
* SPDX-License-Identifier: BSD-3-Clause                                       *
******************************************************************************/
/* Alexander Heinecke, Greg Henry (Intel Corp.)
******************************************************************************/
#ifndef GENERATOR_COMMON_H
#define GENERATOR_COMMON_H

#include <libxsmm_generator.h>
#include <libxsmm_cpuid.h>

/*@TODO check if we want to use enums here? Has this implications in the encoder? */
/* defining register mappings */
#define LIBXSMM_X86_GP_REG_RAX               0
#define LIBXSMM_X86_GP_REG_RCX               1
#define LIBXSMM_X86_GP_REG_RDX               2
#define LIBXSMM_X86_GP_REG_RBX               3
#define LIBXSMM_X86_GP_REG_RSP               4
#define LIBXSMM_X86_GP_REG_RBP               5
#define LIBXSMM_X86_GP_REG_RSI               6
#define LIBXSMM_X86_GP_REG_RDI               7
#define LIBXSMM_X86_GP_REG_R8                8
#define LIBXSMM_X86_GP_REG_R9                9
#define LIBXSMM_X86_GP_REG_R10              10
#define LIBXSMM_X86_GP_REG_R11              11
#define LIBXSMM_X86_GP_REG_R12              12
#define LIBXSMM_X86_GP_REG_R13              13
#define LIBXSMM_X86_GP_REG_R14              14
#define LIBXSMM_X86_GP_REG_R15              15
#define LIBXSMM_X86_GP_REG_UNDEF           127

/* define a place holder to handle AVX and SSE with a single encoder function
   using this values as the third operand means SSE */
#define LIBXSMM_X86_VEC_REG_UNDEF          255
#define LIBXSMM_X86_MASK_REG_UNDEF         255
#define LIBXSMM_X86_AVX512_MASK              1  /* this specifies k1 */

/* special value for undefined immediate */
#define LIBXSMM_X86_IMM_UNDEF             1024

/* special instruction */
#define LIBXSMM_X86_INSTR_UNDEF           9999

/* Load/Store/Move instructions */
/* AVX1,AVX2,AVX512 */
#define LIBXSMM_X86_INSTR_VMOVAPD        10000
#define LIBXSMM_X86_INSTR_VMOVUPD        10001
#define LIBXSMM_X86_INSTR_VMOVAPS        10002
#define LIBXSMM_X86_INSTR_VMOVUPS        10003
#define LIBXSMM_X86_INSTR_VBROADCASTSD   10004
#define LIBXSMM_X86_INSTR_VBROADCASTSS   10005
#define LIBXSMM_X86_INSTR_VMOVDDUP       10006
#define LIBXSMM_X86_INSTR_VMOVSD         10007
#define LIBXSMM_X86_INSTR_VMOVSS         10008
#define LIBXSMM_X86_INSTR_VPBROADCASTB   10025
#define LIBXSMM_X86_INSTR_VPBROADCASTW   10026
#define LIBXSMM_X86_INSTR_VPBROADCASTD   10027
#define LIBXSMM_X86_INSTR_VPBROADCASTQ   10028
#define LIBXSMM_X86_INSTR_VMOVDQA32      10029
#define LIBXSMM_X86_INSTR_VMOVDQA64      10030
#define LIBXSMM_X86_INSTR_VMOVDQU8       10031
#define LIBXSMM_X86_INSTR_VMOVDQU16      10032
#define LIBXSMM_X86_INSTR_VMOVDQU32      10033
#define LIBXSMM_X86_INSTR_VMOVDQU64      10034
#define LIBXSMM_X86_INSTR_VMASKMOVPD     10035
#define LIBXSMM_X86_INSTR_VMASKMOVPS     10036
#define LIBXSMM_X86_INSTR_VMOVNTPD       10037
#define LIBXSMM_X86_INSTR_VMOVNTPS       10038
#define LIBXSMM_X86_INSTR_VMOVNTDQ       10039

/* SSE */
#define LIBXSMM_X86_INSTR_MOVAPD         10009
#define LIBXSMM_X86_INSTR_MOVUPD         10010
#define LIBXSMM_X86_INSTR_MOVAPS         10011
#define LIBXSMM_X86_INSTR_MOVUPS         10012
#define LIBXSMM_X86_INSTR_MOVSD          10013
#define LIBXSMM_X86_INSTR_MOVSS          10014
#define LIBXSMM_X86_INSTR_MOVDDUP        10015
#define LIBXSMM_X86_INSTR_SHUFPS         10016
#define LIBXSMM_X86_INSTR_SHUFPD         10017

/* Gather/Scatter instructions */
#define LIBXSMM_X86_INSTR_VGATHERDPS     11000
#define LIBXSMM_X86_INSTR_VGATHERDPD     11001
#define LIBXSMM_X86_INSTR_VGATHERQPS     11002
#define LIBXSMM_X86_INSTR_VGATHERQPD     11003
#define LIBXSMM_X86_INSTR_VSCATTERDPS    11004
#define LIBXSMM_X86_INSTR_VSCATTERDPD    11005
#define LIBXSMM_X86_INSTR_VSCATTERQPS    11006
#define LIBXSMM_X86_INSTR_VSCATTERQPD    11007

/* Shuffle/Permute/Blend instructions */
#define LIBXSMM_X86_INSTR_VSHUFPS        12000
#define LIBXSMM_X86_INSTR_VPERM2F128     12001
#define LIBXSMM_X86_INSTR_VSHUFF64X2     12002
#define LIBXSMM_X86_INSTR_VEXTRACTF32X8  12003
#define LIBXSMM_X86_INSTR_VEXTRACTF64X4  12004
#define LIBXSMM_X86_INSTR_VSHUFPD        12005
#define LIBXSMM_X86_INSTR_VSHUFF32X4     12006
#define LIBXSMM_X86_INSTR_VSHUFI32X4     12007
#define LIBXSMM_X86_INSTR_VSHUFI64X2     12008

/* Vector compute instructions */
/* AVX1,AVX2,AVX512 */
#define LIBXSMM_X86_INSTR_VXORPD         20000
#define LIBXSMM_X86_INSTR_VMULPD         20001
#define LIBXSMM_X86_INSTR_VADDPD         20002
#define LIBXSMM_X86_INSTR_VSUBPD         20003
#define LIBXSMM_X86_INSTR_VFMADD231PD    20004
#define LIBXSMM_X86_INSTR_VFMSUB231PD    20005
#define LIBXSMM_X86_INSTR_VFNMADD231PD   20006
#define LIBXSMM_X86_INSTR_VFNMSUB231PD   20007
#define LIBXSMM_X86_INSTR_VMULSD         20008
#define LIBXSMM_X86_INSTR_VADDSD         20009
#define LIBXSMM_X86_INSTR_VSUBSD         20010
#define LIBXSMM_X86_INSTR_VFMADD231SD    20011
#define LIBXSMM_X86_INSTR_VFMSUB231SD    20012
#define LIBXSMM_X86_INSTR_VFNMADD231SD   20013
#define LIBXSMM_X86_INSTR_VFNMSUB231SD   20014
#define LIBXSMM_X86_INSTR_VXORPS         20015
#define LIBXSMM_X86_INSTR_VMULPS         20016
#define LIBXSMM_X86_INSTR_VADDPS         20017
#define LIBXSMM_X86_INSTR_VSUBPS         20018
#define LIBXSMM_X86_INSTR_VFMADD231PS    20019
#define LIBXSMM_X86_INSTR_VFMSUB231PS    20020
#define LIBXSMM_X86_INSTR_VFNMADD231PS   20021
#define LIBXSMM_X86_INSTR_VFNMSUB231PS   20022
#define LIBXSMM_X86_INSTR_VMULSS         20023
#define LIBXSMM_X86_INSTR_VADDSS         20024
#define LIBXSMM_X86_INSTR_VSUBSS         20025
#define LIBXSMM_X86_INSTR_VFMADD231SS    20026
#define LIBXSMM_X86_INSTR_VFMSUB231SS    20027
#define LIBXSMM_X86_INSTR_VFNMADD231SS   20028
#define LIBXSMM_X86_INSTR_VFNMSUB231SS   20029
#define LIBXSMM_X86_INSTR_VPERMW         20030
#define LIBXSMM_X86_INSTR_VFMADD132PD    20031
#define LIBXSMM_X86_INSTR_VFMSUB132PD    20032
#define LIBXSMM_X86_INSTR_VFNMADD132PD   20033
#define LIBXSMM_X86_INSTR_VFNMSUB132PD   20034
#define LIBXSMM_X86_INSTR_VFMADD132SD    20035
#define LIBXSMM_X86_INSTR_VFMSUB132SD    20036
#define LIBXSMM_X86_INSTR_VFNMADD132SD   20037
#define LIBXSMM_X86_INSTR_VFNMSUB132SD   20038
#define LIBXSMM_X86_INSTR_VFMADD132PS    20039
#define LIBXSMM_X86_INSTR_VFMSUB132PS    20040
#define LIBXSMM_X86_INSTR_VFNMADD132PS   20041
#define LIBXSMM_X86_INSTR_VFNMSUB132PS   20042
#define LIBXSMM_X86_INSTR_VFMADD132SS    20043
#define LIBXSMM_X86_INSTR_VFMSUB132SS    20044
#define LIBXSMM_X86_INSTR_VFNMADD132SS   20045
#define LIBXSMM_X86_INSTR_VFNMSUB132SS   20046
#define LIBXSMM_X86_INSTR_VFMADD213PD    20047
#define LIBXSMM_X86_INSTR_VFMSUB213PD    20048
#define LIBXSMM_X86_INSTR_VFNMADD213PD   20049
#define LIBXSMM_X86_INSTR_VFNMSUB213PD   20050
#define LIBXSMM_X86_INSTR_VFMADD213SD    20051
#define LIBXSMM_X86_INSTR_VFMSUB213SD    20052
#define LIBXSMM_X86_INSTR_VFNMADD213SD   20053
#define LIBXSMM_X86_INSTR_VFNMSUB213SD   20054
#define LIBXSMM_X86_INSTR_VFMADD213PS    20055
#define LIBXSMM_X86_INSTR_VFMSUB213PS    20056
#define LIBXSMM_X86_INSTR_VFNMADD213PS   20057
#define LIBXSMM_X86_INSTR_VFNMSUB213PS   20058
#define LIBXSMM_X86_INSTR_VFMADD213SS    20059
#define LIBXSMM_X86_INSTR_VFMSUB213SS    20060
#define LIBXSMM_X86_INSTR_VFNMADD213SS   20061
#define LIBXSMM_X86_INSTR_VFNMSUB213SS   20062
/* SSE */
#define LIBXSMM_X86_INSTR_XORPD          20063
#define LIBXSMM_X86_INSTR_MULPD          20064
#define LIBXSMM_X86_INSTR_ADDPD          20065
#define LIBXSMM_X86_INSTR_SUBPD          20066
#define LIBXSMM_X86_INSTR_MULSD          20067
#define LIBXSMM_X86_INSTR_ADDSD          20068
#define LIBXSMM_X86_INSTR_SUBSD          20069
#define LIBXSMM_X86_INSTR_XORPS          20070
#define LIBXSMM_X86_INSTR_MULPS          20071
#define LIBXSMM_X86_INSTR_ADDPS          20072
#define LIBXSMM_X86_INSTR_SUBPS          20073
#define LIBXSMM_X86_INSTR_MULSS          20074
#define LIBXSMM_X86_INSTR_ADDSS          20075
#define LIBXSMM_X86_INSTR_SUBSS          20076
/* AVX512F: Integer XOR as there is no FP */
#define LIBXSMM_X86_INSTR_VPXORD         20077
/* additional integer stuff */
#define LIBXSMM_X86_INSTR_VPADDQ         20078
#define LIBXSMM_X86_INSTR_VPADDD         20079
#define LIBXSMM_X86_INSTR_VPADDW         20080
#define LIBXSMM_X86_INSTR_VPADDB         20081
#define LIBXSMM_X86_INSTR_VPMADDWD       20082
#define LIBXSMM_X86_INSTR_VPMADDUBSW     20083
#define LIBXSMM_X86_INSTR_VPADDSW        20084
#define LIBXSMM_X86_INSTR_VPADDSB        20085
#define LIBXSMM_X86_INSTR_VPSUBD         20086
/* Additional vector manipulations */
#define LIBXSMM_X86_INSTR_VUNPCKLPD      20087
#define LIBXSMM_X86_INSTR_VUNPCKLPS      20088
#define LIBXSMM_X86_INSTR_VUNPCKHPD      20089
#define LIBXSMM_X86_INSTR_VUNPCKHPS      20090
#define LIBXSMM_X86_INSTR_VPSRAVD        20091
#define LIBXSMM_X86_INSTR_VCVTDQ2PS      20092
#define LIBXSMM_X86_INSTR_VDIVPS         20093
#define LIBXSMM_X86_INSTR_VDIVPD         20094
#define LIBXSMM_X86_INSTR_VCVTPS2PD      20095
#define LIBXSMM_X86_INSTR_VBLENDMPS      20096
#define LIBXSMM_X86_INSTR_VCMPPS         20097
#define LIBXSMM_X86_INSTR_VPANDD         20098
#define LIBXSMM_X86_INSTR_VPANDQ         20099
#define LIBXSMM_X86_INSTR_VMAXPD         20100
#define LIBXSMM_X86_INSTR_VMAXPS         20101
#define LIBXSMM_X86_INSTR_VCVTPS2PH      20102
#define LIBXSMM_X86_INSTR_VCVTPH2PS      20103
#define LIBXSMM_X86_INSTR_VPERMD         20104
#define LIBXSMM_X86_INSTR_VPMOVDW        20105
#define LIBXSMM_X86_INSTR_VPSRAD         20106
#define LIBXSMM_X86_INSTR_VPSLLD         20107
#define LIBXSMM_X86_INSTR_VPCMPB         20108
#define LIBXSMM_X86_INSTR_VPCMPD         20109
#define LIBXSMM_X86_INSTR_VPCMPW         20110
#define LIBXSMM_X86_INSTR_VPCMPUB        20111
#define LIBXSMM_X86_INSTR_VPCMPUD        20112
#define LIBXSMM_X86_INSTR_VPCMPUW        20113
#define LIBXSMM_X86_INSTR_VPORD          20114
#define LIBXSMM_X86_INSTR_VPSRLD         20115
#define LIBXSMM_X86_INSTR_VPERMT2W       20116
#define LIBXSMM_X86_INSTR_VPMOVSXWD      20117
#define LIBXSMM_X86_INSTR_VPMOVDB        20118
#define LIBXSMM_X86_INSTR_VPMOVSDB       20119
#define LIBXSMM_X86_INSTR_VPMOVUSDB      20120
#define LIBXSMM_X86_INSTR_VCVTPS2DQ      20121
#define LIBXSMM_X86_INSTR_VCVTPS2UDQ     20122
#define LIBXSMM_X86_INSTR_VPMOVZXWD      20123
#define LIBXSMM_X86_INSTR_VPMOVSXBD      20124
#define LIBXSMM_X86_INSTR_VPMOVZXBD      20125
#define LIBXSMM_X86_INSTR_VPBLENDMB      20126
#define LIBXSMM_X86_INSTR_VPBLENDMW      20127
#define LIBXSMM_X86_INSTR_VRCP14PS       20128
#define LIBXSMM_X86_INSTR_VPMAXSD        20129
#define LIBXSMM_X86_INSTR_VPMINSD        20130

/* AVX512, QUAD MADD, QUAD VNNI and VNNI */
#define LIBXSMM_X86_INSTR_V4FMADDPS      26000
#define LIBXSMM_X86_INSTR_V4FNMADDPS     26001
#define LIBXSMM_X86_INSTR_V4FMADDSS      26002
#define LIBXSMM_X86_INSTR_V4FNMADDSS     26003
#define LIBXSMM_X86_INSTR_VP4DPWSSD      26004
#define LIBXSMM_X86_INSTR_VP4DPWSSDS     26005
#define LIBXSMM_X86_INSTR_VPDPWSSD       26006
#define LIBXSMM_X86_INSTR_VPDPWSSDS      26007
#define LIBXSMM_X86_INSTR_VPDPBUSD       26008
#define LIBXSMM_X86_INSTR_VPDPBUSDS      26009

/* AVX512 BF16 */
#define LIBXSMM_X86_INSTR_VDPBF16PS      27000
#define LIBXSMM_X86_INSTR_VCVTNEPS2BF16  27001
#define LIBXSMM_X86_INSTR_VCVTNE2PS2BF16 27002

/* GP instructions */
#define LIBXSMM_X86_INSTR_ADDQ           30000
#define LIBXSMM_X86_INSTR_SUBQ           30001
#define LIBXSMM_X86_INSTR_MOVQ           30002
#define LIBXSMM_X86_INSTR_CMPQ           30003
#define LIBXSMM_X86_INSTR_JL             30004
#define LIBXSMM_X86_INSTR_VPREFETCH0     30005
#define LIBXSMM_X86_INSTR_VPREFETCH1     30006
#define LIBXSMM_X86_INSTR_PREFETCHT0     30007
#define LIBXSMM_X86_INSTR_PREFETCHT1     30008
#define LIBXSMM_X86_INSTR_PREFETCHT2     30009
#define LIBXSMM_X86_INSTR_PREFETCHNTA    30010
#define LIBXSMM_X86_INSTR_MOVL           30011
#define LIBXSMM_X86_INSTR_MOVSLQ         30012
#define LIBXSMM_X86_INSTR_SALQ           30013
#define LIBXSMM_X86_INSTR_IMUL           30014
#define LIBXSMM_X86_INSTR_CMOVZ          30015
#define LIBXSMM_X86_INSTR_CMOVNZ         30016
#define LIBXSMM_X86_INSTR_JE             30017
#define LIBXSMM_X86_INSTR_JZ             30018
#define LIBXSMM_X86_INSTR_JG             30019
#define LIBXSMM_X86_INSTR_JNE            30020
#define LIBXSMM_X86_INSTR_JNZ            30021
#define LIBXSMM_X86_INSTR_JGE            30022
#define LIBXSMM_X86_INSTR_JLE            30023
#define LIBXSMM_X86_INSTR_JMP            30024
#define LIBXSMM_X86_INSTR_POPCNT         30025
#define LIBXSMM_X86_INSTR_TZCNT          30026
#define LIBXSMM_X86_INSTR_LEAQ           30027
#define LIBXSMM_X86_INSTR_ANDQ           30028

/* Mask move instructions */
#define LIBXSMM_X86_INSTR_KMOV           40000
#define LIBXSMM_X86_INSTR_KMOVW          40001
#define LIBXSMM_X86_INSTR_KMOVB          40002
#define LIBXSMM_X86_INSTR_KMOVD          40003
#define LIBXSMM_X86_INSTR_KMOVQ          40004

/* Mask compute instructions */
#define LIBXSMM_X86_INSTR_KXNORW         45000

/* Tile instructions */
/* CPUID: AMX-TILE INTERCEPT: SPR */
#define LIBXSMM_X86_INSTR_LDTILECFG          50001
#define LIBXSMM_X86_INSTR_STTILECFG          50002
#define LIBXSMM_X86_INSTR_TILERELEASE        50003
#define LIBXSMM_X86_INSTR_TILELOADD          50004
#define LIBXSMM_X86_INSTR_TILELOADDT1        50005
#define LIBXSMM_X86_INSTR_TILESTORED         50006
#define LIBXSMM_X86_INSTR_TILEZERO           50007
/* CPUID: AMX-INT8 INTERCEPT: SPR */
#define LIBXSMM_X86_INSTR_TDPBSSD            50008
#define LIBXSMM_X86_INSTR_TDPBSUD            50009
#define LIBXSMM_X86_INSTR_TDPBUSD            50010
#define LIBXSMM_X86_INSTR_TDPBUUD            50011
/* CPUID: AMX-BF16 INTERCEPT: SPR */
#define LIBXSMM_X86_INSTR_TDPBF16PS          50012

/* define error codes */
#define LIBXSMM_ERR_GENERAL               90000
#define LIBXSMM_ERR_ALLOC                 90001
#define LIBXSMM_ERR_BUFFER_TOO_SMALL      90002
#define LIBXSMM_ERR_APPEND_STR            90003
#define LIBXSMM_ERR_ARCH_PREC             90004
#define LIBXSMM_ERR_ARCH                  90005
#define LIBXSMM_ERR_UNSUP_ARCH            90006
#define LIBXSMM_ERR_LDA                   90007
#define LIBXSMM_ERR_LDB                   90008
#define LIBXSMM_ERR_LDC                   90009
#define LIBXSMM_ERR_SPGEMM_GEN            90010
#define LIBXSMM_ERR_CSC_INPUT             90011
#define LIBXSMM_ERR_CSC_READ_LEN          90012
#define LIBXSMM_ERR_CSC_READ_DESC         90013
#define LIBXSMM_ERR_CSC_READ_ELEMS        90014
#define LIBXSMM_ERR_CSC_LEN               90015
#define LIBXSMM_ERR_N_BLOCK               90016
#define LIBXSMM_ERR_M_BLOCK               90017
#define LIBXSMM_ERR_K_BLOCK               90018
#define LIBXSMM_ERR_REG_BLOCK             90019
#define LIBXSMM_ERR_NO_AVX512_BCAST       90020
#define LIBXSMM_ERR_NO_AVX512_QFMA        90021
#define LIBXSMM_ERR_NO_INDEX_SCALE_ADDR   90022
#define LIBXSMM_ERR_UNSUPPORTED_JUMP      90023
#define LIBXSMM_ERR_NO_JMPLBL_AVAIL       90024
#define LIBXSMM_ERR_EXCEED_JMPLBL         90025
#define LIBXSMM_ERR_CSC_ALLOC_DATA        90026
#define LIBXSMM_ERR_CSR_ALLOC_DATA        90027
#define LIBXSMM_ERR_CSR_INPUT             90028
#define LIBXSMM_ERR_CSR_READ_LEN          90029
#define LIBXSMM_ERR_CSR_READ_DESC         90030
#define LIBXSMM_ERR_CSR_READ_ELEMS        90031
#define LIBXSMM_ERR_CSR_LEN               90032
#define LIBXSMM_ERR_UNSUP_DATATYPE        90033
#define LIBXSMM_ERR_UNSUP_DT_FORMAT       90034
#define LIBXSMM_ERR_INVALID_GEMM_CONFIG   90035
#define LIBXSMM_ERR_UNIQUE_VAL            90036
#define LIBXSMM_ERR_VEC_REG_MUST_BE_UNDEF 90037
#define LIBXSMM_ERR_JMPLBL_USED           90038
#define LIBXSMM_ERR_TRANS_B               90039
#define LIBXSMM_ERR_LDB_TRANS             90040
#define LIBXSMM_ERR_VNNI_A                90041
#define LIBXSMM_ERR_VNNI_B                90042
#define LIBXSMM_ERR_NO_AVX512VL           90043

#if defined(LIBXSMM_HANDLE_ERROR_QUIET)
# define LIBXSMM_HANDLE_ERROR(GENERATED_CODE, ERROR_CODE)
# define LIBXSMM_HANDLE_ERROR_VERBOSE(GENERATED_CODE, ERROR_CODE)
#else
# define LIBXSMM_HANDLE_ERROR(GENERATED_CODE, ERROR_CODE) libxsmm_handle_error( \
    GENERATED_CODE, ERROR_CODE, LIBXSMM_FUNCNAME, 1 < libxsmm_ninit ? libxsmm_verbosity : 1)
# define LIBXSMM_HANDLE_ERROR_VERBOSE(GENERATED_CODE, ERROR_CODE) libxsmm_handle_error( \
    GENERATED_CODE, ERROR_CODE, LIBXSMM_FUNCNAME, 1)
#endif

/* micro kernel configuration */
LIBXSMM_EXTERN_C typedef struct libxsmm_micro_kernel_config {
  unsigned int instruction_set;
  unsigned int vector_reg_count;
  unsigned int vector_length;
  unsigned int datatype_size;
  unsigned int a_vmove_instruction;
  unsigned int b_vmove_instruction;
  unsigned int b_shuff_instruction;
  unsigned int c_vmove_instruction;
  unsigned int c_vmove_nts_instruction;
  unsigned int use_masking_a_c;
  unsigned int prefetch_instruction;
  unsigned int vxor_instruction;
  unsigned int vmul_instruction;
  unsigned int vadd_instruction;
  unsigned int alu_add_instruction;
  unsigned int alu_sub_instruction;
  unsigned int alu_cmp_instruction;
  unsigned int alu_jmp_instruction;
  unsigned int alu_mov_instruction;
  char vector_name;

  /* Auxiliary variables for GEMM fusion info  */
  unsigned int fused_eltwise;
  unsigned int m_loop_exists;
  unsigned int n_loop_exists;
  unsigned int fused_bcolbias;
  unsigned int fused_scolbias;
  unsigned int fused_relu;
  unsigned int fused_sigmoid;
  unsigned int overwrite_C;
  unsigned int vnni_format_C;

  /* Register names/logistics for fusion boo-keeping  */
  unsigned int reserved_zmms;
  unsigned int reserved_mask_regs;
  unsigned int vnni_perm_reg;
  unsigned int zero_reg;
  unsigned int vec_x2;
  unsigned int vec_nom;
  unsigned int vec_denom;
  unsigned int vec_c0;
  unsigned int vec_c1;
  unsigned int vec_c2;
  unsigned int vec_c3;
  unsigned int vec_c1_d;
  unsigned int vec_c2_d;
  unsigned int vec_c3_d;
  unsigned int vec_hi_bound;
  unsigned int vec_lo_bound;
  unsigned int vec_ones;
  unsigned int vec_neg_ones;
  unsigned int vec_halves;
  unsigned int mask_hi;
  unsigned int mask_lo;

  /* Auxiliary arrays for micro-kernel iteration space traversal */
  int use_paired_tilestores;
  int _im[4];
  int _in[4];
  int _C_tile_id[4];
  int _C_tile_mate_id[4];
  int _im_offset_prefix_sums[4];
  int _in_offset_prefix_sums[4];
} libxsmm_micro_kernel_config;

/* structure for storing the current gp reg mapping */
LIBXSMM_EXTERN_C typedef struct libxsmm_gp_reg_mapping_struct {
  unsigned int gp_reg_a;
  unsigned int gp_reg_a_base;
  unsigned int gp_reg_b;
  unsigned int gp_reg_b_base;
  unsigned int gp_reg_c;
  unsigned int gp_reg_a_prefetch;
  unsigned int gp_reg_a_offset;
  unsigned int gp_reg_b_prefetch;
  unsigned int gp_reg_b_offset;
/*  unsigned int gp_reg_c_prefetch;*/
  unsigned int gp_reg_mloop;
  unsigned int gp_reg_nloop;
  unsigned int gp_reg_kloop;
  unsigned int gp_reg_reduce_count;
  unsigned int gp_reg_reduce_loop;
  unsigned int gp_reg_a_ptrs;
  unsigned int gp_reg_b_ptrs;
  unsigned int gp_reg_lda;
  unsigned int gp_reg_ldb;
  unsigned int gp_reg_ldc;
  unsigned int gp_reg_scf;
  unsigned int gp_reg_help_0;
  unsigned int gp_reg_help_1;
  unsigned int gp_reg_help_2;
  unsigned int gp_reg_help_3;
  unsigned int gp_reg_help_4;
  unsigned int gp_reg_help_5;
} libxsmm_gp_reg_mapping;

/* structure for storing the current gp reg mapping for matcopy */
LIBXSMM_EXTERN_C typedef struct libxsmm_matcopy_gp_reg_mapping_struct {
  unsigned int gp_reg_a;
  unsigned int gp_reg_lda;
  unsigned int gp_reg_b;
  unsigned int gp_reg_ldb;
  unsigned int gp_reg_a_pf;
  unsigned int gp_reg_b_pf;
  unsigned int gp_reg_m_loop;
  unsigned int gp_reg_n_loop;
  unsigned int gp_reg_help_0;
} libxsmm_matcopy_gp_reg_mapping;

/* matcopy kernel configuration */
LIBXSMM_EXTERN_C typedef struct libxsmm_matcopy_kernel_config_struct {
  unsigned int instruction_set;
  unsigned int vector_reg_count;
  unsigned int vector_length;
  unsigned int datatype_size;
  unsigned int prefetch_instruction;
  unsigned int vmove_instruction;
  unsigned int alu_add_instruction;
  unsigned int alu_cmp_instruction;
  unsigned int alu_jmp_instruction;
  unsigned int alu_mov_instruction;
  unsigned int vxor_instruction;
  char vector_name;
} libxsmm_matcopy_kernel_config;

/* structure for storing the current gp reg mapping for mateltwise */
LIBXSMM_EXTERN_C typedef struct libxsmm_mateltwise_gp_reg_mapping_struct {
  unsigned int gp_reg_param_struct;
  unsigned int gp_reg_in;
  unsigned int gp_reg_ldi;
  unsigned int gp_reg_out;
  unsigned int gp_reg_ldo;
  unsigned int gp_reg_relumask;
  unsigned int gp_reg_reduced_elts;
  unsigned int gp_reg_reduced_elts_squared;
  unsigned int gp_reg_scale_vals;
  unsigned int gp_reg_shift_vals;
  unsigned int gp_reg_bias_vals;
  unsigned int gp_reg_m_loop;
  unsigned int gp_reg_n_loop;
} libxsmm_mateltwise_gp_reg_mapping;

/* mateltwise kernel configuration */
LIBXSMM_EXTERN_C typedef struct libxsmm_mateltwise_kernel_config_struct {
  unsigned int instruction_set;
  unsigned int vector_reg_count;
  unsigned int vector_length_in;
  unsigned int vector_length_out;
  unsigned int datatype_size_in;
  unsigned int datatype_size_out;
  unsigned int vmove_instruction_in;
  unsigned int vmove_instruction_out;
  unsigned int alu_add_instruction;
  unsigned int alu_cmp_instruction;
  unsigned int alu_jmp_instruction;
  unsigned int alu_mov_instruction;
  unsigned int vxor_instruction;
  char vector_name;
} libxsmm_mateltwise_kernel_config;

/* structure for storing the current gp reg mapping for transpose */
LIBXSMM_EXTERN_C typedef struct libxsmm_transpose_gp_reg_mapping_struct {
  unsigned int gp_reg_a;
  unsigned int gp_reg_lda;
  unsigned int gp_reg_b;
  unsigned int gp_reg_ldb;
  unsigned int gp_reg_m_loop;
  unsigned int gp_reg_n_loop;
  unsigned int gp_reg_help_0;
  unsigned int gp_reg_help_1;
  unsigned int gp_reg_help_2;
  unsigned int gp_reg_help_3;
  unsigned int gp_reg_help_4;
  unsigned int gp_reg_help_5;
} libxsmm_transpose_gp_reg_mapping;

/* transpose kernel configuration */
LIBXSMM_EXTERN_C typedef struct libxsmm_transpose_kernel_config_struct {
  unsigned int instruction_set;
  unsigned int vector_reg_count;
  char vector_name;
} libxsmm_transpose_kernel_config;

/* structure for tracking local labels in assembly we don't allow overlapping loops */
LIBXSMM_EXTERN_C typedef struct libxsmm_loop_label_tracker_struct {
  unsigned int label_address[512];
  unsigned int label_count;
} libxsmm_loop_label_tracker;

/* structure to save jump properties to the same destination */
LIBXSMM_EXTERN_C typedef struct libxsmm_jump_source_struct {
  unsigned int instr_type[512];
  unsigned int instr_addr[512];
  unsigned int ref_count;
} libxsmm_jump_source;

/* structure for tracking arbitrary jump labels in assembly code */
LIBXSMM_EXTERN_C typedef struct libxsmm_jump_label_tracker_struct {
  unsigned int        label_address[512];
  libxsmm_jump_source label_source[512];
} libxsmm_jump_label_tracker;

LIBXSMM_EXTERN_C typedef struct libxsmm_blocking_info_t {
  unsigned int tiles;
  unsigned int sizes[4];
  unsigned int blocking;
  unsigned int block_size;
} libxsmm_blocking_info_t;

/* Auxiliary stach variable enumeration in GEMM */
typedef enum libxsmm_gemm_stack_var {
  LIBXSMM_GEMM_STACK_VAR_NONE               =  0,
  LIBXSMM_GEMM_STACK_VAR_PFA_PTR            =  1,
  LIBXSMM_GEMM_STACK_VAR_PFB_PTR            =  2,
  LIBXSMM_GEMM_STACK_VAR_A_OFFS_BRGEMM_PTR  =  3,
  LIBXSMM_GEMM_STACK_VAR_B_OFFS_BRGEMM_PTR  =  4,
  LIBXSMM_GEMM_STACK_VAR_INT8_SCF           =  5,
  LIBXSMM_GEMM_STACK_VAR_GEMM_SCRATCH_PTR   =  6,
  LIBXSMM_GEMM_STACK_VAR_ELT_BIAS_PTR       =  7,
  LIBXSMM_GEMM_STACK_VAR_ELT_OUTPUT_PTR     =  8,
  LIBXSMM_GEMM_STACK_VAR_ARG_7              =  9,
  LIBXSMM_GEMM_STACK_VAR_ARG_8              = 10,
  LIBXSMM_GEMM_STACK_VAR_ARG_9              = 11,
  LIBXSMM_GEMM_STACK_VAR_ARG_10             = 12
} libxsmm_gemm_stack_var;

/* compressed meltw reduce structure */
typedef enum libxsmm_meltw_comp_redu_flags {
  LIBXSMM_MELTW_COMP_FLAG_REDUCE_NONE         = 0,
  LIBXSMM_MELTW_COMP_FLAG_REDUCE_OP_ADD       = 1,
  LIBXSMM_MELTW_COMP_FLAG_REDUCE_OP_MAX       = 2,
  LIBXSMM_MELTW_COMP_FLAG_REDUCE_OP_MUL       = 3,
  LIBXSMM_MELTW_COMP_FLAG_REDUCE_ROWS         = 4,
  LIBXSMM_MELTW_COMP_FLAG_REDUCE_COLS         = 5,
  LIBXSMM_MELTW_COMP_FLAG_REDUCE_ELTS         = 6,
  LIBXSMM_MELTW_COMP_FLAG_REDUCE_ELTS_SQUARED = 7,
  LIBXSMM_MELTW_COMP_FLAG_REDUCE_OP_ADD_ROWS  = 8,
  LIBXSMM_MELTW_COMP_FLAG_REDUCE_OP_ADD_COLS  = 9
} libxsmm_meltw_comp_redu_flags;

/* compressed meltw scale structure */
typedef enum libxsmm_meltw_comp_scal_flags {
  LIBXSMM_MELTW_COMP_FLAG_SCALE_NONE                     = 0,
  LIBXSMM_MELTW_COMP_FLAG_SCALE_MULT                     = 1,
  LIBXSMM_MELTW_COMP_FLAG_SCALE_SHIFT                    = 2,
  LIBXSMM_MELTW_COMP_FLAG_SCALE_ADD_BIAS                 = 3,
  LIBXSMM_MELTW_COMP_FLAG_SCALE_ROWS                     = 4,
  LIBXSMM_MELTW_COMP_FLAG_SCALE_COLS                     = 5,
  LIBXSMM_MELTW_COMP_FLAG_SCALE_MULT_ROWS                = 6,
  LIBXSMM_MELTW_COMP_FLAG_SCALE_SHIFT_ROWS               = 7,
  LIBXSMM_MELTW_COMP_FLAG_SCALE_ADD_BIAS_ROWS            = 8,
  LIBXSMM_MELTW_COMP_FLAG_SCALE_MULT_SHIFT_ROWS          = 9,
  LIBXSMM_MELTW_COMP_FLAG_SCALE_ADD_BIAS_SHIFT_ROWS      = 10,
  LIBXSMM_MELTW_COMP_FLAG_SCALE_MULT_ADD_BIAS_ROWS       = 11,
  LIBXSMM_MELTW_COMP_FLAG_SCALE_MULT_SHIFT_ADD_BIAS_ROWS = 12,
  LIBXSMM_MELTW_COMP_FLAG_SCALE_MULT_COLS                = 13,
  LIBXSMM_MELTW_COMP_FLAG_SCALE_SHIFT_COLS               = 14,
  LIBXSMM_MELTW_COMP_FLAG_SCALE_ADD_BIAS_COLS            = 15,
  LIBXSMM_MELTW_COMP_FLAG_SCALE_MULT_SHIFT_COLS          = 16,
  LIBXSMM_MELTW_COMP_FLAG_SCALE_ADD_BIAS_SHIFT_COLS      = 17,
  LIBXSMM_MELTW_COMP_FLAG_SCALE_MULT_ADD_BIAS_COLS       = 18,
  LIBXSMM_MELTW_COMP_FLAG_SCALE_MULT_SHIFT_ADD_BIAS_COLS = 19
} libxsmm_meltw_comp_scal_flags;

/* compressed metlw cvta strcuture */
typedef enum libxsmm_meltw_comp_cvta_flags {
  LIBXSMM_MELTW_COMP_FLAG_CVTA_NONE           = 0,
  LIBXSMM_MELTW_COMP_FLAG_CVTA_FUSE_RELU      = 1,
  LIBXSMM_MELTW_COMP_FLAG_CVTA_FUSE_TANH      = 2,
  LIBXSMM_MELTW_COMP_FLAG_CVTA_FUSE_SIGM      = 3
} libxsmm_meltw_comp_cvta_flags;

/* compressed meltw acvt structure */
typedef enum libxsmm_meltw_comp_acvt_flags {
  LIBXSMM_MELTW_COMP_FLAG_ACVT_NONE           = 0,
  LIBXSMM_MELTW_COMP_FLAG_ACVT_FUSE_TANH      = 1,
  LIBXSMM_MELTW_COMP_FLAG_ACVT_FUSE_SIGM      = 2
} libxsmm_meltw_comp_acvt_flags;

/* compressed meltw cbiasact strcuture */
typedef enum libxsmm_meltw_comp_cbiasact_flags {
  LIBXSMM_MELTW_COMP_FLAG_CBIASACT_NONE                         =  0,
  LIBXSMM_MELTW_COMP_FLAG_CBIASACT_COLBIAS                      =  1,
  LIBXSMM_MELTW_COMP_FLAG_CBIASACT_ACT_RELU                     =  2,
  LIBXSMM_MELTW_COMP_FLAG_CBIASACT_ACT_TANH                     =  3,
  LIBXSMM_MELTW_COMP_FLAG_CBIASACT_ACT_SIGM                     =  4,
  LIBXSMM_MELTW_COMP_FLAG_CBIASACT_ACT_GELU                     =  5,
  LIBXSMM_MELTW_COMP_FLAG_CBIASACT_OVERWRITE_C                  =  6,
  LIBXSMM_MELTW_COMP_FLAG_CBIASACT_COLBIAS_ACT_RELU             =  7,
  LIBXSMM_MELTW_COMP_FLAG_CBIASACT_COLBIAS_ACT_TANH             =  8,
  LIBXSMM_MELTW_COMP_FLAG_CBIASACT_COLBIAS_ACT_SIGM             =  9,
  LIBXSMM_MELTW_COMP_FLAG_CBIASACT_COLBIAS_ACT_GELU             = 10,
  LIBXSMM_MELTW_COMP_FLAG_CBIASACT_COLBIAS_ACT_RELU_OVERWRITE_C = 11,
  LIBXSMM_MELTW_COMP_FLAG_CBIASACT_COLBIAS_ACT_TANH_OVERWRITE_C = 12,
  LIBXSMM_MELTW_COMP_FLAG_CBIASACT_COLBIAS_ACT_SIGM_OVERWRITE_C = 13,
  LIBXSMM_MELTW_COMP_FLAG_CBIASACT_COLBIAS_ACT_GELU_OVERWRITE_C = 14,
  LIBXSMM_MELTW_COMP_FLAG_CBIASACT_COLBIAS_OVERWRITE_C          = 15,
  LIBXSMM_MELTW_COMP_FLAG_CBIASACT_ACT_RELU_OVERWRITE_C         = 16,
  LIBXSMM_MELTW_COMP_FLAG_CBIASACT_ACT_TANH_OVERWRITE_C         = 17,
  LIBXSMM_MELTW_COMP_FLAG_CBIASACT_ACT_SIGM_OVERWRITE_C         = 18,
  LIBXSMM_MELTW_COMP_FLAG_CBIASACT_ACT_GELU_OVERWRITE_C         = 19
} libxsmm_meltw_comp_cbiasact_flags;

LIBXSMM_API_INTERN
void libxsmm_reset_loop_label_tracker( libxsmm_loop_label_tracker* io_loop_label_tracker );

LIBXSMM_API_INTERN
void libxsmm_reset_jump_label_tracker( libxsmm_jump_label_tracker* io_jump_lable_tracker );

LIBXSMM_API_INTERN
void libxsmm_get_x86_gp_reg_name( const unsigned int i_gp_reg_number,
                                  char*              o_gp_reg_name,
                                  const int          i_gp_reg_name_max_length );

LIBXSMM_API_INTERN
unsigned int libxsmm_check_x86_gp_reg_callee_save( const unsigned int i_gp_reg_number );

LIBXSMM_API_INTERN
void libxsmm_get_x86_instr_name( const unsigned int i_instr_number,
                                 char*              o_instr_name,
                                 const int          i_instr_name_max_length );

LIBXSMM_API_INTERN
void libxsmm_reset_x86_gp_reg_mapping( libxsmm_gp_reg_mapping* io_gp_reg_mapping );

LIBXSMM_API_INTERN
unsigned int libxsmm_is_x86_vec_instr_single_precision( const unsigned int i_instr_number );

/* some string manipulation helper needed to generated code */
LIBXSMM_API_INTERN
void libxsmm_append_code_as_string( libxsmm_generated_code* io_generated_code,
                                    const char*             i_code_to_append,
                                    const int               i_append_length );

LIBXSMM_API_INTERN
void libxsmm_close_function( libxsmm_generated_code* io_generated_code );

LIBXSMM_API_INTERN
void libxsmm_mmfunction_signature( libxsmm_generated_code*       io_generated_code,
                                  const char*                    i_routine_name,
                                  const libxsmm_gemm_descriptor* i_xgemm_desc );

LIBXSMM_API_INTERN
void libxsmm_generator_isa_check_header( libxsmm_generated_code* io_generated_code );

LIBXSMM_API_INTERN
void libxsmm_generator_isa_check_footer( libxsmm_generated_code* io_generated_code );

LIBXSMM_API_INTERN
void libxsmm_handle_error( libxsmm_generated_code* io_generated_code,
                           const unsigned int      i_error_code,
                           const char*             context,
                           int emit_message );

LIBXSMM_API_INTERN unsigned int libxsmm_compute_equalized_blocking(
  unsigned int i_size, unsigned int i_max_block,
  unsigned int* o_range_1, unsigned int* o_block_1,
  unsigned int* o_range_2, unsigned int* o_block_2 );

/** helper functions for compressing and decompressing meltw flags */
LIBXSMM_API_INTERN libxsmm_meltw_comp_redu_flags libxsmm_get_meltw_comp_redu_flags( libxsmm_meltw_redu_flags flags );
LIBXSMM_API_INTERN libxsmm_meltw_redu_flags libxsmm_get_meltw_redu_flags( libxsmm_meltw_comp_redu_flags flags );
LIBXSMM_API_INTERN libxsmm_meltw_comp_scal_flags libxsmm_get_meltw_comp_scal_flags( libxsmm_meltw_scal_flags flags );
LIBXSMM_API_INTERN libxsmm_meltw_scal_flags libxsmm_get_meltw_scal_flags( libxsmm_meltw_comp_scal_flags flags );
LIBXSMM_API_INTERN libxsmm_meltw_comp_cvta_flags libxsmm_get_meltw_comp_cvta_flags( libxsmm_meltw_cvta_flags flags );
LIBXSMM_API_INTERN libxsmm_meltw_cvta_flags libxsmm_get_meltw_cvta_flags( libxsmm_meltw_comp_cvta_flags flags );
LIBXSMM_API_INTERN libxsmm_meltw_comp_acvt_flags libxsmm_get_meltw_comp_acvt_flags( libxsmm_meltw_acvt_flags flags );
LIBXSMM_API_INTERN libxsmm_meltw_acvt_flags libxsmm_get_meltw_acvt_flags( libxsmm_meltw_comp_acvt_flags flags );
LIBXSMM_API_INTERN libxsmm_meltw_comp_cbiasact_flags libxsmm_get_meltw_comp_cbiasact_flags( libxsmm_meltw_cbiasact_flags flags );
LIBXSMM_API_INTERN libxsmm_meltw_cbiasact_flags libxsmm_get_meltw_cbiasact_flags( libxsmm_meltw_comp_cbiasact_flags flags );

#endif /* GENERATOR_COMMON_H */

