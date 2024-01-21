//
//  jbtools.m
//  Bootstrap
//
//  Created by Chris Coding on 1/20/24.
//

#import <Foundation/Foundation.h>
#include "jbtools.h"


// vnode functions and more

uint64_t getProc(pid_t pid) {
    uint64_t proc = get_kernproc();
    printf("[+] kernproc: 0x%llx\n", proc);
    
    while (true) {
        if(kread32(proc + off_p_pid) == pid) {
            printf("[+] found proc: 0x%llx\n", proc);
            return proc;
        }
        proc = kread64(proc + off_p_list_le_prev);
        if(!proc) {
            return -1;
        }
    }
    printf("[-] getProc failed\n");
    return 0;
}

uint64_t getVnodeAtPath(char* filename) {
    printf("[+] getVnodeAtPath(%s)\n", filename);
    int file_index = open(filename, O_RDONLY);
    if (file_index == -1) return -1;
    
    uint64_t proc = getProc(getpid());
    printf("[+] proc: 0x%llx\n", proc);

    uint64_t filedesc_pac = kread64(proc + off_p_pfd);
    uint64_t filedesc = filedesc_pac | pac_mask;
    uint64_t openedfile = kread64(filedesc + (8 * file_index));
    uint64_t fileglob_pac = kread64(openedfile + off_fp_glob);
    uint64_t fileglob = fileglob_pac | pac_mask;
    uint64_t vnode_pac = kread64(fileglob + off_fg_data);
    uint64_t vnode = vnode_pac | pac_mask;
    
    close(file_index);
    
    return vnode;
}

uint64_t getVnodeAtPathByChdir(char *path) {
    printf("[+] getVnodeAtPathByChdir(%s)\n", path);
    if(access(path, F_OK) == -1) {
        printf("access not OK\n");
        return -1;
    }
    if(chdir(path) == -1) {
        printf("chdir not OK\n");
        return -1;
    }
    uint64_t fd_cdir_vp = kread64(getProc(getpid()) + off_p_pfd + off_fd_cdir);
    chdir("/");
    printf("[+] fd_cdir_vp: 0x%llx\n", fd_cdir_vp);
    return fd_cdir_vp;
}






// Credit to wh1te4ever for vnode swapping - https://github.com/wh1te4ever/kfund/blob/972651c0b4c81098b844b29d17741cb445772c74/kfd/fun/vnode.m#L217

bool enable_sbInjection(u64 kfd,int method) {
    
    SYSLOG("[SB Injection] enabling SB Injection..");

    _offsets_init(); // initiate offsets
    init_krw(kfd);
    int fd;
    int fd2;                            // this sorta looks like a super car... like Thrust SSC...
    kern_return_t kr;
    NSFileManager *FM = [NSFileManager defaultManager];
    const char *xpc_origlocation = "/usr/libexec/xpcproxy";
    const char *xpc_new_location = "/var/mobile/xpcproxy"; // TODO: Make this modified xpc, include it in this project, move it to this location
    const char *lcd_origlocation = "/sbin/launchd";
    const char *Bootstrap_patchloc = "/var/mobile/Documents/BSTRPFiles";
    
    if([FM fileExistsAtPath:@"/var/mobile/Documents/BSTRPFiles/xpcproxy"] || [FM fileExistsAtPath:@"/var/mobile/Documents/BSTRPFiles/launchd"]) {
        SYSLOG("[SB Injection] patched versions of xpc and/or launchd exists, just gonna resign them");
        if(method == 1) {
            if(running_IO) {
                goto resignL2;
            }
            goto resignL1;
        } else {
            goto resignX;
        }
        
    }
    
    if(method == 2) {
        goto xpc;
    }
    
    SYSLOG("[SB Injection] executing launchd patch");
    
    uint64_t launchvnode = getVnodeAtPath(lcd_origlocation);
    if(!ADDRISVALID(launchvnode)) {
        SYSLOG("[SB Injection] ERR: unable to get launchd vnode");
        return false;
    }
    
    if(![FM fileExistsAtPath:@(Bootstrap_patchloc) isDirectory:YES]) {
        kr = mkdir(Bootstrap_patchloc, 0777);
        if(![FM fileExistsAtPath:@(Bootstrap_patchloc) isDirectory:YES]) {
            SYSLOG("[SB Injection] ERR: unable to create patch path");
            return false;
        }
    }
    
    if(running_IO == false) { // running_IO = true = device on ios 15
        
        // modify namecache - TODO
        SYSLOG("[SB Injection] modifying namecache (iOS 16)");
        
        /*
         \
         \
         \
         */
        
resignL1:;
        SYSLOG("[SB Injection] signing launchd (iOS 16)");
        
        /*
         \
         \
         \
         */
        
        return true;
        
    } else {goto ios15;}
    
    
    
ios15:;
    SYSLOG("[SB Injection] changing NSExecutablePath (iOS 15)");
    
    /*
     \
     \
     \
     */
    
    
resignL2:;
    SYSLOG("[SB Injection] signing launchd (iOS 15)");
    
    /*
     \
     \
     \
     */
    return true;
    
    
xpc:;
    
    SYSLOG("[SB Injection] executing xpcproxy patch");
    
    fd = open(xpc_origlocation, O_RDONLY);
    if(fd == -1) {
        SYSLOG("ERR: Unable to open xpcproxy");
        return false;
    }
    SYSLOG("fd returned: %d", fd);
    
    off_t xpcfile_size = lseek(fd, 0, SEEK_END);
    SYSLOG("xpcfile_size: %llx", xpcfile_size);
    if(xpcfile_size <= 0 ) { return false; }
    
    // move fake xpcproxy to Bootstrap files location
    
    /*
     \
     \
     \
     */
    
    /* Ensure fake xpcproxy exists before continuing */
    if(![FM fileExistsAtPath:@"/var/mobile/Documents/BSTRPFiles/xpcproxy"]) {
        SYSLOG("ERR: Fake xpcproxy is not in the right location");
        return false;
    }
    fd2 = open(xpc_new_location, O_RDONLY);
    if(fd2 == -1) {
        SYSLOG("ERR: Unable to open fake xpcproxy");
        return false;
    }
    SYSLOG("fd2 returned: %d", fd2);
    off_t xpcfake_size = lseek(fd2, 0, SEEK_END);
    SYSLOG("xpcfile_size: %llx", xpcfile_size);
    if(xpcfake_size <= 0 ) { return false; }
    
    // swap vnodes - TODO
    
    /*
     \
     \
     \
     */
    
resignX:;
    
    SYSLOG("[SB Injection] Signing xpcproxy");
    
    /*
     \
     \
     \
     */
    
    if(fd && fd2 != 0) {
        close(fd);
        close(fd2);
    }
    
    return true;
}

