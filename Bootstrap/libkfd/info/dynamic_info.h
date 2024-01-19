/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef dynamic_info_h
#define dynamic_info_h

#include "../common.h"
#include "static_info.h"

struct kqworkloop {
    u64 kqwl_state;
    u64 kqwl_p;
    u64 kqwl_owner;
    u64 kqwl_dynamicid;
    u64 object_size;
};

static const struct kqworkloop kqworkloop_versions[] = {
    { .kqwl_state = 0x10, .kqwl_p = 0x18, .kqwl_owner = 0xd0, .kqwl_dynamicid = 0xe8, .object_size = 0x108 },
    { .kqwl_state = 0x10, .kqwl_p = 0x18, .kqwl_owner = 0xd0, .kqwl_dynamicid = 0xe8, .object_size = 0x108 },
    { .kqwl_state = 0x10, .kqwl_p = 0x18, .kqwl_owner = 0xd0, .kqwl_dynamicid = 0xe8, .object_size = 0x108 },
    { .kqwl_state = 0x10, .kqwl_p = 0x18, .kqwl_owner = 0xd0, .kqwl_dynamicid = 0xe8, .object_size = 0x108 },
    
    { .kqwl_state = 0x10, .kqwl_p = 0x18, .kqwl_owner = 0xd0, .kqwl_dynamicid = 0xe8, .object_size = 0x108 }, // iOS 15.0 - 15.1.1 arm64
    { .kqwl_state = 0x10, .kqwl_p = 0x18, .kqwl_owner = 0xd0, .kqwl_dynamicid = 0xe8, .object_size = 0x108 }, // iOS 15.0 - 15.1.1 arm64e
    
    { .kqwl_state = 0x10, .kqwl_p = 0x18, .kqwl_owner = 0xd0, .kqwl_dynamicid = 0xe8, .object_size = 0x108 }, // iOS 15.2 - 15.3.1 arm64
    { .kqwl_state = 0x10, .kqwl_p = 0x18, .kqwl_owner = 0xd0, .kqwl_dynamicid = 0xe8, .object_size = 0x108 }, // iOS 15.2 - 15.3.1 arm64e
    
    { .kqwl_state = 0x10, .kqwl_p = 0x18, .kqwl_owner = 0xd0, .kqwl_dynamicid = 0xe8, .object_size = 0x108 }, // iOS 15.4 - 15.7.8 arm64
    { .kqwl_state = 0x10, .kqwl_p = 0x18, .kqwl_owner = 0xd0, .kqwl_dynamicid = 0xe8, .object_size = 0x108 }, // iOS 15.4 - 15.7.2 arm64e
    
    { .kqwl_state = 0x10, .kqwl_p = 0x18, .kqwl_owner = 0xd0, .kqwl_dynamicid = 0xe8, .object_size = 0x108 }, // iOS 14.0 - 14.4
    { .kqwl_state = 0x10, .kqwl_p = 0x18, .kqwl_owner = 0xd0, .kqwl_dynamicid = 0xe8, .object_size = 0x108 }, // iOS 14.5 - 14.8.1
};

typedef u16 kqworkloop_kqwl_state_t;
typedef u64 kqworkloop_kqwl_p_t;
typedef u64 kqworkloop_kqwl_owner_t;
typedef u64 kqworkloop_kqwl_dynamicid_t;

struct proc {
    u64 p_list_le_next;
    u64 p_list_le_prev;
    u64 task;
    u64 p_pid;
    u64 p_fd_fd_ofiles;
    u64 object_size;
};

