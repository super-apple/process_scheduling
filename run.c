#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

/* the structure of a process */
typedef struct node {
	int index;
	int arrival_time;
	int priority;
	int age;
	int cpu_time;
	struct node *next;

	int end_time;	// the time when a process terminates
	int ready_time;	// the first time a process get a run
	int running_time;	// the overall running time of a process
	int last_start_time;	// last time when a process gets its quantum
	int last_ran_time;	// last time when a process leaves its quantum
	int waiting_time;
	int queue_one_ran_time;
	int queue_two_ran_time;
} Process;

/* execute a process in queue 1 */
void execute_process(Process **process) {
	(*process)->running_time = (*process)->running_time + 1;
}

/* test whether a process is finished */
bool process_is_terminated(Process *process) {
	if (process->cpu_time == process->running_time) {
		return true;
	}
	else {
		return false;
	}
}

/* add item to the index of a queue */
void enqueue(Process **head, Process *new_process, int index) {
	Process *current = *head;
	Process *prev = *head;
	Process *this = *head;
	// check whether the list is NULL
	if (*head == NULL) {
		*head = malloc(sizeof(Process));
		current = *head;
		current->next = NULL;
	}
	else {
		// add to the end, if index == -1
		if (index == -1) {
			while (current->next != NULL) {
				current = current->next;
			}
			current->next = malloc(sizeof(Process));
			current = current->next;
			current->next = NULL;
		}
		// add to the beginning, if index == 0
		else if (index == 0) {
			this = malloc(sizeof(Process));
			this->next = *head;
			*head = this;
			current = *head;
		}
		else {
			for (int i = 0; i < index; i++) {
				prev = current;
				current = current->next;
			}
			this = malloc(sizeof(Process));
			this->next = current;
			prev->next = this;
			current = this;
		}
	}

	current->index = new_process->index;
	current->arrival_time = new_process->arrival_time;
	current->priority = new_process->priority;
	current->age = new_process->age;
	current->cpu_time = new_process->cpu_time;
	

	current->end_time = new_process->end_time;
	current->ready_time = new_process->ready_time;
	current->running_time = new_process->running_time;
	current->last_start_time = new_process->last_start_time;
	current->last_ran_time = new_process->last_ran_time;
	current->waiting_time = new_process->waiting_time;
	current->queue_one_ran_time = new_process->queue_one_ran_time;
	current->queue_two_ran_time = new_process->queue_two_ran_time;
}

/* delete the given item from the queue */
/* if the given one is null, delete the first item of the queue */
void dequeue(Process **head, Process *process) {
	// delete the first item
	if (*head != NULL) {
		if (process->index == (*head)->index) {
			Process *current = *head;
			*head = (*head)->next;
			free(current);
			return;
		}
		// not the first item
		else {
			Process *current = (*head)->next;
			Process *prev = *head;
			while (current != NULL) {
				if (process->index == current->index) {
					Process *temp = current;
					prev->next = current->next;
					free(temp);
					return;
				}
				prev = current;
				current = current->next;
			}
		}
	}
}

/* test whether a queue is empty */
bool queue_is_empty(Process *head) {
	if (head == NULL) {
		return true;
	}
	else {
		return false;
	}
}

