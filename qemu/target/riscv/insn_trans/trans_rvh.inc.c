/*
 * RISC-V translation routines for the RVXI Base Integer Instruction Set.
 *
 * Copyright (c) 2020 Western Digital
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2 or later, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

static void check_access(DisasContext *ctx) {
    if (!ctx->hlsx) {
        if (ctx->virt_enabled) {
            generate_exception(ctx, RISCV_EXCP_VIRT_INSTRUCTION_FAULT);
        } else {
            generate_exception(ctx, RISCV_EXCP_ILLEGAL_INST);
        }
    }
}

static bool trans_hlv_b(DisasContext *ctx, arg_hlv_b *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    REQUIRE_EXT(ctx, RVH);
    TCGv t0 = tcg_temp_new(tcg_ctx);
    TCGv t1 = tcg_temp_new(tcg_ctx);

    check_access(ctx);

    gen_get_gpr(tcg_ctx, t0, a->rs1);

    tcg_gen_qemu_ld_tl(tcg_ctx, t1, t0, ctx->mem_idx | TB_FLAGS_PRIV_HYP_ACCESS_MASK, MO_SB);
    gen_set_gpr(tcg_ctx, a->rd, t1);

    tcg_temp_free(tcg_ctx, t0);
    tcg_temp_free(tcg_ctx, t1);
    return true;
}

static bool trans_hlv_h(DisasContext *ctx, arg_hlv_h *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    REQUIRE_EXT(ctx, RVH);
    TCGv t0 = tcg_temp_new(tcg_ctx);
    TCGv t1 = tcg_temp_new(tcg_ctx);

    check_access(ctx);

    gen_get_gpr(tcg_ctx, t0, a->rs1);

    tcg_gen_qemu_ld_tl(tcg_ctx, t1, t0, ctx->mem_idx | TB_FLAGS_PRIV_HYP_ACCESS_MASK, MO_TESW);
    gen_set_gpr(tcg_ctx, a->rd, t1);

    tcg_temp_free(tcg_ctx, t0);
    tcg_temp_free(tcg_ctx, t1);
    return true;
}

static bool trans_hlv_w(DisasContext *ctx, arg_hlv_w *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    REQUIRE_EXT(ctx, RVH);
    TCGv t0 = tcg_temp_new(tcg_ctx);
    TCGv t1 = tcg_temp_new(tcg_ctx);

    check_access(ctx);

    gen_get_gpr(tcg_ctx, t0, a->rs1);

    tcg_gen_qemu_ld_tl(tcg_ctx, t1, t0, ctx->mem_idx | TB_FLAGS_PRIV_HYP_ACCESS_MASK, MO_TESL);
    gen_set_gpr(tcg_ctx, a->rd, t1);

    tcg_temp_free(tcg_ctx, t0);
    tcg_temp_free(tcg_ctx, t1);
    return true;
}

static bool trans_hlv_bu(DisasContext *ctx, arg_hlv_bu *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    REQUIRE_EXT(ctx, RVH);
    TCGv t0 = tcg_temp_new(tcg_ctx);
    TCGv t1 = tcg_temp_new(tcg_ctx);

    check_access(ctx);

    gen_get_gpr(tcg_ctx, t0, a->rs1);

    tcg_gen_qemu_ld_tl(tcg_ctx, t1, t0, ctx->mem_idx | TB_FLAGS_PRIV_HYP_ACCESS_MASK, MO_UB);
    gen_set_gpr(tcg_ctx, a->rd, t1);

    tcg_temp_free(tcg_ctx, t0);
    tcg_temp_free(tcg_ctx, t1);
    return true;
}

static bool trans_hlv_hu(DisasContext *ctx, arg_hlv_hu *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    REQUIRE_EXT(ctx, RVH);
    TCGv t0 = tcg_temp_new(tcg_ctx);
    TCGv t1 = tcg_temp_new(tcg_ctx);

    check_access(ctx);

    gen_get_gpr(tcg_ctx, t0, a->rs1);
    tcg_gen_qemu_ld_tl(tcg_ctx, t1, t0, ctx->mem_idx | TB_FLAGS_PRIV_HYP_ACCESS_MASK, MO_TEUW);
    gen_set_gpr(tcg_ctx, a->rd, t1);

    tcg_temp_free(tcg_ctx, t0);
    tcg_temp_free(tcg_ctx, t1);
    return true;
}

static bool trans_hsv_b(DisasContext *ctx, arg_hsv_b *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    REQUIRE_EXT(ctx, RVH);
    TCGv t0 = tcg_temp_new(tcg_ctx);
    TCGv dat = tcg_temp_new(tcg_ctx);

    check_access(ctx);

    gen_get_gpr(tcg_ctx, t0, a->rs1);
    gen_get_gpr(tcg_ctx, dat, a->rs2);

    tcg_gen_qemu_st_tl(tcg_ctx, dat, t0, ctx->mem_idx | TB_FLAGS_PRIV_HYP_ACCESS_MASK, MO_SB);

    tcg_temp_free(tcg_ctx, t0);
    tcg_temp_free(tcg_ctx, dat);
    return true;
}

static bool trans_hsv_h(DisasContext *ctx, arg_hsv_h *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    REQUIRE_EXT(ctx, RVH);
    TCGv t0 = tcg_temp_new(tcg_ctx);
    TCGv dat = tcg_temp_new(tcg_ctx);

    check_access(ctx);

    gen_get_gpr(tcg_ctx, t0, a->rs1);
    gen_get_gpr(tcg_ctx, dat, a->rs2);

    tcg_gen_qemu_st_tl(tcg_ctx, dat, t0, ctx->mem_idx | TB_FLAGS_PRIV_HYP_ACCESS_MASK, MO_TESW);

    tcg_temp_free(tcg_ctx, t0);
    tcg_temp_free(tcg_ctx, dat);
    return true;
}

static bool trans_hsv_w(DisasContext *ctx, arg_hsv_w *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    REQUIRE_EXT(ctx, RVH);
    TCGv t0 = tcg_temp_new(tcg_ctx);
    TCGv dat = tcg_temp_new(tcg_ctx);

    check_access(ctx);

    gen_get_gpr(tcg_ctx, t0, a->rs1);
    gen_get_gpr(tcg_ctx, dat, a->rs2);

    tcg_gen_qemu_st_tl(tcg_ctx, dat, t0, ctx->mem_idx | TB_FLAGS_PRIV_HYP_ACCESS_MASK, MO_TESL);

    tcg_temp_free(tcg_ctx, t0);
    tcg_temp_free(tcg_ctx, dat);
    return true;
}

#ifdef TARGET_RISCV64
static bool trans_hlv_wu(DisasContext *ctx, arg_hlv_wu *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    REQUIRE_EXT(ctx, RVH);
    TCGv t0 = tcg_temp_new(tcg_ctx);
    TCGv t1 = tcg_temp_new(tcg_ctx);

    check_access(ctx);

    gen_get_gpr(tcg_ctx, t0, a->rs1);

    tcg_gen_qemu_ld_tl(tcg_ctx, t1, t0, ctx->mem_idx | TB_FLAGS_PRIV_HYP_ACCESS_MASK, MO_TEUL);
    gen_set_gpr(tcg_ctx, a->rd, t1);

    tcg_temp_free(tcg_ctx, t0);
    tcg_temp_free(tcg_ctx, t1);
    return true;
}

static bool trans_hlv_d(DisasContext *ctx, arg_hlv_d *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    REQUIRE_EXT(ctx, RVH);
    TCGv t0 = tcg_temp_new(tcg_ctx);
    TCGv t1 = tcg_temp_new(tcg_ctx);

    check_access(ctx);

    gen_get_gpr(tcg_ctx, t0, a->rs1);

    tcg_gen_qemu_ld_tl(tcg_ctx, t1, t0, ctx->mem_idx | TB_FLAGS_PRIV_HYP_ACCESS_MASK, MO_TEQ);
    gen_set_gpr(tcg_ctx, a->rd, t1);

    tcg_temp_free(tcg_ctx, t0);
    tcg_temp_free(tcg_ctx, t1);
    return true;
}

static bool trans_hsv_d(DisasContext *ctx, arg_hsv_d *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    REQUIRE_EXT(ctx, RVH);
    TCGv t0 = tcg_temp_new(tcg_ctx);
    TCGv dat = tcg_temp_new(tcg_ctx);

    check_access(ctx);

    gen_get_gpr(tcg_ctx, t0, a->rs1);
    gen_get_gpr(tcg_ctx, dat, a->rs2);

    tcg_gen_qemu_st_tl(tcg_ctx, dat, t0, ctx->mem_idx | TB_FLAGS_PRIV_HYP_ACCESS_MASK, MO_TEQ);

    tcg_temp_free(tcg_ctx, t0);
    tcg_temp_free(tcg_ctx, dat);
    return true;
}
#endif

static bool trans_hlvx_hu(DisasContext *ctx, arg_hlvx_hu *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    REQUIRE_EXT(ctx, RVH);
    TCGv t0 = tcg_temp_new(tcg_ctx);
    TCGv t1 = tcg_temp_new(tcg_ctx);

    check_access(ctx);

    gen_get_gpr(tcg_ctx, t0, a->rs1);

    gen_helper_hyp_hlvx_hu(tcg_ctx, t1, tcg_ctx->cpu_env, t0);
    gen_set_gpr(tcg_ctx, a->rd, t1);

    tcg_temp_free(tcg_ctx, t0);
    tcg_temp_free(tcg_ctx, t1);
    return true;
}

static bool trans_hlvx_wu(DisasContext *ctx, arg_hlvx_wu *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    REQUIRE_EXT(ctx, RVH);
    TCGv t0 = tcg_temp_new(tcg_ctx);
    TCGv t1 = tcg_temp_new(tcg_ctx);

    check_access(ctx);

    gen_get_gpr(tcg_ctx, t0, a->rs1);

    gen_helper_hyp_hlvx_wu(tcg_ctx, t1, tcg_ctx->cpu_env, t0);
    gen_set_gpr(tcg_ctx, a->rd, t1);

    tcg_temp_free(tcg_ctx, t0);
    tcg_temp_free(tcg_ctx, t1);
    return true;
}

static bool trans_hfence_gvma(DisasContext *ctx, arg_sfence_vma *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    REQUIRE_EXT(ctx, RVH);
    gen_helper_hyp_gvma_tlb_flush(tcg_ctx, tcg_ctx->cpu_env);
    return true;
}

static bool trans_hfence_vvma(DisasContext *ctx, arg_sfence_vma *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    REQUIRE_EXT(ctx, RVH);
    gen_helper_hyp_tlb_flush(tcg_ctx, tcg_ctx->cpu_env);
    return true;
}
