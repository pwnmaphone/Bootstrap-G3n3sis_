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

#define dynamic_sizeof(object) (object##_versions[kfd->info.env.vid].object_size)

#define dynamic_offsetof(object, field) (object##_versions[kfd->info.env.vid].field)


#define dynamic_uget(object, field, object_uaddr)                                             \
    ({                                                                                        \
        uint64_t field_uaddr = (uint64_t)(object_uaddr) + dynamic_offsetof(object, field);    \
        object##_##field##_t field_value = *(volatile object##_##field##_t*)(field_uaddr);    \
        field_value;                                                                          \
    })


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


#define dynamic_uset(object, field, object_uaddr, field_value)                                   \
    do {                                                                                         \
        uint64_t field_uaddr = (uint64_t)(object_uaddr) + dynamic_offsetof(object, field);       \
        *(volatile object##_##field##_t*)(field_uaddr) = (object##_##field##_t)(field_value);    \
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

// for ios 15 support

#define dynamic_kget2(object, field, object_kaddr)                                   \
    ({                                                                              \
        uint64_t buffer = 0;                                                             \
        uint64_t field_kaddr = (uint64_t)(object_kaddr) + dynamic_offsetof(object, field);    \
        kread((uint64_t)(kfd), (field_kaddr), (&buffer), (sizeof(buffer)));              \
        object##_##field##_t field_value = *(object##_##field##_t*)(&buffer);       \
        field_value;                                                                \
    })

#define dynamic_kset_u64(object, field, object_kaddr, field_value)                  \
    do {                                                                            \
        uint64_t buffer = field_value;                                                   \
        uint64_t field_kaddr = (uint64_t)(object_kaddr) + dynamic_offsetof(object, field);    \
        kwrite((uint64_t)(kfd), (&buffer), (field_kaddr), (sizeof(buffer)));             \
    } while (0)


extern const char info_copy_sentinel[16];
extern const uint64_t info_copy_sentinel_size;

void info_init(struct kfd* kfd);

void info_run(struct kfd* kfd);

void info_free(struct kfd* kfd);

#endif /* info_h */
