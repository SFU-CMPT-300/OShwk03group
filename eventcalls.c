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

#include <cs2456/eventcalls.h>

struct event global_event;
rwlock_t eventID_list_lock;
bool event_initialized;

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

void initiate_global(void){
  eventID_list_lock = RW_LOCK_UNLOCKED;

  //  unsigned long flags; // for the lock
  //  read_lock_irqsave(&eventID_list_lock, flags);
  INIT_LIST_HEAD(&global_event.eventID_list);
  //  read_unlock_irqrestore(&eventID_list_lock, flags);

  global_event.eventID = 0;
  init_waitqueue_head(&global_event.waitQ);
  event_initialized = true;
}

asmlinkage int sys_doeventopen(void){
  
  //  if(! event_initialized){
  //    initiate_global();
  //  }

  struct event * my_event = kmalloc(sizeof(struct event), GFP_KERNEL);

  // Initialize the event's list.
  INIT_LIST_HEAD(&(my_event->eventID_list));

  // Add the event's list to the main list.
  unsigned long flags; // for the lock
  write_lock_irqsave(&eventID_list_lock, flags);
  struct event * pos;
  printk("Before Adding\n");
  list_for_each_entry(pos, &global_event.eventID_list, eventID_list){
    printk("%d -> ",pos->eventID);
  }
  printk("\n");

  list_add_tail(&(my_event->eventID_list), &global_event.eventID_list);

  int maxID = list_entry(&(my_event->eventID_list),
                         struct event,
                         eventID_list)->eventID;

  my_event->eventID = maxID + 1;

  printk("%i, %i, %i\n", maxID, my_event->eventID, global_event.eventID);

  init_waitqueue_head(&(my_event->waitQ));
  printk("After Adding\n");
  list_for_each_entry(pos, &global_event.eventID_list, eventID_list){
    printk("%d -> ",pos->eventID);
  }
  printk("\n");

  write_unlock_irqrestore(&eventID_list_lock, flags);

  return my_event->eventID;
}

asmlinkage int sys_doeventclose(int eventID){
  unsigned long flags;

  read_lock_irqsave(&eventID_list_lock, flags);
  struct event * this_event = get_event(eventID);
  read_unlock_irqrestore(&eventID_list_lock, flags);

  int result = sys_doeventsig(eventID);

  write_lock_irqsave(&eventID_list_lock, flags);
  list_del(&(this_event->eventID_list));
  write_unlock_irqrestore(&eventID_list_lock, flags);

  kfree(this_event);

  return result;

  return -1;
}

asmlinkage int sys_doeventwait(int eventID){

  unsigned long flags;
  read_lock_irqsave(&eventID_list_lock, flags);
  struct event * this_event = get_event(eventID);

  int x = this_event->go_aheads;

  read_unlock_irqrestore(&eventID_list_lock, flags);

  while(x==this_event->go_aheads) {
    interruptible_sleep_on(&(this_event->waitQ));
  }

  return 0;
}

asmlinkage int sys_doeventsig(int eventID){

  unsigned long flags;

  write_lock_irqsave(&eventID_list_lock, flags);

  struct event * this_event = get_event(eventID);
  // Right here, the event could be deleted by another process.
  // But not with this handy write lock!
  this_event->go_aheads++;
  // Same as above.
  int result = kernel_list_length(&(this_event->waitQ.task_list));
  wake_up_interruptible(&(this_event->waitQ));

  write_unlock_irqrestore(&eventID_list_lock, flags);

  return result;

  return -1;
}

/* TODO:
 * Prevent spurious wakeups.  Could be done by using
   uninterruptible sleep and wake_up, but that prevents
   important interrupts like ctrl-C. (Done)
 * Lock to ensure integrity of each eventID_list. (Done)

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
