/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (c) 2021 Loongson Technology Corporation Limited
 */

static bool gen_rr(DisasContext *ctx, arg_rr *a,
                   DisasExtend src_ext, DisasExtend dst_ext,
                   void (*func)(TCGContext *, TCGv, TCGv))
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    TCGv dest = gpr_dst(ctx, a->rd, dst_ext);
    TCGv src1 = gpr_src(ctx, a->rj, src_ext);

    func(tcg_ctx, dest, src1);
    gen_set_gpr(tcg_ctx, a->rd, dest, dst_ext);

    return true;
}

static void gen_bytepick_w(TCGContext *tcg_ctx, TCGv dest, TCGv src1, TCGv src2, target_long sa)
{
    tcg_gen_concat_tl_i64(tcg_ctx, dest, src1, src2);
    tcg_gen_sextract_i64(tcg_ctx, dest, dest, (32 - sa * 8), 32);
}

static void gen_bytepick_d(TCGContext *tcg_ctx, TCGv dest, TCGv src1, TCGv src2, target_long sa)
{
    tcg_gen_extract2_i64(tcg_ctx, dest, src1, src2, (64 - sa * 8));
}

static void gen_bstrins(TCGContext *tcg_ctx, TCGv dest, TCGv src1,
                        unsigned int ls, unsigned int len)
{
    tcg_gen_deposit_tl(tcg_ctx, dest, dest, src1, ls, len);
}

static bool gen_rr_ms_ls(DisasContext *ctx, arg_rr_ms_ls *a,
                         DisasExtend src_ext, DisasExtend dst_ext,
                         void (*func)(TCGContext *, TCGv, TCGv, unsigned int, unsigned int))
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    TCGv dest = gpr_dst(ctx, a->rd, dst_ext);
    TCGv src1 = gpr_src(ctx, a->rj, src_ext);

    if (a->ls > a->ms) {
        return false;
    }

    func(tcg_ctx, dest, src1, a->ls, a->ms - a->ls + 1);
    gen_set_gpr(tcg_ctx, a->rd, dest, dst_ext);

    return true;
}

static void gen_clz_w(TCGContext *tcg_ctx, TCGv dest, TCGv src1)
{
    tcg_gen_clzi_tl(tcg_ctx, dest, src1, TARGET_LONG_BITS);
    tcg_gen_subi_tl(tcg_ctx, dest, dest, TARGET_LONG_BITS - 32);
}

static void gen_clo_w(TCGContext *tcg_ctx, TCGv dest, TCGv src1)
{
    tcg_gen_not_tl(tcg_ctx, dest, src1);
    tcg_gen_ext32u_tl(tcg_ctx, dest, dest);
    gen_clz_w(tcg_ctx, dest, dest);
}

static void gen_ctz_w(TCGContext *tcg_ctx, TCGv dest, TCGv src1)
{
    tcg_gen_ori_tl(tcg_ctx, dest, src1, (target_ulong)MAKE_64BIT_MASK(32, 32));
    tcg_gen_ctzi_tl(tcg_ctx, dest, dest, TARGET_LONG_BITS);
}

static void gen_cto_w(TCGContext *tcg_ctx, TCGv dest, TCGv src1)
{
    tcg_gen_not_tl(tcg_ctx, dest, src1);
    gen_ctz_w(tcg_ctx, dest, dest);
}

static void gen_clz_d(TCGContext *tcg_ctx, TCGv dest, TCGv src1)
{
    tcg_gen_clzi_i64(tcg_ctx, dest, src1, TARGET_LONG_BITS);
}

static void gen_clo_d(TCGContext *tcg_ctx, TCGv dest, TCGv src1)
{
    tcg_gen_not_tl(tcg_ctx, dest, src1);
    gen_clz_d(tcg_ctx, dest, dest);
}

static void gen_ctz_d(TCGContext *tcg_ctx, TCGv dest, TCGv src1)
{
    tcg_gen_ctzi_tl(tcg_ctx, dest, src1, TARGET_LONG_BITS);
}

