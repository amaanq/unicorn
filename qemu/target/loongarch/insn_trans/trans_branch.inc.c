/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (c) 2021 Loongson Technology Corporation Limited
 */

static bool trans_b(DisasContext *ctx, arg_b *a)
{
    gen_goto_tb(ctx, 0, ctx->base.pc_next + a->offs);
    ctx->base.is_jmp = DISAS_NORETURN;
    return true;
}

static bool trans_bl(DisasContext *ctx, arg_bl *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    tcg_gen_movi_tl(tcg_ctx, tcg_ctx->cpu_gpr[1], ctx->base.pc_next + 4);
    gen_goto_tb(ctx, 0, ctx->base.pc_next + a->offs);
    ctx->base.is_jmp = DISAS_NORETURN;
    return true;
}

static bool trans_jirl(DisasContext *ctx, arg_jirl *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    TCGv dest = gpr_dst(ctx, a->rd, EXT_NONE);
    TCGv src1 = gpr_src(ctx, a->rj, EXT_NONE);

    tcg_gen_addi_tl(tcg_ctx, tcg_ctx->cpu_pc, src1, a->offs);
    tcg_gen_movi_tl(tcg_ctx, dest, ctx->base.pc_next + 4);
    gen_set_gpr(tcg_ctx, a->rd, dest, EXT_NONE);
    tcg_gen_lookup_and_goto_ptr(tcg_ctx);
    ctx->base.is_jmp = DISAS_NORETURN;
    return true;
}

static void gen_bc(DisasContext *ctx, TCGv src1, TCGv src2,
                   target_long offs, TCGCond cond)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    TCGLabel *l = gen_new_label(tcg_ctx);
    tcg_gen_brcond_tl(tcg_ctx, cond, src1, src2, l);
    gen_goto_tb(ctx, 1, ctx->base.pc_next + 4);
    gen_set_label(tcg_ctx, l);
    gen_goto_tb(ctx, 0, ctx->base.pc_next + offs);
    ctx->base.is_jmp = DISAS_NORETURN;
}

static bool gen_rr_bc(DisasContext *ctx, arg_rr_offs *a, TCGCond cond)
{
    TCGv src1 = gpr_src(ctx, a->rj, EXT_NONE);
    TCGv src2 = gpr_src(ctx, a->rd, EXT_NONE);

    gen_bc(ctx, src1, src2, a->offs, cond);
    return true;
}

static bool gen_rz_bc(DisasContext *ctx, arg_r_offs *a, TCGCond cond)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    TCGv src1 = gpr_src(ctx, a->rj, EXT_NONE);
    TCGv src2 = tcg_constant_tl(tcg_ctx, 0);

    gen_bc(ctx, src1, src2, a->offs, cond);
    return true;
}

static bool gen_cz_bc(DisasContext *ctx, arg_c_offs *a, TCGCond cond)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    TCGv src1 = tcg_temp_new(tcg_ctx);
    TCGv src2 = tcg_constant_tl(tcg_ctx, 0);

    tcg_gen_ld8u_tl(tcg_ctx, src1, tcg_ctx->cpu_env,
                    offsetof(CPULoongArchState, cf[a->cj]));
    gen_bc(ctx, src1, src2, a->offs, cond);
    return true;
}

TRANS(beq, gen_rr_bc, TCG_COND_EQ)
TRANS(bne, gen_rr_bc, TCG_COND_NE)
TRANS(blt, gen_rr_bc, TCG_COND_LT)
TRANS(bge, gen_rr_bc, TCG_COND_GE)
TRANS(bltu, gen_rr_bc, TCG_COND_LTU)
TRANS(bgeu, gen_rr_bc, TCG_COND_GEU)
TRANS(beqz, gen_rz_bc, TCG_COND_EQ)
TRANS(bnez, gen_rz_bc, TCG_COND_NE)
TRANS(bceqz, gen_cz_bc, TCG_COND_EQ)
TRANS(bcnez, gen_cz_bc, TCG_COND_NE)
