/*
 * QEMU Alpha CPU
 *
 * Copyright (c) 2007 Jocelyn Mayer
 * Copyright (c) 2012 SUSE LINUX Products GmbH
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see
 * <http://www.gnu.org/licenses/lgpl-2.1.html>
 */

#include "cpu.h"
#include "exec/exec-all.h"


static void alpha_cpu_set_pc(CPUState *cs, vaddr value)
{
    AlphaCPU *cpu = ALPHA_CPU(cs);

    cpu->env.pc = value;
}

static bool alpha_cpu_has_work(CPUState *cs)
{
    /* Here we are checking to see if the CPU should wake up from HALT.
       We will have gotten into this state only for WTINT from PALmode.  */
    /* ??? I'm not sure how the IPL state works with WTINT to keep a CPU
       asleep even if (some) interrupts have been asserted.  For now,
       assume that if a CPU really wants to stay asleep, it will mask
       interrupts at the chipset level, which will prevent these bits
       from being set in the first place.  */
    return cs->interrupt_request & (CPU_INTERRUPT_HARD
                                    | CPU_INTERRUPT_TIMER
                                    | CPU_INTERRUPT_SMP
                                    | CPU_INTERRUPT_MCHK);
}

static void alpha_cpu_realizefn(CPUState *dev)
{
    CPUState *cs = CPU(dev);

    cpu_exec_realizefn(cs);
}

static void ev4_cpu_initfn(CPUState *obj)
{
    AlphaCPU *cpu = ALPHA_CPU(obj);
    CPUAlphaState *env = &cpu->env;

    env->implver = IMPLVER_2106x;
}

static void ev5_cpu_initfn(CPUState *obj)
{
    AlphaCPU *cpu = ALPHA_CPU(obj);
    CPUAlphaState *env = &cpu->env;

    env->implver = IMPLVER_21164;
}

static void ev56_cpu_initfn(CPUState *obj)
{
    AlphaCPU *cpu = ALPHA_CPU(obj);
    CPUAlphaState *env = &cpu->env;

    env->amask |= AMASK_BWX;
}

static void pca56_cpu_initfn(CPUState *obj)
{
    AlphaCPU *cpu = ALPHA_CPU(obj);
    CPUAlphaState *env = &cpu->env;

    env->amask |= AMASK_MVI;
}

static void ev6_cpu_initfn(CPUState *obj)
{
    AlphaCPU *cpu = ALPHA_CPU(obj);
    CPUAlphaState *env = &cpu->env;

    env->implver = IMPLVER_21264;
    env->amask = AMASK_BWX | AMASK_FIX | AMASK_MVI | AMASK_TRAP;
}

static void ev67_cpu_initfn(CPUState *obj)
{
    AlphaCPU *cpu = ALPHA_CPU(obj);
    CPUAlphaState *env = &cpu->env;

    env->amask |= AMASK_CIX | AMASK_PREFETCH;
}

static void alpha_cpu_initfn(struct uc_struct *uc, CPUState *obj)
{
    AlphaCPU *cpu = ALPHA_CPU(obj);
    CPUAlphaState *env = &cpu->env;

    env->uc = uc;
    cpu_set_cpustate_pointers(cpu);

    env->lock_addr = -1;
    env->flags = ENV_FLAG_PAL_MODE | ENV_FLAG_FEN;
}

static void alpha_cpu_class_init(CPUClass *oc)
{
    CPUClass *cc = CPU_CLASS(oc);

    cc->has_work = alpha_cpu_has_work;
    cc->do_interrupt = alpha_cpu_do_interrupt;
    cc->cpu_exec_interrupt = alpha_cpu_exec_interrupt;
    cc->set_pc = alpha_cpu_set_pc;
    cc->tlb_fill = alpha_cpu_tlb_fill;
    cc->do_unaligned_access = alpha_cpu_do_unaligned_access;
    cc->get_phys_page_debug = alpha_cpu_get_phys_page_debug;
    cc->tcg_initialize = alpha_translate_init;
}

struct AlphaCPUInfo {
    const char *name;
    void (*initfn)(CPUState *obj);
};

static const struct AlphaCPUInfo alpha_cpu_type_infos[] = {
    {"ev4", ev4_cpu_initfn},
    {"ev5", ev5_cpu_initfn},
    {"ev56", ev56_cpu_initfn},
    {"pca56", pca56_cpu_initfn},
    {"ev6", ev6_cpu_initfn},
    {"ev67", ev67_cpu_initfn},
    {"ev68", NULL},
};

AlphaCPU *cpu_alpha_init(struct uc_struct *uc) {
    AlphaCPU *cpu;
    CPUState *cs;
    CPUClass *cc;

    cpu = qemu_memalign(8, sizeof(*cpu));
    if (cpu == NULL) {
        return NULL;
    }
    memset(cpu, 0, sizeof(*cpu));

    if (uc->cpu_model == INT_MAX) {
        uc->cpu_model = UC_CPU_ALPHA_EV68;
    } else if (uc->cpu_model >= ARRAY_SIZE(alpha_cpu_type_infos)) {
        free(cpu);
        return NULL;
    }

    cs = (CPUState *)cpu;
    cc = (CPUClass *)&cpu->cc;
    cs->cc = cc;
    cs->uc = uc;
    uc->cpu = cs;

    cpu_class_init(uc, cc);

    alpha_cpu_class_init(cc);

    cpu_common_initfn(uc, cs);

    alpha_cpu_initfn(uc, cs);

    if (alpha_cpu_type_infos[uc->cpu_model].initfn) {
        alpha_cpu_type_infos[uc->cpu_model].initfn(cs);
    }

    alpha_cpu_realizefn(cs);

    // init addres space
    cpu_address_space_init(cs, 0, cs->memory);

    qemu_init_vcpu(cs);

    return cpu;
}
