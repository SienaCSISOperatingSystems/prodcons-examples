#
# Makefile for prodcons-shmem example
#

PROGRAMS=prodcons-shmem-oneempty prodcons-shmem-counter

all:	$(PROGRAMS)

prodcons-shmem-oneempty:	prodcons-shmem-oneempty.c
	gcc -Wall -o prodcons-shmem-oneempty prodcons-shmem-oneempty.c

prodcons-shmem-counter:	prodcons-shmem-counter.c
	gcc -Wall -o prodcons-shmem-counter prodcons-shmem-counter.c

clean::
	/bin/rm -f $(PROGRAMS)
