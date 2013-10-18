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

    // Test signalling.
    pid=fork();
    switch(pid){
        case 0:
            fork();
            fork();
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
            sigd=syscall(_sys_doeventsig_,event);
            printf("%d processes signaled\n",sigd);
            printf("sleeping to let the childeren wait\n");
            sleep(2);
            printf("closing event %d ...\n",event);
            sigd=syscall(_sys_doeventclose_,event);
            printf("%d processes signaled\n",sigd);
            break;
        
    }
    
    int eid = syscall(_doeventopen_);
    printf("\nOpened event %i", eid);

    syscall(_doeventclose_, eid);
    printf("\nClosed event %i", eid);

    eid = syscall(_doeventopen_);
    printf("\nOpened event %i", eid);

    syscall(_doeventclose_, eid);
    printf("\nClosed event %i", eid);

    

}