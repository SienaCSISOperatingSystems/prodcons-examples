#
# Makefile for prodcons-sysvsemaphores example

PROGRAMS=buffer producer consumer

all:	$(PROGRAMS)

buffer:	buffer.c buffer.h
	gcc -Wall -o buffer buffer.c

producer:	producer.c
	gcc -Wall -o producer producer.c

consumer:	consumer.c
	gcc -Wall -o consumer consumer.c

clean::
	/bin/rm -f $(PROGRAMS)