static const struct proc proc_versions[] = {
    { .p_list_le_next = 0x0, .p_list_le_prev = 0x8, .task = 0x10, .p_pid = 0x60, .p_fd_fd_ofiles = 0xf8, .object_size = 0x538 },
    { .p_list_le_next = 0x0, .p_list_le_prev = 0x8, .task = 0x10, .p_pid = 0x60, .p_fd_fd_ofiles = 0xf8, .object_size = 0x730 },
    { .p_list_le_next = 0x0, .p_list_le_prev = 0x8, .task = 0x10, .p_pid = 0x60, .p_fd_fd_ofiles = 0xf8, .object_size = 0x580 },
    { .p_list_le_next = 0x0, .p_list_le_prev = 0x8, .task = 0x10, .p_pid = 0x60, .p_fd_fd_ofiles = 0xf8, .object_size = 0x778 },
    
    // Note: sizes below here are wrong idc
    { .p_list_le_next = 0x0, .p_list_le_prev = 0x8, .task = 0x10, .p_pid = 0x68, .p_fd_fd_ofiles = 0x110, .object_size = 0x4B0 }, // iOS 15.0 - 15.1.1 arm64
    { .p_list_le_next = 0x0, .p_list_le_prev = 0x8, .task = 0x10, .p_pid = 0x68, .p_fd_fd_ofiles = 0x100, .object_size = 0x4B0 }, // iOS 15.0 - 15.1.1 arm64e
    
    { .p_list_le_next = 0x0, .p_list_le_prev = 0x8, .task = 0x10, .p_pid = 0x68, .p_fd_fd_ofiles = 0xf8, .object_size = 0x4B0 }, // iOS 15.2 - 15.3.1 arm64
    { .p_list_le_next = 0x0, .p_list_le_prev = 0x8, .task = 0x10, .p_pid = 0x68, .p_fd_fd_ofiles = 0xf8, .object_size = 0x4B0 }, // iOS 15.2 - 15.3.1 arm64e
    
    { .p_list_le_next = 0x0, .p_list_le_prev = 0x8, .task = 0x10, .p_pid = 0x68, .p_fd_fd_ofiles = 0xf8, .object_size = 0x4B0 }, // iOS 15.4 - 15.7.8 arm64
    { .p_list_le_next = 0x0, .p_list_le_prev = 0x8, .task = 0x10, .p_pid = 0x68, .p_fd_fd_ofiles = 0xf8, .object_size = 0x4B0 }, // iOS 15.4 - 15.7.2 arm64e
    
    { .p_list_le_next = 0x0, .p_list_le_prev = 0x8, .task = 0x10, .p_pid = 0x68, .p_fd_fd_ofiles = 0xf8, .object_size = 0x4B0 }, // iOS 14.0 - 14.4
    { .p_list_le_next = 0x0, .p_list_le_prev = 0x8, .task = 0x10, .p_pid = 0x68, .p_fd_fd_ofiles = 0xf8, .object_size = 0x4B0 }, // iOS 14.5 - 14.8.1
};

typedef u64 proc_p_list_le_next_t;
typedef u64 proc_p_list_le_prev_t;
typedef u64 proc_task_t;
typedef i32 proc_p_pid_t;
typedef u64 proc_p_fd_fd_ofiles_t;

struct task {
    u64 map;
    u64 threads_next;
    u64 threads_prev;
    u64 itk_space;
    u64 object_size;
};

static const struct task task_versions[] = {
    { .map = 0x28, .threads_next = 0x58, .threads_prev = 0x60, .itk_space = 0x300, .object_size = 0x648 },
    { .map = 0x28, .threads_next = 0x58, .threads_prev = 0x60, .itk_space = 0x300, .object_size = 0x640 },
    { .map = 0x28, .threads_next = 0x58, .threads_prev = 0x60, .itk_space = 0x300, .object_size = 0x658 },
    { .map = 0x28, .threads_next = 0x58, .threads_prev = 0x60, .itk_space = 0x300, .object_size = 0x658 },
    
    // Note: sizes below here are wrong idc
    { .map = 0x28, .threads_next = 0x58, .threads_prev = 0x60, .itk_space = 0x330, .object_size = 0x6B0 }, // iOS 15.0 - 15.1.1 arm64
    { .map = 0x28, .threads_next = 0x58, .threads_prev = 0x60, .itk_space = 0x330, .object_size = 0x6B0 }, // iOS 15.0 - 15.1.1 arm64e
    
    { .map = 0x28, .threads_next = 0x58, .threads_prev = 0x60, .itk_space = 0x308, .object_size = 0x6B0 }, // iOS 15.2 - 15.3.1 arm64
    { .map = 0x28, .threads_next = 0x58, .threads_prev = 0x60, .itk_space = 0x308, .object_size = 0x6B0 }, // iOS 15.2 - 15.3.1 arm64e
    
    { .map = 0x28, .threads_next = 0x58, .threads_prev = 0x60, .itk_space = 0x308, .object_size = 0x6B0 }, // iOS 15.4 - 15.7.8 arm64
    { .map = 0x28, .threads_next = 0x58, .threads_prev = 0x60, .itk_space = 0x308, .object_size = 0x6B0 }, // iOS 15.4 - 15.7.2 arm64e
    
    { .map = 0x28, .threads_next = 0x58, .threads_prev = 0x60, .itk_space = 0x330, .object_size = 0x6B0 }, // iOS 14.0 - 14.4
    { .map = 0x20, .threads_next = 0x50, .threads_prev = 0x58, .itk_space = 0x338, .object_size = 0x6B0 }, // iOS 14.5 - 14.8.1
};

