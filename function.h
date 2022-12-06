/* COMP30023 project 1 : allocate
 * Student name: Cheng, Chun-Wen. Student ID: 1025323
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <math.h>

/****************************************************************/

typedef struct cpu cpu_t;

typedef struct process process_t;

typedef struct p_data p_data_t;

typedef struct cpu_data cpu_data_t;

typedef struct p_list p_list_t;

typedef struct cpu_list cpu_list_t;


/* the data needed for a process */
struct p_data {
	int arr_time;					/* arriving time */
	int id;
	int sub_id;						/* sub_id is given to all sub-processes of it */
	float exec_time;				/* execution time */
	float remain_time;				/* remaining time */
	int split_num;					/* the number of sub-processes */
	int sub_left;					/* the number of unfinished sub-processes */
	int complete_time;				/* the time when it is finished */
	char parallelisable;
};

/* the attributes under a process */
struct process {
	p_data_t data;
	process_t *next;
};

/* the data needed for a CPU */
struct cpu_data {
	float remain_time;				/* remaining time */
	int id;
};

/* the attributes under a CPU */
struct cpu {
	cpu_data_t data;
	p_list_t *list;					/* the list with all processes assigned to it */
	process_t *curr_process;		/* the process it currently works on */
	cpu_t *next;
};

/* linked-list of processes */
struct p_list {
	process_t *head;
	process_t *foot;
};

/* linked-list of CPUs */
struct cpu_list {
	cpu_t *head;
	cpu_t *foot;
};

/****************************************************************/

/* function prototypes */
void free_list(p_list_t *list);
void create_cpu(cpu_t **list, int num);
void remove_max_remain_cpu(cpu_list_t *list);
void split_process(p_list_t *task_list, int num);
void remove_process(p_list_t *list, process_t *process);
void allocate_to_list(p_list_t *list, process_t *process);
void print_complete(p_list_t *complete_list, int time, int remain_process);
void split_to_cpu(cpu_t **cpu_list, process_t *process, int num, int split_num);
void execute_shortest(cpu_t *cpu, p_list_t *complete_list, int time, int *remain_process);
void assign_work(p_list_t *task_list, cpu_t **cpu_list, int num, int time, bool challenge, int *remain_process);
bool all_finish(p_list_t *task_list, cpu_t **cpu_list, int num);
int calculate_turnaroud(p_list_t *complete_list);
int find_split_num(cpu_t **cpu_list, int num, int time);
int convert_split_time(float exec_time, float split_num);
int process_left(p_list_t *complete_list, process_t *process);
float find_max_cpu_time(cpu_list_t *list);
float calculate_avr_overhead(p_list_t *complete_list);
float calculate_max_overhead(p_list_t *complete_list);
p_list_t *make_empty_list(void);
p_list_t *insert_to_foot(p_list_t *list, FILE *fp);
p_list_t *insert_data_to_list(p_list_t *list, FILE *fp);
p_list_t *insert_in_order(p_list_t *list, process_t *process);
p_list_t *process_to_allocate (p_list_t *task_list, int time);
process_t *insert_data(process_t *process, FILE *fp);
cpu_t *find_fastest_cpu(cpu_t **cpu_list, int num);
cpu_list_t *assign_cpu_to_list(cpu_t **cpu_list, int num);
