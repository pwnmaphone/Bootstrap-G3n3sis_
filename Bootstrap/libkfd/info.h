/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef info_h
#define info_h

#include "info/dynamic_info.h"
#include "info/static_info.h"
#include "../libkfd.h"
#include "common.h"

/*
 * Note that these macros assume that the kfd pointer is in scope.
 */
#define dynamic_info(field_name)    (kern_versions[kfd->info.env.vid].field_name)

#define dynamic_kget(field_name, object_kaddr)                                    \
    ({                                                                            \
        uint64_t tmp_buffer = 0;                                                       \
        uint64_t field_kaddr = (uint64_t)(object_kaddr) + dynamic_info(field_name);         \
        kread((uint64_t)(kfd), (field_kaddr), (&tmp_buffer), (sizeof(tmp_buffer)));    \
        tmp_buffer;                                                               \
    })


#define dynamic_kset(field_name, new_value, object_kaddr)                          \
    do {                                                                           \
        uint64_t tmp_buffer = new_value;                                                \
        uint64_t field_kaddr = (uint64_t)(object_kaddr) + dynamic_info(field_name);          \
        kwrite((uint64_t)(kfd), (&tmp_buffer), (field_kaddr), (sizeof(tmp_buffer)));    \
    } while (0)

#define static_kget(object_name, field_name, object_kaddr)                            \
    ({                                                                                \
        uint64_t tmp_buffer = 0;                                                           \
        uint64_t field_kaddr = (uint64_t)(object_kaddr) + offsetof(object_name, field_name);    \
        kread((uint64_t)(kfd), (field_kaddr), (&tmp_buffer), (sizeof(tmp_buffer)));        \
        tmp_buffer;                                                                   \
    })

#define static_kset(object_name, field_name, new_value, object_kaddr)                 \
    do {                                                                              \
        uint64_t tmp_buffer = new_value;                                                   \
        uint64_t field_kaddr = (uint64_t)(object_kaddr) + offsetof(object_name, field_name);    \
        kwrite((uint64_t)(kfd), (&tmp_buffer), (field_kaddr), (sizeof(tmp_buffer)));       \
    } while (0)

extern const char info_copy_sentinel[16];
extern const uint64_t info_copy_sentinel_size;

void info_init(struct kfd* kfd);

void info_run(struct kfd* kfd);

void info_free(struct kfd* kfd);

#endif /* info_h */
