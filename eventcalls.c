//headers from timer.c
#include <linux/kernel_stat.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/percpu.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/swap.h>
#include <linux/pid_namespace.h>
#include <linux/notifier.h>
#include <linux/thread_info.h>
#include <linux/time.h>
#include <linux/jiffies.h>
#include <linux/posix-timers.h>
#include <linux/cpu.h>
#include <linux/syscalls.h>
#include <linux/delay.h>
#include <linux/tick.h>
#include <linux/kallsyms.h>

#include <asm/uaccess.h>
#include <asm/unistd.h>
#include <asm/div64.h>
#include <asm/timex.h>
#include <asm/io.h>

#include <linux/kernel.h>
// Good linux kernel linked list explanations at:
// (1) http://isis.poly.edu/kulesh/stuff/src/klist/
// (2) http://www.makelinux.net/ldd3/chp-11-sect-5

#include <linux/wait.h>
#include <linux/list.h>
// Note to self, this website told us to use asm/spinlock.h,
// but that didn't work...
// http://www.linuxgrill.com/anonymous/fire/netfilter/
// kernel-hacking-HOWTO-5.html
#include <linux/spinlock.h>

// Prototypes:
int kernel_list_length(struct list_head * lh);

struct event * get_event(int eventID);

void initiate_global(void);

asmlinkage int sys_doeventopen(void);
asmlinkage int sys_doeventclose(int eventID);
asmlinkage int sys_doeventwait(int eventID);
asmlinkage int sys_doeventsig(int eventID);

// Define event.
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

  bool go_ahead;
};

rwlock_t eventID_list_lock;

// Declare and initialize the eventID_list.
// Was going to do this, but need events to persist
// outside of methods, will they in this structure?
struct event global_event;
bool initiated = false;

int kernel_list_length(struct list_head * lh){
  int result = 0;
  struct list_head * pos;
  list_for_each(pos, lh) {
    result++;
  }
  return result;
}

struct event * get_event(int eventID){
  
  struct event * pos;

  list_for_each_entry(pos, &global_event.eventID_list, eventID_list){
    if(pos->eventID == eventID){
      return pos;
    }
  }
}

void initiate_global(){
  INIT_LIST_HEAD(&global_event.eventID_list);
  global_event.eventID = 0;
  init_waitqueue_head(&global_event.waitQ);
  eventID_list_lock = RW_LOCK_UNLOCKED;
  initiated = true;
}

asmlinkage int sys_doeventopen(){
  if(! initiated){
    initiate_global();
  }

  struct event * my_event = kmalloc(sizeof(my_event), GFP_KERNEL);
  // Initialize the event's list.
  INIT_LIST_HEAD(&(my_event->eventID_list));

  // Add the event's list to the main list.
  list_add_tail(&(my_event->eventID_list), &global_event.eventID_list);

  

  int maxID = list_entry(&(my_event->eventID_list),
                         struct event,
                         eventID_list)->eventID;

  my_event->eventID = maxID + 1;

  init_waitqueue_head(&(my_event->waitQ));
  return my_event->eventID;
}

asmlinkage int sys_doeventclose(int eventID){

  struct event * this_event = get_event(eventID);
  int result = sys_doeventsig(eventID);
  list_del(&(this_event->eventID_list));
  kfree(this_event);

  return result;

  return -1;
}

asmlinkage int sys_doeventwait(int eventID){
  struct event * this_event = get_event(eventID);
  this_event->go_ahead = false;

  while(!this_event->go_ahead) {
    interruptible_sleep_on(&(this_event->waitQ));
  }

  return 0;
}

asmlinkage int sys_doeventsig(int eventID){
  struct event * this_event = get_event(eventID);
  this_event->go_ahead = true;

  int result = kernel_list_length(&(this_event->waitQ.task_list));

  wake_up_interruptible(&(this_event->waitQ));

  return result;

  return -1;
}

/* TODO:
 * Prevent spurious wakeups.  Could be done by using
   uninterruptible sleep and wake_up, but that prevents
   important interrupts like ctrl-C.
 * Lock to ensure integrity of each eventID_list.

 */



  // This just prints the first eventID as a test.  It has
  // nothing to do with the intended purpose of doeventclose,
  // but the good news is that it works :)
/*  printk("%i", list_first_entry(&global_event.eventID_list,
                                struct event,
                                eventID_list)->eventID); */


//  struct event * pos;
//  struct event * n;
  //  list_for_each_entry_safe(pos, n,
  //                          &global_event.eventID_list, eventID_list){
  // if(pos->eventID == eventID){
  //   sys_doeventsig(eventID);
  //   list_del(&(pos->eventID_list));
  //   kfree(pos);
  //  }
  // }


  /*  DEFINE_WAIT(wait);
  add_wait_queue(&(this_event->waitQ), &wait);

  while(!this_event->go_ahead) {
    
    prepare_to_wait(&(this_event->waitQ), &wait, TASK_INTERRUPTIBLE);
  }
  finish_wait(&(this_event->waitQ), &wait);
  */
