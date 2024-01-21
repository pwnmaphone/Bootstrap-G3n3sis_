//
//  jbtools.h
//  Bootstrap
//
//  Created by Chris Coding on 1/20/24.
//

#ifndef jbtools_h
#define jbtools_h

#include "libkfd.h"
#include "krw.h"
#include "utils.h"

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

bool enable_sbInjection(u64 kfd,int method);
#endif /* jbtools_h */
