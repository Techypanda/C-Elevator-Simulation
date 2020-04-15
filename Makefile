# Author: Jonathan Wright
# Date: 7/04/2020
# Conditional Comp: None

CC = gcc
CFLAGS = -Wall -Werror -pedantic -ansi -std=c89 -c
EXEC = lift_sim_A
OBJS = request.o program.o queue.o list.o lifts.o
LINKERFLAGS = -pthread


ifdef DEBUG
CFLAGS += -D DEBUG -g # -g for valgrind
DEBUG: clean $(EXEC)
endif

ifdef SANITIZE
LINKERFLAGS += null
endif

$(EXEC) : $(OBJS)
	$(CC) $(LINKERFLAGS) $(OBJS) -o $(EXEC)

lifts.o : lifts.c lifts.h
	$(CC) $(CFLAGS) lifts.c

program.o : program.c program.h
	$(CC) $(CFLAGS) program.c

request.o : request.c request.h
	$(CC) $(CFLAGS) request.c

queue.o : queue.c queue.h
	$(CC) $(CFLAGS) queue.c

list.o : list.c list.h
	$(CC) $(CFLAGS) list.c

queueTestHarness.o : queueTestHarness.c
	$(CC) $(CFLAGS) queueTestHarness.c

update :
	git pull origin master

queueTest : list.o queue.o queueTestHarness.o
	$(CC) list.o queue.o queueTestHarness.o -o queueTest

siminputgenerator.class : siminputgenerator.java
	javac siminputgenerator.java

siminput : siminputgenerator.class
	java siminputgenerator.java

clean :
	rm -Rf $(EXEC) queueTest *.o *.log *.class
