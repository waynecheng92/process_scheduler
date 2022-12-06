/* COMP30023 project 1 : allocate
 * Student name: Cheng, Chun-Wen. Student ID: 1025323
 */

#include "function.h"

/****************************************************************/

/* calculates and returns turnaround time of all finished processes */
int
calculate_turnaroud(p_list_t *complete_list) {

	process_t *curr;
	curr = complete_list->head;
	float turnaround = 0, count = 0;

	//loop through all processes in complete_list
	while (curr != NULL) {
		//making sure process has no unfinished sub-processes left before calculation 
		if (curr->data.sub_left == 0) {
			turnaround += (curr->data.complete_time - curr->data.arr_time);
			count++;
		}
		curr = curr->next;
	}
	return (int)ceil(turnaround / count);
}

/* calculates and returns average overhead time of all finished processes */
float
calculate_avr_overhead(p_list_t *complete_list) {

	int count = 0;
	float total = 0;
	process_t *curr;
	curr = complete_list->head;

	//loop through all processes in complete_list
	while (curr != NULL) {
		//making sure process has no unfinished sub-processes left before calculation 
		if (curr->data.sub_left == 0) {
			total += ((curr->data.complete_time - curr->data.arr_time) / curr->data.exec_time);
			count++;
		}
		curr = curr->next;
	}
	return total / count;
}

/* calculates and returns the max overhead time of all finished processes */
float
calculate_max_overhead(p_list_t *complete_list) {

	float max_overtime = 0, curr_overtime;
	process_t *curr;
	curr = complete_list->head;

	//loop through all processes in complete_list
	while (curr != NULL) {
		//making sure process has no unfinished sub-processes left before calculation 
		if (curr->data.sub_left == 0) {
			curr_overtime = (curr->data.complete_time - curr->data.arr_time) / curr->data.exec_time;
			if (curr_overtime > max_overtime) {
				max_overtime = curr_overtime;
			}
		}
		curr = curr->next;
	}
	return max_overtime;
}

/* prints all the finished processes but exclusive of those have not had all their sub-processes finished */
void 
print_complete(p_list_t *complete_list, int time, int remain_process) {

	int sub_left;
	process_t *curr;
	
	curr = complete_list->head;

	//loop through all processes in complete_list
	while (curr != NULL) {
		if (curr->data.complete_time == time) {
			sub_left = curr->data.sub_left;
			//making sure it only prints those processes having all their sub-processes finished
			if (sub_left == 0) {
				printf("%d,FINISHED,pid=%d,proc_remaining=%d\n", time, curr->data.id, remain_process);
			}
		}
		curr = curr->next;
	}
}

/* insert a process to the correct place of list according to its execution time and id */
p_list_t
*insert_in_order(p_list_t *list, process_t *process) {

	process_t *curr = list->head, *prev;

	//insert to head of list if list has no processes in
	if (curr == NULL) {
		list->head = list->foot = process;
	}
	else {
		//if process's execution time is less than the execution time of the head of the list then let process 
		//be the new list head
		if (process->data.exec_time < list->head->data.exec_time) {
			list->head = process;
			process->next = curr;
		}
		//if process's execution time is larger than or equal to the execution time of the foot of the list then
		//let process be the new list foot
		else if (process->data.exec_time >= list->foot->data.exec_time) {
			list->foot->next = process;
			list->foot = process;
		}
		else {
			prev = list->head;
			curr = curr->next;
			//jump over those in the list having their execution less than or equal to process's execution time
			while (curr != NULL) {
				//jump over by resetting pointers prev and curr if process's execution time is larger than the one's
				//found
				if (process->data.exec_time > curr->data.exec_time) {
					prev = curr;
					curr = curr->next;
				}
				//if the one having its execution time equal to process's execution time is found then jump over 
				//all those having their execution time equal to process's execution time
				else if (process->data.exec_time == curr->data.exec_time) {
					//continually jump over by resetting pointers prev and curr until the one having its execution 
					//time larger than process's exexcution time is found
					while (curr->data.exec_time <= process->data.exec_time) {
						prev = curr;
						curr = curr->next;
					}
					prev->next = process;
					process->next = curr;
					break;
				}
				//insert process to the front of the one with its execution time larger than the process's
				else {
					prev->next = process;
					process->next = curr;
					break;
				}
			}
		}
	}
	return list;
}

