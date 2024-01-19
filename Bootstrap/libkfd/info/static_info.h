/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef static_info_h
#define static_info_h

#include "../../libkfd.h"
#include <sys/ioctl.h>

struct fileglob {
    struct {
        u64 le_next;
        u64 le_prev;
    } _msglist;
    u32 fg_flag;
    u32 fg_count;
    u32 fg_msgcount;
    i32 fg_lflags;
    u64 fg_cred;
    u64 fg_ops;
    i64 fg_offset;
    u64 fg_data;
    u64 fg_vn_data;
    u64 fg_lock[2];
};


typedef enum {
    DTYPE_VNODE = 1,
    DTYPE_SOCKET,
    DTYPE_PSXSHM,
    DTYPE_PSXSEM,
    DTYPE_KQUEUE,
    DTYPE_PIPE,
    DTYPE_FSEVENTS,
    DTYPE_ATALK,
    DTYPE_NETPOLICY,
    DTYPE_CHANNEL,
    DTYPE_NEXUS
} file_type_t;

struct fileops {
    file_type_t fo_type;
    void* fo_read;
    void* fo_write;
    void* fo_ioctl;
    void* fo_select;
    void* fo_close;
    void* fo_kqfilter;
    void* fo_drain;
};

struct fileproc_guard {
    u64 fpg_wset;
    u64 fpg_guard;
};

struct fileproc {
    u32 fp_iocount;
    u32 fp_vflags;
    u16 fp_flags;
    u16 fp_guard_attrs;
    u64 fp_glob;
    union {
        u64 fp_wset;
        u64 fp_guard;
    };
};

struct ipc_entry {
    union {
        u64 ie_object;
        u64 ie_volatile_object;
    };
    u32 ie_bits;
    u32 ie_dist:12;
    u32 ie_index:32;
    union {
        u32 ie_next;
        u32 ie_request;
    };
};


struct ipc_port {
    struct {
        u32 io_bits;
        u32 io_references;
    } ip_object;
    u64 ip_waitq_and_ip_messages[7];
    union {
        u64 ip_receiver;
        u64 ip_destination;
        u32 ip_timestamp;
    };
    union {
        u64 ip_kobject;
        u64 ip_imp_task;
        u64 ip_sync_inheritor_port;
        u64 ip_sync_inheritor_knote;
        u64 ip_sync_inheritor_ts;
    };
    union {
        i32 ip_pid;
        u64 ip_twe;
        u64 ip_pdrequest;
    };
    u64 ip_nsrequest;
    u64 ip_requests;
    union {
        u64 ip_premsg;
        u64 ip_send_turnstile;
    };
    u64 ip_context;
    u32 ip_impcount;
    u32 ip_mscount;
    u32 ip_srights;
    u32 ip_sorights;
    union {
        u64 ip_kolabel;
        u64 ip_splabel;
    };
};


struct ipc_space {
    u64 is_lock[2];
    u32 is_bits;
    u32 is_table_hashed;
    u32 is_table_free;
    u64 is_table;
    u64 is_task;
    u64 is_grower;
    u64 is_label;
    u32 is_low_mod;
    u32 is_high_mod;
    struct {
        u32 seed[4];
        u32 state;
        u64 lock[2];
    } bool_gen;
    u32 is_entropy[1];
    i32 is_node_id;
};

#define GUARD_REQUIRED (1u << 1)

/*
 * kqueue stuff
 */

#define KQ_WORKLOOP_CREATE     0x01
#define KQ_WORKLOOP_DESTROY    0x02

#define KQ_WORKLOOP_CREATE_SCHED_PRI      0x01
#define KQ_WORKLOOP_CREATE_SCHED_POL      0x02
#define KQ_WORKLOOP_CREATE_CPU_PERCENT    0x04

struct kqueue_workloop_params {
    i32 kqwlp_version;
    i32 kqwlp_flags;
    u64 kqwlp_id;
    i32 kqwlp_sched_pri;
    i32 kqwlp_sched_pol;
    i32 kqwlp_cpu_percent;
    i32 kqwlp_cpu_refillms;
} __attribute__((packed));

