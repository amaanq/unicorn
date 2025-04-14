/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (c) 2021 Loongson Technology Corporation Limited
 */

static bool gen_ll(DisasContext *ctx, arg_rr_i *a, MemOp mop)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    TCGv dest = gpr_dst(ctx, a->rd, EXT_NONE);
    TCGv src1 = gpr_src(ctx, a->rj, EXT_NONE);
    TCGv t0 = tcg_temp_new(tcg_ctx);

    tcg_gen_addi_tl(tcg_ctx, t0, src1, a->imm);
    tcg_gen_qemu_ld_i64(tcg_ctx, dest, t0, ctx->mem_idx, mop);
    tcg_gen_st_tl(tcg_ctx, t0, tcg_ctx->cpu_env, offsetof(CPULoongArchState, lladdr));
    tcg_gen_st_tl(tcg_ctx, dest, tcg_ctx->cpu_env, offsetof(CPULoongArchState, llval));
    gen_set_gpr(tcg_ctx, a->rd, dest, EXT_NONE);
    tcg_temp_free(tcg_ctx, t0);

    return true;
}

static bool gen_sc(DisasContext *ctx, arg_rr_i *a, MemOp mop)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    TCGv dest = gpr_dst(ctx, a->rd, EXT_NONE);
    TCGv src1 = gpr_src(ctx, a->rj, EXT_NONE);
    TCGv src2 = gpr_src(ctx, a->rd, EXT_NONE);
    TCGv t0 = tcg_temp_new(tcg_ctx);
    TCGv val = tcg_temp_new(tcg_ctx);

    TCGLabel *l1 = gen_new_label(tcg_ctx);
    TCGLabel *done = gen_new_label(tcg_ctx);

    tcg_gen_addi_tl(tcg_ctx, t0, src1, a->imm);
    tcg_gen_brcond_tl(tcg_ctx, TCG_COND_EQ, t0, tcg_ctx->cpu_lladdr, l1);
    tcg_gen_movi_tl(tcg_ctx, dest, 0);
    tcg_gen_br(tcg_ctx, done);

    gen_set_label(tcg_ctx, l1);
    tcg_gen_mov_tl(tcg_ctx, val, src2);
    /* generate cmpxchg */
    tcg_gen_atomic_cmpxchg_tl(tcg_ctx, t0, tcg_ctx->cpu_lladdr, tcg_ctx->cpu_llval,
                              val, ctx->mem_idx, mop);
    tcg_gen_setcond_tl(tcg_ctx, TCG_COND_EQ, dest, t0, tcg_ctx->cpu_llval);
    gen_set_label(tcg_ctx, done);
    gen_set_gpr(tcg_ctx, a->rd, dest, EXT_NONE);
    tcg_temp_free(tcg_ctx, t0);
    tcg_temp_free(tcg_ctx, val);

    return true;
}

static bool gen_am(DisasContext *ctx, arg_rrr *a,
                   void (*func)(TCGContext *, TCGv, TCGv, TCGv, TCGArg, MemOp),
                   MemOp mop)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    TCGv dest = gpr_dst(ctx, a->rd, EXT_NONE);
    TCGv addr = gpr_src(ctx, a->rj, EXT_NONE);
    TCGv val = gpr_src(ctx, a->rk, EXT_NONE);

    if (a->rd != 0 && (a->rj == a->rd || a->rk == a->rd)) {
        qemu_log_mask(LOG_GUEST_ERROR,
                      "Warning: source register overlaps destination register"
                      "in atomic insn at pc=0x" TARGET_FMT_lx "\n",
                      ctx->base.pc_next - 4);
        return false;
    }

    func(tcg_ctx, dest, addr, val, ctx->mem_idx, mop);
    gen_set_gpr(tcg_ctx, a->rd, dest, EXT_NONE);

    return true;
}