char read_input(char *file_name, Process **head) {
	/* read processes in file from argv */
	FILE *fp;
	char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int i = 0;

	fp = fopen(file_name, "r");
	if (fp == NULL) {
		exit(EXIT_FAILURE);
	}

	// define a linked list of the input processes
    char process_prefix;

	while ((read = getline(&line, &len, fp)) != -1) {
		Process *temp_process = NULL;
		temp_process = malloc(sizeof(Process));

		char *token = NULL;
		token = strtok(line, " ");

		i = 0;
		char *temp_list[5];
		while (token != NULL) {
			temp_list[i] = token;
			i = i + 1;
			token = strtok(NULL, " ");
		}

		process_prefix = temp_list[0][0];
		for (i = 1; i < strlen(temp_list[0]); i++) {
			temp_list[0][i - 1] = temp_list[0][i];
		}
		temp_list[0][i - 1] = '\0';
		
		temp_process->index = (int) strtol(temp_list[0], (char **)NULL, 10);
		temp_process->arrival_time = (int) strtol(temp_list[1], (char **)NULL, 10);
		temp_process->priority = (int) strtol(temp_list[2], (char **)NULL, 10);
		temp_process->age = (int) strtol(temp_list[3], (char **)NULL, 10);
		temp_process->cpu_time = (int) strtol(temp_list[4], (char **)NULL, 10);
		temp_process->next = NULL;

		temp_process->end_time = -1;
		temp_process->ready_time = -1;
		temp_process->running_time = 0;
		temp_process->last_start_time = -1;
		temp_process->last_ran_time = -1;
		temp_process->waiting_time = 0;
		temp_process->queue_one_ran_time = 0;
		temp_process->queue_two_ran_time = 0;

		// add process to the end of the queue
		enqueue(&(*head), temp_process, -1);

		free(temp_process);
	}

	fclose(fp);
	if (line) {
		free(line);
	}

	return process_prefix;
}

void place_in_queue(Process *process, Process **queue_1, Process **queue_2, Process **queue_3, int queue_1_priority, int queue_2_priority) {
	int index = 0;
	// printf("the process we received: %d\n", process->queue_two_ran_time);
	if (process->priority >= queue_1_priority) {
		Process *current = *queue_1;
		index = 0;
		while (current != NULL) {
			if (process->priority <= current->priority) {
				index = index + 1;
			}
			current = current->next;
		}
		enqueue(&(*queue_1), process, index);
	}
	else if (process->priority >= queue_2_priority) {
		enqueue(&(*queue_2), process, -1);
	}
	else {
		enqueue(&(*queue_3), process, -1);
	}
}

void admit_new_process(Process **process_list, int current_time, Process **queue_1, Process **queue_2, Process **queue_3, int queue_1_priority, int queue_2_priority) {
	Process *current = *process_list;
	while (current != NULL) {
		if (current_time >= current->arrival_time) {
			place_in_queue(current, &(*queue_1), &(*queue_2), &(*queue_3), queue_1_priority, queue_2_priority);
			if (!queue_is_empty(*process_list)) {
				dequeue(&(*process_list), current);
			}
		}
		current = current->next;
	}
}

void dispatch_ready_process(Process **current_process, Process **queue_1, Process **queue_2, Process **queue_3, int current_time, int *current_units_executed) {
	bool new_dispatched = false;

	// check queue 1 first
	if (*current_process == NULL && !queue_is_empty(*queue_1)) {
		*current_process = *queue_1;
		new_dispatched = true;
	}
	// check queue 2 if none in queue 1
	else if (*current_process == NULL && !queue_is_empty(*queue_2)) {
		*current_process = *queue_2;
		(*current_process)->age = 0;
		new_dispatched = true;
	}
	// check queue 3 if none in both queue 1 and queue 2
	else if (*current_process == NULL && !queue_is_empty(*queue_3)) {
		*current_process = *queue_3;
		(*current_process)->age = 0;
		new_dispatched = true;
	}

	//If process is dispatched and it is the first time, record the ready time
	//If process has ran before, sum up the extra waiting time using last-ran-time
	if (new_dispatched == true) {
		if ((*current_process)->ready_time == -1) {
			(*current_process)->ready_time = current_time;
		}
		if ((*current_process)->last_ran_time != -1) {
			(*current_process)->waiting_time = (*current_process)->waiting_time + current_time - (*current_process)->last_ran_time;
		}
		(*current_process)->last_start_time = current_time;
		*current_units_executed = 0;
	}
}

/* giving a process CPU time */
void execute_current_process(Process **current_process, int *current_time, int *current_units_executed) {
	if (*current_process != NULL) {
		execute_process(&(*current_process));
		*current_time = (*current_time) + 1;
		*current_units_executed = (*current_units_executed) + 1;
	}

}

