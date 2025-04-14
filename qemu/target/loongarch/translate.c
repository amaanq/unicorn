/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * LoongArch emulation for QEMU - main translation routines.
 *
 * Copyright (c) 2021 Loongson Technology Corporation Limited
 */

#include "qemu/osdep.h"
#include "cpu.h"
#include "tcg/tcg-op.h"
#include "exec/translator.h"
#include "exec/helper-proto.h"
#include "exec/helper-gen.h"

#include "exec/translator.h"
#include "fpu/softfloat.h"
#include "translate.h"
#include "internals.h"

// /* Global register indices */
// TCGv tcg_ctx->cpu_gpr[32], tcg_ctx->cpu_pc;
// static TCGv tcg_ctx->cpu_lladdr, tcg_ctx->cpu_llval;
// TCGv_i64 tcg_ctx->cpu_fpr[32];

#include "exec/gen-icount.h"

#define DISAS_STOP        DISAS_TARGET_0
#define DISAS_EXIT        DISAS_TARGET_1
#define DISAS_EXIT_UPDATE DISAS_TARGET_2

static inline int plus_1(DisasContext *ctx, int x)
{
    return x + 1;
}

static inline int shl_2(DisasContext *ctx, int x)
{
    return x << 2;
}

/*
 * LoongArch the upper 32 bits are undefined ("can be any value").
 * QEMU chooses to nanbox, because it is most likely to show guest bugs early.
 */
static void gen_nanbox_s(TCGContext *tcg_ctx, TCGv_i64 out, TCGv_i64 in)
{
    tcg_gen_ori_i64(tcg_ctx, out, in, MAKE_64BIT_MASK(32, 32));
}

void generate_exception(DisasContext *ctx, int excp)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    tcg_gen_movi_tl(tcg_ctx, tcg_ctx->cpu_pc, ctx->base.pc_next);
    gen_helper_raise_exception(tcg_ctx, tcg_ctx->cpu_env, tcg_constant_i32(tcg_ctx, excp));
    ctx->base.is_jmp = DISAS_NORETURN;
}

static inline void gen_goto_tb(DisasContext *ctx, int n, target_ulong dest)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    // if (translator_use_goto_tb(&ctx->base, dest)) {
    //     tcg_gen_goto_tb(tcg_ctx, n);
    //     tcg_gen_movi_tl(tcg_ctx, tcg_ctx->cpu_pc, dest);
    //     tcg_gen_exit_tb(tcg_ctx, ctx->base.tb, n);
    // } else {
    tcg_gen_movi_tl(tcg_ctx, tcg_ctx->cpu_pc, dest);
    tcg_gen_lookup_and_goto_ptr(tcg_ctx);
    // }
}

static void loongarch_tr_init_disas_context(DisasContextBase *dcbase,
                                            CPUState *cs)
{
    int64_t bound;
    DisasContext *ctx = container_of(dcbase, DisasContext, base);
    struct uc_struct *uc = cs->uc;
    TCGContext *tcg_ctx = uc->tcg_ctx;

    // unicorn setup
    ctx->uc = cs->uc;

    ctx->page_start = ctx->base.pc_first & TARGET_PAGE_MASK;
    ctx->mem_idx = ctx->base.tb->flags;

    /* Bound the number of insns to execute to those left on the page.  */
    bound = -(ctx->base.pc_first | TARGET_PAGE_MASK) / 4;
    ctx->base.max_insns = MIN(ctx->base.max_insns, bound);

    ctx->ntemp = 0;
    memset(ctx->temp, 0, sizeof(ctx->temp));

    ctx->zero = tcg_constant_tl(tcg_ctx, 0);
}

static void loongarch_tr_tb_start(DisasContextBase *dcbase, CPUState *cs)
{
}

static void loongarch_tr_insn_start(DisasContextBase *dcbase, CPUState *cs)
{
    DisasContext *ctx = container_of(dcbase, DisasContext, base);
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;

    tcg_gen_insn_start(tcg_ctx, ctx->base.pc_next);
}

/*
 * Wrappers for getting reg values.
 *
 * The $zero register does not have tcg_ctx->cpu_gpr[0] allocated -- we supply the
 * constant zero as a source, and an uninitialized sink as destination.
 *
 * Further, we may provide an extension for word operations.
 */
static TCGv temp_new(DisasContext *ctx)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    assert(ctx->ntemp < ARRAY_SIZE(ctx->temp));
    return ctx->temp[ctx->ntemp++] = tcg_temp_new(tcg_ctx);
}

static TCGv gpr_src(DisasContext *ctx, int reg_num, DisasExtend src_ext)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    TCGv t;

    if (reg_num == 0) {
        return ctx->zero;
    }

    switch (src_ext) {
    case EXT_NONE:
        return tcg_ctx->cpu_gpr[reg_num];
    case EXT_SIGN:
        t = temp_new(ctx);
        tcg_gen_ext32s_tl(tcg_ctx, t, tcg_ctx->cpu_gpr[reg_num]);
        return t;
    case EXT_ZERO:
        t = temp_new(ctx);
        tcg_gen_ext32u_tl(tcg_ctx, t, tcg_ctx->cpu_gpr[reg_num]);
        return t;
    }
    g_assert_not_reached();
}

static TCGv gpr_dst(DisasContext *ctx, int reg_num, DisasExtend dst_ext)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    if (reg_num == 0 || dst_ext) {
        return temp_new(ctx);
    }
    return tcg_ctx->cpu_gpr[reg_num];
}

