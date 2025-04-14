#include "unicorn_test.h"

const uint64_t code_start = 0xfffffc0000310000;
const uint64_t code_len = 0x4000;

static void uc_common_setup(uc_engine **uc, uc_arch arch, uc_mode mode,
                            const char *code, uint64_t size,
                            uc_cpu_alpha cpu_model)
{
    OK(uc_open(arch, mode, uc));
    OK(uc_ctl_set_cpu_model(*uc, cpu_model));
    OK(uc_mem_map(*uc, code_start, code_len, UC_PROT_ALL));
    OK(uc_mem_write(*uc, code_start, code, size));
}

static bool test_mem(uc_engine *uc, uc_mem_type type, uint64_t addr, int size,
                     int64_t val, void *data)
{
    printf("test_mem: %d %d %lx %d %lx\n", type, addr, addr, size, val);
}

static void test_code(uc_engine *uc, uint64_t address, uint32_t size,
                      void *user_data)
{
    printf("test_code: %lx %d\n", address, size);
}

static void test_add(void)
{

    uc_engine *uc;
    uint32_t r0 = 0x1;
    uint32_t r1 = 0x2;
    char code[] = {
        // 0x10, 0x01, 0x00, 0x20 // addq r0, r1, r0
        0x40,
        0x41,
        0x00,
        0x03,
    };

    uc_common_setup(&uc, UC_ARCH_ALPHA, UC_MODE_LITTLE_ENDIAN, code,
                    sizeof(code), UC_CPU_ALPHA_EV4);

    OK(uc_reg_write(uc, UC_ALPHA_REG_R0, &r0));
    OK(uc_reg_write(uc, UC_ALPHA_REG_R1, &r1));

    uc_hook hh;
    OK(uc_hook_add(uc, &hh, UC_HOOK_MEM_VALID | UC_HOOK_MEM_INVALID, test_mem,
                   NULL, 1, 0));
    OK(uc_hook_add(uc, &hh, UC_HOOK_CODE, test_code, NULL, 1, 0));

    OK(uc_emu_start(uc, code_start, code_start + sizeof(code), 0, 0));

    OK(uc_reg_read(uc, UC_ALPHA_REG_R0, &r0));
    OK(uc_reg_read(uc, UC_ALPHA_REG_R1, &r1));

    TEST_CHECK(r0 == 0x3);
    TEST_CHECK(r1 == 0x2);

    OK(uc_close(uc));
}

TEST_LIST = {{"test_add", test_add}, {NULL, NULL}};