/* interrupt the running process either in queue 1, queue 2 or queue 3 */
// in the following cases:
// 1. process is terminated, either in queue 1, queue 2 or queue 3
// 2. process running in queue 2 or queue 3 was interrupted by new coming of queue 1
// 3. process running in queue 3 was interrupted by new coming of queue 2
// 4. process running in queue 1 finished its quantum
// 5. process running in queue 2 finished its quantum
// 6. process running in queue 3 finished its quantum
void interrupt_running_process(Process **current_process, Process **finished_process, Process **queue_1, Process **queue_2, Process **queue_3, int current_time, int queue_1_priority, int queue_2_priority, int *current_process_index, int current_units_executed, int queue_1_quantum, int queue_2_quantum, int queue_3_quantum, int queue_1_decrease_threshold, int queue_2_decrease_threshold) {
	if (*current_process != NULL) {
		// case 1: process is terminated
		if (process_is_terminated(*current_process)) {
			(*current_process)->end_time = current_time;
			(*current_process)->queue_one_ran_time = 0;
			(*current_process)->queue_two_ran_time = 0;
			dequeue(&(*queue_1), *current_process);
			dequeue(&(*queue_2), *current_process);
			dequeue(&(*queue_3), *current_process);
			enqueue(&(*finished_process), *current_process, -1);
			*current_process = NULL;
		}
		// case 2: process in queue 2 (or 3) is running while a process jumps in queue 1
		// Record last-ran-time and place it to the right queue
		else if (!queue_is_empty(*queue_1) && (*current_process)->priority < queue_1_priority) {
			(*current_process)->last_ran_time = current_time;
			(*current_process)->queue_two_ran_time = (*current_process)->queue_two_ran_time + current_time - (*current_process)->last_start_time;
			if ((*current_process)->queue_two_ran_time >= queue_2_decrease_threshold) {
				(*current_process)->priority = (*current_process)->priority - 1;
				(*current_process)->queue_two_ran_time = 0;
			}
			dequeue(&(*queue_2), *current_process);
			place_in_queue(*current_process, &(*queue_1), &(*queue_2), &(*queue_3), queue_1_priority, queue_2_priority);
			*current_process_index = (*current_process)->index;
			*current_process = NULL;
		}
		// case 3: process in queue 3 is running while a process jumps in queue 2
		else if (!queue_is_empty(*queue_2) && (*current_process)->priority < queue_2_priority) {
			(*current_process)->last_ran_time = current_time;
			dequeue(&(*queue_3), *current_process);
			place_in_queue(*current_process, &(*queue_1), &(*queue_2), &(*queue_3), queue_1_priority, queue_2_priority);
			*current_process_index = (*current_process)->index;
			*current_process = NULL;
		}
		// case 4: process in queue 1 left its quantum
		else if (current_units_executed == queue_1_quantum && (*current_process)->priority >= queue_1_priority) {
			(*current_process)->last_ran_time = current_time;

			// TEST
			// printf("test: %d, %d\n", (*current_process)->index, (*current_process)->queue_one_ran_time);

			(*current_process)->queue_one_ran_time = (*current_process)->queue_one_ran_time + queue_1_quantum;
			if ((*current_process)->queue_one_ran_time >= queue_1_decrease_threshold) {
				(*current_process)->priority = (*current_process)->priority - 1;
				(*current_process)->queue_one_ran_time = 0;
			}

			dequeue(&(*queue_1), *current_process);
			place_in_queue(*current_process, &(*queue_1), &(*queue_2), &(*queue_3), queue_1_priority, queue_2_priority);
			*current_process_index = (*current_process)->index;
			*current_process = NULL;
		}
		// case 5: process in queue 2 left its quantum
		else if (queue_is_empty(*queue_1) && (*current_process)->priority >= queue_2_priority && (*current_process)->priority < queue_1_priority) {
			(*current_process)->last_ran_time = current_time;

			int temp = (*current_process)->queue_two_ran_time + current_time - (*current_process)->last_start_time;
			// process in queue 2 reached its quantum limits
			if (temp >= queue_2_decrease_threshold) {
				(*current_process)->priority = (*current_process)->priority - 1;
				(*current_process)->queue_two_ran_time = 0;

				dequeue(&(*queue_2), *current_process);
				place_in_queue(*current_process, &(*queue_1), &(*queue_2), &(*queue_3), queue_1_priority, queue_2_priority);
				*current_process_index = (*current_process)->index;
				*current_process = NULL;
			}
			// process in queue 2 didn't reach its quantum limits
			else if (current_units_executed == queue_2_quantum) {
				(*current_process)->queue_two_ran_time = (*current_process)->queue_two_ran_time + queue_2_quantum;
				if ((*current_process)->queue_two_ran_time >= queue_2_decrease_threshold) {
					(*current_process)->priority = (*current_process)->priority - 1;
					(*current_process)->queue_two_ran_time = 0;
				}

				dequeue(&(*queue_2), *current_process);
				place_in_queue(*current_process, &(*queue_1), &(*queue_2), &(*queue_3), queue_1_priority, queue_2_priority);
				*current_process_index = (*current_process)->index;
				*current_process = NULL;
			}
		}
		// case 6: process in queue 3 left its quantum
		else if (current_units_executed == queue_3_quantum && (*current_process)->priority < queue_2_priority) {
			(*current_process)->last_ran_time = current_time;

			dequeue(&(*queue_3), *current_process);
			place_in_queue(*current_process, &(*queue_1), &(*queue_2), &(*queue_3), queue_1_priority, queue_2_priority);
			*current_process_index = (*current_process)->index;
			*current_process = NULL;
		}
	}
}