typedef u64 task_map_t;
typedef u64 task_threads_next_t;
typedef u64 task_threads_prev_t;
typedef u64 task_itk_space_t;

struct thread {
    u64 task_threads_next;
    u64 task_threads_prev;
    u64 map;
    u64 thread_id;
    u64 object_size;
};

static const struct thread thread_versions[] = {
    { .task_threads_next = 0x368, .task_threads_prev = 0x370, .map = 0x380, .thread_id = 0x420, .object_size = 0x4c8 },
    { .task_threads_next = 0x368, .task_threads_prev = 0x370, .map = 0x380, .thread_id = 0x418, .object_size = 0x4c0 },
    { .task_threads_next = 0x3c0, .task_threads_prev = 0x3c8, .map = 0x3d8, .thread_id = 0x490, .object_size = 0x650 },
    { .task_threads_next = 0x3c0, .task_threads_prev = 0x3c8, .map = 0x3d8, .thread_id = 0x490, .object_size = 0x650 },
    
    // Note: sizes below here are wrong idc
    { .task_threads_next = 0x420, .task_threads_prev = 0x428, .map = 0x438, .thread_id = 0x578, .object_size = 0x610 }, // iOS 15.0 - 15.1.1 arm64
    { .task_threads_next = 0x400, .task_threads_prev = 0x408, .map = 0x418, .thread_id = 0x560, .object_size = 0x610 }, // iOS 15.0 - 15.1.1 arm64e
    
    { .task_threads_next = 0x3b0, .task_threads_prev = 0x3b8, .map = 0x3c8, .thread_id = 0x460, .object_size = 0x610 }, // iOS 15.2 - 15.3.1 arm64
    { .task_threads_next = 0x388, .task_threads_prev = 0x390, .map = 0x3a0, .thread_id = 0x438, .object_size = 0x610 }, // iOS 15.2 - 15.3.1 arm64e
    
    { .task_threads_next = 0x3a8, .task_threads_prev = 0x3b0, .map = 0x3c0, .thread_id = 0x458, .object_size = 0x610 }, // iOS 15.4 - 15.7.8 arm64
    { .task_threads_next = 0x388, .task_threads_prev = 0x390, .map = 0x3a0, .thread_id = 0x440, .object_size = 0x610 }, // iOS 15.4 - 15.7.2 arm64e
    
    { .task_threads_next = 0x420, .task_threads_prev = 0x428, .map = 0x438, .thread_id = 0x578, .object_size = 0x610 }, // iOS 14.0 - 14.4
    { .task_threads_next = 0x420, .task_threads_prev = 0x428, .map = 0x438, .thread_id = 0x578, .object_size = 0x610 }, // iOS 14.5 - 14.8.1
};

typedef u64 thread_task_threads_next_t;
typedef u64 thread_task_threads_prev_t;
typedef u64 thread_map_t;
typedef u64 thread_thread_id_t;

struct uthread {
    u64 object_size;
};

static const struct uthread uthread_versions[] = {
    { .object_size = 0x200 },
    { .object_size = 0x200 },
    { .object_size = 0x1b0 },
    { .object_size = 0x1b0 },
    
    // Note: sizes below here are wrong idc
    { .object_size = 0x1b0 }, // iOS 15.0 - 15.1.1 arm64
    { .object_size = 0x1b0 }, // iOS 15.0 - 15.1.1 arm64e
    
    { .object_size = 0x1b0 }, // iOS 15.2 - 15.3.1 arm64
    { .object_size = 0x1b0 }, // iOS 15.2 - 15.3.1 arm64e
    
    { .object_size = 0x1b0 }, // iOS 15.4 - 15.7.8 arm64
    { .object_size = 0x1b0 }, // iOS 15.4 - 15.7.8 arm64e
    
    { .object_size = 0x1b0 }, // iOS 14.0 - 14.4
    { .object_size = 0x1b0 }, // iOS 14.5 - 14.8.1
};


struct vm_map {
    u64 hdr_links_prev;
    u64 hdr_links_next;
    u64 min_offset;
    u64 max_offset;
    u64 hdr_nentries;
    u64 hdr_nentries_u64;
    u64 hdr_rb_head_store_rbh_root;
    u64 pmap;
    u64 hint;
    u64 hole_hint;
    u64 holes_list;
    u64 object_size;
};