static void gen_cto_d(TCGContext *tcg_ctx, TCGv dest, TCGv src1)
{
    tcg_gen_not_tl(tcg_ctx, dest, src1);
    gen_ctz_d(tcg_ctx, dest, dest);
}

static void gen_revb_2w(TCGContext *tcg_ctx, TCGv dest, TCGv src1)
{
    tcg_gen_bswap64_i64(tcg_ctx, dest, src1);
    tcg_gen_rotri_i64(tcg_ctx, dest, dest, 32);
}

static void gen_revb_2h(TCGContext *tcg_ctx, TCGv dest, TCGv src1)
{
    TCGv mask = tcg_constant_tl(tcg_ctx, 0x00FF00FF);
    TCGv t0 = tcg_temp_new(tcg_ctx);
    TCGv t1 = tcg_temp_new(tcg_ctx);

    tcg_gen_shri_tl(tcg_ctx, t0, src1, 8);
    tcg_gen_and_tl(tcg_ctx, t0, t0, mask);
    tcg_gen_and_tl(tcg_ctx, t1, src1, mask);
    tcg_gen_shli_tl(tcg_ctx, t1, t1, 8);
    tcg_gen_or_tl(tcg_ctx, dest, t0, t1);

    tcg_temp_free(tcg_ctx, t0);
    tcg_temp_free(tcg_ctx, t1);
}

static void gen_revb_4h(TCGContext *tcg_ctx, TCGv dest, TCGv src1)
{
    TCGv mask = tcg_constant_tl(tcg_ctx, 0x00FF00FF00FF00FFULL);
    TCGv t0 = tcg_temp_new(tcg_ctx);
    TCGv t1 = tcg_temp_new(tcg_ctx);

    tcg_gen_shri_tl(tcg_ctx, t0, src1, 8);
    tcg_gen_and_tl(tcg_ctx, t0, t0, mask);
    tcg_gen_and_tl(tcg_ctx, t1, src1, mask);
    tcg_gen_shli_tl(tcg_ctx, t1, t1, 8);
    tcg_gen_or_tl(tcg_ctx, dest, t0, t1);

    tcg_temp_free(tcg_ctx, t0);
    tcg_temp_free(tcg_ctx, t1);
}

static void gen_revh_2w(TCGContext *tcg_ctx, TCGv dest, TCGv src1)
{
    TCGv_i64 t0 = tcg_temp_new_i64(tcg_ctx);
    TCGv_i64 t1 = tcg_temp_new_i64(tcg_ctx);
    TCGv_i64 mask = tcg_constant_i64(tcg_ctx, 0x0000ffff0000ffffull);

    tcg_gen_shri_i64(tcg_ctx, t0, src1, 16);
    tcg_gen_and_i64(tcg_ctx, t1, src1, mask);
    tcg_gen_and_i64(tcg_ctx, t0, t0, mask);
    tcg_gen_shli_i64(tcg_ctx, t1, t1, 16);
    tcg_gen_or_i64(tcg_ctx, dest, t1, t0);

    tcg_temp_free_i64(tcg_ctx, t0);
    tcg_temp_free_i64(tcg_ctx, t1);
}

static void gen_revh_d(TCGContext *tcg_ctx, TCGv dest, TCGv src1)
{
    TCGv t0 = tcg_temp_new(tcg_ctx);
    TCGv t1 = tcg_temp_new(tcg_ctx);
    TCGv mask = tcg_constant_tl(tcg_ctx, 0x0000FFFF0000FFFFULL);

    tcg_gen_shri_tl(tcg_ctx, t1, src1, 16);
    tcg_gen_and_tl(tcg_ctx, t1, t1, mask);
    tcg_gen_and_tl(tcg_ctx, t0, src1, mask);
    tcg_gen_shli_tl(tcg_ctx, t0, t0, 16);
    tcg_gen_or_tl(tcg_ctx, t0, t0, t1);
    tcg_gen_rotri_tl(tcg_ctx, dest, t0, 32);

    tcg_temp_free(tcg_ctx, t0);
    tcg_temp_free(tcg_ctx, t1);
}

