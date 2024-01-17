/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef static_info_h
#define static_info_h

#include "../../libkfd.h"
#include <sys/ioctl.h>


/*
 * makedefs/MakeInc.def
 */

#define ARM64_LINK_ADDR    0xfffffff007004000

/*
 * osfmk/arm64/proc_reg.h
 */

#define ARM_PGSHIFT    (14ull)
#define ARM_PGBYTES    (1ull << ARM_PGSHIFT)
#define ARM_PGMASK     (ARM_PGBYTES - 1ull)

#define T1SZ_BOOT    17ull

#define AP_RWNA    (0x0ull << 6)
#define AP_RWRW    (0x1ull << 6)
#define AP_RONA    (0x2ull << 6)
#define AP_RORO    (0x3ull << 6)

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

#define ARM_16K_TT_L0_SIZE          0x0000800000000000ull
#define ARM_16K_TT_L0_OFFMASK       0x00007fffffffffffull
#define ARM_16K_TT_L0_SHIFT         47
#define ARM_16K_TT_L0_INDEX_MASK    0x0000800000000000ull

#define ARM_16K_TT_L1_SIZE          0x0000001000000000ull
#define ARM_16K_TT_L1_OFFMASK       0x0000000fffffffffull
#define ARM_16K_TT_L1_SHIFT         36
#define ARM_16K_TT_L1_INDEX_MASK    0x00007ff000000000ull

#define ARM_16K_TT_L2_SIZE          0x0000000002000000ull
#define ARM_16K_TT_L2_OFFMASK       0x0000000001ffffffull
#define ARM_16K_TT_L2_SHIFT         25
#define ARM_16K_TT_L2_INDEX_MASK    0x0000000ffe000000ull

#define ARM_16K_TT_L3_SIZE          0x0000000000004000ull
#define ARM_16K_TT_L3_OFFMASK       0x0000000000003fffull
#define ARM_16K_TT_L3_SHIFT         14
#define ARM_16K_TT_L3_INDEX_MASK    0x0000000001ffc000ull

/*
 * osfmk/arm/pmap/pmap_pt_geometry.h
 */

#define PMAP_TT_L0_LEVEL    0x0
#define PMAP_TT_L1_LEVEL    0x1
#define PMAP_TT_L2_LEVEL    0x2
#define PMAP_TT_L3_LEVEL    0x3

/*
 * osfmk/kern/bits.h
 */

#define BIT(b)    (1ULL << (b))

/*
 * osfmk/arm/machine_routines.h
 */

#define ONES(x)          (BIT((x))-1)
#define PTR_MASK         ONES(64-T1SZ_BOOT)
#define PAC_MASK         (~PTR_MASK)
#define SIGN(p)          ((p) & BIT(55))
#define UNSIGN_PTR(p)    (SIGN(p) ? ((p) | PAC_MASK) : ((p) & ~PAC_MASK))

/*
 * osfmk/kern/kalloc.h
 */

#define KHEAP_MAX_SIZE    (32ull * 1024ull)

/*
 * osfmk/ipc/ipc_init.c
 */

/*
 * osfmk/vm/vm_map_store.h
 */

struct vm_map_store {
    struct {
        uint64_t rbe_left;
        uint64_t rbe_right;
        uint64_t rbe_parent;
    } entry;
};

struct vm_map_links {
    uint64_t prev;
    uint64_t next;
    uint64_t start;
    uint64_t end;
};

struct vm_map_header {
    struct vm_map_links links;
    int32_t nentries;
    uint16_t page_shift;
    uint16_t
        entries_pageable:1,
        __padding:15;
    struct {
        uint64_t rbh_root;
    } rb_head_store;
};

/*
 * osfmk/vm/vm_map.h
 */

