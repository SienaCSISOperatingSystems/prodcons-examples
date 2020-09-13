/*
  Producer-consumer example with POSIX shmem and SysV semaphores

  Buffer data type header file, defining data structure
  and fixed shmem and semaphore identifiers

  Jim Teresco, Williams College
  March, 2005

*/

#define BUFFER_SIZE 5

typedef struct {
  int buffer[BUFFER_SIZE];
  int in;
  int out;
} shared_data;

#define SHMEM_ID 93
#define FULLSLOTS 2065
#define EMPTYSLOTS 2066
#define MUTEX 2067
