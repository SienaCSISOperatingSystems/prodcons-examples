/*
  Producer-consumer example with POSIX shmem and SysV semaphores

  Jim Teresco, Williams College
  March, 2005
  Updated October 2006
  Updated March 2008, Mount Holyoke College
  Updated February 2012, Siena College
  
  Note: the allocated semaphores can be seen with the ipcs command
*/

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>

#include "buffer.h"

static int segment_id;
static shared_data *data;
/* three semaphore IDs */
static int emptyslots, fullslots, mutex;

/* signal handler that will clean up the shmem */
void cleanup(int sig) {
  int error = 0;

  if (sig != -1)
    printf("Buffer got signal %d, cleaning up and exiting\n", sig);
  /* detach from shared memory segment */
  if (shmdt((void *)data) == -1) {
    perror("shmdt");
    error = 1;
  }
  
  /* free shared memory segment */
  if (shmctl(segment_id, IPC_RMID, NULL) == -1) {
    perror("shmctl");
    error = 1;
  }

  /* free the semaphores */
  if (semctl(emptyslots, 0, IPC_RMID, NULL) == -1) {
    perror("semctl (emptyslots)");
    error = 1;
  }
  if (semctl(fullslots, 0, IPC_RMID, NULL) == -1) {
    perror("semctl (fullslots)");
    error = 1;
  }
  if (semctl(mutex, 0, IPC_RMID, NULL) == -1) {
    perror("semctl (mutex)");
    error = 1;
  }

  exit(error);
}

int main(int argc, char *argv[]) {
  union semun {
    int     val;            /* value for SETVAL */
    struct  semid_ds *buf;  /* buffer for IPC_STAT & IPC_SET */
    u_short *array;         /* array for GETALL & SETALL */
  } argument;

  /* allocate a chunk of shared memory */
  /* This is a "named" shmem chunk -- the same name will be used
     by the producer and consumer processes, and we request read
     and write access, and create the segment (IPC_CREAT).
     Once we allocate it, we can the allocation from the command line
     with the ipcs command */
  segment_id = shmget(SHMEM_ID, sizeof(shared_data), SHM_R|SHM_W|IPC_CREAT);
  if (segment_id == -1) {
    perror("shmget");
    exit(1);
  }

  /* attach a pointer to the shared memory */
  data = (shared_data *)shmat(segment_id, NULL, 0);
  if (data == (shared_data *)-1) {
    perror("shmat");
    exit(1);
  }

  data->in = 0;
  data->out = 0;

  /* trap the crtl-c or kill -TERM that might kill this process so we 
     can clean up.  Note: other signals will be able to kill this job without
     the appropriate cleanup taking place
  */
  if (signal(SIGINT, cleanup) == SIG_ERR) {
    perror("signal");
    cleanup(-1);
  }
  if (signal(SIGTERM, cleanup) == SIG_ERR) {
    perror("signal");
    cleanup(-1);
  }

  /* Create the semaphores */
  /* in each case here, we get a named semaphore using a name that will
     also be used by the producers and/or consumers, we create an array
     of 1 semaphore, and create it (IPC_CREAT) with read access for the
     user (SEM_R), and alter access for the user (SEM_A).  Once created,
     the semaphores can also be seen with the ipcs command.  */
  if ((fullslots = semget(FULLSLOTS, 1, SEM_R|SEM_A|IPC_CREAT)) == -1) {
    perror("semget (fullslots)");
    cleanup(-1);
  }
  if ((emptyslots = semget(EMPTYSLOTS, 1, SEM_R|SEM_A|IPC_CREAT)) == -1) {
    perror("semget (emptyslots)");
    cleanup(-1);
  }
  if ((mutex = semget(MUTEX, 1, SEM_R|SEM_A|IPC_CREAT)) == -1) {
    perror("semget (mutex)");
    cleanup(-1);
  }

  /* set initial values of the semaphores */
  argument.val = BUFFER_SIZE;
  if (semctl(emptyslots, 0, SETVAL, argument) == -1) {
    perror("semctl (emptyslots)");
    cleanup(-1);
  }
  argument.val = 0;
  if (semctl(fullslots, 0, SETVAL, argument) == -1) {
    perror("semctl (fullslots)");
    cleanup(-1);
  }
  argument.val = 1;
  if (semctl(mutex, 0, SETVAL, argument) == -1) {
    perror("semctl (mutex)");
    cleanup(-1);
  }

  /* now sit here and sleep -- awaken only on response to a signal */
  /* the program will terminate when the cleanup function is called,
     does its thing, then calls exit() */
  while (1) {
    sleep(100);
  }

  /* this should never happen */
  return 1;
}