static const struct vm_map vm_map_versions[] = {
    {
        .hdr_links_prev = 0x10,
        .hdr_links_next = 0x18,
        .min_offset = 0x20,
        .max_offset = 0x28,
        .hdr_nentries = 0x30,
        .hdr_nentries_u64 = 0x30,
        .hdr_rb_head_store_rbh_root = 0x38,
        .pmap = 0x40,
        .hint = 0x98,
        .hole_hint = 0xa0,
        .holes_list = 0xa8,
        .object_size = 0xc0,
    },
    {
        .hdr_links_prev = 0x10,
        .hdr_links_next = 0x18,
        .min_offset = 0x20,
        .max_offset = 0x28,
        .hdr_nentries = 0x30,
        .hdr_nentries_u64 = 0x30,
        .hdr_rb_head_store_rbh_root = 0x38,
        .pmap = 0x40,
        .hint = 0x98,
        .hole_hint = 0xa0,
        .holes_list = 0xa8,
        .object_size = 0xc0,
    },
    {
        .hdr_links_prev = 0x10,
        .hdr_links_next = 0x18,
        .min_offset = 0x20,
        .max_offset = 0x28,
        .hdr_nentries = 0x30,
        .hdr_nentries_u64 = 0x30,
        .hdr_rb_head_store_rbh_root = 0x38,
        .pmap = 0x40,
        .hint = 0x80,
        .hole_hint = 0x88,
        .holes_list = 0x90,
        .object_size = 0xa8,
    },
    {
        .hdr_links_prev = 0x10,
        .hdr_links_next = 0x18,
        .min_offset = 0x20,
        .max_offset = 0x28,
        .hdr_nentries = 0x30,
        .hdr_nentries_u64 = 0x30,
        .hdr_rb_head_store_rbh_root = 0x38,
        .pmap = 0x40,
        .hint = 0x80,
        .hole_hint = 0x88,
        .holes_list = 0x90,
        .object_size = 0xa8,
    },
    
    {
        .hdr_links_prev = 0x10,
        .hdr_links_next = 0x18,
        .min_offset = 0x20,
        .max_offset = 0x28,
        .hdr_nentries = 0x30,
        .hdr_nentries_u64 = 0x30,
        .hdr_rb_head_store_rbh_root = 0x40,
        .pmap = 0x48,
        .hint = 0x100,
        .hole_hint = 0x108,
        .holes_list = 0x110,
        .object_size = 0x128,
    }, // iOS 15.0 - 15.1.1 arm64
    {
        .hdr_links_prev = 0x10,
        .hdr_links_next = 0x18,
        .min_offset = 0x20,
        .max_offset = 0x28,
        .hdr_nentries = 0x30,
        .hdr_nentries_u64 = 0x30,
        .hdr_rb_head_store_rbh_root = 0x40,
        .pmap = 0x48,
        .hint = 0x100,
        .hole_hint = 0x108,
        .holes_list = 0x110,
        .object_size = 0x128,
    }, // iOS 15.0 - 15.1.1 arm64e
    
    {
        .hdr_links_prev = 0x10,
        .hdr_links_next = 0x18,
        .min_offset = 0x20,
        .max_offset = 0x28,
        .hdr_nentries = 0x30,
        .hdr_nentries_u64 = 0x30,
        .hdr_rb_head_store_rbh_root = 0x40,
        .pmap = 0x48,
        .hint = 0x100,
        .hole_hint = 0x108,
        .holes_list = 0x110,
        .object_size = 0x128,
    }, // iOS 15.2 - 15.3.1 arm64
    {
        .hdr_links_prev = 0x10,
        .hdr_links_next = 0x18,
        .min_offset = 0x20,
        .max_offset = 0x28,
        .hdr_nentries = 0x30,
        .hdr_nentries_u64 = 0x30,
        .hdr_rb_head_store_rbh_root = 0x40,
        .pmap = 0x48,
        .hint = 0x100,
        .hole_hint = 0x108,
        .holes_list = 0x110,
        .object_size = 0x128,
    }, // iOS 15.2 - 15.3.1 arm64e
    
    {
        .hdr_links_prev = 0x10,
        .hdr_links_next = 0x18,
        .min_offset = 0x20,
        .max_offset = 0x28,
        .hdr_nentries = 0x30,
        .hdr_nentries_u64 = 0x30,
        .hdr_rb_head_store_rbh_root = 0x38,
        .pmap = 0x40,
        .hint = 0x78,
        .hole_hint = 0x80,
        .holes_list = 0x88,
        .object_size = 0xa0,
    }, // iOS 15.4 - 15.7.8 arm64
    {
        .hdr_links_prev = 0x10,
        .hdr_links_next = 0x18,
        .min_offset = 0x20,
        .max_offset = 0x28,
        .hdr_nentries = 0x30,
        .hdr_nentries_u64 = 0x30,
        .hdr_rb_head_store_rbh_root = 0x38,
        .pmap = 0x40,
        .hint = 0x80,
        .hole_hint = 0x80,
        .holes_list = 0x88,
        .object_size = 0xa0,
    }, // iOS 15.4 - 15.7.8 arm64e
    {
        .hdr_links_prev = 0x10,
        .hdr_links_next = 0x18,
        .min_offset = 0x20,
        .max_offset = 0x28,
        .hdr_nentries = 0x30,
        .hdr_nentries_u64 = 0x30,
        .hdr_rb_head_store_rbh_root = 0x40,//different start
        .pmap = 0x48,
        .hint = 0xf0,
        .hole_hint = 0xf8,
        .holes_list = 0x100,
        .object_size = 0x120,
    }, // iOS 14.0 - 14.4
    {
        .hdr_links_prev = 0x10,
        .hdr_links_next = 0x18,
        .min_offset = 0x20,
        .max_offset = 0x28,
        .hdr_nentries = 0x30,
        .hdr_nentries_u64 = 0x30,
        .hdr_rb_head_store_rbh_root = 0x40,//different start
        .pmap = 0x48,
        .hint = 0xf0,
        .hole_hint = 0xf8,
        .holes_list = 0x100,
        .object_size = 0x120,
    }, // iOS 14.5 - 14.8.1
};

