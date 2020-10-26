#include "cpu-scheduler.h"

bool compare_process_tasks(process* first, process* second) {
	int current_time_first = 0, current_time_second = 0;
	if (!first->tasks.empty()) {
		current_time_first = first->tasks.front()->time;
	}
	if (!second->tasks.empty()) {
		current_time_second = second->tasks.front()->time;
	}
	return (current_time_first < current_time_second);
}

void sjf_algo(process* current_process, task *current_task, std::list<process*>* cpu, std::list<process*>* io) {
	if (current_task->time == 0) {
		task_complete(current_process);
		io->push_back(current_process);
		cpu->pop_front();
		if (!cpu->empty()) {
			cpu->sort(compare_process_tasks);
		}
	}
}

void run_sjf(std::list<process*>* cpu, std::list<process*>* io) {
	init();
	cpu->sort(compare_process_tasks);
	io->sort(compare_process_tasks);
	run(sjf_algo);
}
