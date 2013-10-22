OShwk03group
============

The implementations are as follows:


struct event
  Contains a WaitQueue (called waitQ) and an eventID.  WaitQueue is
  implemented in <linux/wait.h>.

struct event * get_event(int eventID)
  Returns a pointer to the event with id "eventID"

void initiate_global(void)
  Initiates a global event so that there is a place to build a linked
  list of other events.

int doeventopen()
  Creates a new event, returning event ID on success, -1 on failure.
  
    Implementation:
      Creates a new eventid.  Implement eventIDlist as kernel linked list.
      Add the new id to the linked list, and return the id.

int doeventclose(int eventID)
  Destroys the event with the given event ID and signal any processes
  waiting on the event to leave the event. Return number of processes
  signaled on success and -1 on failure.

    Implementation:
      Store size of waitQ, call doeventsig(eventID), removes the id from
      eventIDlist, and returns the stored size of waitQueue.

int doeventwait(int eventID)
  Blocks process until the event is signaled. Return 1 on success and
  -1 on failure.

    Implementation:
      Add the calling process to the waitQ for eventID.  (What is success
      and what is failure?)

int doeventsig(int eventID)
  Unblocks all waiting processes; ignored if no processes are blocked.
  Return number of processes signaled on success and -1 on failure.

    Implementation:
      Using wait queues (Linux Kernel Development p. 58).  Each eventid
      will have its own wait queue, specifying all tasks which are waiting
      on that event.  wake_up wakes up every task in that queue.
