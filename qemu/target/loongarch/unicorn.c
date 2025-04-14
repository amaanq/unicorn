/* Unicorn Emulator Engine */
/* By Nguyen Anh Quynh <aquynh@gmail.com>, 2015 */
/* Created for Unicorn Engine by Amaan Qureshi <amaanq12@gmail.com>, 2025 */

#include "cpu.h"
#include "unicorn_common.h"
#include "unicorn.h"

LoongArchCPU *cpu_loongarch_init(struct uc_struct *uc);

static void loongarch_set_pc(struct uc_struct *uc, uint64_t address)
{
    ((CPULoongArchState *)uc->cpu->env_ptr)->pc = address;
}

static uint64_t loongarch_get_pc(struct uc_struct *uc)
{
    return ((CPULoongArchState *)uc->cpu->env_ptr)->pc;
}

static void loongarch_release(void *ctx)
{
    int i;
    TCGContext *tcg_ctx = (TCGContext *)ctx;
    LoongArchCPU *cpu = (LoongArchCPU *)tcg_ctx->uc->cpu;
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

static void reg_reset(struct uc_struct *uc)
{
    CPUArchState *env = uc->cpu->env_ptr;

    memset(env->gpr, 0, sizeof(env->gpr));
    memset(env->fpr, 0, sizeof(env->fpr));

    env->pc = 0;
    env->lladdr = 0;
    env->llval = 0;
}

DEFAULT_VISIBILITY
uc_err reg_read(void *_env, int mode, unsigned int regid, void *value,
                size_t *size)
{
    CPULoongArchState *env = _env;
    uc_err ret = UC_ERR_ARG;

    if (regid >= UC_LOONGARCH_REG_R0 && regid <= UC_LOONGARCH_REG_R31) {
        CHECK_REG_TYPE(uint64_t);
        *(uint64_t *)value = env->gpr[regid - UC_LOONGARCH_REG_R0];
    } else if (regid >= UC_LOONGARCH_REG_F0 && regid <= UC_LOONGARCH_REG_F31) {
        CHECK_REG_TYPE(uint64_t);
        *(uint64_t *)value = env->fpr[regid - UC_LOONGARCH_REG_F0];
    } else {
        switch (regid) {
        default:
            break;
        case UC_LOONGARCH_REG_PC:
            CHECK_REG_TYPE(uint64_t);
            *(uint64_t *)value = env->pc;
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
    CPULoongArchState *env = _env;
    uc_err ret = UC_ERR_ARG;

    if (regid >= UC_LOONGARCH_REG_R1 &&
        regid <= UC_LOONGARCH_REG_R31) { // R0 is 0
        CHECK_REG_TYPE(uint64_t);
        env->gpr[regid - UC_LOONGARCH_REG_R0] = *(uint64_t *)value;
    } else if (regid >= UC_LOONGARCH_REG_F1 &&
               regid <= UC_LOONGARCH_REG_F31) { // F0 is 0
        CHECK_REG_TYPE(uint64_t);
        env->fpr[regid - UC_LOONGARCH_REG_F0] = *(uint64_t *)value;
    } else {
        switch (regid) {
        default:
            break;
        case UC_LOONGARCH_REG_PC:
            CHECK_REG_TYPE(uint64_t);
            env->pc = *(uint64_t *)value;
            *setpc = 1;
            break;
        }
    }
    CHECK_RET_DEPRECATE(ret, regid);
    return ret;
}

static int loongarch_cpus_init(struct uc_struct *uc, const char *cpu_model)
{
    LoongArchCPU *cpu;

    cpu = cpu_loongarch_init(uc);
    if (cpu == NULL) {
        return -1;
    }
    return 0;
}

DEFAULT_VISIBILITY
void uc_init(struct uc_struct *uc)
{
    uc->release = loongarch_release;
    uc->reg_read = reg_read;
    uc->reg_write = reg_write;
    uc->reg_reset = reg_reset;
    uc->set_pc = loongarch_set_pc;
    uc->get_pc = loongarch_get_pc;
    uc->cpus_init = loongarch_cpus_init;
    uc->cpu_context_size = offsetof(CPULoongArchState, elf_address);
    uc_common_init(uc);
}