/* splits all parallelisable processes if their execution time is larger than the average execution time of 
all processes before splitting */
void split_process(p_list_t *task_list, int num) {

	float average_exec_time, total_exec_time = 0;
	int count = 0, split_num;
	p_list_t *para_process;
	process_t *curr, *temp, *sub_process;

	curr = task_list->head;
	para_process = make_empty_list();

	//move all parallelisable processes into para_process
	while (curr != NULL) {
		total_exec_time += curr->data.exec_time;
		count++;
		if (curr->data.parallelisable == 'p') {
			remove_process(task_list, curr);
			allocate_to_list(para_process, curr);
			temp = curr->next;
			curr->next = NULL;
			curr = temp;
			continue;
		}
		curr = curr->next;
	}

	average_exec_time = total_exec_time / count;
	curr = para_process->head;

	//move all parallelisable processes and sub-processes back into task_list
	while (curr != NULL) {
		//processes with execution time lower than the average execution time dont have to be split
		if (curr->data.exec_time <= average_exec_time) {
			remove_process(para_process, curr);
			allocate_to_list(task_list, curr);
			temp = curr->next;
			curr->next = NULL;
			curr = temp;
		}
		else {
			// spilt number is the smaller one of number of CPU and process's execution time
			if (curr->data.exec_time > num) {
				split_num = num;
			} 
			else {
				split_num = curr->data.exec_time;
			}
			//create corresponding number of sub-processes then put them into task_list
			for (count = 0 ; count < split_num ; count++) {
				sub_process = (process_t *)malloc(sizeof(*sub_process));
				assert(sub_process != NULL);
				sub_process->data.arr_time = curr->data.arr_time;
				sub_process->data.id = curr->data.id;
				sub_process->data.sub_id = count;
				sub_process->data.exec_time = curr->data.exec_time;
				sub_process->data.remain_time = (int)ceil(curr->data.exec_time / split_num) + 1;
				sub_process->data.split_num = split_num;
				sub_process->next = NULL;
				allocate_to_list(task_list, sub_process);
			}
			temp = curr->next;
			//free the split process 
			free(curr);
			curr = temp;
		}
	}
}


/* assign those processes to CPUs if their arriving time is less or equal to time */
void
assign_work(p_list_t *task_list, cpu_t **cpu_list, int num, int time, bool challenge, int *remain_process) {

	int split_num;
	process_t *curr, *temp, *assign_process = NULL;
	p_list_t *assign_list;
	cpu_t *assign;

	if (challenge) {
		//loop through all CPUs and assign process to those that are idle
		for (int i = 0 ; i < num ; i++) {
			if (cpu_list[i]->curr_process == NULL) {
				curr = task_list->head;
				//loop through all processes in task_list to check if there is an arrived process 
				while (curr != NULL) {
					if (curr->data.arr_time <= time) {
						assign_process = curr;
						break;
					}
					curr = curr->next;
				}
				if (assign_process != NULL) {
					curr = assign_process->next;
					//find the arrived process with largest remaining time and assign it to an idle CPU
					while (curr != NULL) {
						if (curr->data.arr_time <= time && curr->data.remain_time > assign_process->data.remain_time) {
							assign_process = curr;
						}
						curr = curr->next;
					}
					remove_process(task_list, assign_process);
					allocate_to_list(cpu_list[i]->list, assign_process);
					cpu_list[i]->data.remain_time += assign_process->data.remain_time;
					assign_process->next = NULL;
					assign_process = NULL;
					(*remain_process)++;
				}
			}
		}
	}

	else {
		assign_list = process_to_allocate(task_list, time);
		curr = assign_list->head;
		//loop through all processes in assign_list
		while (curr != NULL) {
			//add one to remain_process for calculating the proc_remaing with FINISHED printing statement
			(*remain_process)++;
			//if the process is parallelizable and the number of CPU is larger than 1
			if (curr->data.parallelisable == 'p' && num > 1) {
				// spilt number is the smaller one of number of CPU and process's execution time
				if (curr->data.exec_time > num) {
					split_num = num;
				}
				else {
					split_num = curr->data.exec_time;
				}
				curr->data.split_num = split_num;
				//if no need to split process into sub-processes
				if (split_num == 1) {
					assign = find_fastest_cpu(cpu_list, num);
					allocate_to_list(assign->list, curr);
					//reset the assigned CPU's remaining time by add it with the execution time of process
					assign->data.remain_time += curr->data.exec_time;
					remove_process(assign_list, curr);
					temp = curr->next;
					//curr's next must be NULL after being allocated to the foot of the assigned CPU's work list
					curr->next = NULL;
					curr = temp;
					continue;
				}
				//if need to split process into sub-processes
				else {
					split_to_cpu(cpu_list, curr, num, split_num);
					remove_process(assign_list, curr);
					temp = curr->next;
					//curr is split into sub-processes hence need to free
					free(curr);
					curr = temp;
					continue;
				}
			}
			else {
				assign = find_fastest_cpu(cpu_list, num);
				allocate_to_list(assign->list, curr);
				assign->data.remain_time += curr->data.exec_time;
				remove_process(assign_list, curr);
				temp = curr->next;
				//curr's next must be NULL after being allocated to the foot of the assigned CPU's work list
				curr->next = NULL;
				curr = temp;
				continue;
			} 
			curr = curr->next;
		}
		free(assign_list);
	}
}

