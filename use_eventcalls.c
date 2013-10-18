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

  //Open event.
  int eid = syscall(_doeventopen_);
  printf("\nOpened event %i", eid);

  //Close event.
  syscall(_doeventclose_, eid);
  printf("\nClosed event %i", eid);

  //Open event and verify that it gets the
  //expected number
  int eid = syscall(_doeventopen_);
  printf("\nOpened event %i", eid);

  printf("\nComing back from the kernel level!\n\n");

  return 0;
}
