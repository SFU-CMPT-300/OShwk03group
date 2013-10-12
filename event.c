#include <linux/wait.h>

struct event
{
  int eventID;
  wait_queue_head_t waitQ;
  //  init_waitqueue_head(&waitQ);
};
