/*
  Producer-consumer example with POSIX shmem and SysV semaphores

  Producer implementation

  Jim Teresco, Williams College
  March, 2005
  Updated March 2008, Mount Holyoke College
  Updated February 2012, Siena College
*/

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include "buffer.h"

int main(int argc, char *argv[]) {
  int number_of_items;
  int item_id;
  int i;
  int segment_id;
  shared_data *data;

  /* semaphore IDs */
  int emptyslots, fullslots, mutex;
  /* operation parameter for the semaphore ops */
  struct sembuf operations[1];

  /* first parameter is how many items to produce */
  number_of_items = 10;
  if (argc > 1) {
    number_of_items = atoi(argv[1]);
  }

  /* second parameter is the starting ID of items (they're just ints) */
  item_id = 1;
  if (argc > 2) {
    item_id = atoi(argv[2]);
  }

  /* get access to the chunk of shared memory that is the buffer */
  /* we use the same shared memory ID as the buffer processes,
     and get it for reading and writing, but unlike in the buffer, we don't
     specify the create flag IPC_CREAT */
  segment_id = shmget(SHMEM_ID, sizeof(shared_data), SHM_R|SHM_W);

  /* attach a pointer to the shared memory */
  data = (shared_data *)shmat(segment_id, NULL, 0);

  /* get access to the semaphores, again using names but not creating them,
     since they were created by the buffer process. */
  if ((fullslots = semget(FULLSLOTS, 1, SEM_R|SEM_A)) == -1) {
    perror("semget (fullslots)");
  }
  if ((emptyslots = semget(EMPTYSLOTS, 1, SEM_R|SEM_A)) == -1) {
    perror("semget (emptyslots)");
  }
  if ((mutex = semget(MUTEX, 1, SEM_R|SEM_A)) == -1) {
    perror("semget (mutex)");
  }
  
  /* seed the random number generator on pid */
  srand(getpid());
  
  for (i=0; i<number_of_items; i++) {

    /* simulate the cost of producing the item by 
       sleeping for a small random number of seconds */
    sleep(rand()%4+1);
    
    /* put the produced value in the buffer when there's space */
    printf("%s [%d]: produced %d\n", argv[0], getpid(), item_id);
    
    /* WAIT(EMPTYSLOTS); */
    /* which semaphore in the array? */
    operations[0].sem_num = 0;
    /* what to do (wait == subtract 1) */
    operations[0].sem_op = -1;
    /* no flags set here means wait if necessary (don't just return) */
    operations[0].sem_flg = 0;
    
    /* do the actual wait operation */
    if (semop(emptyslots, operations, 1) == -1) {
      perror("semop (wait emptyslots)");
    }
    
    /* WAIT(MUTEX); */
    /* which semaphore in the array? */
    operations[0].sem_num = 0;
    /* what to do (wait == subtract 1) */
    operations[0].sem_op = -1;
    /* no flags set here means wait if necessary (don't just return) */
    operations[0].sem_flg = 0;
    
    /* do the actual wait operation */
    if (semop(mutex, operations, 1) == -1) {
      perror("semop (wait mutex)");
    }
    
    printf("%s [%d]: adding item %d at slot %d\n", 
	   argv[0], getpid(), item_id, data->in);
    
    data->buffer[data->in] = item_id;
    
    data->in = (data->in + 1)%BUFFER_SIZE;
    
    /* SIGNAL(MUTEX); */
    /* which semaphore in the array? */
    operations[0].sem_num = 0;
    /* what to do (signal == add 1) */
    operations[0].sem_op = 1;
    /* no flags set here means wait if necessary (don't just return) */
    operations[0].sem_flg = 0;
    
    /* do the actual wait operation */
    if (semop(mutex, operations, 1) == -1) {
      perror("semop (signal mutex)");
    }
      
    /* SIGNAL(FULLSLOTS); */
    /* which semaphore in the array? */
    operations[0].sem_num = 0;
    /* what to do (signal == add 1) */
    operations[0].sem_op = 1;
    /* no flags set here means wait if necessary (don't just return) */
    operations[0].sem_flg = 0;
    
    /* do the actual wait operation */
    if (semop(fullslots, operations, 1) == -1) {
      perror("semop (signal fullslots)");
    }
    
    /* bump up item ID for next item to be produced */
    item_id++;
    
  }
  /* detach from shared memory segment */
  shmdt(data);
  
  return 0;
}