struct vm_map_entry {
    struct vm_map_links links;
    struct vm_map_store store;
    union {
        uint64_t vme_object_value;
        struct {
            uint64_t vme_atomic:1;
            uint64_t is_sub_map:1;
            uint64_t vme_submap:60;
        };
        struct {
            uint32_t vme_ctx_atomic:1;
            uint32_t vme_ctx_is_sub_map:1;
            uint32_t vme_context:30;
            uint32_t vme_object;
        };
    };
    uint64_t
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
    uint16_t wired_count;
    uint16_t user_wired_count;
};

struct _vm_map {
    uint64_t lock[2];
    struct vm_map_header hdr;
    uint64_t pmap;
    uint64_t size;
    uint64_t size_limit;
    uint64_t data_limit;
    uint64_t user_wire_limit;
    uint64_t user_wire_size;
#if TARGET_MACOS
    uint64_t vmmap_high_start;
#else /* TARGET_MACOS */
    uint64_t user_range[4];
#endif /* TARGET_MACOS */
    union {
        uint64_t vmu1_highest_entry_end;
        uint64_t vmu1_lowest_unnestable_start;
    } vmu1;
    uint64_t hint;
    union {
        uint64_t vmmap_hole_hint;
        uint64_t vmmap_corpse_footprint;
    } vmmap_u_1;
    union {
        uint64_t _first_free;
        uint64_t _holes;
    } f_s;
    uint32_t map_refcnt;
    uint32_t
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
    uint32_t timestamp;
};

/*
 * osfmk/arm/pmap/pmap.h
 */