__options_decl(kq_state_t, u16, {
    KQ_SLEEP         = 0x0002,
    KQ_PROCWAIT      = 0x0004,
    KQ_KEV32         = 0x0008,
    KQ_KEV64         = 0x0010,
    KQ_KEV_QOS       = 0x0020,
    KQ_WORKQ         = 0x0040,
    KQ_WORKLOOP      = 0x0080,
    KQ_PROCESSING    = 0x0100,
    KQ_DRAIN         = 0x0200,
    KQ_DYNAMIC       = 0x0800,
    KQ_R2K_ARMED     = 0x1000,
    KQ_HAS_TURNSTILE = 0x2000,
});

/*
 * proc_info stuff
 */

#define PROC_INFO_CALL_LISTPIDS             0x1
#define PROC_INFO_CALL_PIDINFO              0x2
#define PROC_INFO_CALL_PIDFDINFO            0x3
#define PROC_INFO_CALL_KERNMSGBUF           0x4
#define PROC_INFO_CALL_SETCONTROL           0x5
#define PROC_INFO_CALL_PIDFILEPORTINFO      0x6
#define PROC_INFO_CALL_TERMINATE            0x7
#define PROC_INFO_CALL_DIRTYCONTROL         0x8
#define PROC_INFO_CALL_PIDRUSAGE            0x9
#define PROC_INFO_CALL_PIDORIGINATORINFO    0xa
#define PROC_INFO_CALL_LISTCOALITIONS       0xb
#define PROC_INFO_CALL_CANUSEFGHW           0xc
#define PROC_INFO_CALL_PIDDYNKQUEUEINFO     0xd
#define PROC_INFO_CALL_UDATA_INFO           0xe
#define PROC_INFO_CALL_SET_DYLD_IMAGES      0xf
#define PROC_INFO_CALL_TERMINATE_RSR        0x10

struct vinfo_stat {
    u32 vst_dev;
    u16 vst_mode;
    u16 vst_nlink;
    u64 vst_ino;
    u32 vst_uid;
    u32 vst_gid;
    i64 vst_atime;
    i64 vst_atimensec;
    i64 vst_mtime;
    i64 vst_mtimensec;
    i64 vst_ctime;
    i64 vst_ctimensec;
    i64 vst_birthtime;
    i64 vst_birthtimensec;
    i64 vst_size;
    i64 vst_blocks;
    i32 vst_blksize;
    u32 vst_flags;
    u32 vst_gen;
    u32 vst_rdev;
    i64 vst_qspare[2];
};

#define PROC_PIDFDVNODEINFO         1
#define PROC_PIDFDVNODEPATHINFO     2
#define PROC_PIDFDSOCKETINFO        3
#define PROC_PIDFDPSEMINFO          4
#define PROC_PIDFDPSHMINFO          5
#define PROC_PIDFDPIPEINFO          6
#define PROC_PIDFDKQUEUEINFO        7
#define PROC_PIDFDATALKINFO         8
#define PROC_PIDFDKQUEUE_EXTINFO    9
#define PROC_PIDFDCHANNELINFO       10

struct proc_fileinfo {
    u32 fi_openflags;
    u32 fi_status;
    i64 fi_offset;
    i32 fi_type;
    u32 fi_guardflags;
};

struct psem_info {
    struct vinfo_stat psem_stat;
    char psem_name[1024];
};

struct psem_fdinfo {
    struct proc_fileinfo pfi;
    struct psem_info pseminfo;
};

#define PROC_PIDDYNKQUEUE_INFO       0
#define PROC_PIDDYNKQUEUE_EXTINFO    1

struct kqueue_info {
    struct vinfo_stat kq_stat;
    u32 kq_state;
    u32 rfu_1;
};

struct kqueue_dyninfo {
    struct kqueue_info kqdi_info;
    u64 kqdi_servicer;
    u64 kqdi_owner;
    u32 kqdi_sync_waiters;
    u8 kqdi_sync_waiter_qos;
    u8 kqdi_async_qos;
    u16 kqdi_request_state;
    u8 kqdi_events_qos;
    u8 kqdi_pri;
    u8 kqdi_pol;
    u8 kqdi_cpupercent;
    u8 _kqdi_reserved0[4];
    u64 _kqdi_reserved1[4];
};

/*
 * perfmon stuff
 */

#define PERFMON_SPEC_MAX_ATTR_COUNT (32)

