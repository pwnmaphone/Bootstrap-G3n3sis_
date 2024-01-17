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
        SYSLOG("ERR: one or more addresses given are invalid!");
        return -1;
    }
    
    SYSLOG("Starting Patchfinder");
    uint64_t kslide = kernel_base - 0xFFFFFFF007004000;
    
    set_kfd(kfd);
    set_kbase(kernel_base);
    
    pfinder_t pfinder;
    k = pfinder_init(&pfinder);
    
    if(k != KERN_SUCCESS) {
        SYSLOG("ERR: Patchfinder was unable to initialize corectly");
        goto kpffailed;
    }
    
    SYSLOG("Patchfinder initiated");
    
    uint64_t cdevsw = pfinder_cdevsw(pfinder);
    if(cdevsw) off_cdevsw = cdevsw - kslide;
    SYSLOG("cdevsw: 0x%llx\n", off_cdevsw);
    
    uint64_t gPhysBase = pfinder_gPhysBase(pfinder);
    if(gPhysBase) off_gPhysBase = gPhysBase - kslide;
    SYSLOG("gPhysBase: 0x%llx\n", off_gPhysBase);
    
    uint64_t gPhysSize = pfinder_gPhysSize(pfinder);
    if(gPhysSize) off_gPhysSize = gPhysSize - kslide;
    SYSLOG("gPhysSize: 0x%llx\n", off_gPhysSize);
    
    uint64_t gVirtBase = pfinder_gVirtBase(pfinder);
    if(gVirtBase) off_gVirtBase = gVirtBase - kslide;
    SYSLOG("gVirtBase: 0x%llx\n", off_gVirtBase);
    
    uint64_t perfmon_dev_open = pfinder_perfmon_dev_open(pfinder);
    if(perfmon_dev_open) off_perfmon_dev_open = perfmon_dev_open - kslide;
    SYSLOG("perfmon_dev_open: 0x%llx\n", off_perfmon_dev_open);
    
    uint64_t perfmon_devices = pfinder_perfmon_devices(pfinder);
    if(perfmon_devices) off_perfmon_devices = perfmon_devices - kslide;
    SYSLOG("perfmon_devices: 0x%llx\n", off_perfmon_devices);
    
    uint64_t ptov_table = pfinder_ptov_table(pfinder);
    if(ptov_table) off_ptov_table = ptov_table - kslide;
    SYSLOG("ptov_table: 0x%llx\n", off_ptov_table);
    
    uint64_t vn_kqfilter = pfinder_vn_kqfilter(pfinder);
    if(vn_kqfilter) off_vn_kqfilter = vn_kqfilter - kslide;
    SYSLOG("vn_kqfilter: 0x%llx\n", off_vn_kqfilter);
    
    uint64_t proc_object_size = pfinder_proc_object_size(pfinder);
    if(proc_object_size) off_proc_object_size = proc_object_size;
    SYSLOG("proc_object_size: 0x%llx\n", off_proc_object_size);
    
    pfinder_term(&pfinder);
    
    /*
    SYSLOG("Readying KernelCache for patchfinder");
    const char *kernelPath = "/var/mobile/Documents/offsets/kernelcache";
    const char *kernelPathdec = "/var/mobile/Documents/offsets/kernelcache.dec";
    removeIfExist(kernelPath);
    removeIfExist(kernelPathdec);
    const char *origKC = get_kernel_path();
    assert(access(origKC, F_OK) == 0);
    
    SYSLOG("KernelCache is at: %s", origKC);
    
    // copy kernelcache and move it to kernelPath
    
    kern_return_t k = mkdir("/var/mobile/Documents/offsets/", 0777);
    if(k != KERN_SUCCESS) {
        SYSLOG("ERR: Unable to create offset path");
        return -1;
    }
    */
  /*
    k = [[NSFileManager defaultManager] copyItemAtPath:@(origKC) toPath:@"/var/mobile/Documents/offsets/" error:&error];
    if(![[NSFileManager defaultManager] fileExistsAtPath:@"/var/mobile/Documents/offsets/kernelcache"] || k != KERN_SUCCESS) {
        SYSLOG("ERR: Unable to copy kernelcache to path: %s\n", [[error localizedDescription] UTF8String]);
        return -1;
    }
    */
