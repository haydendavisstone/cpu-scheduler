#include "cpu-scheduler.h"

void mlfq_algo(process* current_process, task* current_task, std::list<process*>* cpu, std::list<process*>* io) {
	if (current_task->time == 0) {
		task_complete(current_process);
		io->push_back(current_process);
		cpu->pop_front();
		return;
	}

	mlfq* custom = (mlfq*)current_process->custom;
	if (
		custom->priority < custom->max_priority &&
		current_task->stats.burst_time >= (custom->time_quantum * custom->priority)
	) {
		custom->priority++;
		cpu->push_back(current_process);
		cpu->pop_front();
	}
}

void init_mlfq(std::list<process*>* cpu) {
	for (std::list<process*>::iterator p = cpu->begin(); p != cpu->end(); ++p) {
		mlfq* custom = NULL;
		if ((custom = (mlfq*)malloc(sizeof(mlfq))) != NULL) {
			custom->priority = 1;
			custom->max_priority = 3;
			custom->time_quantum = 5;
		}
		(*p)->custom = custom;
	}
}

void delete_mlfq(std::list<process*>* cpu) {
	for (std::list<process*>::iterator p = cpu->begin(); p != cpu->end(); ++p) {
		for (std::list<task*>::iterator t = (*p)->tasks.begin(); t != (*p)->tasks.end(); ++t) {
			task* current_task = *t;
			if (current_task->custom) free(current_task->custom);
		}
	}
}

void run_mlfq(std::list<process*>* cpu, std::list<process*>* io) {
	init();
	init_mlfq(cpu);
	run(mlfq_algo);
	delete_mlfq(cpu);
}
