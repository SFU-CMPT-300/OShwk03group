#include <stdio.h>
#include <linux/unistd.h>
#include <sys/syscall.h>

#define _sys_doeventopen_ 327
#define _sys_doeventclose_ 328
#define _sys_doeventwait_ 329
#define _sys_doeventsig_ 330

int main(int argc, char *argv[]){
    int i;
    int pid;
    int event;
    event=syscall(_sys_doeventopen_);
    pid=fork();
    switch(pid){
        case 0:
            printf("child waiting on %d\n",event);
            syscall(_sys_doeventwait_,event);
            printf("child woken!\n");
            printf("child waiting on %d\n",event);
            syscall(_sys_doeventwait_,event);
            printf("child woken!\n");
            break;
        case -1:
            printf("could not fork\n");
            break;
        default:
            printf("sleeping to let the childeren wait\n");
            sleep(2);
            printf("signaling event %d ...\n",event);
            syscall(_sys_doeventsig_,event);
            printf("sleeping to let the childeren wait\n");
            sleep(2);
            printf("closing event %d ...\n",event);
            syscall(_sys_doeventclose_,event);
            break;
        
    }
}
