#include "thread.h"

#include <SDL.h>
#include <stdlib.h>

struct px_thread { SDL_Thread *thread; int priority, core; };
struct px_mutex { SDL_mutex *mutex; };
struct px_event { SDL_mutex *mutex; SDL_cond *cond; int signalled; enum px_event_reset reset; };
struct px_semaphore { SDL_mutex *mutex; SDL_cond *cond; uint32_t count, maximum; };

struct px_thread *px_thread_create(px_thread_fn fn, void *arg, const char *name, int priority, int core)
{
    struct px_thread *t = calloc(1, sizeof *t);
    if (!t) return NULL;
    t->priority = priority; t->core = core;
    t->thread = SDL_CreateThread(fn, name ? name : "px", arg);
    if (!t->thread) { free(t); return NULL; }
    return t;
}

int px_thread_join(struct px_thread *t)
{
    int status = 0;
    if (!t) return 0;
    SDL_WaitThread(t->thread, &status);
    free(t);
    return status;
}

int px_thread_priority(const struct px_thread *t) { return t ? t->priority : 0; }

struct px_mutex *px_mutex_create(void)
{
    struct px_mutex *m = calloc(1, sizeof *m);
    if (m && !(m->mutex = SDL_CreateMutex())) { free(m); return NULL; }
    return m;
}
void px_mutex_lock(struct px_mutex *m) { SDL_LockMutex(m->mutex); }
void px_mutex_unlock(struct px_mutex *m) { SDL_UnlockMutex(m->mutex); }
void px_mutex_destroy(struct px_mutex *m) { if (m) { SDL_DestroyMutex(m->mutex); free(m); } }

struct px_event *px_event_create(enum px_event_reset reset)
{
    struct px_event *e = calloc(1, sizeof *e);
    if (!e) return NULL;
    e->reset = reset;
    e->mutex = SDL_CreateMutex(); e->cond = SDL_CreateCond();
    if (!e->mutex || !e->cond) { px_event_destroy(e); return NULL; }
    return e;
}
void px_event_signal(struct px_event *e)
{
    SDL_LockMutex(e->mutex);
    e->signalled = 1;
    if (e->reset == PX_EVENT_STICKY) SDL_CondBroadcast(e->cond); else SDL_CondSignal(e->cond);
    SDL_UnlockMutex(e->mutex);
}
void px_event_clear(struct px_event *e) { SDL_LockMutex(e->mutex); e->signalled = 0; SDL_UnlockMutex(e->mutex); }
int px_event_wait(struct px_event *e, int64_t timeout_ns)
{
    SDL_LockMutex(e->mutex);
    const Uint64 deadline = timeout_ns < 0 ? 0 : SDL_GetTicks64() + (Uint64)(timeout_ns / 1000000);
    while (!e->signalled) {
        if (timeout_ns < 0) SDL_CondWait(e->cond, e->mutex);
        else {
            const Uint64 now = SDL_GetTicks64();
            if (now >= deadline || SDL_CondWaitTimeout(e->cond, e->mutex, (Uint32)(deadline - now)) == SDL_MUTEX_TIMEDOUT)
                if (!e->signalled) break;
        }
    }
    const int got = e->signalled;
    if (got && e->reset == PX_EVENT_ONESHOT) e->signalled = 0;
    SDL_UnlockMutex(e->mutex);
    return got;
}
void px_event_destroy(struct px_event *e)
{
    if (!e) return;
    if (e->cond) SDL_DestroyCond(e->cond);
    if (e->mutex) SDL_DestroyMutex(e->mutex);
    free(e);
}

struct px_semaphore *px_semaphore_create(uint32_t initial, uint32_t maximum)
{
    if (initial > maximum) return NULL;
    struct px_semaphore *s = calloc(1, sizeof *s);
    if (!s) return NULL;
    s->count = initial; s->maximum = maximum;
    s->mutex = SDL_CreateMutex(); s->cond = SDL_CreateCond();
    if (!s->mutex || !s->cond) { px_semaphore_destroy(s); return NULL; }
    return s;
}
int px_semaphore_release(struct px_semaphore *s, uint32_t count)
{
    SDL_LockMutex(s->mutex);
    const int ok = count <= s->maximum - s->count;
    if (ok) { s->count += count; SDL_CondBroadcast(s->cond); }
    SDL_UnlockMutex(s->mutex);
    return ok;
}
void px_semaphore_acquire(struct px_semaphore *s)
{
    SDL_LockMutex(s->mutex);
    while (!s->count) SDL_CondWait(s->cond, s->mutex);
    --s->count;
    SDL_UnlockMutex(s->mutex);
}
void px_semaphore_destroy(struct px_semaphore *s)
{
    if (!s) return;
    if (s->cond) SDL_DestroyCond(s->cond);
    if (s->mutex) SDL_DestroyMutex(s->mutex);
    free(s);
}
