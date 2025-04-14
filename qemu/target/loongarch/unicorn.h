/* Unicorn Emulator Engine */
/* By Nguyen Anh Quynh <aquynh@gmail.com>, 2015 */

/* Created for Unicorn Engine by Amaan Qureshi <amaanq12@gmail.com>, 2025
   Copyright 2025 Amaan Qureshi
*/

#ifndef UC_QEMU_TARGET_LOONGARCH_H
#define UC_QEMU_TARGET_LOONGARCH_H

// functions to read & write registers
uc_err reg_read_loongarch(void *env, int mode, unsigned int regid, void *value,
                        size_t *size);
uc_err reg_write_loongarch(void *env, int mode, unsigned int regid,
                         const void *value, size_t *size, int *setpc);

void uc_init_loongarch(struct uc_struct *uc);
#endif
