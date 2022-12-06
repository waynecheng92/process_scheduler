# process_scheduler
An efficient allocation of parallelizable processes under a multi-processor environment with efficiency enhancement by mathematically avoiding idle CPUs and unnecessary splits of processes.

## Project description
A simulator that allocates paralizable processes to N number of CPU(s). The program is be invoked via the command line and given a description of arriving processes 
including their arrival time, execution time in seconds, their id, and whether they are parallelisable or not. The first process always arrives at time 0, 
which is when the simulation begins.

## Note
* A **parallelisable process** is split into k ≤ N subproceses, where k is the largest value for which x/k ≥ 1. Each subprocess is then assigned execution time of ⌈x/k⌉ + 1. 
Subprocesses follow a similar naming convention as above: a process with id i is split into subprocesses with id’s i.0, i.1, ..., i.k′, where k′ = k − 1. Subprocesses 
are then assigned to k processors with shortest time left to complete such that subprocess i.0 is assigned to the CPU that can finish its processes the fastest, i.1 is 
assigned to the second fastest processor and so on. Use CPU ids to break ties, giving preference to smaller processor numbers.

* Example: consider a scenario with 4 CPUs, each with one process left to complete; processes on CPU 0,1,2,3 have remaining time of 30,20,20,10, respectively. Then for a 
process i with execution time of 2, k is set to 2. Its subprocess i.0 and i.1 will be assigned to CPU 3 and 1 respectively.
Once a process or subprocess is assigned to a CPU it cannot migrate to another CPU. A parallelisable process is considered finished when all of its subprocesses have 
finished.


## Challenge: Improve the peformance
You will be asked to measure the overall time of your simulation (Makespan). The challenge task is to come up with an algorithm that has a shorter makespan on a set of tests. 
For this task the choice of k when splitting a parallelisable process is left to you. You are also allowed to “look into the future” and see what processes will be arriving and 
use this information if you choose to.

## Program specification
Take the following command line arguments. The arguments can be passed in any order but you can assume that they will be passed exactly once.

`-f` filename will specify the name of the file describing the processes.
`-p` processors where processors is one of {1,2,N}, N ≤ 1024.
`-c` an optional parameter, when provided, invokes your own scheduler from [Challenge] (#chanllenge:_improve_the_performance).
