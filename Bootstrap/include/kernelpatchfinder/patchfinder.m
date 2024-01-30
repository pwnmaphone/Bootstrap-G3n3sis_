//
//  patchfinder.m
//  kfd
//
//  Created by Seo Hyun-gyu on 1/8/24.
//

#import <Foundation/Foundation.h>
#include <sys/sysctl.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/mman.h>

#include "patchfinder.h"
#include "libdimentio.h"

#define ADDRISVALID(val) ((val) >= 0xffff000000000000 && (val) != 0xffffffffffffffff)

bool did_patchfinder = false;
uint64_t off_cdevsw = 0;
uint64_t off_gPhysBase = 0;
uint64_t off_gPhysSize = 0;
uint64_t off_gVirtBase = 0;
uint64_t off_perfmon_dev_open = 0;
uint64_t off_perfmon_devices = 0;
uint64_t off_ptov_table = 0;
uint64_t off_vn_kqfilter = 0;
uint64_t off_proc_object_size = 0;

/*

const char* getBootManifestHash(void) {
    struct statfs fs;
    if (statfs("/usr/standalone/firmware", &fs) == 0) {
        NSString *mountedPath = [NSString stringWithUTF8String:fs.f_mntfromname];
        NSArray<NSString *> *components = [mountedPath componentsSeparatedByString:@"/"];
        if ([components count] > 3) {
            NSString *substring = components[3];
            return substring.UTF8String;
        }
    }
    return NULL;
}

const char* get_kernel_path(void) {
    NSString *kernelPath = [NSString stringWithFormat:@"/private/preboot/%s%@", getBootManifestHash(), @"/System/Library/Caches/com.apple.kernelcaches/kernelcache"];
    assert([[NSFileManager defaultManager] fileExistsAtPath:kernelPath]);
    return kernelPath.UTF8String;
}

void removeIfExist(const char* path) {
    if(access(path, F_OK) == 0) remove(path);
}
 */

int do_patchfinder(uint64_t kfd, uint64_t kernel_base) {
    kern_return_t k;
    if(did_patchfinder) {
        return 0;
    } else if(!ADDRISVALID(kernel_base) || !ADDRISVALID(kfd)) {
        SYSLOG("[PF] ERR: one or more addresses given are invalid!");
        return -1;
    }
    
    SYSLOG("[PF] Starting Patchfinder");
    uint64_t kslide = kernel_base - 0xFFFFFFF007004000;
    
    set_kfd(kfd);
    set_kbase(kernel_base);
    
    pfinder_t pfinder;
    k = pfinder_init(&pfinder);
    
    if(k != KERN_SUCCESS) {
        SYSLOG("[PF] ERR: Patchfinder was unable to initialize correctly");
        goto kpffailed;
    }
    
    SYSLOG("[PF] Patchfinder initiated");
    
    uint64_t cdevsw = pfinder_cdevsw(pfinder);
    if(cdevsw) off_cdevsw = cdevsw - kslide;
    SYSLOG("[PF] cdevsw: 0x%llx\n", off_cdevsw);
    
    uint64_t gPhysBase = pfinder_gPhysBase(pfinder);
    if(gPhysBase) off_gPhysBase = gPhysBase - kslide;
    SYSLOG("[PF] gPhysBase: 0x%llx\n", off_gPhysBase);
    
    uint64_t gPhysSize = pfinder_gPhysSize(pfinder);
    if(gPhysSize) off_gPhysSize = gPhysSize - kslide;
    SYSLOG("[PF] gPhysSize: 0x%llx\n", off_gPhysSize);
    
    uint64_t gVirtBase = pfinder_gVirtBase(pfinder);
    if(gVirtBase) off_gVirtBase = gVirtBase - kslide;
    SYSLOG("[PF] gVirtBase: 0x%llx\n", off_gVirtBase);
    
    uint64_t perfmon_dev_open = pfinder_perfmon_dev_open(pfinder);
    if(perfmon_dev_open) off_perfmon_dev_open = perfmon_dev_open - kslide;
    SYSLOG("[PF] perfmon_dev_open: 0x%llx\n", off_perfmon_dev_open);
    
    uint64_t perfmon_devices = pfinder_perfmon_devices(pfinder);
    if(perfmon_devices) off_perfmon_devices = perfmon_devices - kslide;
    SYSLOG("[PF] perfmon_devices: 0x%llx\n", off_perfmon_devices);
    
    uint64_t ptov_table = pfinder_ptov_table(pfinder);
    if(ptov_table) off_ptov_table = ptov_table - kslide;
    SYSLOG("[PF] ptov_table: 0x%llx\n", off_ptov_table);
    
    uint64_t vn_kqfilter = pfinder_vn_kqfilter(pfinder);
    if(vn_kqfilter) off_vn_kqfilter = vn_kqfilter - kslide;
    SYSLOG("[PF] vn_kqfilter: 0x%llx\n", off_vn_kqfilter);
    
    uint64_t proc_object_size = pfinder_proc_object_size(pfinder);
    if(proc_object_size) off_proc_object_size = proc_object_size;
    SYSLOG("[PF] proc_object_size: 0x%llx\n", off_proc_object_size);
    
    pfinder_term(&pfinder);
    did_patchfinder = true;
    return 0;
    
    
kpffailed:;
    pfinder_term(&pfinder);
    return -1;
}
