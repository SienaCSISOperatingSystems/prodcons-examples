/*
  Producer-consumer example with POSIX shared memory.

  This one can fail -- both processes coud try to modify the
  shared variable counter concurrently and cause loss of information

  Jim Teresco, Williams College
  February, 2005

  Updated for CSIS 330, Siena College, Spring 2012
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define BUFFER_SIZE 5
#define NUMBER_OF_ITEMS 30

typedef struct {
  int buffer[BUFFER_SIZE];
  int in;
  int out;
  int counter;
} shared_data;

int main(int argc, char *argv[]) {
  int i;
  int segment_id;
  shared_data *data;
  long spin;

  /* allocate a chunk of shared memory */
  segment_id = shmget(IPC_PRIVATE, sizeof(shared_data), SHM_R|SHM_W);

  /* attach a pointer to the shared memory */
  data = (shared_data *)shmat(segment_id, NULL, 0);

  data->in = 0;
  data->out = 0;
  data->counter = 0;

  if (fork() == 0) {
    /* child process -- the consumer */

    /* seed the random number generator on pid */
    srand(getpid());

    for (i=0; i<NUMBER_OF_ITEMS; i++) {

      /* look for a value */
      spin = 0;
      while (data->counter == 0) {
	if (spin == 0) {
	  printf("C: waiting for item\n");
	}
	spin++;
      }
      if (spin > 0) {
	printf("C: done waiting (cycled %ld times)\n", spin);
      }

      /* consume the next available item */
      printf("C: consuming value %d from slot %d (counter=%d->%d)\n", 
	     data->buffer[data->out], data->out, data->counter, 
	     data->counter-1);
      data->out = (data->out + 1)%BUFFER_SIZE;
      /* here's our problem: */
      data->counter--;

      /* simulate the cost of consuming the item */
      /* this is slightly longer than the producer to increase the
	 chances of filling up the buffer */
      sleep(rand()%5+1);
    }

    exit(0);
  }
  else {
    /* parent process -- the producer */

    /* seed the random number generator on pid */
    srand(getpid());

    for (i=0; i<NUMBER_OF_ITEMS; i++) {

      /* simulate the cost of producing the item by 
	 sleeping for a small random number of seconds */
      sleep(rand()%4+1);

      /* put the produced value in the buffer when there's space */
      printf("P: produced %d\n", i);

      spin = 0;
      while (data->counter == BUFFER_SIZE) {
	if (spin == 0) {
	  printf("P: waiting for open slot\n");
	}
	spin++;
      }
      if (spin > 0) {
	printf("P: done waiting (cycled %ld times)\n", spin);
      }

      printf("P: adding item %d at slot %d (counter=%d->%d)\n", i, data->in,
	     data->counter, data->counter+1);

      data->buffer[data->in] = i;

      data->in = (data->in + 1)%BUFFER_SIZE;
      /* here's our problem: */
      data->counter++;
    }

    /* all done producing, now wait for the child to exit */
    wait(NULL);

  }
  /* detach from shared memory segment */
  shmdt(data);
  
  /* free shared memory segment */
  shmctl(segment_id, IPC_RMID, NULL);

  return 0;
}
