# Author: Jonathan Wright
# Date: 7/04/2020
# Conditional Comp: None

CC = gcc
CFLAGS = -Wall -Werror -pedantic -ansi -std=c89 -D _DEFAULT_SOURCE -lrt -pthread -c
EXEC = lift_sim_A
OBJS = request.o program.o queue.o list.o lifts.o
LINKERFLAGS = -pthread -lrt -D _DEFAULT_SOURCE

ifdef DEBUG
CFLAGS += -D DEBUG -g # -g for valgrind
DEBUG: clean $(EXEC)
endif

ifdef SANITIZETHREAD
LINKERFLAGS += -fsanitize=thread
CFLAGS += -fsanitize=thread
SANITIZE: clean $(EXEC)
endif

ifdef NOTSLEEP
CFLAGS += -D NOTSLEEP
NOTSLEEP: clean $(EXEC)
endif

ifdef OUTSIMASSTDOUT
CFLAGS += -D OUTSIMASSTDOUT
OUTSIMASSTDOUT: clean $(EXEC)
endif

ifdef PROCESS
CFLAGS += -D PROCESS
PROCESS: clean $(EXEC)
endif

ifdef PTHREAD
CFLAGS += -D PTHREAD
PTHREAD: clean $(EXEC)
endif

$(EXEC) : $(OBJS)
	$(CC) $(LINKERFLAGS) $(OBJS) -o $(EXEC)

lifts.o : lifts.c lifts.h
	$(CC) $(CFLAGS) lifts.c

program.o : program.c program.h
	$(CC) $(CFLAGS) program.c

request.o : request.c request.h
	$(CC) $(CFLAGS) request.c

list.o : list.c list.h
	$(CC) $(CFLAGS) list.c

sharedBuffer.o : sharedBuffer.c sharedBuffer.h
	$(CC) $(CFLAGS) sharedBuffer.c

queueTestHarness.o : queueTestHarness.c
	$(CC) $(CFLAGS) queueTestHarness.c

sharedlist.o : sharedlist.c sharedlist.h
	$(CC) $(CFLAGS) sharedlist.c

listTest.o : listTest.c
	$(CC) $(CFLAGS) listTest.c

update :
	git pull origin master

queue.o : queue.c queue.h
	$(CC) $(CFLAGS) queue.c

queueTest : list.o queue.o queueTestHarness.o
	$(CC) list.o queue.o queueTestHarness.o -o queueTest

sharedTest : sharedlist.o listTest.o
	$(CC) sharedlist.o listTest.o -o sharedTest

siminputgenerator.class : siminputgenerator.java
	javac siminputgenerator.java

siminput : siminputgenerator.class
	java siminputgenerator.java

clean :
	rm -Rf $(EXEC) queueTest *.o *.log *.class out_sim