static void gen_set_gpr(TCGContext *tcg_ctx, int reg_num, TCGv t, DisasExtend dst_ext)
{
    if (reg_num != 0) {
        switch (dst_ext) {
        case EXT_NONE:
            tcg_gen_mov_tl(tcg_ctx, tcg_ctx->cpu_gpr[reg_num], t);
            break;
        case EXT_SIGN:
            tcg_gen_ext32s_tl(tcg_ctx, tcg_ctx->cpu_gpr[reg_num], t);
            break;
        case EXT_ZERO:
            tcg_gen_ext32u_tl(tcg_ctx, tcg_ctx->cpu_gpr[reg_num], t);
            break;
        default:
            g_assert_not_reached();
        }
    }
}

#include "decode-insns.inc.c"
#include "insn_trans/trans_arith.inc.c"
#include "insn_trans/trans_shift.inc.c"
#include "insn_trans/trans_bit.inc.c"
#include "insn_trans/trans_memory.inc.c"
#include "insn_trans/trans_atomic.inc.c"
#include "insn_trans/trans_extra.inc.c"
#include "insn_trans/trans_farith.inc.c"
#include "insn_trans/trans_fcmp.inc.c"
#include "insn_trans/trans_fcnv.inc.c"
#include "insn_trans/trans_fmov.inc.c"
#include "insn_trans/trans_fmemory.inc.c"
#include "insn_trans/trans_branch.inc.c"
#include "insn_trans/trans_privileged.inc.c"

static void loongarch_tr_translate_insn(DisasContextBase *dcbase, CPUState *cs)
{
    CPULoongArchState *env = cs->env_ptr;
    DisasContext *ctx = container_of(dcbase, DisasContext, base);
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;

    ctx->opcode = cpu_ldl_code(env, ctx->base.pc_next);

    if (!decode(ctx, ctx->opcode)) {
        qemu_log_mask(LOG_UNIMP, "Error: unknown opcode. "
                      TARGET_FMT_lx ": 0x%x\n",
                      ctx->base.pc_next, ctx->opcode);
        generate_exception(ctx, EXCCODE_INE);
    }

    for (int i = ctx->ntemp - 1; i >= 0; --i) {
        tcg_temp_free(tcg_ctx, ctx->temp[i]);
        ctx->temp[i] = NULL;
    }
    ctx->ntemp = 0;

    ctx->base.pc_next += 4;
}

static void loongarch_tr_tb_stop(DisasContextBase *dcbase, CPUState *cs)
{
    DisasContext *ctx = container_of(dcbase, DisasContext, base);
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;

    switch (ctx->base.is_jmp) {
    case DISAS_STOP:
        tcg_gen_movi_tl(tcg_ctx, tcg_ctx->cpu_pc, ctx->base.pc_next);
        tcg_gen_lookup_and_goto_ptr(tcg_ctx);
        break;
    case DISAS_TOO_MANY:
        gen_goto_tb(ctx, 0, ctx->base.pc_next);
        break;
    case DISAS_NORETURN:
        break;
    case DISAS_EXIT_UPDATE:
        tcg_gen_movi_tl(tcg_ctx, tcg_ctx->cpu_pc, ctx->base.pc_next);
        // QEMU_FALLTHROUGH;
    case DISAS_EXIT:
        tcg_gen_exit_tb(tcg_ctx, NULL, 0);
        break;
    default:
        g_assert_not_reached();
    }
}

static const TranslatorOps loongarch_tr_ops = {
    .init_disas_context = loongarch_tr_init_disas_context,
    .tb_start           = loongarch_tr_tb_start,
    .insn_start         = loongarch_tr_insn_start,
    .translate_insn     = loongarch_tr_translate_insn,
    .tb_stop            = loongarch_tr_tb_stop,
};

void gen_intermediate_code(CPUState *cs, TranslationBlock *tb, int max_insns)
{
    DisasContext ctx;

    memset(&ctx, 0, sizeof(ctx));
    translator_loop(&loongarch_tr_ops, &ctx.base, cs, tb, max_insns);
}

void loongarch_translate_init(struct uc_struct *uc)
{
    TCGContext *tcg_ctx = uc->tcg_ctx;
    int i;

    tcg_ctx->cpu_gpr[0] = NULL;
    for (i = 1; i < 32; i++) {
        tcg_ctx->cpu_gpr[i] = tcg_global_mem_new(tcg_ctx, tcg_ctx->cpu_env,
                                        offsetof(CPULoongArchState, gpr[i]),
                                        regnames[i]);
    }

    for (i = 0; i < 32; i++) {
        int off = offsetof(CPULoongArchState, fpr[i]);
        tcg_ctx->cpu_fpr[i] = tcg_global_mem_new_i64(tcg_ctx, tcg_ctx->cpu_env, off, fregnames[i]);
    }

    tcg_ctx->cpu_pc = tcg_global_mem_new(tcg_ctx, tcg_ctx->cpu_env, offsetof(CPULoongArchState, pc), "pc");
    tcg_ctx->cpu_lladdr = tcg_global_mem_new(tcg_ctx, tcg_ctx->cpu_env,
                    offsetof(CPULoongArchState, lladdr), "lladdr");
    tcg_ctx->cpu_llval = tcg_global_mem_new(tcg_ctx, tcg_ctx->cpu_env,
                    offsetof(CPULoongArchState, llval), "llval");
}

void restore_state_to_opc(CPULoongArchState *env, TranslationBlock *tb,
                          target_ulong *data)
{
    env->pc = data[0];
}
