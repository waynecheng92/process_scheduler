# process_scheduler
An efficient allocation of parallelizable processes under a multi-processor environment with efficiency enhancement by mathematically avoiding idle CPUs and unnecessary splits of processes.


## Project description
A simulator that allocates paralizable processes to N number of CPU(s). The program is be invoked via the command line and given a description of arriving processes including their arrival time, execution time in seconds, their id, and whether they are parallelisable or not. The first process always arrives at time 0, which is when the simulation begins.


## Note
* A **parallelisable process** is split into k ≤ N subproceses, where k is the largest value for which x/k ≥ 1. Each subprocess is then assigned execution time of ⌈x/k⌉ + 1. Subprocesses follow a similar naming convention as above: a process with id i is split into subprocesses with id’s i.0, i.1, ..., i.k′, where k′ = k − 1. Subprocesses are then assigned to k processors with shortest time left to complete such that subprocess i.0 is assigned to the CPU that can finish its processes the fastest, i.1 is assigned to the second fastest processor and so on. Use CPU ids to break ties, giving preference to smaller processor numbers.

* Example: consider a scenario with 4 CPUs, each with one process left to complete; processes on CPU 0,1,2,3 have remaining time of 30,20,20,10, respectively. Then for a process i with execution time of 2, k is set to 2. Its subprocess i.0 and i.1 will be assigned to CPU 3 and 1 respectively.
Once a process or subprocess is assigned to a CPU it cannot migrate to another CPU. A parallelisable process is considered finished when all of its subprocesses have 
finished.


## Challenge: Improve the performance
You will be asked to measure the overall time of your simulation (Makespan). The challenge task is to come up with an algorithm that has a shorter makespan on a set of tests. For this task the choice of k when splitting a parallelisable process is left to you. You are also allowed to “look into the future” and see what processes will be arriving and use this information if you choose to.


## Program specification
Take the following command line arguments. The arguments can be passed in any order but you can assume that they will be passed exactly once.

* `-f` *filename* will specify the name of the file describing the processes.
* `-p` *processors* where processors is one of {1,2,N}, N ≤ 1024.
* `-c` an optional parameter, when provided, invokes your own scheduler from [Challenge](#chanllenge).

  The *filename* contains the processes to be executed and has the following format. Each line of the file corresponds to a process. The first line         refers to the first process that needs to be executed, and the last line refers to the last process to be executed. Each line consists of a space-       separated tuple *(time-arrived, process-id, execution-time, parallelisable)*. You can assume that the file will be sorted by time-arrived which is an     integer between 0 and 2^32 indicating seconds; all *process-ids* will be distinct integers between 0 and 2^32 and the first process will always have     *time-arrived* set to 0; *execution-time* will be an integer between 0 and 2^32 indicating seconds; *parallelisable* is either n or p. If it is p, then   the corresponding process is parallelisable; if it is n, it is not. You can ignore n/p when -p is 1. More than one process can arrive at the same time.

* Example: `./allocate -f processes.txt -p 1`.
  The allocation program is required to simulate execution of processes in the file processes.txt on a single CPU.

  Given processes.txt with the following information:  
    ```
    0 4 30 n  
    3 2 40 n  
    5 1 20 n  
    20 3 30 n  
    ```

  The program should simulate execution of 4 processes where process 4 arrives at time 0, needs 30 seconds running time to finish; process 2 arrives at     time 3 and needs 40 seconds of time to complete.
  Each line (including the last) will be terminated with a LF (ASCII 0x0a) control character.
  You can read the whole file before starting the simulation or read one line at a time. We will not give malformed input (e.g., negative number of         processors after `-p` or more than 4 columns in the process description file).

## Expected ouput
### Execution transcript  
For the following events the code should print out a line in the following format:
* When a (sub)process starts and every time it resumes its execution:  

     ```
     <current-time>,RUNNING,pid=<process-id>,remaining_time=<T>,cpu=<cpu-id>\n  
     ```
  where:  
  `current-time` refers to the time at which CPU is given to a process;  
  `process-id` refers to the id of the process that is about to be run;  
  `T` refers to the remaining execution time for this process;  
  `cpu-id` refers to the processor where the process is scheduled on. It can be 0, 1, 2 . . . , N − 1 when
  -p N for N ≥ 1;  
  
  Sample output could be:  
    ```
    20,RUNNING,pid=15,remaining_time=10,cpu=0
    ```
* Every time a process finishes:
    ```
    <current-time>,FINISHED,pid=<process-id>,proc_remaining=<num-proc-left>\n
    ```
  where:  
  `<current-time>` is as above for the RUNNING event;  
  `process-id` refers to the id of the process that has just been completed;  
  `num-proc-left` refers to the number of processes that are waiting to be executed over all processors (i.e., those that have already     arrived but not yet completed, including those that have unfinished subprocesses).
  
* If there is more than one event to be printed at the same time: print FINISHED before RUNNING and print events for smaller CPU ids first.
  Example: Consider the last remaining process which has 10 seconds left to completion. Then the following lines may be printed:
  ```
  20,RUNNING,pid=15,remaining_time=10,cpu=1  
  30,FINISHED,pid=15,proc_remaining=0
  ```
  
  