TRANS(ll_w, gen_ll, MO_TESL)
TRANS(sc_w, gen_sc, MO_TESL)
TRANS(ll_d, gen_ll, MO_TEUQ)
TRANS(sc_d, gen_sc, MO_TEUQ)
TRANS(amswap_w, gen_am, tcg_gen_atomic_xchg_tl, MO_TESL)
TRANS(amswap_d, gen_am, tcg_gen_atomic_xchg_tl, MO_TEUQ)
TRANS(amadd_w, gen_am, tcg_gen_atomic_fetch_add_tl, MO_TESL)
TRANS(amadd_d, gen_am, tcg_gen_atomic_fetch_add_tl, MO_TEUQ)
TRANS(amand_w, gen_am, tcg_gen_atomic_fetch_and_tl, MO_TESL)
TRANS(amand_d, gen_am, tcg_gen_atomic_fetch_and_tl, MO_TEUQ)
TRANS(amor_w, gen_am, tcg_gen_atomic_fetch_or_tl, MO_TESL)
TRANS(amor_d, gen_am, tcg_gen_atomic_fetch_or_tl, MO_TEUQ)
TRANS(amxor_w, gen_am, tcg_gen_atomic_fetch_xor_tl, MO_TESL)
TRANS(amxor_d, gen_am, tcg_gen_atomic_fetch_xor_tl, MO_TEUQ)
TRANS(ammax_w, gen_am, tcg_gen_atomic_fetch_smax_tl, MO_TESL)
TRANS(ammax_d, gen_am, tcg_gen_atomic_fetch_smax_tl, MO_TEUQ)
TRANS(ammin_w, gen_am, tcg_gen_atomic_fetch_smin_tl, MO_TESL)
TRANS(ammin_d, gen_am, tcg_gen_atomic_fetch_smin_tl, MO_TEUQ)
TRANS(ammax_wu, gen_am, tcg_gen_atomic_fetch_umax_tl, MO_TESL)
TRANS(ammax_du, gen_am, tcg_gen_atomic_fetch_umax_tl, MO_TEUQ)
TRANS(ammin_wu, gen_am, tcg_gen_atomic_fetch_umin_tl, MO_TESL)
TRANS(ammin_du, gen_am, tcg_gen_atomic_fetch_umin_tl, MO_TEUQ)
TRANS(amswap_db_w, gen_am, tcg_gen_atomic_xchg_tl, MO_TESL)
TRANS(amswap_db_d, gen_am, tcg_gen_atomic_xchg_tl, MO_TEUQ)
TRANS(amadd_db_w, gen_am, tcg_gen_atomic_fetch_add_tl, MO_TESL)
TRANS(amadd_db_d, gen_am, tcg_gen_atomic_fetch_add_tl, MO_TEUQ)
TRANS(amand_db_w, gen_am, tcg_gen_atomic_fetch_and_tl, MO_TESL)
TRANS(amand_db_d, gen_am, tcg_gen_atomic_fetch_and_tl, MO_TEUQ)
TRANS(amor_db_w, gen_am, tcg_gen_atomic_fetch_or_tl, MO_TESL)
TRANS(amor_db_d, gen_am, tcg_gen_atomic_fetch_or_tl, MO_TEUQ)
TRANS(amxor_db_w, gen_am, tcg_gen_atomic_fetch_xor_tl, MO_TESL)
TRANS(amxor_db_d, gen_am, tcg_gen_atomic_fetch_xor_tl, MO_TEUQ)
TRANS(ammax_db_w, gen_am, tcg_gen_atomic_fetch_smax_tl, MO_TESL)
TRANS(ammax_db_d, gen_am, tcg_gen_atomic_fetch_smax_tl, MO_TEUQ)
TRANS(ammin_db_w, gen_am, tcg_gen_atomic_fetch_smin_tl, MO_TESL)
TRANS(ammin_db_d, gen_am, tcg_gen_atomic_fetch_smin_tl, MO_TEUQ)
TRANS(ammax_db_wu, gen_am, tcg_gen_atomic_fetch_umax_tl, MO_TESL)
TRANS(ammax_db_du, gen_am, tcg_gen_atomic_fetch_umax_tl, MO_TEUQ)
TRANS(ammin_db_wu, gen_am, tcg_gen_atomic_fetch_umin_tl, MO_TESL)
TRANS(ammin_db_du, gen_am, tcg_gen_atomic_fetch_umin_tl, MO_TEUQ)
