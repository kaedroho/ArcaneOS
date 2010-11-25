#include "x86/sync.h"
#include "x86/mt.h"
#include "x86/sys.h"

SYSCALL_DEFINE( sync_mutex_lock,
                syscall_state_none, 0, syscall_datatype_ptr
                )(struct sync_mutex* mutex) {
    return mutex->lock(mutex);
}
SYSCALL_DEFINE( sync_mutex_unlock,
                syscall_state_none, 0, syscall_datatype_ptr
                )(struct sync_mutex* mutex) {
    return mutex->unlock(mutex);
}

err_t sync_thread_mutex_lock(struct sync_mutex* mutex) {
    if (mutex->owner && mutex->owner != mt_current_thread)
        return ERR(1, err_facility_sync, sync_err_locked);

    err_t result = ERR_OK;

    unsigned handle = irq_lock();

    if (mutex->owner) {
        if (mutex->owner == mt_current_thread)
            mutex->count++;
        else
            result = ERR(1, err_facility_sync, sync_err_locked);
    } else {
        mutex->owner = mt_current_thread;
        mutex->count = 1;
    }

    irq_unlock(handle);

    return result;
}
err_t sync_thread_mutex_unlock(struct sync_mutex* mutex) {
    if (!mutex->owner || mutex->owner != mt_current_thread)
        return ERR(1, err_facility_sync, sync_err_notowned);

    err_t result = ERR_OK;

    unsigned handle = irq_lock();

    if (mutex->owner) {
        if (mutex->owner == mt_current_thread) {
            if (!--mutex->count)
                mutex->owner = 0;
        } else
            result = ERR(1, err_facility_sync, sync_err_notowned);
    } else {
        result = ERR(1, err_facility_sync, sync_err_notowned);
    }

    irq_unlock(handle);

    return result;
}
