#pragma once

#include <iostream>
#include <list> 
#include <iterator>
#include <functional>

#define MAX_PROCESSES 8
#define MAX_TASKS 32

enum task_type {
	CPU_TASK,
	IO_TASK
};

struct task_stats {
	int arrival_time;
	int completion_time;
	int burst_time;
	int turn_around_time;
	int waiting_time;
};

struct task {
	task_stats stats;
	int time;
	task_type type;
	void* custom;
};

struct process {
	std::list<task*> tasks;
	std::list<task*> done;
	void* custom;
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
