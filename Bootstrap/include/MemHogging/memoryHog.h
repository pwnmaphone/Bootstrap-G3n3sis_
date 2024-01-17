//
//  memoryHog.h
//  Bootstrap
//
//  Created by Chris Coding on 1/13/24.
//

#ifndef memoryHog_h
#define memoryHog_h

#include <Security/Security.h>
#include <CoreFoundation/CoreFoundation.h>

#define MEMORYSTATUS_CMD_SET_JETSAM_TASK_LIMIT        6
#define MEMORYSTATUS_CMD_GET_MEMLIMIT_PROPERTIES      8

extern bool hogged_memory;

typedef struct CF_BRIDGED_TYPE(id) __SecTask *SecTaskRef;
SecTaskRef SecTaskCreateFromSelf(CFAllocatorRef _Nullable allocator);
CFTypeRef SecTaskCopyValueForEntitlement(SecTaskRef task, CFStringRef entitlement, CFErrorRef *error);

int memorystatus_control(uint32_t command, int32_t pid, uint32_t flags, void *buffer, size_t buffersize);

bool hasEntitlement(CFStringRef entitlement);

typedef struct memorystatus_memlimit_properties {
    int32_t memlimit_active;                /* jetsam memory limit (in MB) when process is active */
    uint32_t memlimit_active_attr;
    int32_t memlimit_inactive;              /* jetsam memory limit (in MB) when process is inactive */
    uint32_t memlimit_inactive_attr;
} memorystatus_memlimit_properties_t;

typedef struct memorystatus_memlimit_properties2 {
    memorystatus_memlimit_properties_t v1;
    uint32_t memlimit_increase;             /* jetsam memory limit increase (in MB) for active and inactive states */
    uint32_t memlimit_increase_bytes;       /* bytes used to determine the jetsam memory limit increase, for active and inactive states */
} memorystatus_memlimit_properties2_t;

uint64_t* Hog_memory(void);
bool free_memory(uint64_t* mem);
#endif /* memoryHog_h */
