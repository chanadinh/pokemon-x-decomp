/* Threads, a mutex, both event kinds and a semaphore on the SDL backend. */
#include "thread.h"

#include <assert.h>
#include <stdio.h>

static struct px_mutex *mutex;
static struct px_semaphore *slots;
static struct px_event *go;
static int counter;

static int worker(void *arg)
{
    px_event_wait(go, -1); /* sticky: every worker passes */
    for (int i = 0; i < 1000; ++i) { px_mutex_lock(mutex); ++counter; px_mutex_unlock(mutex); }
    px_semaphore_release(slots, 1);
    return *(int *)arg;
}

int main(void)
{
    mutex = px_mutex_create(); slots = px_semaphore_create(0, 4); go = px_event_create(PX_EVENT_STICKY);
    assert(mutex && slots && go && !px_semaphore_create(5, 4));
    int ids[4] = {1, 2, 3, 4};
    struct px_thread *t[4];
    for (int i = 0; i < 4; ++i) assert((t[i] = px_thread_create(worker, &ids[i], "worker", 0x30, -2)));
    assert(px_thread_priority(t[0]) == 0x30);
    px_event_signal(go);
    for (int i = 0; i < 4; ++i) px_semaphore_acquire(slots);
    int sum = 0;
    for (int i = 0; i < 4; ++i) sum += px_thread_join(t[i]);
    assert(sum == 10 && counter == 4000);
    assert(!px_semaphore_release(slots, 5) && px_semaphore_release(slots, 4));
    struct px_event *once = px_event_create(PX_EVENT_ONESHOT);
    assert(!px_event_wait(once, 1000000)); /* times out */
    px_event_signal(once);
    assert(px_event_wait(once, 0) && !px_event_wait(once, 1000000)); /* cleared by the wake */
    assert(px_event_wait(go, 0) && px_event_wait(go, 0));              /* sticky stays */
    px_event_clear(go);
    assert(!px_event_wait(go, 0));
    px_event_destroy(once); px_event_destroy(go); px_semaphore_destroy(slots); px_mutex_destroy(mutex);
    puts("thread tests passed");
    return 0;
}