/* according to the arriving time of processes, allocate them to assign_list then return */
p_list_t
*process_to_allocate(p_list_t *task_list, int time) {

	process_t *curr, *temp;
	p_list_t *assign_list;

	curr = task_list->head;
	assign_list = make_empty_list();
	//loop through all processes in task_list
	while (curr != NULL) {
		if (curr->data.arr_time == time) {
			remove_process(task_list, curr);
			temp = curr->next;
			curr->next = NULL;
			assign_list = insert_in_order(assign_list, curr);
			curr = temp;
			continue;
		}
		curr = curr->next;
	}
	return assign_list;
}

/* split process into spilt_num of sub-processes then assigned them to split_num of CPUs
with less remaining time */
void
split_to_cpu(cpu_t **cpu_list, process_t *process, int num, int split_num) {

	float count = 0;
	cpu_list_t *temp_list;
	process_t *sub_process;
	cpu_t *curr;

	temp_list = assign_cpu_to_list(cpu_list, num);
	curr = temp_list->head;

	//remove the CPUs with larger remaining time from temp_list
	while (num > split_num) {
		remove_max_remain_cpu(temp_list);
		num--;
	}
	
	curr = temp_list->head;
	//create and assign values to the newly created sub-processes then assign the sub-processes to the CPUs
	//left in temp_list
	while (curr != NULL && count < split_num) {
		sub_process = (process_t *)malloc(sizeof(*sub_process));
		assert(sub_process != NULL);
		sub_process->data.arr_time = process->data.arr_time;
		sub_process->data.id = process->data.id;
		sub_process->data.sub_id = count;
		sub_process->data.exec_time = process->data.exec_time;
		sub_process->data.remain_time = (int)ceil(process->data.exec_time / split_num) + 1;
		sub_process->data.split_num = split_num;
		sub_process->next = NULL;
		//assign sub-process to the CPU
		allocate_to_list(curr->list, sub_process);
		//reset the assigned CPU's remaining time by add it with the execution time of the sub-process
		curr->data.remain_time += sub_process->data.exec_time;
		curr = curr->next;
		count++;
	}
	free(temp_list);
}

/* put all the CPUs in cpu_list into a newly created linked-list of CPU then return */
cpu_list_t
*assign_cpu_to_list(cpu_t **cpu_list, int num) {

	int count = 0;
	cpu_list_t *temp_list;
	cpu_t *curr;

	temp_list = (cpu_list_t*)malloc(sizeof(*temp_list));
	assert(temp_list != NULL);

	temp_list->head = cpu_list[count];
	temp_list->foot = cpu_list[num - 1];
	curr = cpu_list[count++];

	while (count < num) {
		curr->next = cpu_list[count++];
	 	curr = curr->next;
	}

	return temp_list;
}

/* to calculate the execution time of a sub-process by the execution time of its parent process and round up to 
integer */
int
convert_split_time(float exec_time, float split_num) {

	float split_time = exec_time / split_num;

	if (split_time > (int)floor(split_time)) {
		return (int)ceil(split_time) + 1;
	}
	else {
		return split_time + 1;
	}
}

/* to find the best split number resulting in the smallest remaining time */
int
find_split_num(cpu_t **cpu_list, int num, int time) {

	int split_num, temp_split_num, max_remain_time, max_cpu_time, split_time, temp_max_time;
	cpu_list_t *temp_list;

	temp_list = assign_cpu_to_list(cpu_list, num);

	//to make sure split number is not exceeding number of CPUs and time divided by split number 
	//is larger than or equal to 1
	if (time < num) {
		split_num = time;
	}
	else {
		split_num = num;
	}

	max_cpu_time = find_max_cpu_time(temp_list);
	split_time = convert_split_time(time, split_num);
	max_remain_time = max_cpu_time + split_time;

	temp_split_num = split_num - 1;
	//loop through all possible split number and record the best one
	while (temp_split_num >= 1) {
		remove_max_remain_cpu(temp_list);
		max_cpu_time = find_max_cpu_time(temp_list);
		if (temp_split_num > 1) {
			split_time = convert_split_time(time, temp_split_num);
		} 
		//split number does not have to add one if it is 1
		else {
			split_time = time;
		}

		temp_max_time = max_cpu_time + split_time;
		//if a better split number is found then record it
		if (temp_max_time <= max_remain_time) {
			max_remain_time = temp_max_time;
			split_num = temp_split_num;
		}
		temp_split_num--;
	}
	free(temp_list);
	return split_num;
}

