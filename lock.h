#ifndef __LOCK_H
#define __LOCK_H

#ifdef cplusplus
extern "C" {
#endif

#define MUTEX_ACQUIRED 0
#define MUTEX_FAILED !MUTEX_LOCKED

typedef int spinlock;
typedef int atomic_int;

int try_lock(spinlock* s);
int get_lock(spinlock* s);
int free_lock(spinlock* s);

int atomic_inc(atomic_int* i);
int atomic_dec(atomic_int* i);
int atomic_set(atomic_int* i, int val);

#ifdef cplusplus
}
#endif

#endif
