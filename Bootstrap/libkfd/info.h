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
 * Helper macros for dynamic types.
 */

#define dynamic_sizeof(object) (object##_versions[kfd->info.env.vid].object_size)

#define dynamic_offsetof(object, field) (object##_versions[kfd->info.env.vid].field)

#define dynamic_uget(object, field, object_uaddr)                                             \
    ({                                                                                        \
        u64 field_uaddr = (u64)(object_uaddr) + dynamic_offsetof(object, field);              \
        object##_##field##_t field_value = *(volatile object##_##field##_t*)(field_uaddr);    \
        field_value;                                                                          \
    })

#define dynamic_uset(object, field, object_uaddr, field_value)                                   \
    do {                                                                                         \
        u64 field_uaddr = (u64)(object_uaddr) + dynamic_offsetof(object, field);                 \
        *(volatile object##_##field##_t*)(field_uaddr) = (object##_##field##_t)(field_value);    \
    } while (0)

#define dynamic_kget(object, field, object_kaddr)                                   \
    ({                                                                              \
        u64 buffer = 0;                                                             \
        u64 field_kaddr = (u64)(object_kaddr) + dynamic_offsetof(object, field);    \
        kread((u64)(kfd), (field_kaddr), (&buffer), (sizeof(buffer)));              \
        object##_##field##_t field_value = *(object##_##field##_t*)(&buffer);       \
        field_value;                                                                \
    })

#define dynamic_kset_u64(object, field, object_kaddr, field_value)                  \
    do {                                                                            \
        u64 buffer = field_value;                                                   \
        u64 field_kaddr = (u64)(object_kaddr) + dynamic_offsetof(object, field);    \
        kwrite((u64)(kfd), (&buffer), (field_kaddr), (sizeof(buffer)));             \
    } while (0)

/*
 * Helper macros for static types.
 */

#define static_sizeof(object) (sizeof(struct object))

#define static_offsetof(object, field) (offsetof(struct object, field))

#define static_uget(object, field, object_uaddr) (((volatile struct object*)(object_uaddr))->field)

#define static_uset(object, field, object_uaddr, field_value)                  \
    do {                                                                       \
        (((volatile struct object*)(object_uaddr))->field = (field_value));    \
    } while (0)

#define static_kget(object, field_type, field, object_kaddr)                       \
    ({                                                                             \
        u64 buffer = 0;                                                            \
        u64 field_kaddr = (u64)(object_kaddr) + static_offsetof(object, field);    \
        kread((u64)(kfd), (field_kaddr), (&buffer), (sizeof(buffer)));             \
        field_type field_value = *(field_type*)(&buffer);                          \
        field_value;                                                               \
    })

#define static_kset_u64(object, field, object_kaddr, field_value)                  \
    do {                                                                           \
        u64 buffer = field_value;                                                  \
        u64 field_kaddr = (u64)(object_kaddr) + static_offsetof(object, field);    \
        kwrite((u64)(kfd), (&buffer), (field_kaddr), (sizeof(buffer)));            \
    } while (0)

static const u64 ios_16_0_a   = 0x0000373533413032; // 20A357
static const u64 ios_16_0_b   = 0x0000323633413032; // 20A362
static const u64 ios_16_0_1   = 0x0000313733413032; // 20A371
static const u64 ios_16_0_2   = 0x0000303833413032; // 20A380
static const u64 ios_16_0_3   = 0x0000323933413032; // 20A392
static const u64 ios_16_1     = 0x0000003238423032; // 20B82
static const u64 ios_16_1_1   = 0x0000313031423032; // 20B101
static const u64 ios_16_1_2   = 0x0000303131423032; // 20B110
static const u64 ios_16_2     = 0x0000003536433032; // 20C65
static const u64 ios_16_3     = 0x0000003734443032; // 20D47
static const u64 ios_16_3_1   = 0x0000003736443032; // 20D67
static const u64 ios_16_4     = 0x0000373432453032; // 20E247
static const u64 ios_16_4_1   = 0x0000323532453032; // 20E252
static const u64 ios_16_5     = 0x0000003636463032; // 20F66
static const u64 ios_16_5_1   = 0x0000003537463032; // 20F75

static const u64 macos_13_0   = 0x0000303833413232; // 22A380
static const u64 macos_13_0_1 = 0x0000303034413232; // 22A400
static const u64 macos_13_1   = 0x0000003536433232; // 22C65
static const u64 macos_13_2   = 0x0000003934443232; // 22D49
static const u64 macos_13_2_1 = 0x0000003836443232; // 22D68
static const u64 macos_13_3   = 0x0000323532453232; // 22E252
static const u64 macos_13_3_1 = 0x0000313632453232; // 22E261
static const u64 macos_13_4   = 0x0000003636463232; // 22F66

//#define t1sz_boot (17ull)
#define t1sz_boot (25ull)
#define ptr_mask ((1ull << (64ull - t1sz_boot)) - 1ull)
#define pac_mask (~ptr_mask)
#define unsign_kaddr(kaddr) ((kaddr) | (pac_mask))

extern const char copy_sentinel[16];
extern const u64 copy_sentinel_size;

void info_init(struct kfd* kfd);

void info_run(struct kfd* kfd);

void info_free(struct kfd* kfd);

#endif /* info_h */