struct pmap {
    uint64_t tte;
    uint64_t ttep;
    uint64_t min;
    uint64_t max;
    uint64_t pmap_pt_attr;
    uint64_t ledger;
    uint64_t rwlock[2];
    struct {
        uint64_t next;
        uint64_t prev;
    } pmaps;
    uint64_t tt_entry_free;
    uint64_t nested_pmap;
    uint64_t nested_region_addr;
    uint64_t nested_region_size;
    uint64_t nested_region_true_start;
    uint64_t nested_region_true_end;
    uint64_t nested_region_asid_bitmap;
    uint32_t nested_region_asid_bitmap_size;
    uint64_t reserved0;
    uint64_t reserved1;
    uint64_t reserved2;
    uint64_t reserved3;
    int32_t ref_count;
    int32_t nested_count;
    uint32_t nested_no_bounds_refcnt;
    uint16_t hw_asid;
    uint8_t sw_asid;
    bool reserved4;
    bool pmap_vm_map_cs_enforced;
    bool reserved5;
    uint32_t reserved6;
    uint8_t reserved7;
    uint8_t type;
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

/*
 * bsd/kern/kern_guarded.c
 */

#define GUARD_REQUIRED (1u << 1)

/*
 * bsd/sys/file_internal.h
 */

struct fileproc_guard {
    uint64_t fpg_wset;
    uint64_t fpg_guard;
};

struct fileproc {
    uint32_t fp_iocount;
    uint32_t fp_vflags;
    uint16_t fp_flags;
    uint16_t fp_guard_attrs;
    uint64_t fp_glob;
    union {
        uint64_t fp_wset;
        uint64_t fp_guard;
    };
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

struct fileglob {
    struct {
        uint64_t le_next;
        uint64_t le_prev;
    } f_msglist;
    uint32_t fg_flag;
    uint32_t fg_count;
    uint32_t fg_msgcount;
    int32_t fg_lflags;
    uint64_t fg_cred;
    uint64_t fg_ops;
    int64_t fg_offset;
    uint64_t fg_data;
    uint64_t fg_vn_data;
    uint64_t fg_lock[2];
};

/*
 * bsd/sys/perfmon_private.h
 */

struct perfmon_layout {
    uint16_t pl_counter_count;
    uint16_t pl_fixed_offset;
    uint16_t pl_fixed_count;
    uint16_t pl_unit_count;
    uint16_t pl_reg_count;
    uint16_t pl_attr_count;
};

typedef char perfmon_name_t[16];

struct perfmon_event {
    char pe_name[32];
    uint64_t pe_number;
    uint16_t pe_counter;
};

struct perfmon_attr {
    perfmon_name_t pa_name;
    uint64_t pa_value;
};

struct perfmon_spec {
    struct perfmon_event* ps_events;
    struct perfmon_attr* ps_attrs;
    uint16_t ps_event_count;
    uint16_t ps_attr_count;
};

enum perfmon_ioctl {
    PERFMON_CTL_ADD_EVENT = _IOWR('P', 5, struct perfmon_event),
    PERFMON_CTL_SPECIFY = _IOWR('P', 10, struct perfmon_spec),
};

/*
 * osfmk/kern/perfmon.h
 */

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

#define PERFMON_SPEC_MAX_ATTR_COUNT    (32)

/*
 * osfmk/machine/machine_perfmon.h
 */

struct perfmon_counter {
    uint64_t pc_number;
};

struct perfmon_config {
    struct perfmon_source* pc_source;
    struct perfmon_spec pc_spec;
    uint16_t pc_attr_ids[PERFMON_SPEC_MAX_ATTR_COUNT];
    struct perfmon_counter* pc_counters;
    uint64_t pc_counters_used;
    uint64_t pc_attrs_used;
    bool pc_configured:1;
};

/*
 * bsd/dev/dev_perfmon.c
 */

struct perfmon_device {
    void* pmdv_copyout_buf;
    uint64_t pmdv_mutex[2];
    struct perfmon_config* pmdv_config;
    bool pmdv_allocated;
};

/*
 * bsd/pthread/workqueue_syscalls.h
 */

#define KQ_WORKLOOP_CREATE     0x01
#define KQ_WORKLOOP_DESTROY    0x02

#define KQ_WORKLOOP_CREATE_SCHED_PRI      0x01
#define KQ_WORKLOOP_CREATE_SCHED_POL      0x02
#define KQ_WORKLOOP_CREATE_CPU_PERCENT    0x04

struct kqueue_workloop_params {
    int32_t kqwlp_version;
    int32_t kqwlp_flags;
    uint64_t kqwlp_id;
    int32_t kqwlp_sched_pri;
    int32_t kqwlp_sched_pol;
    int32_t kqwlp_cpu_percent;
    int32_t kqwlp_cpu_refillms;
} __attribute__((packed));

/*
 * bsd/pthread/workqueue_internal.h
 */

struct workq_threadreq_s {
    union {
        uint64_t tr_entry[3];
        uint64_t tr_link[1];
        uint64_t tr_thread;
    };
    uint16_t tr_count;
    uint8_t tr_flags;
    uint8_t tr_state;
    uint8_t tr_qos;
    uint8_t tr_kq_override_index;
    uint8_t tr_kq_qos_index;
};

/*
 * bsd/sys/event.h
 */

struct kqtailq {
    uint64_t tqh_first;
    uint64_t tqh_last;
};

/*
 * bsd/sys/eventvar.h
 */

__options_decl(kq_state_t, uint16_t, {
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

struct kqueue {
    uint64_t kq_lock[2];
    kq_state_t kq_state;
    uint16_t kq_level;
    uint32_t kq_count;
    uint64_t kq_p;
    uint64_t kq_knlocks[1];
};

struct kqworkloop {
    struct kqueue kqwl_kqueue;
    struct kqtailq kqwl_queue[6];
    struct kqtailq kqwl_suppressed;
    struct workq_threadreq_s kqwl_request;
    uint64_t kqwl_preadopt_tg;
    uint64_t kqwl_statelock[2];
    uint64_t kqwl_owner;
    uint32_t kqwl_retains;
    uint8_t kqwl_wakeup_qos;
    uint8_t kqwl_iotier_override;
    uint16_t kqwl_preadopt_tg_needs_redrive;
    uint64_t kqwl_turnstile;
    uint64_t kqwl_dynamicid;
    uint64_t kqwl_params;
    uint64_t kqwl_hashlink[2];
};

/*
 * bsd/kern/posix_sem.c
 */

struct pseminfo {
    uint32_t psem_flags;
    uint32_t psem_usecount;
    uint16_t psem_mode;
    uint32_t psem_uid;
    uint32_t psem_gid;
    char psem_name[32];
    uint64_t psem_semobject;
    uint64_t psem_label;
    int32_t psem_creator_pid;
    uint64_t psem_creator_uniqueid;
};

struct psemnode {
    uint64_t pinfo;
    uint64_t padding;
};

/*
 * osfmk/kern/sync_sema.h
 */

struct semaphore {
    struct {
        uint64_t next;
        uint64_t prev;
    } task_link;
    char waitq[24];
    uint64_t owner;
    uint64_t port;
    uint32_t ref_count;
    int32_t count;
};

/*
 * bsd/sys/vnode_internal.h
 */

struct vnode {
    uint64_t v_lock[2];
    uint64_t v_freelist[2];
    uint64_t v_mntvnodes[2];
    uint64_t v_ncchildren[2];
    uint64_t v_nclinks[1];
    uint64_t v_defer_reclaimlist;
    uint32_t v_listflag;
    uint32_t v_flag;
    uint16_t v_lflag;
    uint8_t v_iterblkflags;
    uint8_t v_references;
    int32_t v_kusecount;
    int32_t v_usecount;
    int32_t v_iocount;
    uint64_t v_owner;
    uint16_t v_type;
    uint16_t v_tag;
    uint32_t v_id;
    union {
        uint64_t vu_mountedhere;
        uint64_t vu_socket;
        uint64_t vu_specinfo;
        uint64_t vu_fifoinfo;
        uint64_t vu_ubcinfo;
    } v_un;
    // ...
};

/*
 * bsd/miscfs/specfs/specdev.h
 */

struct specinfo {
    uint64_t si_hashchain;
    uint64_t si_specnext;
    int64_t si_flags;
    int32_t si_rdev;
    int32_t si_opencount;
    int32_t si_size;
    int64_t si_lastr;
    uint64_t si_devsize;
    uint8_t si_initted;
    uint8_t si_throttleable;
    uint16_t si_isssd;
    uint32_t si_devbsdunit;
    uint64_t si_throttle_mask;
};

/*
 * bsd/sys/proc_info.h
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
    uint32_t vst_dev;
    uint16_t vst_mode;
    uint16_t vst_nlink;
    uint64_t vst_ino;
    uint32_t vst_uid;
    uint32_t vst_gid;
    int64_t vst_atime;
    int64_t vst_atimensec;
    int64_t vst_mtime;
    int64_t vst_mtimensec;
    int64_t vst_ctime;
    int64_t vst_ctimensec;
    int64_t vst_birthtime;
    int64_t vst_birthtimensec;
    int64_t vst_size;
    int64_t vst_blocks;
    int32_t vst_blksize;
    uint32_t vst_flags;
    uint32_t vst_gen;
    uint32_t vst_rdev;
    int64_t vst_qspare[2];
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
    uint32_t fi_openflags;
    uint32_t fi_status;
    int64_t fi_offset;
    int32_t fi_type;
    uint32_t fi_guardflags;
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
    uint32_t kq_state;
    uint32_t rfu_1;
};

struct kqueue_dyninfo {
    struct kqueue_info kqdi_info;
    uint64_t kqdi_servicer;
    uint64_t kqdi_owner;
    uint32_t kqdi_sync_waiters;
    uint8_t kqdi_sync_waiter_qos;
    uint8_t kqdi_async_qos;
    uint16_t kqdi_request_state;
    uint8_t kqdi_events_qos;
    uint8_t kqdi_pri;
    uint8_t kqdi_pol;
    uint8_t kqdi_cpupercent;
    uint8_t _kqdi_reserved0[4];
    uint64_t _kqdi_reserved1[4];
};

#endif /* static_info_h */
