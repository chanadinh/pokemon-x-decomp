#ifndef PX_THREAD_H
#define PX_THREAD_H
#include <stdint.h>

/* Threads and synchronisation in place of the 3DS kernel's (threads with a
 * priority and a preferred core; mutexes; events that reset on wake-up or
 * stay signalled; counting semaphores), on SDL's primitives. Priority and
 * core are recorded but not enforced: the host schedules. Code that needs an
 * order must synchronise for it; the renderer's bands, for instance, each
 * draw every triangle in submission order into disjoint rows, so their
 * output does not depend on scheduling. */
struct px_thread; struct px_mutex; struct px_event; struct px_semaphore;
typedef int (*px_thread_fn)(void *arg);
/* 0x18..0x3f is the 3DS application priority range (lower runs first). */
struct px_thread *px_thread_create(px_thread_fn fn, void *arg, const char *name, int priority, int core);
int px_thread_join(struct px_thread *thread); /* returns fn's result; frees the thread */
int px_thread_priority(const struct px_thread *thread);
struct px_mutex *px_mutex_create(void);
void px_mutex_lock(struct px_mutex *mutex);
void px_mutex_unlock(struct px_mutex *mutex);
void px_mutex_destroy(struct px_mutex *mutex);
enum px_event_reset { PX_EVENT_ONESHOT = 0, PX_EVENT_STICKY = 1 };
struct px_event *px_event_create(enum px_event_reset reset);
void px_event_signal(struct px_event *event);
void px_event_clear(struct px_event *event);
/* Waits until signalled (timeout_ns < 0: forever); 1 if signalled. A
 * one-shot event clears as it wakes one waiter. */
int px_event_wait(struct px_event *event, int64_t timeout_ns);
void px_event_destroy(struct px_event *event);
struct px_semaphore *px_semaphore_create(uint32_t initial, uint32_t maximum);
int px_semaphore_release(struct px_semaphore *semaphore, uint32_t count); /* 0 beyond the maximum */
void px_semaphore_acquire(struct px_semaphore *semaphore);
void px_semaphore_destroy(struct px_semaphore *semaphore);
#endif
