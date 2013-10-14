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
#include <linux/prinfo.h> //Our stuff!

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
};

// Declare and initialize the eventID_list.
// Was going to do this, but need events to persist
// outside of methods, will they in this structure?
struct event global_event;
bool initiated = false;

void initiate_global(){
  INIT_LIST_HEAD(&global_event.eventID_list);
  global_event.eventID = 0;
  init_waitqueue_head(&global_event.waitQ);
  initiated = true;  
}

asmlinkage int sys_doeventopen(){
  if(! initiated){
    initiate_global();
  }

  struct event my_event;

  // Initialize the event's list.
  INIT_LIST_HEAD(&my_event.eventID_list);

  // Add the event's list to the main list.
  list_add_tail(&my_event.eventID_list, &global_event.eventID_list);

  //  my_event.eventID = eventID_list->prev.eventID + 1;

  init_waitqueue_head(&my_event.waitQ);
  return my_event.eventID;
}

asmlinkage int sys_doeventclose(int eventID){
  if(! initiated){
    initiate_global();
  }
  // This just prints the first eventID as a test.  It has
  // nothing to do with the intended purpose of doeventclose,
  // but the good news is that it works :)
  printk("%i", list_first_entry(&global_event.eventID_list,
                                struct event,
                                eventID_list)->eventID);
  return 0;
}

asmlinkage int sys_doeventwait(int eventID){
  if(! initiated){
    initiate_global();
  }

  struct event my_event;
  return 0;
}

asmlinkage int sys_doeventsig(int eventID){
  if(! initiated){
    initiate_global();
  }
  struct event my_event;
  return 0;
}
