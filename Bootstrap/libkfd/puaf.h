/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef puaf_h
#define puaf_h

#include "puaf/landa.h"
#include "puaf/physpuppet.h"
#include "puaf/smith.h"

#include "../libkfd.h"
#include "common.h"

/*
 * Helper puaf functions.
 */

void puaf_helper_get_vm_map_first_and_last(uint64_t* first_out, uint64_t* last_out);

void puaf_helper_get_vm_map_min_and_max(uint64_t* min_out, uint64_t* max_out);

void puaf_helper_give_ppl_pages(void);

#define puaf_method_case(method)                                 \
    case puaf_##method: {                                        \
        const char* method_name = #method;                       \
        print_string(method_name);                               \
        kfd->puaf.puaf_method_ops.init = method##_init;          \
        kfd->puaf.puaf_method_ops.run = method##_run;            \
        kfd->puaf.puaf_method_ops.cleanup = method##_cleanup;    \
        kfd->puaf.puaf_method_ops.free = method##_free;          \
        break;                                                   \
    }

void puaf_init(struct kfd* kfd, uint64_t puaf_pages, uint64_t puaf_method);

void puaf_run(struct kfd* kfd);

void puaf_cleanup(struct kfd* kfd);

void puaf_free(struct kfd* kfd);


#endif /* puaf_h */
