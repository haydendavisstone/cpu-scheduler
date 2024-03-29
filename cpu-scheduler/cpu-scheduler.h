#pragma once

#include <iostream>
#include <list> 
#include <iterator>
#include <functional>

#define MAX_PROCESSES 8
#define MAX_TASKS 32

struct process_stats {
	int arrival_time;
	int waiting_time;
	int burst_time;
	int io_time;
	int completion_time;
	int turn_around_time; // (Waiting time) + (CPU burst time) + (I/O time)
	int response_time; // First time it arrives on CPU
};

struct task {
	process_stats stats;
	int time;
	void* custom;
};

struct process {
	int index;
	std::list<task*> tasks;
	std::list<task*> done;
	void* custom;
	process_stats stats;
};

struct mlfq {
	int time_quantum;
	int max_priority;
	int priority;
};

typedef std::function<void(process* current_process, task *current_task, std::list<process*>* main_list, std::list<process*>* other_list)> algo_func;

int init();
void run(algo_func algo_cb);
void task_complete(process* current_process);
void print_process_lists(std::list<process*> process_list, const char* message);

void run_fcfs(std::list<process*>* cpu, std::list<process*>* io);
void run_mlfq(std::list<process*>* cpu, std::list<process*>* io);
void run_sjf(std::list<process*>* cpu, std::list<process*>* io);