static void gen_maskeqz(TCGContext *tcg_ctx, TCGv dest, TCGv src1, TCGv src2)
{
    TCGv zero = tcg_constant_tl(tcg_ctx, 0);

    tcg_gen_movcond_tl(tcg_ctx, TCG_COND_EQ, dest, src2, zero, zero, src1);
}

static void gen_masknez(TCGContext *tcg_ctx, TCGv dest, TCGv src1, TCGv src2)
{
    TCGv zero = tcg_constant_tl(tcg_ctx, 0);

    tcg_gen_movcond_tl(tcg_ctx, TCG_COND_NE, dest, src2, zero, zero, src1);
}

TRANS(ext_w_h, gen_rr, EXT_NONE, EXT_NONE, tcg_gen_ext16s_tl)
TRANS(ext_w_b, gen_rr, EXT_NONE, EXT_NONE, tcg_gen_ext8s_tl)
TRANS(clo_w, gen_rr, EXT_NONE, EXT_NONE, gen_clo_w)
TRANS(clz_w, gen_rr, EXT_ZERO, EXT_NONE, gen_clz_w)
TRANS(cto_w, gen_rr, EXT_NONE, EXT_NONE, gen_cto_w)
TRANS(ctz_w, gen_rr, EXT_NONE, EXT_NONE, gen_ctz_w)
TRANS(clo_d, gen_rr, EXT_NONE, EXT_NONE, gen_clo_d)
TRANS(clz_d, gen_rr, EXT_NONE, EXT_NONE, gen_clz_d)
TRANS(cto_d, gen_rr, EXT_NONE, EXT_NONE, gen_cto_d)
TRANS(ctz_d, gen_rr, EXT_NONE, EXT_NONE, gen_ctz_d)
TRANS(revb_2h, gen_rr, EXT_NONE, EXT_SIGN, gen_revb_2h)
TRANS(revb_4h, gen_rr, EXT_NONE, EXT_NONE, gen_revb_4h)
TRANS(revb_2w, gen_rr, EXT_NONE, EXT_NONE, gen_revb_2w)
TRANS(revb_d, gen_rr, EXT_NONE, EXT_NONE, tcg_gen_bswap64_i64)
TRANS(revh_2w, gen_rr, EXT_NONE, EXT_NONE, gen_revh_2w)
TRANS(revh_d, gen_rr, EXT_NONE, EXT_NONE, gen_revh_d)
TRANS(bitrev_4b, gen_rr, EXT_ZERO, EXT_SIGN, gen_helper_bitswap)
TRANS(bitrev_8b, gen_rr, EXT_NONE, EXT_NONE, gen_helper_bitswap)
TRANS(bitrev_w, gen_rr, EXT_NONE, EXT_SIGN, gen_helper_bitrev_w)
TRANS(bitrev_d, gen_rr, EXT_NONE, EXT_NONE, gen_helper_bitrev_d)
TRANS(maskeqz, gen_rrr, EXT_NONE, EXT_NONE, EXT_NONE, gen_maskeqz)
TRANS(masknez, gen_rrr, EXT_NONE, EXT_NONE, EXT_NONE, gen_masknez)
TRANS(bytepick_w, gen_rrr_sa, EXT_NONE, EXT_NONE, gen_bytepick_w)
TRANS(bytepick_d, gen_rrr_sa, EXT_NONE, EXT_NONE, gen_bytepick_d)
TRANS(bstrins_w, gen_rr_ms_ls, EXT_NONE, EXT_NONE, gen_bstrins)
TRANS(bstrins_d, gen_rr_ms_ls, EXT_NONE, EXT_NONE, gen_bstrins)
TRANS(bstrpick_w, gen_rr_ms_ls, EXT_NONE, EXT_SIGN, tcg_gen_extract_tl)
TRANS(bstrpick_d, gen_rr_ms_ls, EXT_NONE, EXT_NONE, tcg_gen_extract_tl)
