/* Unicorn Emulator Engine */
/* By Nguyen Anh Quynh <aquynh@gmail.com>, 2015 */

/* Created for Unicorn Engine by Amaan Qureshi <amaanq12@gmail.com> */

#ifndef UC_QEMU_TARGET_ALPHA_H
#define UC_QEMU_TARGET_ALPHA_H

// functions to read & write registers
uc_err reg_read_alpha(void *env, int mode, unsigned int regid, void *value,
                      size_t *size);
uc_err reg_write_alpha(void *env, int mode, unsigned int regid,
                       const void *value, size_t *size, int *setpc);

void uc_init_alpha(struct uc_struct *uc);
#endif
