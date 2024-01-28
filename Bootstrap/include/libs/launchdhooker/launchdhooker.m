#include <Foundation/Foundation.h>
#include <mach-o/dyld.h>
#include <mach-o/dyld_images.h>
#include <bsm/audit.h>
#include <xpc/xpc.h>
#include <stdio.h>
#include "fishhook.h"
#include <spawn.h>
#include <limits.h>
#include <dirent.h>
#include <stdbool.h>
#include <errno.h>
#include <signal.h>

#define PT_DETACH 11    /* stop tracing a process */
#define PT_ATTACHEXC 14 /* attach to running process with signal exception */
int ptrace(int request, pid_t pid, caddr_t addr, int data);

int posix_spawnattr_set_launch_type_np(posix_spawnattr_t *attr, uint8_t launch_type);

int (*orig_csops)(pid_t pid, unsigned int  ops, void * useraddr, size_t usersize);
int (*orig_csops_audittoken)(pid_t pid, unsigned int  ops, void * useraddr, size_t usersize, audit_token_t * token);
int (*orig_posix_spawn)(pid_t * __restrict pid, const char * __restrict path,
                        const posix_spawn_file_actions_t *file_actions,
                        const posix_spawnattr_t * __restrict attrp,
                        char *const argv[ __restrict], char *const envp[ __restrict]);

int (*orig_posix_spawnp)(pid_t *restrict pid, const char *restrict path, const posix_spawn_file_actions_t *restrict file_actions, const posix_spawnattr_t *restrict attrp, char *const argv[restrict], char *const envp[restrict]);
