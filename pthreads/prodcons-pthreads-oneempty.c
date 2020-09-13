/*
  Producer-consumer example with pthreads

  This one avoids trouble by always leaving one slot empty in the buffer

  Jim Teresco, Williams College
  February, 2005

  Updated for CSIS 330, Siena College, Spring 2012

*/

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define BUFFER_SIZE 5
#define NUMBER_OF_ITEMS 30

/* shared variables */
int buffer[BUFFER_SIZE];
int in;
int out;

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
    while (((in+1)%BUFFER_SIZE) == out) {
      if (spin == 0) {
	printf("P: waiting for open slot\n");
      }
      spin++;
    }
    if (spin > 0) {
      printf("P: done waiting (cycled %ld times)\n", spin);
    }
    
    printf("P: adding item %d at slot %d (in=%d,out=%d)\n", i, in,
	   in, out);
    
    buffer[in] = i;
    
    in = (in + 1)%BUFFER_SIZE;
  }
}

/* consumer thread */
void consumer(void *args) {
  long spin;
  int i;

  for (i=0; i<NUMBER_OF_ITEMS; i++) {
    
    /* look for a value */
    spin = 0;
    while (in == out) {
      if (spin == 0) {
	printf("C: waiting for item\n");
      }
      spin++;
    }
    if (spin > 0) {
      printf("C: done waiting (cycled %ld times)\n", spin);
    }
    
    /* consume the next available item */
    printf("C: consuming value %d from slot %d (in=%d,out=%d)\n", 
	   buffer[out], out, in, out);
    out = (out + 1)%BUFFER_SIZE;
    
    /* simulate the cost of consuming the item */
    /* this is slightly longer than the producer to increase the
       chances of filling up the buffer */
    sleep(rand()%5+1);
  } 
}

/* main program - just starts up threads */
int main(int argc, char *argv[]) {
  pthread_t producer_id, consumer_id;
  int rc;

  /* initialize the shared data */
  in = 0;
  out = 0;

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
 
  return 0;
}
