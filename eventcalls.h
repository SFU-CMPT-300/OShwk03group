#ifndef __EVENTCALLS_H
#define __EVENTCALLS_H

#include <linux/list.h>
#include <linux/wait.h>
#include <linux/spinlock.h>

int kernel_list_length(struct list_head * lh);

struct event * get_event(int eventID);

void initiate_global(void);

asmlinkage int sys_doeventopen(void);
asmlinkage int sys_doeventclose(int eventID);
asmlinkage int sys_doeventwait(int eventID);
asmlinkage int sys_doeventsig(int eventID);

struct event
{
  int eventID;

  // INIT_LIST_HEAD(&my_event.eventID_list)
  // needs to be done afterward in
  // sys_doeventopen().

  struct list_head eventID_list;

  // init_waitqueue_head(&waitQ) needs to be
  // done afterward in sys_doeventopen().
  wait_queue_head_t waitQ;
  int go_aheads;
};

extern rwlock_t eventID_list_lock;

extern struct event global_event;

// extern bool event_initialized = false;
extern bool event_initialized;

#endif