typedef u64 vm_map_hdr_links_prev_t;
typedef u64 vm_map_hdr_links_next_t;
typedef u64 vm_map_min_offset_t;
typedef u64 vm_map_max_offset_t;
typedef i32 vm_map_hdr_nentries_t;
typedef u64 vm_map_hdr_nentries_u64_t;
typedef u64 vm_map_hdr_rb_head_store_rbh_root_t;
typedef u64 vm_map_pmap_t;
typedef u64 vm_map_hint_t;
typedef u64 vm_map_hole_hint_t;
typedef u64 vm_map_holes_list_t;

struct _vm_map {
    u64 lock[2];
    struct vm_map_header *hdr;
    u64 pmap;
    u64 size;
    u64 size_limit;
    u64 data_limit;
    u64 user_wire_limit;
    u64 user_wire_size;
#if TARGET_MACOS
    u64 vmmap_high_start;
#else /* TARGET_MACOS */
    u64 user_range[4];
#endif /* TARGET_MACOS */
    union {
        u64 vmu1_highest_entry_end;
        u64 vmu1_lowest_unnestable_start;
    } vmu1;
    u64 hint;
    union {
        u64 vmmap_hole_hint;
        u64 vmmap_corpse_footprint;
    } vmmap_u_1;
    union {
        u64 _first_free;
        u64 _holes;
    } f_s;
    u32 map_refcnt;
    u32
        wait_for_space:1,
        wiring_required:1,
        no_zero_fill:1,
        mapped_in_other_pmaps:1,
        switch_protect:1,
        disable_vmentry_reuse:1,
        map_disallow_data_exec:1,
        holelistenabled:1,
        is_nested_map:1,
        map_disallow_new_exec:1,
        jit_entry_exists:1,
        has_corpse_footprint:1,
        terminated:1,
        is_alien:1,
        cs_enforcement:1,
        cs_debugged:1,
        reserved_regions:1,
        single_jit:1,
        never_faults:1,
        uses_user_ranges:1,
        pad:12;
    u32 timestamp;
};

struct IOSurface {
    u64 isa;
    u64 PixelFormat;
    u64 AllocSize;
    u64 UseCountPtr;
    u64 IndexedTimestampPtr;
    
    u64 ReadDisplacement;
};