/*
    FILE *kc_file = fopen(origKC, "rb");
    if(kc_file == NULL) {
        SYSLOG("ERR: Unable to open kernelcache");
        goto kpffailed;
    }
        
        k = [[NSFileManager defaultManager] createFileAtPath:@"/var/mobile/Documents/offsets/kernelcache" contents:NULL attributes:NULL];
        if(![[NSFileManager defaultManager] fileExistsAtPath:@"/var/mobile/Documents/offsets/kernelcache/kernelcache"] || k != KERN_SUCCESS) {
            SYSLOG("ERR: Unable to create dummmy kernelcache at path");
            goto kpffailed;
        }
        
    FILE *new_KC = fopen(kernelPath, "wb");
    if(new_KC == NULL) {
        SYSLOG("ERR: Unable to open dummy kernelcache");
        goto kpffailed;
    }
    
    // Not efficient but it should work for now
    int data;
    for(;;) {
        data = fgetc(kc_file);
        if(data == EOF) {
            break;
        }
        fputc(data, new_KC);
    }
    
    // check if both files are the same size
    int fp, fp2;
    fp = fseek(kc_file, 0, SEEK_END);
    size_t fps = ftell((FILE*)fp);
    
    fp2 = fseek(new_KC, 0, SEEK_END);
    size_t fp2s = ftell((FILE*)fp2);
    if(fps != fp2s) {
        SYSLOG("ERR: Unable to copy full data to dummy kernelcache");
        goto kpffailed;
    }
    
    fclose(kc_file);
    fclose(new_KC);
 
    SYSLOG("Kernelcache copied at offset path");
    
    // decompress kernel then run Patchfinder
    static FILE *file;
    
    file = fopen(kernelPathdec, "rb");
    if (!file) {
        SYSLOG("ERR: Failed to open decompressed kernelcache");
        goto kpffailed;
    }
    
    fclose(file);
    SYSLOG("Kernel Decompressed, running Patchfinder");
 */
    
    /*
    int pf = init_kernel(NULL, 0, "/var/mobile/Documents/offsets/kernelcache.dec");
    if(pf != 0) {
        SYSLOG("ERR: Patchfinder returned a non zero value");
        goto kpffailed;
    }
    
    off_cdevsw = find_cdevsw();
    SYSLOG("cdevsw offset: 0x%llx\n", off_cdevsw);
    off_gPhysBase = find_gPhysBase();
    SYSLOG("gPhysBase offset: 0x%llx\n", off_gPhysBase);
    off_gPhysSize = find_gPhysSize();
    SYSLOG("gPhysSize offset: 0x%llx\n", off_gPhysSize);
    off_gVirtBase = find_gVirtBase();
    SYSLOG("gVirtBase offset: 0x%llx\n", off_gVirtBase);
    off_perfmon_dev_open = find_perfmon_dev_open();
    SYSLOG("perfmon_dev_open offset: 0x%llx\n", off_perfmon_dev_open);
    off_perfmon_devices = find_perfmon_devices();
    SYSLOG("perfmon_devices offset: 0x%llx\n", off_perfmon_devices);
    off_ptov_table = find_ptov_table();
    SYSLOG("ptov_table offset: 0x%llx\n", off_ptov_table);
    off_vn_kqfilter = find_vn_kqfilter();
    SYSLOG("vn_kqfilter offset: 0x%llx\n", off_vn_kqfilter);
    off_proc_object_size = find_proc_object_size();
    SYSLOG("proc_object_size offset: 0x%llx\n", off_proc_object_size);
    
    term_kernel();
    */
     
   // SYSLOG("Patchfinder returned %d", pf);
    did_patchfinder = true;
    return 0;
    
    
kpffailed:;
    pfinder_term(&pfinder);
    //term_kernel();
    /*
    removeIfExist(kernelPath);
    removeIfExist(kernelPathdec);
    removeIfExist("/var/mobile/Documents/offsets/");
     */
    //if(file != NULL) fclose(file);
    //if(fp != 0) close(fp);
    //if(fp2 != 0) close(fp2);
    //if(kc_file != NULL) fclose(kc_file);
    //if(new_KC != NULL) fclose(new_KC);
    return -1;
}
