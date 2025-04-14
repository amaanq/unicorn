/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (c) 2021 Loongson Technology Corporation Limited
 *
 * Helpers for IOCSR reads/writes
 */

#include "qemu/osdep.h"
#include "cpu.h"
#include "qemu/host-utils.h"
#include "exec/helper-proto.h"
#include "exec/exec-all.h"
#include "exec/cpu_ldst.h"

uint64_t helper_iocsrrd_b(CPULoongArchState *env, target_ulong r_addr)
{
    return glue(address_space_ldub, UNICORN_ARCH_POSTFIX)(env->uc, &env->address_space_iocsr, r_addr,
                              MEMTXATTRS_UNSPECIFIED, NULL);
}

uint64_t helper_iocsrrd_h(CPULoongArchState *env, target_ulong r_addr)
{
    return glue(address_space_lduw, UNICORN_ARCH_POSTFIX)(env->uc, &env->address_space_iocsr, r_addr,
                              MEMTXATTRS_UNSPECIFIED, NULL);
}

uint64_t helper_iocsrrd_w(CPULoongArchState *env, target_ulong r_addr)
{
    return glue(address_space_ldl, UNICORN_ARCH_POSTFIX)(env->uc, &env->address_space_iocsr, r_addr,
                             MEMTXATTRS_UNSPECIFIED, NULL);
}

uint64_t helper_iocsrrd_d(CPULoongArchState *env, target_ulong r_addr)
{
    return glue(address_space_ldq, UNICORN_ARCH_POSTFIX)(env->uc, &env->address_space_iocsr, r_addr,
                             MEMTXATTRS_UNSPECIFIED, NULL);
}

void helper_iocsrwr_b(CPULoongArchState *env, target_ulong w_addr,
                      target_ulong val)
{
    glue(address_space_stb, UNICORN_ARCH_POSTFIX)(env->uc, &env->address_space_iocsr, w_addr,
                      val, MEMTXATTRS_UNSPECIFIED, NULL);
}

void helper_iocsrwr_h(CPULoongArchState *env, target_ulong w_addr,
                      target_ulong val)
{
    glue(address_space_stw, UNICORN_ARCH_POSTFIX)(env->uc, &env->address_space_iocsr, w_addr,
                      val, MEMTXATTRS_UNSPECIFIED, NULL);
}

void helper_iocsrwr_w(CPULoongArchState *env, target_ulong w_addr,
                      target_ulong val)
{
    glue(address_space_stl, UNICORN_ARCH_POSTFIX)(env->uc, &env->address_space_iocsr, w_addr,
                      val, MEMTXATTRS_UNSPECIFIED, NULL);
}

void helper_iocsrwr_d(CPULoongArchState *env, target_ulong w_addr,
                      target_ulong val)
{
    glue(address_space_stq, UNICORN_ARCH_POSTFIX)(env->uc, &env->address_space_iocsr, w_addr,
                      val, MEMTXATTRS_UNSPECIFIED, NULL);
}
