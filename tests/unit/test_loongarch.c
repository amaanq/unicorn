#include "unicorn_test.h"

const uint64_t code_start = 0x4000;
const uint64_t code_len = 0x4000;

// # CHECK-ASM-AND-OBJ: add.w $a5, $ra, $s8
// # CHECK-ASM: encoding: [0x29,0x7c,0x10,0x00]
// add.w $a5, $ra, $s8

static void uc_common_setup(uc_engine **uc, uc_arch arch, uc_mode mode,
                            const char *code, uint64_t size)
{
    OK(uc_open(arch, mode, uc));
    OK(uc_mem_map(*uc, code_start, code_len, UC_PROT_ALL));
    OK(uc_mem_write(*uc, code_start, code, size));
}

static bool test_mem(uc_engine *uc, uc_mem_type type, uint64_t addr, int size,
                     int64_t val, void *data)
{
    printf("test_mem: %d %d 0x%x %d %lx\n", type, addr, addr, size, val);
	return false;
}

static void test_code(uc_engine *uc, uint64_t address, uint32_t size,
                      void *user_data)
{
    printf("test_code: %lx %d\n", address, size);
}

static void test_add(void)
{

    uc_engine *uc;
    uint32_t ra = 0x1;
    uint32_t s8 = 0x2;
    uint32_t a5;
    char code[] = {
        0x29, 0x7c, 0x10, 0x00, // add.w $a5, $ra, $s8
    };

    uc_common_setup(&uc, UC_ARCH_LOONGARCH, UC_MODE_LITTLE_ENDIAN, code,
                    sizeof(code));

    OK(uc_reg_write(uc, UC_LOONGARCH_REG_RA, &ra));
    OK(uc_reg_write(uc, UC_LOONGARCH_REG_S8, &s8));

    uc_hook hh;
    OK(uc_hook_add(uc, &hh, UC_HOOK_MEM_VALID | UC_HOOK_MEM_INVALID, test_mem,
                   NULL, 1, 0));
    OK(uc_hook_add(uc, &hh, UC_HOOK_CODE, test_code, NULL, 1, 0));

    OK(uc_emu_start(uc, code_start, code_start + sizeof(code), 0, 0));

    OK(uc_reg_read(uc, UC_LOONGARCH_REG_RA, &ra));
    OK(uc_reg_read(uc, UC_LOONGARCH_REG_S8, &s8));
    OK(uc_reg_read(uc, UC_LOONGARCH_REG_A5, &a5));

    TEST_CHECK(ra == 0x1);
    TEST_CHECK(s8 == 0x2);
    TEST_CHECK(a5 == 0x3);

    OK(uc_close(uc));
}

TEST_LIST = {{"test_add", test_add}, {NULL, NULL}};
