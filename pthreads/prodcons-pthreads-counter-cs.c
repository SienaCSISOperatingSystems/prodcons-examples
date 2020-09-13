/*
  Producer-consumer example with pthreads

  Both processes could try to modify the shared variable counter
  concurrently and cause loss of information, but this is avoided by
  using Peterson's algorithm to prevent unsafe access

  Jim Teresco, Williams College
  February, 2005

*/

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define BUFFER_SIZE 5
#define NUMBER_OF_ITEMS 30

/* the shared data structures -- just global variables in this case */
int buffer[BUFFER_SIZE];
int in;
int out;
int counter;
/* shared variables to implement Peterson's Algorithm to protect
   counter */
int turn=0;
int flag[2] = {0,0};

/* producer thread */
void producer(void *args) {
  int i;
  long spin;

  for (i=0; i<NUMBER_OF_ITEMS; i++) {
    
    /* simulate the cost of producing the item by 
       sleeping for a small random number of seconds */
    sleep(rand()%4+1);
    
    /* put the produced value in the buffer when there's space */
    printf("P: produced %d\n", i);
    
    spin = 0;
    while (counter == BUFFER_SIZE) {
      if (spin == 0) {
	printf("P: waiting for open slot\n");
      }
      spin++;
    }
    if (spin > 0) {
      printf("P: done waiting (cycled %ld times)\n", spin);
    }
    
    printf("P: adding item %d at slot %d (counter=%d->%d)\n", i, in,
	   counter, counter+1);
    
    buffer[in] = i;
    
    in = (in + 1)%BUFFER_SIZE;
    /* Need to protect this modification of counter with mutual exclusion */
    /* Enter CS begins */
    flag[0] = 1;
    turn = 1;
    while (flag[1] && turn==1); /* busy wait */
    /* Enter CS ends */

    /* we can now modify counter */
    counter++;

    /* Exit CS begins */
    flag[0] = 0;
    /* Exit CS ends */
  }
}

/* consumer thread */
void consumer(void *args) {
  long spin;
  int i;
  
  for (i=0; i<NUMBER_OF_ITEMS; i++) {
    
    /* look for a value */
    spin = 0;
    while (counter == 0) {
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
	   buffer[out], out, counter, counter-1);
    out = (out + 1)%BUFFER_SIZE;
    /* Need to protect this modification of counter with mutual exclusion */
    /* Enter CS begins */
    flag[1] = 1;
    turn = 0;
    while (flag[0] && turn==0); /* busy wait */
    /* Enter CS ends */

    /* we can now modify counter */
    counter--;

    /* Exit CS begins */
    flag[1] = 0;
    /* Exit CS ends */
    
    /* simulate the cost of consuming the item */
    /* this is slightly longer than the producer to increase the
       chances of filling up the buffer */
    sleep(rand()%5+1);
  }
}

/* main program, just starts up the threads */
int main(int argc, char *argv[]) {
  pthread_t producer_id, consumer_id;
  int rc;

  /* initialize shared data */
  in = 0;
  out = 0;
  counter = 0;

  /* seed the random number generator on pid */
  srand(getpid());

  /* create the consumer */
  rc = pthread_create(&consumer_id, NULL, (void *)&consumer, NULL);

  if (rc != 0) {
    fprintf(stderr, "Could not create consumer child thread\n");
    exit(1);
  }

  /* create the producer */
  rc = pthread_create(&producer_id, NULL, (void *)&producer, NULL);

  if (rc != 0) {
    fprintf(stderr, "Could not create producer child thread\n");
    exit(1);
  }

  /* wait for the child threads to exit */
  pthread_join(producer_id,NULL);
  pthread_join(consumer_id,NULL);
 
  printf("Final counter is %d\n", counter);

  return 0;
}
