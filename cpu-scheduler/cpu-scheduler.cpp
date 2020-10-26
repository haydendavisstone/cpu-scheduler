#include "cpu-scheduler.h"

using namespace std;

int assignment_data[MAX_PROCESSES][MAX_TASKS] = {
	{ 5, 27, 3, 31, 5, 43, 4, 18, 6, 22, 4, 26, 3, 24, 4 },
	{ 4, 48, 5, 44, 7, 42, 12, 37, 9, 76, 4, 41, 9, 31, 7, 43, 8 },
	{ 8, 33, 12, 41, 18, 65, 14, 21, 4, 61, 15, 18, 14, 26, 5, 31, 6 },
	{ 3, 35, 4, 41, 5, 45, 3, 51, 4, 61, 5, 54, 6, 82, 5, 77, 3 },
	{ 16, 24, 17, 21, 5, 36, 16, 26, 7, 31, 13, 28, 11, 21, 6, 13, 3, 11, 4 },
	{ 11, 22, 4, 8, 5, 10, 6, 12, 7, 14, 9, 18, 12, 24, 15, 30, 8 },
	{ 14, 46, 17, 41, 11, 42, 15, 21, 4, 32, 7, 19, 16, 33, 10 },
	{ 4, 14, 5, 33, 6, 51, 14, 73, 16, 87, 6 }
};

process processes[MAX_PROCESSES];
std::list<process*> cpu;
std::list<process*> io;
int g_time = 0;

int init() {
	cout << "Initalizing..." << endl;
	cpu.clear();
	io.clear();
	g_time = 0;
	for (int i = 0; i < MAX_PROCESSES && assignment_data[i]; i++) {
		process p;
		for (int j = 0; j < MAX_TASKS && assignment_data[i][j]; j++) {
			task *t = NULL;
			if (((task*)malloc(sizeof(task))) != NULL) {
				t->time = assignment_data[i][j];
				t->type = (task_type)(j % 2);
				t->stats = { -1, -1, 0, -1, -1 };
				t->custom = NULL;
			}
			p.tasks.push_back(t);
		}
		processes[i] = p;
		cpu.push_back(&processes[i]);
	}

	cout << "Initalized!" << endl;
	return 0;
}

void run_cpu() {
	process* p = cpu.front();
	if (p->tasks.empty()) return;
	task* t = p->tasks.front();
	if (t->stats.arrival_time < 0) {
		t->stats.arrival_time = g_time;
	}
	--t->time;
	t->stats.burst_time++;
	++g_time;
}

void run_io() {
	if (io.empty()) return;
	for (std::list<process*>::iterator p = io.begin(); p != io.end();) {
		if ((*p)->tasks.empty()) {
			p = io.erase(p);
			continue;
		}
		task* current_task = (*p)->tasks.front();
		current_task->time--;
		if (current_task->time == 0) {
			(*p)->tasks.pop_front();
			cpu.push_back((*p));
			p = io.erase(p);
		} else {
			++p;
		}
	}
}

void run(algo_func algo_cb) {
	int g_time = 0;
	while (!cpu.empty() || !io.empty()) {
		if (!cpu.empty()) run_cpu();
		if (!io.empty()) run_io();
		if (algo_cb && !cpu.empty()) {
			process* current_process = cpu.front();
			if (current_process->tasks.empty()) return;
			task* current_task = current_process->tasks.front();
			algo_cb(current_process, current_task, &cpu, &io);
		}
		//print_process_lists(cpu, "CPU List");
		//print_process_lists(io, "IO List");
	}
}

void task_complete(process *current_process) {
	task *complete_task = current_process->tasks.front();
	complete_task->stats.completion_time = g_time;
	if (complete_task->time == 0) {
		current_process->done.push_back(complete_task);
		current_process->tasks.pop_front();
	}
}

void print_process_lists(list<process*> process_list, const char* message) {
	cout << endl << endl << message;
	for (list<process*>::iterator p = process_list.begin(); p != process_list.end(); ++p) {
		process* current_process = *p;
		cout << endl;
		if (current_process->tasks.empty()) {
			cout << "Empty";
			continue;
		}
		if (current_process->custom) {
			mlfq* custom = (mlfq*)current_process->custom;
			cout << "(" << custom->priority << ")";
		}
		for (list<task*>::iterator t = current_process->tasks.begin(); t != current_process->tasks.end(); ++t) {
			task *current_task = *t;
			cout << ' ' << current_task->time;
		}
	}
}

void print_process_stats(const char* message) {
	cout << endl << endl << message;
	for (int i = 0; i < MAX_PROCESSES; i++) {
		process p = processes[i];
		cout << endl << "PROCESS " << i << endl;
		for (list<task*>::iterator t = p.done.begin(); t != p.done.end(); ++t) {
			task* current_task = *t;
			current_task->stats.turn_around_time = current_task->stats.completion_time - current_task->stats.arrival_time;
			current_task->stats.waiting_time = current_task->stats.turn_around_time - current_task->stats.burst_time;
			cout << "Arrival Time:\t\t" << current_task->stats.arrival_time << endl;
			cout << "Completion Time:\t" << current_task->stats.completion_time << endl;
			cout << "Burst Time:\t\t" << current_task->stats.burst_time << endl;
			cout << "Turn Around Time:\t" << current_task->stats.turn_around_time << endl;
			cout << "Waiting Time:\t\t" << current_task->stats.waiting_time << endl;
			cout << "-------------------" << endl;
		}
		cout << endl;
	}
}

int main() {
	cout << "-- CPU Scheduler --" << endl;
	run_fcfs(&cpu, &io);
	print_process_stats("FCFS STATS");
	run_sjf(&cpu, &io);
	print_process_stats("SJF STATS");
	run_mlfq(&cpu, &io);
	print_process_stats("MLFQ STATS");
}