#ifndef sched_scheduler_h
#define sched_scheduler_h

#include <pony.h>
#include <platform.h>
#include "actor/messageq.h"
#include "gc/gc.h"
#include "mpmcq.h"

PONY_EXTERN_C_BEGIN

typedef void (*trace_object_fn)(pony_ctx_t* ctx, void* p, pony_trace_fn f);

typedef void (*trace_actor_fn)(pony_ctx_t* ctx, pony_actor_t* actor);

typedef struct scheduler_t scheduler_t;

typedef struct pony_ctx_t
{
  scheduler_t* scheduler;
  pony_actor_t* current;
  trace_object_fn trace_object;
  trace_actor_fn trace_actor;
  gcstack_t* stack;
  actormap_t acquire;
} pony_ctx_t;

struct scheduler_t
{
  // These are rarely changed.
  pony_thread_id_t tid;
  uint32_t cpu;
  uint32_t node;
  bool terminate;
  bool asio_stopped;

  // These are changed primarily by the owning scheduler thread.
  __pony_spec_align__(struct scheduler_t* last_victim, 64);

  pony_ctx_t ctx;
  uint32_t block_count;
  uint32_t ack_token;
  uint32_t ack_count;

  // These are accessed by other scheduler threads. The mpmcq_t is aligned.
  mpmcq_t q;
  messageq_t mq;
};

pony_ctx_t* scheduler_init(uint32_t threads, bool noyield);

bool scheduler_start(bool library);

void scheduler_stop();

void scheduler_add(pony_ctx_t* ctx, pony_actor_t* actor);

uint32_t scheduler_cores();

void scheduler_terminate();

PONY_EXTERN_C_END

#endif
