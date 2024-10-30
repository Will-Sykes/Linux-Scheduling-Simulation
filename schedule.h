// William Sykes
// 5/26/2024
// Operating Systems
#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <pthread.h>

// PCB structure with packing to ensure correct alignment
#pragma pack(push, 1)
typedef struct {
    unsigned char priority;
    char process_name[32];
    int process_id;
    unsigned char activity_status;
    int cpu_burst_time;
    int base_register;
    long limit_register;
    int number_of_files;
    long check_sum;
    unsigned char process_type;
} PCB;
#pragma pack(pop)

// Arguments for processor threads
typedef struct {
    PCB *processes;
    int num_processes;
    int scheduling_algorithm;
    int processor_index;
} ProcessorArgs;

// Shared data structure for load balancing
typedef struct {
    int *queue_sizes;
    int num_processors;
    pthread_mutex_t mutex;
} LoadBalancer;

extern LoadBalancer load_balancer;
extern PCB **all_processes;
extern pthread_t *threads;

// Function prototypes
void *cpu_simulation(void *arg);
void priority_scheduling(PCB *processes, int num_processes, int processor_index);
void sjf_scheduling(PCB *processes, int num_processes, int processor_index);
void fcfs_scheduling(PCB *processes, int num_processes, int processor_index);
void rr_scheduling(PCB *processes, int num_processes, int processor_index);
void load_balance(PCB **all_processes, int processor_index);
void read_process_file(const char *filename, PCB **processes, int *num_processes, int *total_memory, int *total_files);
void assign_processes_to_processors(PCB *processes, int num_processes, int num_processors, double *loads, int *scheduling_algorithms, PCB ***all_processes);
void print_pcb(PCB *process);

#endif // SCHEDULE_H