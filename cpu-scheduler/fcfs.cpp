#include "cpu-scheduler.h"

void fcfs_algo(process* current_process, task* current_task, std::list<process*>* cpu, std::list<process*>* io) {
	if (current_task->time == 0) {
		task_complete(current_process);
		io->push_back(current_process);
		cpu->pop_front();
	}
}

void run_fcfs(std::list<process*>* cpu, std::list<process*>* io) {
	init();
	run(fcfs_algo);
}
