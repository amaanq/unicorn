/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (c) 2021 Loongson Technology Corporation Limited
 */

static const uint32_t fcsr_mask[4] = {
    UINT32_MAX, FCSR0_M1, FCSR0_M2, FCSR0_M3
};

static bool trans_fsel(DisasContext *ctx, arg_fsel *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    TCGv zero = tcg_constant_tl(tcg_ctx, 0);
    TCGv cond = tcg_temp_new(tcg_ctx);

    tcg_gen_ld8u_tl(tcg_ctx, cond, tcg_ctx->cpu_env, offsetof(CPULoongArchState, cf[a->ca]));
    tcg_gen_movcond_tl(tcg_ctx, TCG_COND_EQ, tcg_ctx->cpu_fpr[a->fd], cond, zero,
                       tcg_ctx->cpu_fpr[a->fj], tcg_ctx->cpu_fpr[a->fk]);
    tcg_temp_free(tcg_ctx, cond);

    return true;
}

static bool gen_f2f(DisasContext *ctx, arg_ff *a,
                    void (*func)(TCGContext *, TCGv, TCGv), bool nanbox)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    TCGv dest = tcg_ctx->cpu_fpr[a->fd];
    TCGv src = tcg_ctx->cpu_fpr[a->fj];

    func(tcg_ctx, dest, src);
    if (nanbox) {
        gen_nanbox_s(tcg_ctx, tcg_ctx->cpu_fpr[a->fd], tcg_ctx->cpu_fpr[a->fd]);
    }

    return true;
}

static bool gen_r2f(DisasContext *ctx, arg_fr *a,
                    void (*func)(TCGContext *, TCGv, TCGv))
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    TCGv src = gpr_src(ctx, a->rj, EXT_NONE);

    func(tcg_ctx, tcg_ctx->cpu_fpr[a->fd], src);
    return true;
}

static bool gen_f2r(DisasContext *ctx, arg_rf *a,
                    void (*func)(TCGContext *, TCGv, TCGv))
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    TCGv dest = gpr_dst(ctx, a->rd, EXT_NONE);

    func(tcg_ctx, dest, tcg_ctx->cpu_fpr[a->fj]);
    gen_set_gpr(tcg_ctx, a->rd, dest, EXT_NONE);

    return true;
}

static bool trans_movgr2fcsr(DisasContext *ctx, arg_movgr2fcsr *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    uint32_t mask = fcsr_mask[a->fcsrd];
    TCGv Rj = gpr_src(ctx, a->rj, EXT_NONE);

    if (mask == UINT32_MAX) {
        tcg_gen_st32_i64(tcg_ctx, Rj, tcg_ctx->cpu_env, offsetof(CPULoongArchState, fcsr0));
    } else {
        TCGv_i32 fcsr0 = tcg_temp_new_i32(tcg_ctx);
        TCGv_i32 temp = tcg_temp_new_i32(tcg_ctx);

        tcg_gen_ld_i32(tcg_ctx, fcsr0, tcg_ctx->cpu_env, offsetof(CPULoongArchState, fcsr0));
        tcg_gen_extrl_i64_i32(tcg_ctx, temp, Rj);
        tcg_gen_andi_i32(tcg_ctx, temp, temp, mask);
        tcg_gen_andi_i32(tcg_ctx, fcsr0, fcsr0, ~mask);
        tcg_gen_or_i32(tcg_ctx, fcsr0, fcsr0, temp);
        tcg_gen_st_i32(tcg_ctx, fcsr0, tcg_ctx->cpu_env, offsetof(CPULoongArchState, fcsr0));

        tcg_temp_free_i32(tcg_ctx, temp);
        tcg_temp_free_i32(tcg_ctx, fcsr0);
    }

    /*
     * Install the new rounding mode to fpu_status, if changed.
     * Note that FCSR3 is exactly the rounding mode field.
     */
    if (mask & FCSR0_M3) {
        gen_helper_set_rounding_mode(tcg_ctx, tcg_ctx->cpu_env);
    }
    return true;
}

static bool trans_movfcsr2gr(DisasContext *ctx, arg_movfcsr2gr *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    TCGv dest = gpr_dst(ctx, a->rd, EXT_NONE);

    tcg_gen_ld32u_i64(tcg_ctx, dest, tcg_ctx->cpu_env, offsetof(CPULoongArchState, fcsr0));
    tcg_gen_andi_i64(tcg_ctx, dest, dest, fcsr_mask[a->fcsrs]);
    gen_set_gpr(tcg_ctx, a->rd, dest, EXT_NONE);

    return true;
}

static void gen_movgr2fr_w(TCGContext *tcg_ctx, TCGv dest, TCGv src)
{
    tcg_gen_deposit_i64(tcg_ctx, dest, dest, src, 0, 32);
}

static void gen_movgr2frh_w(TCGContext *tcg_ctx, TCGv dest, TCGv src)
{
    tcg_gen_deposit_i64(tcg_ctx, dest, dest, src, 32, 32);
}

static void gen_movfrh2gr_s(TCGContext *tcg_ctx, TCGv dest, TCGv src)
{
    tcg_gen_sextract_tl(tcg_ctx, dest, src, 32, 32);
}

static bool trans_movfr2cf(DisasContext *ctx, arg_movfr2cf *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    TCGv t0 = tcg_temp_new(tcg_ctx);

    tcg_gen_andi_tl(tcg_ctx, t0, tcg_ctx->cpu_fpr[a->fj], 0x1);
    tcg_gen_st8_tl(tcg_ctx, t0, tcg_ctx->cpu_env, offsetof(CPULoongArchState, cf[a->cd & 0x7]));
    tcg_temp_free(tcg_ctx, t0);

    return true;
}

static bool trans_movcf2fr(DisasContext *ctx, arg_movcf2fr *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    tcg_gen_ld8u_tl(tcg_ctx, tcg_ctx->cpu_fpr[a->fd], tcg_ctx->cpu_env,
                    offsetof(CPULoongArchState, cf[a->cj & 0x7]));
    return true;
}

static bool trans_movgr2cf(DisasContext *ctx, arg_movgr2cf *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    TCGv t0 = tcg_temp_new(tcg_ctx);

    tcg_gen_andi_tl(tcg_ctx, t0, gpr_src(ctx, a->rj, EXT_NONE), 0x1);
    tcg_gen_st8_tl(tcg_ctx, t0, tcg_ctx->cpu_env, offsetof(CPULoongArchState, cf[a->cd & 0x7]));
    tcg_temp_free(tcg_ctx, t0);

    return true;
}

static bool trans_movcf2gr(DisasContext *ctx, arg_movcf2gr *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    tcg_gen_ld8u_tl(tcg_ctx, gpr_dst(ctx, a->rd, EXT_NONE), tcg_ctx->cpu_env,
                    offsetof(CPULoongArchState, cf[a->cj & 0x7]));
    return true;
}

TRANS(fmov_s, gen_f2f, tcg_gen_mov_tl, true)
TRANS(fmov_d, gen_f2f, tcg_gen_mov_tl, false)
TRANS(movgr2fr_w, gen_r2f, gen_movgr2fr_w)
TRANS(movgr2fr_d, gen_r2f, tcg_gen_mov_tl)
TRANS(movgr2frh_w, gen_r2f, gen_movgr2frh_w)
TRANS(movfr2gr_s, gen_f2r, tcg_gen_ext32s_tl)
TRANS(movfr2gr_d, gen_f2r, tcg_gen_mov_tl)
TRANS(movfrh2gr_s, gen_f2r, gen_movfrh2gr_s)
