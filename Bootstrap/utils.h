#ifndef utils_h
#define utils_h

#import <Foundation/Foundation.h>
#include "include/krw.h"
#include "libkfd.h"

struct  namecache {
    TAILQ_ENTRY(namecache)  nc_entry;       /* chain of all entries */
    TAILQ_ENTRY(namecache)  nc_child;       /* chain of ncp's that are children of a vp */
    union {
        LIST_ENTRY(namecache)  nc_link; /* chain of ncp's that 'name' a vp */
        TAILQ_ENTRY(namecache) nc_negentry; /* chain of ncp's that 'name' a vp */
    } nc_un;
    LIST_ENTRY(namecache)   nc_hash;        /* hash chain */
    vnode_t                 nc_dvp;         /* vnode of parent of name */
    vnode_t                 nc_vp;          /* vnode the name refers to */
    unsigned int            nc_hashval;     /* hashval of stringname */
    const char              *nc_name;       /* pointer to segment name in string cache */
};

extern const char** environ;

#define ADDRISVALID(val) ((val) >= 0xffff000000000000 && (val) != 0xffffffffffffffff)

uint64_t exploit_runner(const char *exploit_string, uint64_t pages);

bool enable_sbInjection(u64 kfd,int method);

uint64_t jbrand();

uint64_t jbrand_new();

NSString* find_jbroot();

NSString* jbroot(NSString *path);

int is_jbroot_name(const char* name);

NSString* rootfsPrefix(NSString* path);

NSString* getBootSession();

int spawn(const char* path, const char** argv, const char** envp, void(^std_out)(char*,int), void(^std_err)(char*,int));

int spawnBootstrap(const char** argv, NSString** stdOut, NSString** stdErr);

int spawnRoot(NSString* path, NSArray* args, NSString** stdOut, NSString** stdErr);

void machoGetInfo(FILE* candidateFile, bool *isMachoOut, bool *isLibraryOut);

BOOL isDefaultInstallationPath(NSString* _path);

void killAllForApp(const char* bundlePath);



@interface LSApplicationWorkspace : NSObject
+ (id)defaultWorkspace;
- (BOOL)openApplicationWithBundleID:(id)arg1;
- (BOOL)_LSPrivateRebuildApplicationDatabasesForSystemApps:(BOOL)arg1
                                                  internal:(BOOL)arg2
                                                      user:(BOOL)arg3;
@end

@interface LSPlugInKitProxy : NSObject
+(id)pluginKitProxyForIdentifier:(id)arg1 ;
- (NSString *)bundleIdentifier;
@property (nonatomic,readonly) NSURL *dataContainerURL;
@end

#endif /* utils_h */
