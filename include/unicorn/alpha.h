/* Unicorn Engine */
/* By Nguyen Anh Quynh <aquynh@gmail.com>, 2015-2017 */
/* This file is released under LGPL2.
   See COPYING.LGPL2 in root directory for more details
*/

/* Created for Unicorn Engine by Amaan Qureshi <amaanq12@gmail.com> */

#ifndef UNICORN_ALPHA_H
#define UNICORN_ALPHA_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#pragma warning(disable : 4201)
#endif

//> Alpha CPU
typedef enum uc_cpu_alpha {
    UC_CPU_ALPHA_EV4 = 0,
    UC_CPU_ALPHA_EV5,
    UC_CPU_ALPHA_EV56,
    UC_CPU_ALPHA_PCA56,
    UC_CPU_ALPHA_EV6,
    UC_CPU_ALPHA_EV67,
    UC_CPU_ALPHA_EV68,

	UC_CPU_ALPHA_ENDING,
} uc_cpu_alpha;

//> Alpha registers
typedef enum uc_alpha_reg {
    UC_ALPHA_REG_INVALID = 0,

    //> General purpose registers
    UC_ALPHA_REG_R0,
    UC_ALPHA_REG_R1,
    UC_ALPHA_REG_R2,
    UC_ALPHA_REG_R3,
    UC_ALPHA_REG_R4,
    UC_ALPHA_REG_R5,
    UC_ALPHA_REG_R6,
    UC_ALPHA_REG_R7,
    UC_ALPHA_REG_R8,
    UC_ALPHA_REG_R9,
    UC_ALPHA_REG_R10,
    UC_ALPHA_REG_R11,
    UC_ALPHA_REG_R12,
    UC_ALPHA_REG_R13,
    UC_ALPHA_REG_R14,
    UC_ALPHA_REG_R15,
    UC_ALPHA_REG_R16,
    UC_ALPHA_REG_R17,
    UC_ALPHA_REG_R18,
    UC_ALPHA_REG_R19,
    UC_ALPHA_REG_R20,
    UC_ALPHA_REG_R21,
    UC_ALPHA_REG_R22,
    UC_ALPHA_REG_R23,
    UC_ALPHA_REG_R24,
    UC_ALPHA_REG_R25,
    UC_ALPHA_REG_R26,
    UC_ALPHA_REG_R27,
    UC_ALPHA_REG_R28,
    UC_ALPHA_REG_R29,
    UC_ALPHA_REG_R30,
    UC_ALPHA_REG_R31,

    //> Floating point registers
    UC_ALPHA_REG_F0,
    UC_ALPHA_REG_F1,
    UC_ALPHA_REG_F2,
    UC_ALPHA_REG_F3,
    UC_ALPHA_REG_F4,
    UC_ALPHA_REG_F5,
    UC_ALPHA_REG_F6,
    UC_ALPHA_REG_F7,
    UC_ALPHA_REG_F8,
    UC_ALPHA_REG_F9,
    UC_ALPHA_REG_F10,
    UC_ALPHA_REG_F11,
    UC_ALPHA_REG_F12,
    UC_ALPHA_REG_F13,
    UC_ALPHA_REG_F14,
    UC_ALPHA_REG_F15,
    UC_ALPHA_REG_F16,
    UC_ALPHA_REG_F17,
    UC_ALPHA_REG_F18,
    UC_ALPHA_REG_F19,
    UC_ALPHA_REG_F20,
    UC_ALPHA_REG_F21,
    UC_ALPHA_REG_F22,
    UC_ALPHA_REG_F23,
    UC_ALPHA_REG_F24,
    UC_ALPHA_REG_F25,
    UC_ALPHA_REG_F26,
    UC_ALPHA_REG_F27,
    UC_ALPHA_REG_F28,
    UC_ALPHA_REG_F29,
    UC_ALPHA_REG_F30,
    UC_ALPHA_REG_F31,

    UC_ALPHA_REG_PC,

    //> Control registers
    UC_ALPHA_REG_LR0,
    UC_ALPHA_REG_LR1,
    UC_ALPHA_REG_FPCR,
	UC_ALPHA_REG_PTBR,

    //> Alias registers
    UC_ALPHA_REG_ZERO = UC_ALPHA_REG_R31,
    UC_ALPHA_REG_FZERO = UC_ALPHA_REG_F31,
    UC_ALPHA_REG_LOCK_ADDR = UC_ALPHA_REG_LR0,
    UC_ALPHA_REG_LOCK_VALUE = UC_ALPHA_REG_LR1,

    UC_ALPHA_REG_ENDING,
} uc_alpha_reg;

#endif