struct perfmon_layout {
    u16 pl_counter_count;
    u16 pl_fixed_offset;
    u16 pl_fixed_count;
    u16 pl_unit_count;
    u16 pl_reg_count;
    u16 pl_attr_count;
};

typedef char perfmon_name_t[16];

struct perfmon_event {
    char pe_name[32];
    u64 pe_number;
    u16 pe_counter;
};

struct perfmon_attr {
    perfmon_name_t pa_name;
    u64 pa_value;
};

struct perfmon_spec {
    struct perfmon_event* ps_events;
    struct perfmon_attr* ps_attrs;
    u16 ps_event_count;
    u16 ps_attr_count;
};

enum perfmon_kind {
    perfmon_cpmu,
    perfmon_upmu,
    perfmon_kind_max,
};

struct perfmon_source {
    const char* ps_name;
    const perfmon_name_t* ps_register_names;
    const perfmon_name_t* ps_attribute_names;
    struct perfmon_layout ps_layout;
    enum perfmon_kind ps_kind;
    bool ps_supported;
};

struct perfmon_counter {
    u64 pc_number;
};

struct perfmon_config {
    struct perfmon_source* pc_source;
    struct perfmon_spec pc_spec;
    u16 pc_attr_ids[PERFMON_SPEC_MAX_ATTR_COUNT];
    struct perfmon_counter* pc_counters;
    u64 pc_counters_used;
    u64 pc_attrs_used;
    bool pc_configured:1;
};

struct perfmon_device {
    void* pmdv_copyout_buf;
    u64 pmdv_mutex[2];
    struct perfmon_config* pmdv_config;
    bool pmdv_allocated;
};

enum perfmon_ioctl {
    PERFMON_CTL_ADD_EVENT = _IOWR('P', 5, struct perfmon_event),
    PERFMON_CTL_SPECIFY = _IOWR('P', 10, struct perfmon_spec),
};

/*
 * pmap stuff
 */

#define AP_RWNA   (0x0ull << 6)
#define AP_RWRW   (0x1ull << 6)
#define AP_RONA   (0x2ull << 6)
#define AP_RORO   (0x3ull << 6)

#define ARM_PTE_TYPE              0x0000000000000003ull
#define ARM_PTE_TYPE_VALID        0x0000000000000003ull
#define ARM_PTE_TYPE_MASK         0x0000000000000002ull
#define ARM_TTE_TYPE_L3BLOCK      0x0000000000000002ull
#define ARM_PTE_ATTRINDX          0x000000000000001cull
#define ARM_PTE_NS                0x0000000000000020ull
#define ARM_PTE_AP                0x00000000000000c0ull
#define ARM_PTE_SH                0x0000000000000300ull
#define ARM_PTE_AF                0x0000000000000400ull
#define ARM_PTE_NG                0x0000000000000800ull
#define ARM_PTE_ZERO1             0x000f000000000000ull
#define ARM_PTE_HINT              0x0010000000000000ull
#define ARM_PTE_PNX               0x0020000000000000ull
#define ARM_PTE_NX                0x0040000000000000ull
#define ARM_PTE_ZERO2             0x0380000000000000ull
#define ARM_PTE_WIRED             0x0400000000000000ull
#define ARM_PTE_WRITEABLE         0x0800000000000000ull
#define ARM_PTE_ZERO3             0x3000000000000000ull
#define ARM_PTE_COMPRESSED_ALT    0x4000000000000000ull
#define ARM_PTE_COMPRESSED        0x8000000000000000ull

#define ARM_TTE_VALID         0x0000000000000001ull
#define ARM_TTE_TYPE_MASK     0x0000000000000002ull
#define ARM_TTE_TYPE_TABLE    0x0000000000000002ull
#define ARM_TTE_TYPE_BLOCK    0x0000000000000000ull
#define ARM_TTE_TABLE_MASK    0x0000fffffffff000ull
#define ARM_TTE_PA_MASK       0x0000fffffffff000ull

#define PMAP_TT_L0_LEVEL    0x0
#define PMAP_TT_L1_LEVEL    0x1
#define PMAP_TT_L2_LEVEL    0x2
#define PMAP_TT_L3_LEVEL    0x3

#define ARM_16K_TT_L0_SIZE          0x0000800000000000ull
#define ARM_16K_TT_L0_OFFMASK       0x00007fffffffffffull
#define ARM_16K_TT_L0_SHIFT         47
#define ARM_16K_TT_L0_INDEX_MASK    0x0000800000000000ull

