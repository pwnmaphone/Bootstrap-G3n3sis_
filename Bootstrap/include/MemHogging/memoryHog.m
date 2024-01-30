//
//  memoryHog.m
//  Bootstrap
//
//  Created by Chris Coding on 1/13/24.
//

#import <Foundation/Foundation.h>
#include <os/proc.h>
#include <inttypes.h>

#include "../../syslog.h"
#include "memoryHog.h"

// TODO..

int32_t old_memory_limit = 0;
uint64_t* memory_hog = NULL;
bool hogged_memory = false;

// credit - Mineek

bool hasEntitlement(CFStringRef entitlement) {
    SecTaskRef task = SecTaskCreateFromSelf(NULL);
    CFTypeRef value = SecTaskCopyValueForEntitlement(task, entitlement, NULL);
    if (value != nil) {
        CFRelease(value);
    }
    CFRelease(task);
    return (value != NULL);
}

uint64_t* Hog_memory(void) {
    SYSLOG("[memHog]: Populating memory");
    
    size_t available_memory = os_proc_available_memory();
    
    SYSLOG("[memHog]: available memory: %zx", available_memory);
    
    uint64_t headroomMB = 384;
    size_t STATIC_HEADROOM = (headroomMB * (size_t)1024 * (size_t)1024);
    size_t pagesize = sysconf(_SC_PAGESIZE);    
    size_t hog_headroom = STATIC_HEADROOM + 3072 * pagesize;
    size_t memory_to_hog = available_memory > hog_headroom ? available_memory - hog_headroom: 0;
    
    memorystatus_memlimit_properties2_t memproperties;
    if(hasEntitlement(CFSTR("com.apple.memorystatus"))) {
        uint32_t new_memory_limit = (uint32_t)(NSProcessInfo.processInfo.physicalMemory / UINT64_C(1048576)) * 2;
        int ret = memorystatus_control(MEMORYSTATUS_CMD_GET_MEMLIMIT_PROPERTIES, getpid(), 0, &memproperties, sizeof(memproperties));
        if (ret == 0) {
            SYSLOG("[memHog]: current memory limit: %zu MiB", memproperties.v1.memlimit_active);
            old_memory_limit = memproperties.v1.memlimit_active;
            ret = memorystatus_control(MEMORYSTATUS_CMD_SET_JETSAM_TASK_LIMIT, getpid(), new_memory_limit, NULL, 0);
            if (ret == 0) {
                SYSLOG("[memHog]: The memory limit for pid %d has been set to %u MiB successfully", getpid(), new_memory_limit);
            } else {
                SYSLOG("[memHog]: Failed to set memory limit: %d (%s)", errno, strerror(errno));
                return -1;
            }
        } else {
            SYSLOG("[memoHog]: could not get current memory limits");
            return -1;
        }
    }  
        if (available_memory > hog_headroom) {
        memory_hog = malloc(memory_to_hog);
        if (memory_hog != NULL) {
            for (uint64_t i = 0; i < memory_to_hog / sizeof(uint64_t); i++) {
                memory_hog[i] = 0x4141414141414141;
            }
        }
        SYSLOG("[memHog]: Filled up hogged memory with A's");
            hogged_memory = true;
            return 0;
    } else {
        SYSLOG("[memHog]: Did not hog memory because there is too little free memory");
        return -1;
    }
    return memory_hog;
}

bool free_memory(uint64_t* mem) {
    if(mem == NULL || mem == 1) {
        SYSLOG("[freeMem]: freeing is no need.");
        return true;
    }
    
    free(mem);
    int ret = memorystatus_control(MEMORYSTATUS_CMD_SET_JETSAM_TASK_LIMIT, getpid(), old_memory_limit, NULL, 0);
    if (ret == 0) {
        SYSLOG("[freeMem] The memory limit for pid %d has been set to %u MiB successfully", getpid(), old_memory_limit);
        return true;
    } else {
        SYSLOG("[freeMem] Failed to set memory limit to old: %d (%s)", errno, strerror(errno));
        return false;
    }
    
    return ret == 0 ? true:false;
}
