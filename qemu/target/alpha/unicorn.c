#include "cpu.h"
#include "unicorn.h"
#include "unicorn/alpha.h"
#include "unicorn_common.h"

static void alpha_set_pc(struct uc_struct *uc, uint64_t address)
{
    ((CPUAlphaState *)uc->cpu->env_ptr)->pc = address;
}

static uint64_t alpha_get_pc(struct uc_struct *uc)
{
    return ((CPUAlphaState *)uc->cpu->env_ptr)->pc;
}

static void reg_reset(struct uc_struct *uc)
{
    CPUAlphaState *env = uc->cpu->env_ptr;

    memset(env->ir, 0, sizeof(env->ir));
    memset(env->fir, 0, sizeof(env->fir));

    env->pc = 0;
    env->lock_addr = 0;
    env->lock_value = 0;
    env->fpcr = 0;
}

static void alpha_release(void *ctx)
{
    int i;
    TCGContext *tcg_ctx = (TCGContext *)ctx;
    AlphaCPU *cpu = (AlphaCPU *)tcg_ctx->uc->cpu;
    CPUTLBDesc *d = cpu->neg.tlb.d;
    CPUTLBDescFast *f = cpu->neg.tlb.f;
    CPUTLBDesc *desc;
    CPUTLBDescFast *fast;

    release_common(ctx);
    for (i = 0; i < NB_MMU_MODES; i++) {
        desc = &(d[i]);
        fast = &(f[i]);
        g_free(desc->iotlb);
        g_free(fast->table);
    }
}

DEFAULT_VISIBILITY
uc_err reg_read(void *_env, int mode, unsigned int regid, void *value,
                size_t *size)
{
    CPUAlphaState *env = _env;
    uc_err ret = UC_ERR_ARG;

    if (regid >= UC_ALPHA_REG_R0 && regid <= UC_ALPHA_REG_R31) {
        CHECK_REG_TYPE(uint64_t);
        *(uint64_t *)value = env->ir[regid - UC_ALPHA_REG_R0];
    } else if (regid >= UC_ALPHA_REG_F0 && regid <= UC_ALPHA_REG_F31) {
        CHECK_REG_TYPE(float64);
        *(float64 *)value = env->fir[regid - UC_ALPHA_REG_F0];
    } else {
        switch (regid) {
        default:
            break;
        case UC_ALPHA_REG_PC:
            CHECK_REG_TYPE(uint64_t);
            *(uint64_t *)value = env->pc;
            break;
        case UC_ALPHA_REG_LR0:
            CHECK_REG_TYPE(uint64_t);
            *(uint64_t *)value = env->lock_addr;
            break;
        case UC_ALPHA_REG_LR1:
            CHECK_REG_TYPE(uint64_t);
            *(uint64_t *)value = env->lock_value;
            break;
        case UC_ALPHA_REG_FPCR:
            CHECK_REG_TYPE(uint32_t);
            *(uint32_t *)value = env->fpcr;
            break;
        case UC_ALPHA_REG_PTBR:
            CHECK_REG_TYPE(uint64_t);
            *(uint64_t *)value = env->ptbr;
            break;
        }
    }

    CHECK_RET_DEPRECATE(ret, regid);
    return ret;
}

DEFAULT_VISIBILITY
uc_err reg_write(void *_env, int mode, unsigned int regid, const void *value,
                 size_t *size, int *setpc)
{
    CPUAlphaState *env = _env;
    uc_err ret = UC_ERR_ARG;

    if (regid >= UC_ALPHA_REG_R0 && regid <= UC_ALPHA_REG_R30) { // R31 is zero
        CHECK_REG_TYPE(uint64_t);
        env->ir[regid - UC_ALPHA_REG_R0] = *(uint64_t *)value;
    } else if (regid >= UC_ALPHA_REG_F0 &&
               regid <= UC_ALPHA_REG_F30) { // F31 is zero
        CHECK_REG_TYPE(float64);
        env->fir[regid - UC_ALPHA_REG_F0] = *(float64 *)value;
    } else {
        switch (regid) {
        default:
            break;
        case UC_ALPHA_REG_PC:
            CHECK_REG_TYPE(uint64_t);
            env->pc = *(uint64_t *)value;
            *setpc = 1;
            break;
        case UC_ALPHA_REG_LR0:
            CHECK_REG_TYPE(uint64_t);
            env->lock_addr = *(uint64_t *)value;
            break;
        case UC_ALPHA_REG_LR1:
            CHECK_REG_TYPE(uint64_t);
            env->lock_value = *(uint64_t *)value;
            break;
        case UC_ALPHA_REG_FPCR:
            CHECK_REG_TYPE(uint32_t);
            env->fpcr = *(uint32_t *)value;
            break;
        case UC_ALPHA_REG_PTBR:
            CHECK_REG_TYPE(uint64_t);
            env->ptbr = *(uint64_t *)value;
            break;
        }
    }

    CHECK_RET_DEPRECATE(ret, regid);
    return ret;
}

AlphaCPU *cpu_alpha_init(struct uc_struct *uc);
static int alpha_cpus_init(struct uc_struct *uc, const char *cpu_model)
{
    AlphaCPU *cpu;

    cpu = cpu_alpha_init(uc);
    if (cpu == NULL) {
        return -1;
    }
    return 0;
}

DEFAULT_VISIBILITY
void uc_init(struct uc_struct *uc)
{
    uc->reg_read = reg_read;
    uc->reg_write = reg_write;
    uc->reg_reset = reg_reset;
    uc->release = alpha_release;
    uc->set_pc = alpha_set_pc;
    uc->get_pc = alpha_get_pc;
    uc->cpus_init = alpha_cpus_init;
    uc->cpu_context_size = offsetof(CPUAlphaState, implver);
    uc_common_init(uc);
}