#define ARM_16K_TT_L1_SIZE          0x0000001000000000ull
#define ARM_16K_TT_L1_OFFMASK       0x0000000fffffffffull
#define ARM_16K_TT_L1_SHIFT         36
#define ARM_16K_TT_L1_INDEX_MASK    0x0000007000000000ull

#define ARM_16K_TT_L2_SIZE          0x0000000002000000ull
#define ARM_16K_TT_L2_OFFMASK       0x0000000001ffffffull
#define ARM_16K_TT_L2_SHIFT         25
#define ARM_16K_TT_L2_INDEX_MASK    0x0000000ffe000000ull

#define ARM_16K_TT_L3_SIZE          0x0000000000004000ull
#define ARM_16K_TT_L3_OFFMASK       0x0000000000003fffull
#define ARM_16K_TT_L3_SHIFT         14
#define ARM_16K_TT_L3_INDEX_MASK    0x0000000001ffc000ull

struct pmap {
    u64 tte;
    u64 ttep;
    u64 min;
    u64 max;
    u64 pmap_pt_attr;
    u64 ledger;
    u64 rwlock[2];
    struct {
        u64 next;
        u64 prev;
    } pmaps;
    u64 tt_entry_free;
    u64 nested_pmap;
    u64 nested_region_addr;
    u64 nested_region_size;
    u64 nested_region_true_start;
    u64 nested_region_true_end;
    u64 nested_region_asid_bitmap;
    u32 nested_region_asid_bitmap_size;
    u64 reserved0;
    u64 reserved1;
    u64 reserved2;
    u64 reserved3;
    i32 ref_count;
    i32 nested_count;
    u32 nested_no_bounds_refcnt;
    u16 hw_asid;
    u8 sw_asid;
    bool reserved4;
    bool pmap_vm_map_cs_enforced;
    bool reserved5;
    u32 reserved6;
    u8 reserved7;
    u8 type;
    bool reserved8;
    bool reserved9;
    bool is_rosetta;
    bool nx_enabled;
    bool is_64bit;
    bool nested_has_no_bounds_ref;
    bool nested_bounds_set;
    bool disable_jop;
    bool reserved11;
};

struct pseminfo {
    u32 psem_flags;
    u32 psem_usecount;
    u16 psem_mode;
    u32 psem_uid;
    u32 psem_gid;
    char psem_name[32];
    u64 psem_semobject;
    u64 psem_label;
    i32 psem_creator_pid;
    u64 psem_creator_uniqueid;
};

struct psemnode {
    u64 pinfo;
    u64 padding;
};

struct semaphore {
    struct {
        u64 next;
        u64 prev;
    } task_link;
    char waitq[24];
    u64 owner;
    u64 port;
    u32 ref_count;
    i32 count;
};

#define vme_prev     links.prev
#define vme_next     links.next
#define vme_start    links.start
#define vme_end      links.end

struct vm_map_links {
    u64 prev;
    u64 next;
    u64 start;
    u64 end;
};

struct vm_map_store {
    struct {
        u64 rbe_left;
        u64 rbe_right;
        u64 rbe_parent;
    } entry;
};

struct vm_map_entry {
    struct vm_map_links links;
    struct vm_map_store store;
    union {
        u64 vme_object_value;
        struct {
            u64 vme_atomic:1;
            u64 is_sub_map:1;
            u64 vme_submap:60;
        };
        struct {
            u32 vme_ctx_atomic:1;
            u32 vme_ctx_is_sub_map:1;
            u32 vme_context:30;
            u32 vme_object;
        };
    };
    u64
        vme_alias:12,
        vme_offset:52,
        is_shared:1,
        __unused1:1,
        in_transition:1,
        needs_wakeup:1,
        behavior:2,
        needs_copy:1,
        protection:3,
        used_for_tpro:1,
        max_protection:4,
        inheritance:2,
        use_pmap:1,
        no_cache:1,
        vme_permanent:1,
        superpage_size:1,
        map_aligned:1,
        zero_wired_pages:1,
        used_for_jit:1,
        pmap_cs_associated:1,
        iokit_acct:1,
        vme_resilient_codesign:1,
        vme_resilient_media:1,
        __unused2:1,
        vme_no_copy_on_read:1,
        translated_allow_execute:1,
        vme_kernel_object:1;
    u16 wired_count;
    u16 user_wired_count;
};