/* finds and returns the largest remaining time of all CPUs in list */
float
find_max_cpu_time(cpu_list_t *list) {

	float max_cpu_time = 0;
	cpu_t *curr;
	curr = list->head;

	//loop through all CPUs in list
	while (curr != NULL) {
		if (curr->data.remain_time > max_cpu_time) {
			max_cpu_time = curr->data.remain_time;
		}
		curr = curr->next;
	}

	return max_cpu_time;
}

/* remove the CPU with the largest remaining time from list */
void
remove_max_remain_cpu(cpu_list_t *list) {

	int id;
	float max_remain_time;
	cpu_t *prev, *curr;

	curr = list->head;
	max_remain_time = curr->data.remain_time;
	//loop through all CPUs in list
	while (curr != NULL) {
		//if there is a CPU with larger or equal remaining time detected then record the id of the CPU
		if (curr->data.remain_time >= max_remain_time) {
			max_remain_time = curr->data.remain_time;
			id = curr->data.id;
		}
		curr = curr->next;
	}

	//assign list head to the next one to the current head of list if the id
	//found matches with the id of the current head's
	if (list->head->data.id == id) {
		list->head = list->head->next;
	} 
	else {
		curr = list->head;
		//loop until it reaches the foot of list or if a matching id is found
		while (curr->next != NULL && curr->next->data.id != id) {
			prev = curr;
			curr = curr->next;
		}
		//reaches the foot means the foot's id matches with the id found hence 
		//remove the current foot and assign a new foot
		if (curr->next == NULL) {
			prev->next = curr->next;
			list->foot = prev;
		}
		//here means a matching id of a CPU is found hence remove the CPU from list
		else {
			curr->next = curr->next->next;
		}
	}
}

/* checks if any process is left in task_list or assigned to any CPU */
bool
all_finish(p_list_t *task_list, cpu_t **cpu_list, int num) {

	int finish = true;

	if (task_list->head == NULL) {
		for (int i = 0 ; i < num ; i++) {
			if (cpu_list[i]->list->head != NULL) {
				finish = false;
				break;
			}
		}
	}
	else {
		finish = false;
	}

	return finish;
}

/* find the process in the CPU with the shortest remaining time and run it for one second */
void 
execute_shortest(cpu_t *cpu, p_list_t *complete_list, int time, int *remain_process) {

	process_t *shortest;
	process_t *curr;

	shortest = curr = cpu->list->head;

	//run only if the CPU is assigned with process
	if (cpu->list->head != NULL) {
		//loop through all the processes assigned to the CPU
		while (curr != NULL) {
			//record the process if it has a shorter remaining time than the one we had found
			if (curr->data.remain_time < shortest->data.remain_time) {
				shortest = curr;
			}
			//prefers the process with a smaller id when two processes have equal remaining time 
			else if (curr->data.remain_time == shortest->data.remain_time && 
				curr->data.id < shortest->data.id) {
				shortest = curr;
			}
			curr = curr->next;
		}

		//prints only if the process with the shortest remaining time is different to the previous one 
		//recorded by the CPU
		if (shortest != cpu->curr_process) {
			cpu->curr_process = shortest;
			//print with the below format if the process is not split into sub-processes
			if (shortest->data.split_num == 1) {
				printf("%d,RUNNING,pid=%d,remaining_time=%g,cpu=%d\n", time, shortest->data.id, 
					shortest->data.remain_time, cpu->data.id);
			} 
			////print with the below format if the process is split into sub-processes
			else {
				printf("%d,RUNNING,pid=%d.%d,remaining_time=%g,cpu=%d\n", time, shortest->data.id, 
					shortest->data.sub_id, shortest->data.remain_time, cpu->data.id);
			}
		}

		//remaining time is deducted 
		shortest->data.remain_time -= 1;
		cpu->data.remain_time -= 1;

		//if the remaining time of the process is less or equal to 0 then move it to complete_list
		if (shortest->data.remain_time <= 0) {
			allocate_to_list(complete_list, shortest);
			remove_process(cpu->list, shortest);
			shortest->data.sub_left = process_left(complete_list, shortest);
			shortest->data.complete_time = time + 1;
			shortest->next = NULL;
			cpu->curr_process = NULL;

			//subtract one from remain_process for calculating the proc_remaing with FINISHED printing statement
			//when the process has no unfinished sub-processes left
			if (shortest->data.sub_left == 0) {
				(*remain_process)--;
			}
		}
	}
}

