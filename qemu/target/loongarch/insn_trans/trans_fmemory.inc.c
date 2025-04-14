/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (c) 2021 Loongson Technology Corporation Limited
 */

static void maybe_nanbox_load(TCGContext *tcg_ctx, TCGv freg, MemOp mop)
{
    if ((mop & MO_SIZE) == MO_32) {
        gen_nanbox_s(tcg_ctx, freg, freg);
    }
}

static bool gen_fload_i(DisasContext *ctx, arg_fr_i *a, MemOp mop)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    TCGv addr = gpr_src(ctx, a->rj, EXT_NONE);
    TCGv temp = NULL;

    if (a->imm) {
        temp = tcg_temp_new(tcg_ctx);
        tcg_gen_addi_tl(tcg_ctx, temp, addr, a->imm);
        addr = temp;
    }

    tcg_gen_qemu_ld_tl(tcg_ctx, tcg_ctx->cpu_fpr[a->fd], addr, ctx->mem_idx, mop);
    maybe_nanbox_load(tcg_ctx, tcg_ctx->cpu_fpr[a->fd], mop);

    if (temp) {
        tcg_temp_free(tcg_ctx, temp);
    }

    return true;
}

static bool gen_fstore_i(DisasContext *ctx, arg_fr_i *a, MemOp mop)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    TCGv addr = gpr_src(ctx, a->rj, EXT_NONE);
    TCGv temp = NULL;

    if (a->imm) {
        temp = tcg_temp_new(tcg_ctx);
        tcg_gen_addi_tl(tcg_ctx, temp, addr, a->imm);
        addr = temp;
    }

    tcg_gen_qemu_st_tl(tcg_ctx, tcg_ctx->cpu_fpr[a->fd], addr, ctx->mem_idx, mop);

    if (temp) {
        tcg_temp_free(tcg_ctx, temp);
    }
    return true;
}

static bool gen_floadx(DisasContext *ctx, arg_frr *a, MemOp mop)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    TCGv src1 = gpr_src(ctx, a->rj, EXT_NONE);
    TCGv src2 = gpr_src(ctx, a->rk, EXT_NONE);
    TCGv addr = tcg_temp_new(tcg_ctx);

    tcg_gen_add_tl(tcg_ctx, addr, src1, src2);
    tcg_gen_qemu_ld_tl(tcg_ctx, tcg_ctx->cpu_fpr[a->fd], addr, ctx->mem_idx, mop);
    maybe_nanbox_load(tcg_ctx, tcg_ctx->cpu_fpr[a->fd], mop);
    tcg_temp_free(tcg_ctx, addr);

    return true;
}

static bool gen_fstorex(DisasContext *ctx, arg_frr *a, MemOp mop)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    TCGv src1 = gpr_src(ctx, a->rj, EXT_NONE);
    TCGv src2 = gpr_src(ctx, a->rk, EXT_NONE);
    TCGv addr = tcg_temp_new(tcg_ctx);

    tcg_gen_add_tl(tcg_ctx, addr, src1, src2);
    tcg_gen_qemu_st_tl(tcg_ctx, tcg_ctx->cpu_fpr[a->fd], addr, ctx->mem_idx, mop);
    tcg_temp_free(tcg_ctx, addr);

    return true;
}

static bool gen_fload_gt(DisasContext *ctx, arg_frr *a, MemOp mop)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    TCGv src1 = gpr_src(ctx, a->rj, EXT_NONE);
    TCGv src2 = gpr_src(ctx, a->rk, EXT_NONE);
    TCGv addr = tcg_temp_new(tcg_ctx);

    gen_helper_asrtgt_d(tcg_ctx, tcg_ctx->cpu_env, src1, src2);
    tcg_gen_add_tl(tcg_ctx, addr, src1, src2);
    tcg_gen_qemu_ld_tl(tcg_ctx, tcg_ctx->cpu_fpr[a->fd], addr, ctx->mem_idx, mop);
    maybe_nanbox_load(tcg_ctx, tcg_ctx->cpu_fpr[a->fd], mop);
    tcg_temp_free(tcg_ctx, addr);

    return true;
}

static bool gen_fstore_gt(DisasContext *ctx, arg_frr *a, MemOp mop)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    TCGv src1 = gpr_src(ctx, a->rj, EXT_NONE);
    TCGv src2 = gpr_src(ctx, a->rk, EXT_NONE);
    TCGv addr = tcg_temp_new(tcg_ctx);

    gen_helper_asrtgt_d(tcg_ctx, tcg_ctx->cpu_env, src1, src2);
    tcg_gen_add_tl(tcg_ctx, addr, src1, src2);
    tcg_gen_qemu_st_tl(tcg_ctx, tcg_ctx->cpu_fpr[a->fd], addr, ctx->mem_idx, mop);
    tcg_temp_free(tcg_ctx, addr);

    return true;
}

static bool gen_fload_le(DisasContext *ctx, arg_frr *a, MemOp mop)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    TCGv src1 = gpr_src(ctx, a->rj, EXT_NONE);
    TCGv src2 = gpr_src(ctx, a->rk, EXT_NONE);
    TCGv addr = tcg_temp_new(tcg_ctx);

    gen_helper_asrtle_d(tcg_ctx, tcg_ctx->cpu_env, src1, src2);
    tcg_gen_add_tl(tcg_ctx, addr, src1, src2);
    tcg_gen_qemu_ld_tl(tcg_ctx, tcg_ctx->cpu_fpr[a->fd], addr, ctx->mem_idx, mop);
    maybe_nanbox_load(tcg_ctx, tcg_ctx->cpu_fpr[a->fd], mop);
    tcg_temp_free(tcg_ctx, addr);

    return true;
}

static bool gen_fstore_le(DisasContext *ctx, arg_frr *a, MemOp mop)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    TCGv src1 = gpr_src(ctx, a->rj, EXT_NONE);
    TCGv src2 = gpr_src(ctx, a->rk, EXT_NONE);
    TCGv addr = tcg_temp_new(tcg_ctx);

    gen_helper_asrtle_d(tcg_ctx, tcg_ctx->cpu_env, src1, src2);
    tcg_gen_add_tl(tcg_ctx, addr, src1, src2);
    tcg_gen_qemu_st_tl(tcg_ctx, tcg_ctx->cpu_fpr[a->fd], addr, ctx->mem_idx, mop);
    tcg_temp_free(tcg_ctx, addr);

    return true;
}

TRANS(fld_s, gen_fload_i, MO_TEUL)
TRANS(fst_s, gen_fstore_i, MO_TEUL)
TRANS(fld_d, gen_fload_i, MO_TEUQ)
TRANS(fst_d, gen_fstore_i, MO_TEUQ)
TRANS(fldx_s, gen_floadx, MO_TEUL)
TRANS(fldx_d, gen_floadx, MO_TEUQ)
TRANS(fstx_s, gen_fstorex, MO_TEUL)
TRANS(fstx_d, gen_fstorex, MO_TEUQ)
TRANS(fldgt_s, gen_fload_gt, MO_TEUL)
TRANS(fldgt_d, gen_fload_gt, MO_TEUQ)
TRANS(fldle_s, gen_fload_le, MO_TEUL)
TRANS(fldle_d, gen_fload_le, MO_TEUQ)
TRANS(fstgt_s, gen_fstore_gt, MO_TEUL)
TRANS(fstgt_d, gen_fstore_gt, MO_TEUQ)
TRANS(fstle_s, gen_fstore_le, MO_TEUL)
TRANS(fstle_d, gen_fstore_le, MO_TEUQ)
