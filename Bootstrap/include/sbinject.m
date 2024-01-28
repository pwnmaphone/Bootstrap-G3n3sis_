//
//  sbinject.m
//  Bootstrap
//
//  Created by Chris Coding on 1/20/24.
//

#import <Foundation/Foundation.h>
#include "../bootstrap.h"
#include "sbinject.h"

int file_index;
kern_return_t kr;
bool setupyes;
bool isxpc = false;

// vnode functions and more

uint64_t unsign_kptr(uint64_t pac_kaddr) {
    if ((pac_kaddr & 0xFFFFFF0000000000) == 0xFFFFFF0000000000) {
        return pac_kaddr;
    }
    if(t1sz_boot != 0) {
        return pac_kaddr |= ~((1ULL << (64U - t1sz_boot)) - 1U);
    }
    return pac_kaddr;
}

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


bool Setup_Injection(const char *injectloc, const char *newinjectloc, bool forxpc) {
    
    const char *SpringBoardPath = "/System/Library/CoreServices/SpringBoard.app";
    int returnval;
    NSFileManager* FM = [NSFileManager defaultManager];
    NSError* errhandle;
    NSString* fastSignPath = [NSBundle.mainBundle.bundlePath stringByAppendingPathComponent:@"basebin/fastPathSign"];
    NSString* ldidPath = [NSBundle.mainBundle.bundlePath stringByAppendingPathComponent:@"basebin/ldid"];
    NSString* launchdents = [NSBundle.mainBundle.bundlePath stringByAppendingPathComponent:@"basebin/launchdents.plist"];
    NSString* xpcents = [NSBundle.mainBundle.bundlePath stringByAppendingPathComponent:@"basebin/xpcents.plist"];// need to modify file to have actual xpc ents
    NSString* sbents = [NSBundle.mainBundle.bundlePath stringByAppendingPathComponent:@"basebin/SpringBoardEnts.plist"];
    
    SYSLOG("[Setup Inject] setting up environment for SB Injection");
    
    if([FM fileExistsAtPath:@(newinjectloc)] == 0) {
        SYSLOG("[Setup Inject] NOTICE: (%s) already exists, we're gonna go ahead and resign then return..", newinjectloc);
        goto resign;
    }
    
    if(access(injectloc, F_OK) != 0) {
        SYSLOG("[Setup Inject] ERR: we can't access %s", injectloc);
        return false;
    }
    
    // 1) copy over injectloc to boostrap location
    kr = [FM copyItemAtPath:@(injectloc) toPath:@(newinjectloc) error:&errhandle];
    if(kr != KERN_SUCCESS) {
        SYSLOG("[Setup Inject] ERR: unable to copy xpc/launchd to path! error-string: (%s)", [[errhandle localizedDescription] UTF8String]);
        return false;
    }
    
    SYSLOG("[Setup Inject] copied xpc/launchd binary at path");
    
resign:;
    
    // 2) Copy over springboard.app to bootstrap path
    
    kr = [FM createDirectoryAtURL:jbroot(@"/System/Library/CoreServices/") withIntermediateDirectories:YES attributes:nil error:&errhandle];
    if(kr != KERN_SUCCESS) {
        SYSLOG("[Setup Inject] ERR: unable to create dummy SB path");
        return false;
    }
    kr = [FM copyItemAtPath:@(SpringBoardPath) toPath:jbroot(@(SpringBoardPath)) error:&errhandle];
    if(kr != KERN_SUCCESS) {
        SYSLOG("[Setup Inject] ERR: unable to copy SpringBoard to jbroot path, error-string: (%s)", [[errhandle localizedDescription] UTF8String]);
        goto setupfailed;
    }
    
    // 3) Sign fake SB
    
    returnval = spawnRoot(ldidPath, @[@"-M", sbents, [jbroot(@(SpringBoardPath)) stringByAppendingPathComponent:@"SpringBoard"]], nil, nil);
    if(returnval != 0) {
        SYSLOG("[Setup Inject] ERR: unable to sign fake SpringBoard binary");
        goto setupfailed;
    }
    
    SYSLOG("[Setup Inject] fake SpringBoard was been signed");
    
    // we're gonna sign them with the respective entitlements
    if(forxpc) {
        returnval = spawnRoot(ldidPath, @[@"-M", xpcents, @(newinjectloc)], nil, nil);
    } else {
        returnval = spawnRoot(ldidPath, @[@"-M", launchdents, @(newinjectloc)], nil, nil);
    }
    if(returnval != 0) {
        SYSLOG("[Setup Inject] ERR: an issue occured signing (%s)", newinjectloc);
        return false;
    }
    
    SYSLOG("[Setup Inject] (%s) - was signed successfully", newinjectloc);
    return true;
    
setupfailed:;
    remove(jbroot(@(SpringBoardPath)).UTF8String);
    remove(jbroot(@"/System/Library/CoreServices").UTF8String);
    return false;
}