/* calculates and returns the number of unfinished sub-processes */
int
process_left(p_list_t *complete_list, process_t *process) {

	int process_left, id;
	process_t *curr;

	process_left = process->data.split_num;
	id = process->data.id;
	curr = complete_list->head;

	//loop through all processes in complete_list
	while (curr != NULL) {
		//if there is any having identical id then subtract one from process_left
		if (curr->data.id == id) {
			process_left--;
		}
		curr = curr->next;
	}

	return process_left;
}

/* insert process to the foot of list */
void 
allocate_to_list(p_list_t *list, process_t *process) {

	//if list is empty then assign process to its head and foot 
	if (list->head == NULL) {
		list->head = list->foot = process;
	}
	//else insert to the foot
	else {
		list->foot->next = process;
		list->foot = process;
	}
}

/* remove process from list */
void
remove_process(p_list_t *list, process_t *process) {

	process_t *curr;

	curr = list->head;

	//if process is the only process in list then assign head and foot of list to NULL
	if (list->head == process && list->foot == process) {
		list->head = list->foot = NULL;
	} 
	//if head of list is process then assign head of list to the one next to process
	else if (list->head == process) {
		list->head = process->next;
	}
	else {
		//loop until process (curr->next) is found then remove it 
		while (curr->next != process && curr->next != NULL) {
			curr = curr->next;
		}
		curr->next = process->next;
		//if process is the foot of list then assign the one previous to process (curr) to foot of list
		if (list->foot == process) {
			list->foot = curr;
		}
	}
}

/* finds and returns the CPU with shortest remaining time */
cpu_t
*find_fastest_cpu(cpu_t **cpu_list, int num) {

	cpu_t *fastest;
	int count = 0;
	fastest = cpu_list[count++];

	//loop through all CPUs stored in cpu_list
	while (count < num) {
		//if a CPU with shorter remaining time is found then assign the CPU to fastest
		if (cpu_list[count]->data.remain_time < fastest->data.remain_time) {
			fastest = cpu_list[count];
		}
		count++;
	}

	return fastest;
}

/* creates corresponding number of CPUs and assigns them to list */
void
create_cpu(cpu_t **list, int num) {

	int count = 0;

	while (count < num) {

		cpu_t *new;
		p_list_t *assignment_list;

		new = (cpu_t*)malloc(sizeof(*new));
		assert(new != NULL);

		assignment_list = make_empty_list();
		new->list = assignment_list;
		new->next = NULL;
		new->curr_process = NULL;
		new->data.remain_time = 0;
		new->data.id = count;
		list[count] = new;

		count++;
	}
}

/* creates an empty list of process */
p_list_t
*make_empty_list(void) {

	p_list_t *list;

	list = (p_list_t*)malloc(sizeof(*list));
	assert(list != NULL);
	list->head = list->foot = NULL;

	return list;
}

/* creates a process and assigns data to the process according to the file fp 
then insert it to list and returns list */
p_list_t
*insert_to_foot(p_list_t *list, FILE *fp) {

	process_t *new;

	new = (process_t*)malloc(sizeof(*new));
	assert(new != NULL);

	new = insert_data(new, fp);
	new->next = NULL;

	allocate_to_list(list, new);

	return list;
}

/* repeatedly assigns processes to list until end of file of fp and returns list */
p_list_t
*insert_data_to_list(p_list_t *list, FILE *fp) {

	//loop until nothing left in fp
	while(!feof(fp)) {
		list = insert_to_foot(list, fp);
	}

	return list;
}

/* sets and assigns data read from fp to process then returns process  */
process_t
*insert_data(process_t *process, FILE *fp) {

	fscanf(fp,"%d ", &(process->data.arr_time));
	fscanf(fp,"%d ", &(process->data.id));
	fscanf(fp,"%f ", &(process->data.exec_time));
	fscanf(fp, "%c\n", &(process->data.parallelisable));
	process->data.remain_time = process->data.exec_time;
	process->data.split_num = 1;
	process->data.sub_id = 0;

	return process;
}


/* frees all processes in list including list itself */
void
free_list(p_list_t *list) {

	process_t *curr, *prev;

	assert(list != NULL);
	curr = list->head;

	//loops through all processes in list
	while (curr != NULL) {
		prev = curr;
		curr = curr->next;
		free(prev);
	}
	free(list);
}