/* ageing scheme in queue 2 and queue 3 */
void ageing(Process **queue_1, Process **queue_2, Process **queue_3, int age_threshold_2, int age_threshold_3, int queue_1_priority, int queue_2_priority, int current_process_index, int current_time) {
	// ageing queue 2
	Process *current = *queue_2;
	while (current != NULL) {
		Process *temp = current->next;
		if (current->index != current_process_index && current_time != current->arrival_time) {
			current->age = current->age + 1;
		}
		if (current->age >= age_threshold_2) {
			current->priority = current->priority + 1;
			current->age = 0;
		}
		if (current->priority >= queue_1_priority) {
			dequeue(&(*queue_2), current);
			place_in_queue(current, &(*queue_1), &(*queue_2), &(*queue_3), queue_1_priority, queue_2_priority);
		}
		current = temp;
	}

	// ageing queue 3
	current = *queue_3;
	while (current != NULL) {
		Process *temp = current->next;
		if (current->index != current_process_index && current_time != current->arrival_time) {
			current->age = current->age + 1;
		}
		if (current->age >= age_threshold_3) {
			current->priority = current->priority + 1;
			current->age = 0;
		}
		if (current->priority >= queue_2_priority) {
			dequeue(&(*queue_3), current);
			place_in_queue(current, &(*queue_1), &(*queue_2), &(*queue_3), queue_1_priority, queue_2_priority);
		}
		current = temp;
	}
}

