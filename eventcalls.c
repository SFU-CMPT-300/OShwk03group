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

#include "event.c"

asmlinkage int sys_doeventopen(){
  struct event my_event;
  return 0;
}

asmlinkage int sys_doeventclose(){
  struct event my_event;
  return 0;
}

asmlinkage int sys_doeventwait(){
  struct event my_event;
  return 0;
}

asmlinkage int sys_doeventsig(){
  struct event my_event;
  return 0;
}
