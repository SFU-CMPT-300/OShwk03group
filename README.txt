OShwk03group
============

int doeventopen()
  Creates a new event, returning event ID on success, -1 on failure.


int doeventclose(int eventID)
  Destroy the event with the given event ID and signal any processes
  waiting on the event to leave the event. Return number of processes
  signaled on success and -1 on failure.

int doeventwait(int eventID)
  Blocks process until the event is signaled. Return 1 on success and
  -1 on failure.

int doeventsig(int eventID)
  Unblocks all waiting processes; ignored if no processes are blocked.
  Return number of processes signaled on success and -1 on failure.

  Implementation:
    Using wait queues (Linux Kernel Development p. 58).  Each eventid
    will have its own wait queue, specifying all tasks which are waiting
    on that event.  wake_up wakes up every task in that queue.
