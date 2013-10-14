#include <stdio.h>
#include <linux/unistd.h>
//#include <linux/prinfo.h> //Our stuff!
#include <linux/types.h>
#include <sys/syscall.h>

#define _doeventopen_  327
#define _doeventclose_ 328
#define _doeventwait_  329
#define _doeventsig_   330


int main(int argc, char *argv[])
{
  printf("\nMaking a kernel level system call!\n\n");

  //  syscall(_prinfo_, &x); // x was a struct prinfo.
  //sys_cs2456_test(&x);
  syscall(_doeventopen_);
  printf("\nComing back from the kernel level!\n\n");

  return 0;
}
