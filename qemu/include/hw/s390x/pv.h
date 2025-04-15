/*
 * Protected Virtualization header
 *
 * Copyright IBM Corp. 2020
 * Author(s):
 *  Janosch Frank <frankja@linux.ibm.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or (at
 * your option) any later version. See the COPYING file in the top-level
 * directory.
 */
#ifndef HW_S390_PV_H
#define HW_S390_PV_H

static inline bool s390_is_pv(void) { return false; }
static inline int s390_pv_vm_enable(void) { return 0; }
static inline void s390_pv_vm_disable(void) {}
static inline int s390_pv_set_sec_parms(uint64_t origin, uint64_t length) { return 0; }
static inline int s390_pv_unpack(uint64_t addr, uint64_t size, uint64_t tweak) { return 0; }
static inline void s390_pv_prep_reset(void) {}
static inline int s390_pv_verify(void) { return 0; }
static inline void s390_pv_unshare(void) {}
static inline void s390_pv_inject_reset_error(CPUState *cs) {};

#endif /* HW_S390_PV_H */
