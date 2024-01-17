/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef landa_h
#define landa_h

#include <stdatomic.h>
#include "../common.h"
#include "../../libkfd.h"

struct landa_data {
    atomic_bool main_thread_returned;
    atomic_bool spinner_thread_started;
    vm_address_t copy_src_address;
    vm_address_t copy_dst_address;
    vm_size_t copy_size;
};

void landa_init(struct kfd* kfd);

void landa_run(struct kfd* kfd);

void landa_cleanup(struct kfd* kfd);

void landa_free(struct kfd* kfd);

/*
 * Helper landa functions.
 */

void* landa_helper_spinner_pthread(void* arg);

#endif /* landa_h */
