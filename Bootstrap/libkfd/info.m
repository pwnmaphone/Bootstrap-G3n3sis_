//
//  info.c
//  Bootstrap
//
//  Created by Chris Coding on 1/5/24.
//

#include <stdio.h>
#include "info.h"
#include "../include/include.h"

const char copy_sentinel[16] = "p0up0u was here";
const u64 copy_sentinel_size = sizeof(copy_sentinel);

void info_init(struct kfd* kfd)
{
    /*
     * Initialize the "kfd->info.copy" substructure.
     *
     * Note that the vm_copy() in krkw_helper_grab_free_pages() makes the following assumptions:
     * - The copy size is strictly greater than "msg_ool_size_small".
     * - The src vm_object must have a copy_strategy of MEMORY_OBJECT_COPY_NONE.
     * - The dst vm_object must have a copy_strategy of MEMORY_OBJECT_COPY_SYMMETRIC.
     */
    const u64 msg_ool_size_small = (32 * 1024);
    kfd->info.copy.size = pages(4);
    assert(kfd->info.copy.size > msg_ool_size_small);

    assert_mach(vm_allocate(mach_task_self(), &kfd->info.copy.src_uaddr, kfd->info.copy.size, VM_FLAGS_ANYWHERE | VM_FLAGS_PURGABLE));
    assert_mach(vm_allocate(mach_task_self(), &kfd->info.copy.dst_uaddr, kfd->info.copy.size, VM_FLAGS_ANYWHERE));
    for (u64 offset = pages(0); offset < kfd->info.copy.size; offset += pages(1)) {
        bcopy(copy_sentinel, (void*)(kfd->info.copy.src_uaddr + offset), copy_sentinel_size);
        bcopy(copy_sentinel, (void*)(kfd->info.copy.dst_uaddr + offset), copy_sentinel_size);
    }

    /*
     * Initialize the "kfd->info.env" substructure.
     *
     * Note that:
     * - We boost the file descriptor limit to "maxfilesperproc".
     * - We use the "vid" as a version index to get the offsets and sizes for dynamic types.
     */
    kfd->info.env.pid = getpid();

    thread_identifier_info_data_t data = {};
    thread_info_t info = (thread_info_t)(&data);
    mach_msg_type_number_t count = THREAD_IDENTIFIER_INFO_COUNT;
    assert_mach(thread_info(mach_thread_self(), THREAD_IDENTIFIER_INFO, info, &count));
    kfd->info.env.tid = data.thread_id;

    usize size1 = sizeof(kfd->info.env.maxfilesperproc);
    assert_bsd(sysctlbyname("kern.maxfilesperproc", &kfd->info.env.maxfilesperproc, &size1, NULL, 0));

    struct rlimit rlim = {
        .rlim_cur = kfd->info.env.maxfilesperproc,
        .rlim_max = kfd->info.env.maxfilesperproc
    };
    assert_bsd(setrlimit(RLIMIT_NOFILE, &rlim));
    
    if (@available(iOS 16, *)) {
        usize size2 = sizeof(kfd->info.env.osversion);
        assert_bsd(sysctlbyname("kern.osversion", &kfd->info.env.osversion, &size2, NULL, 0));
        switch (*(u64*)(&kfd->info.env.osversion)) {
            case ios_16_3:
            case ios_16_3_1: {
                kfd->info.env.vid = 0;
                kfd->info.env.ios = true;
                break;
            }
            case ios_16_4:
            case ios_16_4_1:
            case ios_16_5:
            case ios_16_5_1: {
                kfd->info.env.vid = 1;
                kfd->info.env.ios = true;
                break;
            }
            case macos_13_1: {
                kfd->info.env.vid = 2;
                kfd->info.env.ios = false;
                break;
            }
            case macos_13_4: {
                kfd->info.env.vid = 3;
                kfd->info.env.ios = false;
                break;
            }
            default: {
                assert_false("unsupported osversion");
            }
        }
    }
    else {
        kfd->info.env.ios = true;
        if (@available(iOS 15.0, *)) {
            if (@available(iOS 15.4, *)) {
                kfd->info.env.vid = 8;
            }
            else if (@available(iOS 15.2, *)) {
                kfd->info.env.vid = 6;
            }
            else {
                kfd->info.env.vid = 4;
            }
            int ptrAuthVal = 0;
            size_t len = sizeof(ptrAuthVal);
            assert(sysctlbyname("hw.optional.arm.FEAT_PAuth", &ptrAuthVal, &len, NULL, 0) != -1);
            // ^ This seems to fail on iOS 14, luckily there are no differences in offsets between arm64 and arm64e there
            if (ptrAuthVal != 0) {
                kfd->info.env.vid++;
            }
        }
        else if (@available(iOS 14.5, *)) {
            kfd->info.env.vid = 11;
        }
        else if (@available(iOS 14.0, *)) {
            kfd->info.env.vid = 10;
        }
    }

    

    print_i32(kfd->info.env.pid);
    print_u64(kfd->info.env.tid);
    print_u64(kfd->info.env.vid);
    print_bool(kfd->info.env.ios);
    print_string(kfd->info.env.osversion);
    print_u64(kfd->info.env.maxfilesperproc);
}

