## COP 4610 - Programming Assignment CPU Scheduler
### Hayden Stone
### 10/25/2020

## Project objective:
To learn more about OS scheduling through a hands-on simulation programming experience 

## Instructions

Implement the following 3 CPU scheduling algorithms 
* Simulate and evaluate each with the set of eight processes below.
* Use any programming language. The program listing should be submitted with the report.

1. FCFS non-preemptive (partial results provided)
1. SJF non-preemptive 
1. MLFQ - Multilevel Feedback Queue (absolute priority in higher queues)
    1. Queue 1 uses RR scheduling with Tq = 5
    1. Queue 2 uses RR scheduling with Tq = 10
	1. Queue 3 uses FCFS

All processes enter first queue 1. If time quantum (Tq) expires before CPU burst is complete, the process is downgraded to next lower priority queue. Processes are not downgraded when preempted by a higher queue level process. Once a process has been downgraded, it will not be upgraded.

## Assumptions
1. All processes are activated at time 0
1. Assume that no process waits on I/O devices. 
1. After completing an I/O event, a process is transferred to the ready queue.
1. Waiting time is accumulated while a process waits in the ready queue.
1. Turnaround time is a total of (Waiting time) + (CPU burst time) + (I/O time)
1. Response time is the first measure of waiting time from arrival at time 0 until the first time on the CPU. 

## Process Data
process goes {CPU burst, I/O time, CPU burst, I/O time, CPU burst, I/O time,…….., last CPU burst}
```
P1 {5, 27, 3, 31, 5, 43, 4, 18, 6, 22, 4, 26, 3, 24, 4}
P2 {4, 48, 5, 44, 7, 42, 12, 37, 9, 76, 4, 41, 9, 31, 7, 43, 8}
P3 {8, 33, 12, 41, 18, 65, 14, 21, 4, 61, 15, 18, 14, 26, 5, 31, 6}
P4 {3, 35, 4, 41, 5, 45, 3, 51, 4, 61, 5, 54, 6, 82, 5, 77, 3}
P5 {16, 24, 17, 21, 5, 36, 16, 26, 7, 31, 13, 28, 11, 21, 6, 13, 3, 11, 4}
P6 {11, 22, 4, 8, 5, 10, 6, 12, 7, 14, 9, 18, 12, 24, 15, 30, 8}
P7 {14, 46, 17, 41, 11, 42, 15, 21, 4, 32, 7, 19, 16, 33, 10}
P8 {4, 14, 5, 33, 6, 51, 14, 73, 16, 87, 6}
```