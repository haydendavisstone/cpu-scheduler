#include "cpu-scheduler.h"
#include <iomanip>

#define NUM_COLS 4
#define MAX_ROW_WIDTH 25
#define SEPERATOR ' '

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
int g_cpu_time = 0;
int g_time = 0;
int cpu_idle = 0;

//Initialization of processes, their tasks lists, statistics, and anything else necessary.
int init() {
	cout << "Initalizing..." << endl;
	cpu.clear();
	io.clear();
	g_cpu_time = 0;
	g_time = 0;
	for (int i = 0; i < MAX_PROCESSES && assignment_data[i]; i++) {
		process p;
		p.stats = { 0, 0, 0, 0, 0, 0, -1 };
		for (int j = 0; j < MAX_TASKS && assignment_data[i][j]; j++) {
			task *t = NULL;
			if ((t = (task*)malloc(sizeof(task))) != NULL) {
				t->time = assignment_data[i][j];
				t->custom = NULL;
				t->stats = { 0, 0, 0, 0, 0, 0, -1 };
			}
			p.tasks.push_back(t);
		}
		p.custom = NULL;
		p.index = i;
		processes[i] = p;
		cpu.push_back(&processes[i]);
	}

	cout << "Initalized!" << endl;
	return 0;
}


//function to run cpu
void run_cpu() {
	int index = 0;
	for (std::list<process*>::iterator p = cpu.begin(); p != cpu.end(); p++) {
		process* current_process = *p;
		if (current_process->tasks.empty()) return;
		task* current_task = current_process->tasks.front(); //
		if (index == 0) { //get stats
			--current_task->time;
			current_process->stats.burst_time++;
			current_task->stats.burst_time++;
			if (current_process->stats.response_time < 0) {
				current_process->stats.response_time = g_cpu_time;
			}
		} else {
			++current_process->stats.waiting_time;
		}
		++index;
	}

	++g_cpu_time;
}

void run_io() {
	if (io.empty()) return;
	for (std::list<process*>::iterator p = io.begin(); p != io.end();) {
		process *current_process = (*p);

		if (current_process->tasks.empty()) { // IO complete, move to CPU
			p = io.erase(p);
			continue;
		}
		task* current_task = current_process->tasks.front();
		current_task->time--;
		current_process->stats.io_time++;
		if (current_task->time == 0) { // IO complete, move to CPU
			current_process->tasks.pop_front();
			cpu.push_back(current_process);
			p = io.erase(p);
		} else {
			++p;
		}
	}
}

// Scheduler main loop to run CPU, IO, and perform Algo.
void run(algo_func algo_cb) {
	while (!cpu.empty() || !io.empty()) {
		int is_idle = 0; //idle check for calculating cpu utilization
		if (!cpu.empty()) { //run cpu if cpu is not empty
			run_cpu();
		} else {
			is_idle = 1; //cpu empty, set idle
		}
		if (!io.empty()) run_io();
		if (algo_cb && !cpu.empty()) {
			process* current_process = cpu.front();
			if (current_process->tasks.empty()) continue;
			task* current_task = current_process->tasks.front();
			algo_cb(current_process, current_task, &cpu, &io);
		}
		if (cpu.empty() && io.empty()) break;
		if (is_idle) {
			++cpu_idle;
		}
		++g_time;
	}

}

// Completes task and sets stats.
void task_complete(process *current_process) {
	task *complete_task = current_process->tasks.front();
	//cout << "\n\nProcess " << current_process->index << " burst complete" << endl;
	current_process->stats.completion_time = g_cpu_time;
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
		cout << "P" << current_process->index;
		if (current_process->custom) {
			mlfq* custom = (mlfq*)current_process->custom;
			cout << " (" << custom->priority << ")";
		}
		for (list<task*>::iterator t = current_process->tasks.begin(); t != current_process->tasks.end(); ++t) {
			task *current_task = *t;
			cout << ' ' << current_task->time;
		}
	}
}

void print_processes_stats(const char* message) {
	process_stats avg = {0, 0, 0, 0, 0, 0, 0};
	cout << endl << message << endl;
	// header
	cout << "\nCPU UTILIZATION - " << 100 * (g_time - cpu_idle) / (float)g_time << endl;
	cout << setw((MAX_ROW_WIDTH * NUM_COLS) + (NUM_COLS * 2)) << setfill('-') << "-" << endl;
	cout << "|" << setw(MAX_ROW_WIDTH) << setfill(SEPERATOR) << "" << " |";
	cout << setw(MAX_ROW_WIDTH) << setfill(SEPERATOR) << "Waiting Time (Tw)" << " |";
	cout << setw(MAX_ROW_WIDTH) << setfill(SEPERATOR) << "Turn Around Time (Ttr)" << " |";
	cout << setw(MAX_ROW_WIDTH) << setfill(SEPERATOR) << "Response Time (Tr)" << " |";
	// rows
	for (int i = 0; i < MAX_PROCESSES; i++) {
		process p = processes[i];
		cout << endl;
		p.stats.turn_around_time = p.stats.waiting_time + p.stats.burst_time + p.stats.io_time;
		cout << "|" << setw(MAX_ROW_WIDTH) << setfill(SEPERATOR) << i + 1 << " |";
		cout << setw(MAX_ROW_WIDTH) << setfill(SEPERATOR) << p.stats.waiting_time << " |";
		cout << setw(MAX_ROW_WIDTH) << setfill(SEPERATOR) << p.stats.turn_around_time << " |";
		cout << setw(MAX_ROW_WIDTH) << setfill(SEPERATOR) << p.stats.response_time << " |";
		avg.waiting_time += p.stats.waiting_time;
		avg.turn_around_time += p.stats.turn_around_time;
		avg.response_time += p.stats.response_time;
	}
	// Average
	avg.waiting_time = avg.waiting_time / MAX_PROCESSES;
	avg.turn_around_time = avg.turn_around_time / MAX_PROCESSES;
	avg.response_time = avg.response_time / MAX_PROCESSES;
	cout << endl;
	cout << "|" << setw(MAX_ROW_WIDTH) << setfill(SEPERATOR) << "AVG" << " |";
	cout << setw(MAX_ROW_WIDTH) << setfill(SEPERATOR) << avg.waiting_time << " |";
	cout << setw(MAX_ROW_WIDTH) << setfill(SEPERATOR) << avg.turn_around_time << " |";
	cout << setw(MAX_ROW_WIDTH) << setfill(SEPERATOR) << avg.response_time << " |";

	cout << endl << setw((MAX_ROW_WIDTH * NUM_COLS) + (NUM_COLS * 2)) << setfill('-') << "-" << endl;
}

int main() { //main function, runs each algorithim and prints stats.
	cout << "-- CPU Scheduler --" << endl;
	run_fcfs(&cpu, &io);
	print_processes_stats("FCFS STATS");
	run_sjf(&cpu, &io);
	print_processes_stats("SJF STATS");
	run_mlfq(&cpu, &io);
	print_processes_stats("MLFQ STATS");
}