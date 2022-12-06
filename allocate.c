/* COMP30023 project 1 : allocate
 * Student name: Cheng, Chun-Wen. Student ID: 1025323
 */

#include "function.h"

/****************************************************************/

int
main(int argc, char *argv[]) {

	int time = 0, remain_process = 0;
	FILE *fp_r;
	p_list_t *task_list;				/* all processes read will be stored in this list */
	p_list_t *complete_list;			/* all processes completed will be stored in this list */
	long cpu_num;						/* number of CPU */
	bool challenge = false;

	for (int i = 0 ; argv[i] != NULL ; i++) {
		if (!strcmp(argv[i], "-f")) {
			fp_r = fopen(argv[++i], "r");
		}
		else if (!strcmp(argv[i], "-p")) {
			cpu_num = strtol(argv[++i], NULL, 10);
		}
		else if (!strcmp(argv[i], "-c")) {
			challenge = true;
		}
	}


	cpu_t **cpu_list = (cpu_t**)malloc(sizeof(cpu_t)*cpu_num);
	assert(cpu_list != NULL);

	task_list = make_empty_list();

	// store all the data read from the file to task_list
	task_list = insert_data_to_list(task_list, fp_r);

	if (challenge) {
		split_process(task_list, cpu_num);
	}

	complete_list = make_empty_list();

	// create CPUs based on the number required
	create_cpu(cpu_list, cpu_num);

	//loop until the task_list is empty and there has no processes assigned to CPUs
	while (!all_finish(task_list, cpu_list, cpu_num)) {
		print_complete(complete_list, time, remain_process);
		assign_work(task_list, cpu_list, cpu_num, time, challenge, &remain_process);
		for(int id = 0 ; id < cpu_num ; id++) {
			execute_shortest(cpu_list[id], complete_list, time, &remain_process);
		}
		time++;
	}
	print_complete(complete_list, time, remain_process);

	printf("Turnaround time %d\n", calculate_turnaroud(complete_list));
	printf("Time overhead %.2f %.2f\n", calculate_max_overhead(complete_list), calculate_avr_overhead(complete_list));
	printf("Makespan %d\n", time);

	for (int i = 0 ; i < cpu_num ; i++) {
		free(cpu_list[i]->list);
	}

	free(cpu_list);
	free_list(complete_list);
	fclose(fp_r);
}