// Credit to wh1te4ever for vnode swapping - https://github.com/wh1te4ever/kfund/blob/972651c0b4c81098b844b29d17741cb445772c74/kfd/fun/vnode.m#L217

bool enable_sbInjection(u64 kfd,int method) {
    
    SYSLOG("[SB Injection] enabling SB Injection..");
    
    /* location variables */
    _offsets_init(); // initiate offsets
    init_krw(kfd);
    int fd;
    int fd2;
    struct vnode rootvnode = {0};
    struct vnode lcdfilevnode = {0};
    struct vnode flcdfilevnode = {0}; // fake
    struct vnode xpcfilevnode = {0};
    struct vnode fxpcfilevnode = {0}; // fake
    struct namecache nc = {0};
    struct kfd essential;
    u64 selfproc = 0;
    u64 launchvnode = 0;
    
    /* file handling & location variables */
    NSFileManager *FM = [NSFileManager defaultManager];
    const char *Bootstrap_patchloc = jbroot(@"/BSTRPFiles").UTF8String;
    const char *xpc_origlocation = "/usr/libexec/xpcproxy";
    const char *xpc_new_location = Bootstrap_patchloc + *"/xpcproxy"; // TODO:
    const char *lcd_origlocation = "/sbin/launchd";
    const char *new_lcd_location = Bootstrap_patchloc + *"/launchd";

    /*
     We're gonna comment this out since I would want the fakes to be signed upon a reboot to ensure no signing issues + reboot obviously reverts changes
    
     if([FM fileExistsAtPath:@(xpc_new_location)] || [FM fileExistsAtPath:@(new_lcd_location)]) {
        SYSLOG("[SB Injection] NOTICE: patched versions of xpc and/or launchd already exists");
        return true;
    }
*/
    if(![FM fileExistsAtPath:@(Bootstrap_patchloc) isDirectory:YES]) {
        kr = mkdir(Bootstrap_patchloc, 0777);
        if(![FM fileExistsAtPath:@(Bootstrap_patchloc) isDirectory:YES] || kr != KERN_SUCCESS) {
            SYSLOG("[SB Injection] ERR: unable to create patch path");
            return false;
        }
    }
    
    if(method == 1) {
        setupyes = Setup_Injection(lcd_origlocation, new_lcd_location, isxpc);
    } else {
        isxpc = true;
        setupyes = Setup_Injection(xpc_origlocation, xpc_new_location, isxpc);
    }
    
    if(!setupyes) {
        SYSLOG("[SB Injection] ERR: unable to setup injection environment");
        return false;
    }
    
    if(method == 2) {
        goto xpc;
    }
    
    SYSLOG("[SB Injection] executing launchd patch");
    
    launchvnode = getVnodeAtPath(lcd_origlocation);
    if(!ADDRISVALID(launchvnode)) {
        SYSLOG("[SB Injection] ERR: unable to get launchd vnode");
        return false;
    }
    
    if(running_IO == false) { // running_IO = true = device on ios 15
        
    // setting up the u64 locations with structs
    SYSLOG("[SB Injection] modifying namecache (iOS 16)");
    kreadbuf(launchvnode, &lcdfilevnode, sizeof(lcdfilevnode)) ;
    kwrite32(launchvnode+off_vnode_v_usecount, lcdfilevnode.v_usecount+1);
    u64 replace_lcd = getVnodeAtPath(new_lcd_location);
    if(ADDRISVALID(replace_lcd)) {
        SYSLOG("[SB Injection] ERR: Unable to get fake launchd vnode");
        goto failure;
    }
        
    kreadbuf(replace_lcd, &flcdfilevnode, sizeof(flcdfilevnode));
    kwrite32(replace_lcd+off_vnode_v_usecount, flcdfilevnode.v_usecount+1);
    
    SYSLOG("[SB Injection] launchd: %11x, fake launchd: %11x", lcdfilevnode, flcdfilevnode);
        
    // 1) get launchd namecache
        
    u64 launchnc = kread64(launchvnode + off_vnode_v_ncchildren_tqh_first);
    kreadbuf(launchnc, &nc, sizeof(nc));
    
    if(strcmp(nc.nc_name, "launchd") != 0) {
        SYSLOG("[SB Injection] ERR: We don't have the correct namecache!");
        goto failure;
    }
        
    // 2) find & replace launchd's node with the fakes
        
    u64 lcdncvp = (u64)nc.nc_vp;
    if(!ADDRISVALID(lcdncvp)) {
        lcdncvp = kread64(launchnc + off_namecache_nc_vp);
        if(ADDRISVALID(lcdncvp)) {
            SYSLOG("[SB Injection] ERR: unable to grab launchd nc vnode");
            goto failure;
        }
    }
        
    SYSLOG("[SB Injection] replacing namecache vnode pointer");
        
    kwrite64(lcdncvp, replace_lcd);
        
    SYSLOG("[SB Injection] launched namecache node pointer replaced with fake vnode!");
    return true;
 /*
resignL1:;
    SYSLOG("[SB Injection] signing launchd (iOS 16)");
    rebuildSignature(@(new_lcd_location));
    SYSLOG("[SB Injection] fake launchd has been signed! (iOS 16)");
    return true;
            */
    } else {goto ios15;}
        
ios15:;
    SYSLOG("[SB Injection] changing NSExecutablePath (iOS 15)");
        
    /* TODO: find a proper method for this...
    \
    \
    \
    \
    \
    \
    \
    \
    */
    return true;
/*
resignL2:;
    SYSLOG("[SB Injection] signing launchd (iOS 15)");
    rebuildSignature(@(new_lcd_location));
    SYSLOG("[SB Injection] fake launchd has been signed! (iOS 15)");
    return true;
      */
        
xpc:; // xpc method *should* work on ios 15 & 16, we can use this for now-
        
    SYSLOG("[SB Injection] executing xpcproxy patch");
        
    fd = open(xpc_origlocation, O_RDONLY);
    if(fd == -1) {
        SYSLOG("ERR: Unable to open xpcproxy");
        goto failure;
    }
    SYSLOG("fd returned: %d", fd);
        
    off_t xpcfile_size = lseek(fd, 0, SEEK_END);
    SYSLOG("xpcfile_size: %llx", xpcfile_size);
    if(xpcfile_size <= 0 ) { goto failure; }
        
    /* Ensure fake xpcproxy exists before continuing */
    if(![FM fileExistsAtPath:@(xpc_new_location)]) {
        SYSLOG("ERR: Fake xpcproxy is not in the right location");
        goto failure;
    }
    fd2 = open(xpc_new_location, O_RDONLY);
    if(fd2 == -1) {
        SYSLOG("ERR: Unable to open fake xpcproxy");
        goto failure;
    }
    SYSLOG("fd2 returned: %d", fd2);
    off_t xpcfake_size = lseek(fd2, 0, SEEK_END);
    SYSLOG("xpcfile_size: %llx", xpcfile_size);
    if(xpcfake_size <= 0 ) { goto failure; }

    selfproc = essential.info.kernel.current_proc;
    if(!ADDRISVALID(selfproc)) {
        selfproc = getProc(getpid());
        if(!ADDRISVALID(selfproc)) {
            SYSLOG("[SB Injection] ERR: unable to get self proc");
            goto failure;
        }
    }
    
    u64 filedesc_pac = kread64(selfproc + off_p_pfd);
    u64 filedesc = unsign_kptr(filedesc_pac);
    u64 xpcfile = kread64(filedesc + (8 * fd));
    u64 fglob_pac = kread64(xpcfile + off_fp_glob);
    u64 fglob = unsign_kptr(fglob_pac);
    u64 xpcvnode_pac = kread64(fglob + off_fg_data);
    u64 xpcvnode = unsign_kptr(xpcvnode_pac);
    
    kreadbuf(xpcvnode, &xpcfilevnode, sizeof(xpcfilevnode));
    
    u64 launchd_proc = getProc(1);
    u64 textvp_pac = kread64(launchd_proc + off_p_textvp);
    u64 textvp = unsign_kptr(textvp_pac);
    u64 sbin_vnode = unsign_kptr(kread64(textvp + off_vnode_v_parent));
    u64 root_vnode = unsign_kptr(kread64(sbin_vnode + off_vnode_v_parent));
    if(!ADDRISVALID(root_vnode)) {
        SYSLOG("[SB Injection] ERR: unable to get rootvnode");
        goto failure;
    }
    
    kreadbuf(root_vnode, &rootvnode, sizeof(rootvnode));
    SYSLOG("[SB Injection] got rootvnode at: %llx", root_vnode);
    
    u64 rootmount = (u64)rootvnode.v_mount;
    if(!ADDRISVALID(rootmount)) {
        u64 rootmount_pac = kread64(root_vnode + off_vnode_v_mount);
        rootmount = unsign_kptr(rootmount_pac);
    }
    
    u32 rootflags = kread32(rootmount + off_mount_mnt_flag);
    kwrite32(rootmount + off_mount_mnt_flag, rootflags & ~MNT_RDONLY);
    kwrite32(fglob + off_fg_flag, FREAD | FWRITE);
    
    if(xpcfilevnode.v_writecount <= 0) {
        kwrite32((u32)xpcfilevnode.v_writecount, (u32)xpcfilevnode.v_writecount + 1); // dont trust this so i'ma do the long method too lol
        
        u32 xpc_writecount = kread32(xpcvnode + off_vnode_v_writecount);
        kwrite32(xpcvnode + off_vnode_v_writecount, xpc_writecount + 1);
    }
    
    // map xpc & fakexpc then replace
    char* xpcmap = mmap(NULL, xpcfile_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if(xpcmap == MAP_FAILED) {
        SYSLOG("[SB Injection] ERR: unable to map xpcproxy");
        goto failure;
    }
    
    char* fakexpcmap = mmap(NULL, xpcfake_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0);
    if(fakexpcmap == MAP_FAILED) {
        SYSLOG("[SB Injection] ERR: unable to map fake xpcproxy");
        goto failure;
    }
    
    SYSLOG("[SB Injection] xpcproxy & fake xpcproxy mapped");
    
    memcpy(fakexpcmap, xpcmap, xpcfile_size);
    SYSLOG("[SB Injection] msync returned: %d", msync(fakexpcmap, xpcfake_size, MS_SYNC));
    
    // unmap and revert changes of fglob & rootvnode
    munmap(xpcmap, xpcfile_size);
    munmap(fakexpcmap, xpcfake_size);
    
    kwrite32(fglob + off_fg_flag, FREAD);
    kwrite32(rootmount + off_mount_mnt_flag, rootflags);
    
    close(fd);
    close(fd2);
    
    SYSLOG("[SB Injection] xpcproxy has been patched!");
    return true;
/*
resignX:;
        
    SYSLOG("[SB Injection] Signing xpcproxy");
    rebuildSignature(@(xpc_new_location));
        
    if(fd && fd2 != 0) {
        close(fd);
        close(fd2);
    }
    
    SYSLOG("[SB Injection] fake xpcproxy has been signed!");
 */
        
failure:;
    kwrite32(rootmount + off_mount_mnt_flag, rootflags);
    if(fd != 0) close(fd);
    if(fd2 != 0) close(fd2);
    if(file_index != 0) close(file_index);
    if([FM fileExistsAtPath:@(Bootstrap_patchloc) isDirectory:YES]) {
        remove(xpc_new_location);
        remove(new_lcd_location);
        remove(Bootstrap_patchloc);
    }
    return false;
}
