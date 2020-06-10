# OS Assignment
So yeh, I game

#WHAT IS IT?
A Lift Simulator written in C (Multithreaded, Multiprocessed)

#GOAL
I chose to try and make this assignment without memory leaks and global variables I think I succeeded?

#Dependencies
- make (Required to easily compile, but you dont 'need' it if you know all the flags i've created.)
- gcc (Required to compile the code, you can also use clang but I didn't test for clang.)
- c (c89 Unix)
- unix (built for unix environment ((use of pthreads + system v)))

#OPTIONAL FLAGS
- DEBUG (Will output additional things to terminal to help with debugging.)
- SANITIZETHREAD (Will add fsanitize=thread to the compiler flags)
- NOTSLEEP (Will disable sleeping.)
- OUTSIMASSTDOUT (Will output all things that are meant to be sent to out_sim to the standard output instead)
- PROCESS (Will select to use the process implementation) <- DO NOT DEFINE BOTH OF THESE ONLY ONE
- PTHREAD (Will select to use the pthread implementation, by default this is selected) <--|

#How To Run
1) cd into the directory containing code
    2) (make PROCESS=1) OR (make PTHREAD=1) OR make (will select PTHREAD by default.)
    3) Make sure you have a sim_input file. (siminputgenerator.java is included to help if you dont know how,
                                             however the file you place must be called sim_input)
    3) ./lift_sim_A m t (Where m is buffer size ((INTEGER)) and t is lift time ((INTEGER)))
    NOTE: you can estimate the time it will take by doing the formula (t (liftTime) * siminputSize), this will
	  give you a upperbound estimate (It will not take longer than this.
    4) Once done you can read the out_sim file to see the result

Files:
    - lifts.c
        Purpose: Provides Implementation of how the lifts will work.
    - lifts.h
	Purpose: Headerfile for lifts.c
    - list.c
	Purpose: Double Ended Doubly Linked list made for UCP 2019 Assignment Semester 2, Modified again by me.
    - list.h
	Purpose: List.c headerfile.
    - Makefile
	Purpose: Used by make for easy compilimation
    - processLift.h
	Purpose: Provides the arrayQueue ADT and processLift struct
    - program.c
	Purpose: Start and End point for program.
    - program.h
	Purpose: Program headerfile
    - queue.c
	Purpose: Contains list based queue ADT
    - queue.h
	Purpose: queue.c headerfile
    - request.c
	Purpose: contains request struct and the pthread/process implementation for polling requests from file into buffer.
    - request.h
	Purpose: headerfile for request.c