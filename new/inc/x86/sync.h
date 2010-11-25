/* 
 * File:   sync.h
 * Author: Diggory
 *
 * Created on 24 November 2010, 23:02
 */

#ifndef SYNC_H
#define	SYNC_H

#include "x86/syscall.h"

struct sync_mutex {
    void* owner;
    int count;
    err_t (*lock)(struct sync_mutex* mutex);
    err_t (*unlock)(struct sync_mutex* mutex);
};

enum sync_err_code {
    sync_err_none = 0,
    sync_err_locked,
    sync_err_notowned
};

SYSCALL_DECLARE(sync_mutex_lock)(struct sync_mutex* mutex);
SYSCALL_DECLARE(sync_mutex_unlock)(struct sync_mutex* mutex);

err_t sync_thread_mutex_lock(struct sync_mutex* mutex);
err_t sync_thread_mutex_unlock(struct sync_mutex* mutex);
#define sync_thread_mutex {0, 0, &sync_thread_mutex_lock, &sync_thread_mutex_unlock};

#endif	/* SYNC_H */

