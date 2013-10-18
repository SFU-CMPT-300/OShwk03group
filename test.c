#include <stdio.h>
#include <linux/unistd.h>
#include <sys/syscall.h>

#define _doeventopen_ 327
#define _doeventclose_ 328
#define _doeventwait_ 329
#define _doeventsig_ 330

int main(int argc, char *argv[]){
    int i;
    int pid;
    int event;
    int sigd;
    event=syscall(_doeventopen_);

    // Multiple things waiting on multiple events.

    int eid1, eid2, eid3;
    eid1 = syscall(_doeventopen_);
    eid2 = syscall(_doeventopen_);
    eid3 = syscall(_doeventopen_);

    pid = fork();

    switch(pid){
        case 0:
          pid = fork();
          syscall(_doeventwait_, eid1);
          printf("Woken by signal to event %i \n", eid1);
          exit(0);
        case -1:
          printf("This should NEVER happen. \n");
          exit(-1);
        default:
          pid = fork();
          switch(pid){
          case 0:
            syscall(_doeventwait_, eid2);
            printf("Woken by signal to event %i \n", eid2);
            exit(0);
          case -1:
            printf("This should NEVER happen. \n");
            exit(-1);
          default:
            pid = fork();
            switch(pid){
            case 0:
              syscall(_doeventwait_, eid3);
              printf("Woken by signal to event %i \n", eid3);
              exit(0);
            case -1:
              printf("This should NEVER happen. \n");
              exit(-1);
            default:
              sleep(1);
              int num_signalled_1 = syscall(_doeventsig_, eid1);
              printf("Signalled %i processes \n", num_signalled_1);
              int num_signalled_2 = syscall(_doeventsig_, eid2);
              printf("Signalled %i processes \n", num_signalled_2);
              int num_signalled_3 = syscall(_doeventsig_, eid3);
              printf("Signalled %i processes \n", num_signalled_3);
            break;
            }
          }
    }

    syscall(_doeventclose_, eid1);
    printf("Closed event %i \n", eid1);
    syscall(_doeventclose_, eid2);
    printf("Closed event %i \n", eid2);
    syscall(_doeventclose_, eid3);
    printf("Closed event %i \n", eid3);

    // Open and close - make sure closed eid becomes
    // available if it's the last one.
    int eid = syscall(_doeventopen_);
    printf("Opened event %i \n", eid);

    syscall(_doeventclose_, eid);
    printf("Closed event %i \n", eid);

    eid = syscall(_doeventopen_);
    printf("Opened event %i \n", eid);

    syscall(_doeventclose_, eid);
    printf("Closed event %i \n", eid);

}
