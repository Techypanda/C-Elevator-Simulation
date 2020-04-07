# Author: Jonathan Wright
# Date: 7/04/2020
# Conditional Comp: None

CC = gcc
CFLAGS = -Wall -Werror -pedantic -ansi -std=c89 -c89 -c
EXEC = lift_sim_A
OBJS = request.o

ifdef DEBUG
CFLAGS += -D DEBUG -g # -g for valgrind
DEBUG: clean $(EXEC)
endif

$(EXEC) : $(OBJS)
	$(CC) $(OBJS) -o $(EXEC)

request.o : request.c request.h
	$(CC) $(CFLAGS) request.c

update :
	git pull origin master

clean :
	rm -Rf $(EXEC) *.o *.log