#define vme_for_store(kaddr) ((kaddr) ? (((kaddr) - sizeof(struct vm_map_links)) & (~1ull)) : (kaddr))
#define store_for_vme(kaddr) ((kaddr) ? (((kaddr) + sizeof(struct vm_map_links))) : (kaddr))

struct vm_named_entry {
    u64 Lock[2];
    union {
        u64 map;
        u64 copy;
    } backing;
    u64 offset;
    u64 size;
    u64 data_offset;
    u32
        protection:4,
        is_object:1,
        internal:1,
        is_sub_map:1,
        is_copy:1,
        is_fully_owned:1;
};

#define cpy_hdr       c_u.hdr
#define cpy_object    c_u.object
#define cpy_kdata     c_u.kdata

struct rb_head {
    u64 rbh_root;
};

struct vm_map_header {
    struct vm_map_links links;
    i32 nentries;
    u16 page_shift;
    u32
        entries_pageable:1,
        __padding:15;
    struct rb_head rb_head_store;
};

struct vm_map_copy {
    i32 type;
    u64 offset;
    u64 size;
    union {
        struct vm_map_header hdr;
        u64 object;
        u64 kdata;
    } c_u;
};

typedef struct {
    u64 next;
    u64 prev;
} queue_head_t, queue_chain_t;

typedef struct {
    u32 next;
    u32 prev;
} vm_page_queue_head_t, vm_page_queue_chain_t;

#define vmp_pageq    vmp_q_un.vmp_q_pageq
#define vmp_snext    vmp_q_un.vmp_q_snext

struct vm_page {
    union {
        vm_page_queue_chain_t vmp_q_pageq;
        u64 vmp_q_snext;
    } vmp_q_un;
    vm_page_queue_chain_t vmp_listq;
    vm_page_queue_chain_t vmp_specialq;
    u64 vmp_offset;
    u32 vmp_object;
    u32
        vmp_wire_count:16,
        vmp_q_state:4,
        vmp_on_specialq:2,
        vmp_gobbled:1,
        vmp_laundry:1,
        vmp_no_cache:1,
        vmp_private:1,
        vmp_reference:1,
        vmp_lopage:1,
        vmp_realtime:1,
        vmp_unused_page_bits:3;
    u32 vmp_next_m;
    u32
        vmp_busy:1,
        vmp_wanted:1,
        vmp_tabled:1,
        vmp_hashed:1,
        vmp_fictitious:1,
        vmp_clustered:1,
        vmp_pmapped:1,
        vmp_xpmapped:1,
        vmp_wpmapped:1,
        vmp_free_when_done:1,
        vmp_absent:1,
        vmp_error:1,
        vmp_dirty:1,
        vmp_cleaning:1,
        vmp_precious:1,
        vmp_overwriting:1,
        vmp_restart:1,
        vmp_unusual:1,
        vmp_cs_validated:4,
        vmp_cs_tainted:4,
        vmp_cs_nx:4,
        vmp_reusable:1,
        vmp_written_by_kernel:1;
};

static struct vm_page* vm_pages = 0;
static struct vm_page* vm_page_array_beginning_addr = 0;
static struct vm_page* vm_page_array_ending_addr = 0;
static u32 vm_first_phys_ppnum = 0;

#define __WORDSIZE 64

#define TiB(x) ((0ull + (x)) << 40)
#define GiB(x) ((0ull + (x)) << 30)

#if TARGET_MACOS
#define VM_KERNEL_POINTER_SIGNIFICANT_BITS 41
#define VM_MIN_KERNEL_ADDRESS ((u64)(0ull - TiB(2)))
#else /* TARGET_MACOS */
#define VM_KERNEL_POINTER_SIGNIFICANT_BITS 38
#define VM_MIN_KERNEL_ADDRESS ((u64)(0ull - GiB(144)))
#endif /* TARGET_MACOS */

#define VM_MIN_KERNEL_AND_KEXT_ADDRESS VM_MIN_KERNEL_ADDRESS

