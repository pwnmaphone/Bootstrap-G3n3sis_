/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef kread_kqueue_workloop_ctl_h
#define kread_kqueue_workloop_ctl_h

#include "../../../libkfd.h"

#include "../../info/static_info.h"
#include "../../common.h"

void kread_kqueue_workloop_ctl_init(struct kfd* kfd);

void kread_kqueue_workloop_ctl_allocate(struct kfd* kfd, uint64_t id);

bool kread_kqueue_workloop_ctl_search(struct kfd* kfd, uint64_t object_uaddr);

void kread_kqueue_workloop_ctl_kread(struct kfd* kfd, uint64_t kaddr, void* uaddr, uint64_t size);

void kread_kqueue_workloop_ctl_find_proc(struct kfd* kfd);

void kread_kqueue_workloop_ctl_deallocate(struct kfd* kfd, uint64_t id);

void kread_kqueue_workloop_ctl_free(struct kfd* kfd);

/*
 * 64-bit kread function.
 */

uint64_t kread_kqueue_workloop_ctl_kread_u64(struct kfd* kfd, uint64_t kaddr);

#endif /* kread_kqueue_workloop_ctl_h */