/*void status_printer(Process *input_queue, Process *queue_1, Process *queue_2, Process *queue_3, Process *finished_process, int current_time, char prefix) {
	printf("***************************\n");
	printf("The last moment of Time(%d)\n", current_time);
	printf("This is Queue 1:\n");
	printf("Index\tArrival\tPrio\tAge\tCPU_T\tEnd\tReady\tRunning\tLastStart\tLastRan\tWaiting\tIn_Q_1\tIn_Q_2\n");
	Process *current = queue_1;
	while (current != NULL) {
		printf("%c%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", prefix, current->index, current->arrival_time, current->priority, current->age, current->cpu_time, current->end_time, current->ready_time, current->running_time, current->last_start_time, current->last_ran_time, current->waiting_time, current->queue_one_ran_time, current->queue_two_ran_time);
		current = current->next;
	}

	printf("This is Queue 2:\n");
	printf("Index\tArrival\tPrio\tAge\tCPU_T\tEnd\tReady\tRunning\tLastStart\tLastRan\tWaiting\tIn_Q_1\tIn_Q_2\n");
	current = queue_2;
	while (current != NULL) {
		printf("%c%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", prefix, current->index, current->arrival_time, current->priority, current->age, current->cpu_time, current->end_time, current->ready_time, current->running_time, current->last_start_time, current->last_ran_time, current->waiting_time, current->queue_one_ran_time, current->queue_two_ran_time);
		current = current->next;
	}

	printf("This is Queue 3:\n");
	printf("Index\tArrival\tPrio\tAge\tCPU_T\tEnd\tReady\tRunning\tLastStart\tLastRan\tWaiting\tIn_Q_1\tIn_Q_2\n");
	current = queue_3;
	while (current != NULL) {
		printf("%c%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", prefix, current->index, current->arrival_time, current->priority, current->age, current->cpu_time, current->end_time, current->ready_time, current->running_time, current->last_start_time, current->last_ran_time, current->waiting_time, current->queue_one_ran_time, current->queue_two_ran_time);
		current = current->next;
	}

	printf("\n");
	printf("They are finished processes:\n");
	printf("Index\tArrival\tPrio\tAge\tCPU_T\tEnd\tReady\tRunning\tLastStart\tLastRan\tWaiting\tIn_Q_1\tIn_Q_2\n");
	current = finished_process;
	while (current != NULL) {
		printf("%c%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", prefix, current->index, current->arrival_time, current->priority, current->age, current->cpu_time, current->end_time, current->ready_time, current->running_time, current->last_start_time, current->last_ran_time, current->waiting_time, current->queue_one_ran_time, current->queue_two_ran_time);
		current = current->next;
	}

	printf("This is Input queue:\n");
	printf("Index\tArrival\tPrio\tAge\tCPU_T\tEnd\tReady\tRunning\tLastStart\tLastRan\tWaiting\tIn_Q_1\tIn_Q_2\n");
	current = input_queue;
	while (current != NULL) {
		printf("%c%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", prefix, current->index, current->arrival_time, current->priority, current->age, current->cpu_time, current->end_time, current->ready_time, current->running_time, current->last_start_time, current->last_ran_time, current->waiting_time, current->queue_one_ran_time, current->queue_two_ran_time);
		current = current->next;
	}
}*/

void status_printer(Process *input_queue, Process *queue_1, Process *queue_2, Process *queue_3, Process *finished_process, int current_time, char prefix) {
	printf("***************************\n");
	printf("The last moment of Time(%d)\n", current_time);
	printf("This is Queue 1:\n");
	printf("Index\tArrival\tPriority\tage\tCPU_Time\n");
	Process *current = queue_1;
	while (current != NULL) {
		printf("%c%d\t%d\t%d\t%d\t%d\n", prefix, current->index, current->arrival_time, current->priority, current->age, current->cpu_time);
		current = current->next;
	}

	printf("This is Queue 2:\n");
	printf("Index\tArrival\tPriority\tage\tCPU_Time\n");
	current = queue_2;
	while (current != NULL) {
		printf("%c%d\t%d\t%d\t%d\t%d\n", prefix, current->index, current->arrival_time, current->priority, current->age, current->cpu_time);
		current = current->next;
	}

	printf("This is Queue 3:\n");
	printf("Index\tArrival\tPriority\tage\tCPU_Time\n");
	current = queue_3;
	while (current != NULL) {
		printf("%c%d\t%d\t%d\t%d\t%d\n", prefix, current->index, current->arrival_time, current->priority, current->age, current->cpu_time);
		current = current->next;
	}
}