#define VM_PAGE_PACKED_PTR_ALIGNMENT    64
#define VM_PAGE_PACKED_ALIGNED          __attribute__((aligned(VM_PAGE_PACKED_PTR_ALIGNMENT)))
#define VM_PAGE_PACKED_PTR_BITS         31
#define VM_PAGE_PACKED_PTR_SHIFT        6
#define VM_PAGE_PACKED_PTR_BASE         ((usize)(VM_MIN_KERNEL_AND_KEXT_ADDRESS))
#define VM_PAGE_PACKED_FROM_ARRAY       0x80000000

typedef struct vm_packing_params {
    u64 vmpp_base;
    u8 vmpp_bits;
    u8 vmpp_shift;
    bool vmpp_base_relative;
} vm_packing_params_t;

#define VM_PACKING_IS_BASE_RELATIVE(ns) \
    (ns##_BITS + ns##_SHIFT <= VM_KERNEL_POINTER_SIGNIFICANT_BITS)

#define VM_PACKING_PARAMS(ns)                                     \
    (vm_packing_params_t) {                                       \
        .vmpp_base = ns##_BASE,                                   \
        .vmpp_bits = ns##_BITS,                                   \
        .vmpp_shift = ns##_SHIFT,                                 \
        .vmpp_base_relative = VM_PACKING_IS_BASE_RELATIVE(ns),    \
    }

#define VM_UNPACK_POINTER(packed, ns) \
    vm_unpack_pointer(packed, VM_PACKING_PARAMS(ns))

#define VM_PAGE_UNPACK_PTR(p)    (vm_page_unpack_ptr((u64)(p)))
#define VM_OBJECT_UNPACK(p)      ((u64)(VM_UNPACK_POINTER(p, VM_PAGE_PACKED_PTR)))
#define VM_PAGE_OBJECT(p)        (VM_OBJECT_UNPACK((p)->vmp_object))

#define vo_size                   vo_un1.vou_size
#define vo_cache_pages_to_scan    vo_un1.vou_cache_pages_to_scan
#define vo_shadow_offset          vo_un2.vou_shadow_offset
#define vo_cache_ts               vo_un2.vou_cache_ts
#define vo_owner                  vo_un2.vou_owner

struct vm_object {
    vm_page_queue_head_t memq;
    u64 Lock[2];
    union {
        u64 vou_size;
        i32 vou_cache_pages_to_scan;
    } vo_un1;
    u64 memq_hint;
    i32 ref_count;
    u32 resident_page_count;
    u32 wired_page_count;
    u32 reusable_page_count;
    u64 copy;
    u64 shadow;
    u64 pager;
    union {
        u64 vou_shadow_offset;
        u64 vou_cache_ts;
        u64 vou_owner;
    } vo_un2;
    u64 paging_offset;
    u64 pager_control;
    i32 copy_strategy;
    u32
        paging_in_progress:16,
        __object1_unused_bits:16;
    u32 activity_in_progress;
    u32
        all_wanted:11,
        pager_created:1,
        pager_initialized:1,
        pager_ready:1,
        pager_trusted:1,
        can_persist:1,
        internal:1,
        private:1,
        pageout:1,
        alive:1,
        purgable:2,
        purgeable_only_by_kernel:1,
        purgeable_when_ripe:1,
        shadowed:1,
        true_share:1,
        terminating:1,
        named:1,
        shadow_severed:1,
        phys_contiguous:1,
        nophyscache:1,
        for_realtime:1;
    queue_chain_t cached_list;
    u64 last_alloc;
    u64 cow_hint;
    i32 sequential;
    u32 pages_created;
    u32 pages_used;
    u32
        wimg_bits:8,
        code_signed:1,
        transposed:1,
        mapping_in_progress:1,
        phantom_isssd:1,
        volatile_empty:1,
        volatile_fault:1,
        all_reusable:1,
        blocked_access:1,
        set_cache_attr:1,
        object_is_shared_cache:1,
        purgeable_queue_type:2,
        purgeable_queue_group:3,
        io_tracking:1,
        no_tag_update:1,
        eligible_for_secluded:1,
        can_grab_secluded:1,
        __unused_access_tracking:1,
        vo_ledger_tag:3,
        vo_no_footprint:1;
    u8 scan_collisions;
    u8 __object4_unused_bits[1];
    u16 wire_tag;
    u32 phantom_object_id;
    queue_head_t uplq;
    queue_chain_t objq;
    queue_chain_t task_objq;
};



#endif /* static_info_h */
