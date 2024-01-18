/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef dynamic_info_h
#define dynamic_info_h

#include "../common.h"

struct IOSurface {
    uint64_t isa;
    uint64_t PixelFormat;
    uint64_t AllocSize;
    uint64_t UseCountPtr;
    uint64_t IndexedTimestampPtr;
    
    uint64_t ReadDisplacement;
};

static const struct IOSurface IOSurface_versions[] = {
    
    {
        .isa                    = 0x0,
        .PixelFormat            = 0xA4,
        .AllocSize              = 0xAC,
        .UseCountPtr            = 0xC0,
        .IndexedTimestampPtr    = 0x368,
        .ReadDisplacement       = 0x18
    }, // iOS 16.0 - 16.1 arm64
    
    {
        .isa                    = 0x0,
        .PixelFormat            = 0xA4,
        .AllocSize              = 0xAC,
        .UseCountPtr            = 0xC0,
        .IndexedTimestampPtr    = 0x368,
        .ReadDisplacement       = 0x18
    }, // iOS 16.2 - 16.3 arm64
    
    {
        .isa                    = 0x0,
        .PixelFormat            = 0xA4,
        .AllocSize              = 0xAC,
        .UseCountPtr            = 0xC0,
        .IndexedTimestampPtr    = 0x368,
        .ReadDisplacement       = 0x18
    }, // iOS 16.4 - 16.6 arm64
};

typedef uint64_t IOSurface_isa_t;
typedef uint32_t IOSurface_PixelFormat_t;
typedef uint32_t IOSurface_AllocSize_t;
typedef uint64_t IOSurface_UseCountPtr_t;
typedef uint64_t IOSurface_IndexedTimestampPtr_t;
typedef uint32_t IOSurface_ReadDisplacement_t;

struct dynamic_info {
    const char* kern_version;
    bool kread_kqueue_workloop_ctl_supported;
    bool perf_supported;
    // struct proc
    uint64_t proc__p_list__le_prev;
    uint64_t proc__p_pid;
    uint64_t proc__p_fd__fd_ofiles;
    uint64_t proc__object_size;
    // struct task
    uint64_t task__map;
    // struct thread
    uint64_t thread__thread_id;
    // kernelcache static addresses (perf)
    uint64_t kernelcache__cdevsw;                          // "spec_open type" or "Can't mark ptc as kqueue ok"
    uint64_t kernelcache__gPhysBase;                       // "%s: illegal PA: 0x%llx; phys base 0x%llx, size 0x%llx"
    uint64_t kernelcache__gPhysSize;                       // (gPhysBase + 0x8)
    uint64_t kernelcache__gVirtBase;                       // "%s: illegal PA: 0x%llx; phys base 0x%llx, size 0x%llx"
    uint64_t kernelcache__perfmon_dev_open;                // "perfmon: attempt to open unsupported source: 0x%x"
    uint64_t kernelcache__perfmon_devices;                 // "perfmon: %s: devfs_make_node_clone failed"
    uint64_t kernelcache__ptov_table;                      // "%s: illegal PA: 0x%llx; phys base 0x%llx, size 0x%llx"
    uint64_t kernelcache__vn_kqfilter;                     // "Invalid knote filter on a vnode!"
};

static struct dynamic_info kern_versions[] = {
    // iOS 16.5 - iPhone 14 Pro Max
    {
        .kern_version = "Darwin Kernel Version 22.5.0: Mon Apr 24 21:09:28 PDT 2023; root:xnu-8796.122.4~1/RELEASE_ARM64_T8120",
        .kread_kqueue_workloop_ctl_supported = false,
        .perf_supported = true,
        .proc__p_list__le_prev = 0x0008,
        .proc__p_pid = 0x0060,
        .proc__p_fd__fd_ofiles = 0x00f8,
        .proc__object_size = 0x0730,
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
    /*
    // iOS 16.6 - iPhone 12 Pro
    // T1SZ_BOOT must be changed to 25 instead of 17
    {
        .kern_version = "Darwin Kernel Version 22.6.0: Wed Jun 28 20:50:15 PDT 2023; root:xnu-8796.142.1~1/RELEASE_ARM64_T8101",
        .kread_kqueue_workloop_ctl_supported = false,
        .perf_supported = true,
        .proc__p_list__le_prev = 0x0008,
        .proc__p_pid = 0x0060,
        .proc__p_fd__fd_ofiles = 0x00f8,
        .proc__object_size = 0x0730,
        .task__map = 0x0028,
        .thread__thread_id = 0,
        .kernelcache__cdevsw = 0xfffffff00a4a5288,
        .kernelcache__gPhysBase = 0xfffffff0079303b8,
        .kernelcache__gPhysSize = 0xfffffff0079303c0,
        .kernelcache__gVirtBase = 0xfffffff00792e570,
        .kernelcache__perfmon_dev_open = 0xfffffff007ef4278,
        .kernelcache__perfmon_devices = 0xfffffff00a4e5320,
        .kernelcache__ptov_table = 0xfffffff0078e38f0,
        .kernelcache__vn_kqfilter = 0xfffffff007f42f40,
    },
    // macOS 13.4 - MacBook Air (M2, 2022)
    {
        .kern_version = "todo",
        .kread_kqueue_workloop_ctl_supported = false,
        .perf_supported = false,
        .proc__p_list__le_prev = 0x0008,
        .proc__p_pid = 0x0060,
        .proc__p_fd__fd_ofiles = 0x00f8,
        .proc__object_size = 0x0778,
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
    // macOS 13.5 - MacBook Air (M2, 2022)
    {
        .kern_version = "Darwin Kernel Version 22.6.0: Wed Jul  5 22:17:35 PDT 2023; root:xnu-8796.141.3~6/RELEASE_ARM64_T8112",
        .kread_kqueue_workloop_ctl_supported = false,
        .perf_supported = false,
        .proc__p_list__le_prev = 0x0008,
        .proc__p_pid = 0x0060,
        .proc__p_fd__fd_ofiles = 0x00f8,
        .proc__object_size = 0x0778,
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
    
    {
        // iOS 15.0.1 - iPhone Xr
        .kern_version = "Darwin Kernel Version 21.0.0: Sun Aug 15 20:55:51 PDT 2021; root:xnu-8019.12.5~1/RELEASE_ARM64_T8020",
        .kread_kqueue_workloop_ctl_supported = false,
        .perf_supported = true,
        .proc__p_list__le_prev = 0x0008,
        .proc__p_pid = 0x0060,
        .proc__p_fd__fd_ofiles = 0x00f8,
        .proc__object_size = 0x0730,
        .task__map = 0x0028,
        .thread__thread_id = 0,
        .kernelcache__cdevsw = 0xfffffff0077ca2c0, // 0xfffffff007d8a894, // changed
        .kernelcache__gPhysBase = 0xfffffff0077a7e70, // changed
        .kernelcache__gPhysSize = 0xfffffff0077a7fa8 + 8, // changed
        .kernelcache__gVirtBase = 0xfffffff0077a7e78, // changed - unsure..90%
        .kernelcache__perfmon_dev_open = 0xfffffff0077a7340, // changed - unsure..70%
        .kernelcache__perfmon_devices = 0xfffffff009c2e188, // changed
        .kernelcache__ptov_table = 0xfffffff0078e7178, // change
        .kernelcache__vn_kqfilter = 0xfffffff007d76984, // changed
    },
     */
};

#endif /* dynamic_info_h */