void info_run(struct kfd* kfd)
{
    timer_start();

    /*
     * current_proc() and current_task()
     */
    assert(kfd->info.kernel.current_proc);
    u64 signed_task_kaddr = dynamic_kget(proc, task, kfd->info.kernel.current_proc);
    kfd->info.kernel.current_task = unsign_kaddr(signed_task_kaddr);
    print_x64(kfd->info.kernel.current_proc);
    print_x64(kfd->info.kernel.current_task);

    /*
     * current_map()
     */
    u64 signed_map_kaddr = dynamic_kget(task, map, kfd->info.kernel.current_task);
    kfd->info.kernel.current_map = unsign_kaddr(signed_map_kaddr);
    print_x64(kfd->info.kernel.current_map);

    /*
     * current_pmap()
     */
    u64 signed_pmap_kaddr = dynamic_kget(vm_map, pmap, kfd->info.kernel.current_map);
    kfd->info.kernel.current_pmap = unsign_kaddr(signed_pmap_kaddr);
    print_x64(kfd->info.kernel.current_pmap);

    /*
     * current_thread() and current_uthread()
     */
    const bool find_current_thread = false;

    if (find_current_thread) {
        u64 thread_kaddr = dynamic_kget(task, threads_next, kfd->info.kernel.current_task);

        while (true) {
            u64 tid = dynamic_kget(thread, thread_id, thread_kaddr);
            if (tid == kfd->info.env.tid) {
                kfd->info.kernel.current_thread = thread_kaddr;
                kfd->info.kernel.current_uthread = thread_kaddr + dynamic_sizeof(thread);
                break;
            }

            thread_kaddr = dynamic_kget(thread, task_threads_next, thread_kaddr);
        }

        print_x64(kfd->info.kernel.current_thread);
        print_x64(kfd->info.kernel.current_uthread);
    }

    if (kfd->info.kernel.kernel_proc) {
        /*
         * kernel_proc() and kernel_task()
         */
        u64 signed_kernel_task = dynamic_kget(proc, task, kfd->info.kernel.kernel_proc);
        kfd->info.kernel.kernel_task = unsign_kaddr(signed_kernel_task);
        print_x64(kfd->info.kernel.kernel_proc);
        print_x64(kfd->info.kernel.kernel_task);

        /*
         * kernel_map()
         */
        u64 signed_map_kaddr = dynamic_kget(task, map, kfd->info.kernel.kernel_task);
        kfd->info.kernel.kernel_map = unsign_kaddr(signed_map_kaddr);
        print_x64(kfd->info.kernel.kernel_map);

        /*
         * kernel_pmap()
         */
        u64 signed_pmap_kaddr = dynamic_kget(vm_map, pmap, kfd->info.kernel.kernel_map);
        kfd->info.kernel.kernel_pmap = unsign_kaddr(signed_pmap_kaddr);
        print_x64(kfd->info.kernel.kernel_pmap);
    }

    timer_end();
}

void info_free(struct kfd* kfd)
{
    assert_mach(vm_deallocate(mach_task_self(), kfd->info.copy.src_uaddr, kfd->info.copy.size));
    assert_mach(vm_deallocate(mach_task_self(), kfd->info.copy.dst_uaddr, kfd->info.copy.size));
}
