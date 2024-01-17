/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef perf_h
#define perf_h

#include "info/static_info.h"
#include "common.h"

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