/* process scheduler */
void scheduler(Process **process_list, char prefix) {
	/* process scheduler */
	// the queues
	Process *queue_1 = NULL;
	Process *queue_2 = NULL;
	Process *queue_3 = NULL;
	Process *finished_process = NULL;

	// the status of current: time and process
	int current_time = 0;
	Process *current_process = NULL;
	int current_units_executed = 0;
	int current_index = -1;

	// constants for the assignments
	int priority_threshold_1 = 5; //priority reaches 2, promote to queue one
	int priority_threshold_2 = 3; //priority reaches 4, promote to queue two
	int queue_1_quantum = 5;
	int queue_2_quantum = 10;
	int queue_3_quantum = 20;
	int age_threshold_2 = 8;
	int age_threshold_3 = 8;
	int queue_1_decrease_threshold = 5 * queue_1_quantum;
	int queue_2_decrease_threshold = 2 * queue_2_quantum;

	// before we start processing the processes
	// we admit any processes arrived at time = 0
	admit_new_process(
		&(*process_list), 
		current_time, 
		&queue_1, 
		&queue_2, 
		&queue_3, 
		priority_threshold_1, 
		priority_threshold_2
	);


	// print status of this time
	// status_printer(*process_list, queue_1, queue_2, queue_3, finished_process, current_time, prefix);

	//Then methodically loop through these functions in order
	//The last 3 places processes back into the queues.
	//They are in the order of: priority decay in queue1 > new processes > aging priority increase
	do {
		//ready -> run
		dispatch_ready_process(
			&current_process, 
			&queue_1, 
			&queue_2, 
			&queue_3, 
			current_time, 
			&current_units_executed
			);

		// run
		execute_current_process(
			&current_process, 
			&current_time, 
			&current_units_executed
			);

		// new -> ready
		admit_new_process(
			&(*process_list), 
			current_time, 
			&queue_1, 
			&queue_2, 
			&queue_3, 
			priority_threshold_1, 
			priority_threshold_2
			);

		// running -> ready
		interrupt_running_process(
			&current_process, 
			&finished_process, 
			&queue_1, 
			&queue_2, 
			&queue_3, 
			current_time, 
			priority_threshold_1, 
			priority_threshold_2, 
			&current_index, 
			current_units_executed, 
			queue_1_quantum,
			queue_2_quantum,
			queue_3_quantum,
			queue_1_decrease_threshold,
			queue_2_decrease_threshold
			);

		// // new -> ready
		// admit_new_process(
		// 	&(*process_list), 
		// 	current_time, 
		// 	&queue_1, 
		// 	&queue_2, 
		// 	&queue_3, 
		// 	priority_threshold_1, 
		// 	priority_threshold_2
		// 	);

		// ageing processes in queue 2 and queue 3
		if (current_process == NULL) {
			ageing(
				&queue_1,
				&queue_2, 
				&queue_3, 
				age_threshold_2, 
				age_threshold_2, 
				priority_threshold_1, 
				priority_threshold_2, 
				current_index, 
				current_time
				);
			current_index = -1;
		}

		// print status of this time
		// status_printer(*process_list, queue_1, queue_2, queue_3, finished_process, current_time, prefix);

	} while (!queue_is_empty(queue_1) || !queue_is_empty(queue_2) || !queue_is_empty(queue_3) || current_process != NULL);

	
	// printf("\n");
	// printf("They are finished processes:\n");
	printf("Index\tPriority\tArrival\tEnd\tReady\tCPU_Time\tWaiting\n");
	Process *current = finished_process;
	while (current != NULL) {
		printf("%c%d\t%d\t%d\t%d\t%d\t%d\t%d\n", prefix, current->index, current->priority, current->arrival_time, current->end_time, current->ready_time, current->cpu_time, current->waiting_time);
		current = current->next;
	}
}


// here is the main func
int main(int argc, char **argv)
{
	Process *process_list = NULL;

	char process_prefix = read_input(argv[1], &process_list);

	/* schedule processes from inputs */
	scheduler(&process_list, process_prefix);

	return 0;
}

