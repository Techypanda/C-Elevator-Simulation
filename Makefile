# Author: Jonathan Wright
# Date: 7/04/2020
# Conditional Comp: None

CC = gcc
CFLAGS = -Wall -Werror -pedantic -ansi -std=c89 -c89 -c
EXEC = lift_sim_A

ifdef DEBUG
CFLAGS += -D DEBUG -g # -g for valgrind
DEBUG: clean $(EXEC)
endif

update :
	git pull origin master