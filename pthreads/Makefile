# Makefile for pthread bounded buffer programs
#
# Jim Teresco, CS 432
# Williams College
#
# Mon Feb 28 16:06:15 EST 2005

PROGRAMS=prodcons-pthreads-oneempty prodcons-pthreads-counter prodcons-pthreads-counter-cs prodcons-pthreads-counter-sem prodcons-pthreads-counter-mutex
CC=gcc -pthread -g -Wall

all:	$(PROGRAMS)

prodcons-pthreads-oneempty:	prodcons-pthreads-oneempty.c
	$(CC) -o prodcons-pthreads-oneempty prodcons-pthreads-oneempty.c

prodcons-pthreads-counter:	prodcons-pthreads-counter.c
	$(CC) -o prodcons-pthreads-counter prodcons-pthreads-counter.c

prodcons-pthreads-counter-cs:	prodcons-pthreads-counter-cs.c
	$(CC) -o prodcons-pthreads-counter-cs prodcons-pthreads-counter-cs.c

prodcons-pthreads-counter-sem:	prodcons-pthreads-counter-sem.c
	$(CC) -o prodcons-pthreads-counter-sem prodcons-pthreads-counter-sem.c

prodcons-pthreads-counter-mutex:	prodcons-pthreads-counter-mutex.c
	$(CC) -o prodcons-pthreads-counter-mutex prodcons-pthreads-counter-mutex.c

clean::
	/bin/rm -f $(PROGRAMS)
