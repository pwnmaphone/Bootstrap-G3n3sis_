//
//  kread_IOSurface.h
//  Bootstrap
//
//  Created by Chris Coding on 1/18/24.
//

#ifndef kread_IOSurface_h
#define kread_IOSurface_h

#include <stdio.h>
#include "../../krkw.h"

void kread_IOSurface_init(struct kfd* kfd);

void kread_IOSurface_allocate(struct kfd* kfd, uint64_t id);

bool kread_IOSurface_search(struct kfd* kfd, uint64_t object_uaddr);

void kread_IOSurface_kread(struct kfd* kfd, uint64_t kaddr, void* uaddr, uint64_t size);

void kread_IOSurface_find_proc(struct kfd* kfd);

void kread_IOSurface_deallocate(struct kfd* kfd, uint64_t id);

void kread_IOSurface_free(struct kfd* kfd);

uint32_t kread_IOSurface_kread_u32(struct kfd* kfd, uint64_t kaddr);
#endif /* kread_IOSurface_h */