static const struct IOSurface IOSurface_versions[] = {
    // iOS 16 is left to the educated reader to figure out (keep in mind it will only work on arm64)
    { },
    { },
    { },
    { },
    
    { .isa = 0x0, .PixelFormat = 0xA4, .AllocSize = 0xAC, .UseCountPtr = 0xC0, .IndexedTimestampPtr = 0x360, .ReadDisplacement = 0x14 }, // iOS 15.0 - 15.1.1 arm64
    { .isa = 0x0, .PixelFormat = 0xA4, .AllocSize = 0xAC, .UseCountPtr = 0xC0, .IndexedTimestampPtr = 0x360, .ReadDisplacement = 0x14 }, // iOS 15.0 - 15.1.1 arm64e
    
    { .isa = 0x0, .PixelFormat = 0xA4, .AllocSize = 0xAC, .UseCountPtr = 0xC0, .IndexedTimestampPtr = 0x360, .ReadDisplacement = 0x14 }, // iOS 15.2 - 15.3.1 arm64
    { .isa = 0x0, .PixelFormat = 0xA4, .AllocSize = 0xAC, .UseCountPtr = 0xC0, .IndexedTimestampPtr = 0x360, .ReadDisplacement = 0x14 }, // iOS 15.2 - 15.3.1 arm64e
    
    { .isa = 0x0, .PixelFormat = 0xA4, .AllocSize = 0xAC, .UseCountPtr = 0xC0, .IndexedTimestampPtr = 0x360, .ReadDisplacement = 0x14 }, // iOS 15.4 - 15.7.8 arm64
    { .isa = 0x0, .PixelFormat = 0xA4, .AllocSize = 0xAC, .UseCountPtr = 0xC0, .IndexedTimestampPtr = 0x360, .ReadDisplacement = 0x14 }, // iOS 15.4 - 15.7.2 arm64e
    
    { .isa = 0x0, .PixelFormat = 0xA4, .AllocSize = 0xAC, .UseCountPtr = 0xC0, .IndexedTimestampPtr = 0x360, .ReadDisplacement = 0x14 }, // iOS 14.0 - 14.4
    { .isa = 0x0, .PixelFormat = 0xA4, .AllocSize = 0xAC, .UseCountPtr = 0xC0, .IndexedTimestampPtr = 0x360, .ReadDisplacement = 0x14 }, // iOS 14.5 - 14.8.1
    
    
};

typedef u64 IOSurface_isa_t;
typedef u32 IOSurface_PixelFormat_t;
typedef u32 IOSurface_AllocSize_t;
typedef u64 IOSurface_UseCountPtr_t;
typedef u64 IOSurface_IndexedTimestampPtr_t;
typedef u32 IOSurface_ReadDisplacement_t;



struct dynamic_info {
    const char* kern_version;
    bool kread_kqueue_workloop_ctl_supported;
    bool perf_supported;
    // struct proc
    u64 proc__p_list__le_prev;
    u64 proc__p_pid;
    u64 proc__p_fd__fd_ofiles;
    u64 proc__object_size;
    // struct task
    u64 task__map;
    // struct thread
    u64 thread__thread_id;
    // kernelcache static addresses (perf)
    u64 kernelcache__cdevsw;                          // "spec_open type" or "Can't mark ptc as kqueue ok"
    u64 kernelcache__gPhysBase;                       // "%s: illegal PA: 0x%llx; phys base 0x%llx, size 0x%llx"
    u64 kernelcache__gPhysSize;                       // (gPhysBase + 0x8)
    u64 kernelcache__gVirtBase;                       // "%s: illegal PA: 0x%llx; phys base 0x%llx, size 0x%llx"
    u64 kernelcache__perfmon_dev_open;                // "perfmon: attempt to open unsupported source: 0x%x"
    u64 kernelcache__perfmon_devices;                 // "perfmon: %s: devfs_make_node_clone failed"
    u64 kernelcache__ptov_table;                      // "%s: illegal PA: 0x%llx; phys base 0x%llx, size 0x%llx"
    u64 kernelcache__vn_kqfilter;                     // "Invalid knote filter on a vnode!"
};

static struct dynamic_info kern_versions[] = {
    // iOS 16.x / arm64e
    {
        .kern_version = "Darwin Kernel Version 22.x.x / arm64e",
        .kread_kqueue_workloop_ctl_supported = false,
        .perf_supported = true,
        .proc__p_list__le_prev = 0x0008,
        .proc__p_pid = 0x0060,
        .proc__p_fd__fd_ofiles = 0x00f8,
        .proc__object_size = 0,
        .task__map = 0x0028,
        .thread__thread_id = 0,
        .kernelcache__cdevsw = 0,
        .kernelcache__gPhysBase = 0,
        .kernelcache__gPhysSize = 0,
        .kernelcache__gVirtBase = 0,
        .kernelcache__perfmon_dev_open = 0,
        .kernelcache__perfmon_devices = 0,
        .kernelcache__ptov_table = 0,
        .kernelcache__vn_kqfilter = 0,
    },
};
#endif /* dynamic_info_h */
