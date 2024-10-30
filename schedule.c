// William Sykes
// 5/26/2024
// Operating Systems
#include "schedule.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> //debugging

#define TIME_SLICE 2 // Quantum time slice

LoadBalancer load_balancer;
PCB **all_processes = NULL;
pthread_t *threads = NULL;

/**
 * @brief Simulates CPU processing on a given processor.
 *
 * @param arg Pointer to ProcessorArgs structure containing processor parameters.
 * @return NULL
 */
void *cpu_simulation(void *arg) {
    ProcessorArgs *args = (ProcessorArgs *)arg;
    PCB *processes = args->processes;
    int num_processes = args->num_processes;
    int scheduling_algorithm = args->scheduling_algorithm;
    int processor_index = args->processor_index;

    printf("Processor %d started with algorithm %d.\n", processor_index, scheduling_algorithm);

    while (load_balancer.queue_sizes[processor_index] > 0) {
        printf("Processor %d queue size: %d\n", processor_index, load_balancer.queue_sizes[processor_index]);
        switch (scheduling_algorithm) {
            case 1:
                sjf_scheduling(processes, num_processes, processor_index);
                break;
            case 2:
                fcfs_scheduling(processes, num_processes, processor_index);
                break;
            case 3:
                priority_scheduling(processes, num_processes, processor_index);
                break;
            case 4:
                rr_scheduling(processes, num_processes, processor_index);
                break;
            default:
                fprintf(stderr, "Unknown scheduling algorithm %d\n", scheduling_algorithm);
                break;
        }

        if (load_balancer.queue_sizes[processor_index] == 0) {
            printf("Processor %d queue is empty. Initiating load balancing.\n", processor_index);
            load_balance(all_processes, processor_index);
        }
    }

    printf("Processor %d finished execution.\n", processor_index);

    free(processes);
    free(args);
    return NULL;
}
/**
 * @brief Implements Round-Robin scheduling algorithm.
 *
 * @param processes Array of PCB structures representing processes.
 * @param num_processes The number of processes.
 * @param processor_index Index of the processor executing the processes.
 */
void rr_scheduling(PCB *processes, int num_processes, int processor_index) {
    int remaining_processes = num_processes;
    while (remaining_processes > 0) {
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].cpu_burst_time > 0) {
                printf("Processor %d executing process %s (ID: %d) with remaining burst time %d\n",
                       processor_index, processes[i].process_name, processes[i].process_id, processes[i].cpu_burst_time);

                int execute_time = processes[i].cpu_burst_time < TIME_SLICE ? processes[i].cpu_burst_time : TIME_SLICE;
                processes[i].cpu_burst_time -= execute_time;
                sleep(execute_time);

                if (processes[i].cpu_burst_time <= 0) {
                    remaining_processes--;
                    printf("Processor %d process %s (ID: %d) finished execution.\n", processor_index, processes[i].process_name, processes[i].process_id);
                    load_balancer.queue_sizes[processor_index]--;
                }
            }
        }
    }
}
/**
 * @brief Implements First-Come-First-Serve scheduling algorithm.
 *
 * @param processes Array of PCB structures representing processes.
 * @param num_processes The number of processes.
 * @param processor_index Index of the processor executing the processes.
 */
void fcfs_scheduling(PCB *processes, int num_processes, int processor_index) {
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].cpu_burst_time > 0) {
            printf("Processor %d executing process %s (ID: %d) with burst time %d\n",
                   processor_index, processes[i].process_name, processes[i].process_id, processes[i].cpu_burst_time);

            sleep(processes[i].cpu_burst_time);
            processes[i].cpu_burst_time = 0;

            printf("Processor %d process %s (ID: %d) finished execution.\n", processor_index, processes[i].process_name, processes[i].process_id);
            load_balancer.queue_sizes[processor_index]--;
        }
    }
}
/**
 * @brief Compares the CPU burst time of two PCB structures.
 *
 * @param a Pointer to the first PCB structure.
 * @param b Pointer to the second PCB structure.
 * @return The difference between the CPU burst times of the two PCBs.
 */
int compare_burst_time(const void *a, const void *b) {
    PCB *pcb_a = (PCB *)a;
    PCB *pcb_b = (PCB *)b;
    return pcb_a->cpu_burst_time - pcb_b->cpu_burst_time;
}

/**
 * @brief Implements Shortest Job First scheduling algorithm.
 *
 * @param processes Array of PCB structures representing processes.
 * @param num_processes The number of processes.
 * @param processor_index Index of the processor executing the processes.
 */
void sjf_scheduling(PCB *processes, int num_processes, int processor_index) {
    qsort(processes, num_processes, sizeof(PCB), compare_burst_time);

    for (int i = 0; i < num_processes; i++) {
        if (processes[i].cpu_burst_time > 0) {
            printf("Processor %d executing process %s (ID: %d) with burst time %d\n",
                   processor_index, processes[i].process_name, processes[i].process_id, processes[i].cpu_burst_time);

            sleep(processes[i].cpu_burst_time);
            processes[i].cpu_burst_time = 0;

            printf("Processor %d process %s (ID: %d) finished execution.\n", processor_index, processes[i].process_name, processes[i].process_id);
            load_balancer.queue_sizes[processor_index]--;
        }
    }
}
/**
 * @brief Compares the priority of two PCB structures.
 *
 * @param a Pointer to the first PCB structure.
 * @param b Pointer to the second PCB structure.
 * @return The difference between the priorities of the two PCBs.
 */
int compare_priority(const void *a, const void *b) {
    PCB *pcb_a = (PCB *)a;
    PCB *pcb_b = (PCB *)b;
    return pcb_a->priority - pcb_b->priority;
}

/**
 * @brief Implements Priority scheduling algorithm.
 *
 * @param processes Array of PCB structures representing processes.
 * @param num_processes The number of processes.
 * @param processor_index Index of the processor executing the processes.
 */
void priority_scheduling(PCB *processes, int num_processes, int processor_index) {
    qsort(processes, num_processes, sizeof(PCB), compare_priority);

    for (int i = 0; i < num_processes; i++) {
        if (processes[i].cpu_burst_time > 0) {
            printf("Processor %d executing process %s (ID: %d) with priority %d and burst time %d\n",
                   processor_index, processes[i].process_name, processes[i].process_id, processes[i].priority, processes[i].cpu_burst_time);

            sleep(processes[i].cpu_burst_time);
            processes[i].cpu_burst_time = 0;

            printf("Processor %d process %s (ID: %d) finished execution.\n", processor_index, processes[i].process_name, processes[i].process_id);
            load_balancer.queue_sizes[processor_index]--;
        }
    }
}

/**
 * @brief Balances the load by moving processes from the busiest processor to the current processor.
 *
 * @param all_processes Array of arrays of PCB structures representing all processes on each processor.
 * @param processor_index Index of the current processor.
 */
void load_balance(PCB **all_processes, int processor_index) {
    // Lock the mutex to ensure exclusive access to shared data
    pthread_mutex_lock(&load_balancer.mutex);

    // Print the current processor index for debugging
    printf("Entering load_balance. Processor index: %d\n", processor_index);

    // Initialize variables to find the processor with the maximum queue size
    int max_queue_size = 0;
    int max_queue_index = -1;

    // Loop through all processors to find the one with the largest queue size
    for (int i = 0; i < load_balancer.num_processors; i++) {
        if (load_balancer.queue_sizes[i] > max_queue_size) {
            max_queue_size = load_balancer.queue_sizes[i];
            max_queue_index = i;
        }
    }

    // Print the index and size of the processor with the largest queue
    printf("Max queue index: %d, Max queue size: %d\n", max_queue_index, max_queue_size);

    // If no suitable processor is found or all queues are empty, exit the function
    if (max_queue_index == -1 || max_queue_size == 0) {
        printf("Load balancing: No suitable processor found or all queues are empty.\n");
        pthread_mutex_unlock(&load_balancer.mutex);
        return;
    }

    // Calculate the number of processes to move (half of the max queue size)
    int num_processes_to_move = max_queue_size / 2;
    printf("Number of processes to move: %d\n", num_processes_to_move);

    // Get the source array of processes from the busiest processor
    PCB *src = all_processes[max_queue_index];
    if (src == NULL) {
        // If the source pointer is NULL, print an error message and unlock the mutex
        fprintf(stderr, "Source pointer is NULL for processor %d\n", max_queue_index);
        pthread_mutex_unlock(&load_balancer.mutex);
        return;
    }

    // Reallocate memory for the destination processor to accommodate the new processes
    PCB *dest = realloc(all_processes[processor_index], 
        (load_balancer.queue_sizes[processor_index] + num_processes_to_move) * sizeof(PCB));
    if (dest == NULL) {
        //memory reallocation fails, print an error message and unlock the mutex
        fprintf(stderr, "Failed to reallocate memory for processor %d: %s\n", processor_index, strerror(errno));
        pthread_mutex_unlock(&load_balancer.mutex);
        return;
    }
    all_processes[processor_index] = dest;

    // Calculate the starting indices for the source and destination arrays
    int src_start_index = load_balancer.queue_sizes[max_queue_index] - num_processes_to_move;
    int dest_start_index = load_balancer.queue_sizes[processor_index];
    printf("src_start_index: %d, dest_start_index: %d\n", src_start_index, dest_start_index);

    // Move the processes from the source to the destination array
    for (int i = 0; i < num_processes_to_move; i++) {
        dest[dest_start_index + i] = src[src_start_index + i];
    }

    // Update the queue sizes for the source and destination processors
    load_balancer.queue_sizes[processor_index] += num_processes_to_move;
    load_balancer.queue_sizes[max_queue_index] -= num_processes_to_move;

    // Print a message indicating the number of processes moved and the processors involved
    printf("Load balancing: Moved %d processes from processor %d to processor %d\n", 
        num_processes_to_move, max_queue_index, processor_index);

    // Unlock the mutex to allow other threads to access the shared data
    pthread_mutex_unlock(&load_balancer.mutex);
}


/**
 * @brief Reads process data from a file and populates the given processes array.
 *
 * This function reads a binary file containing an array of PCB structures. It allocates memory
 * for the processes array, reads the data from the file, and calculates the total memory
 * used and the total number of files across all processes.
 *
 * @param filename The name of the file to read.
 * @param processes Pointer to an array of PCB structures where the process data will be stored.
 * @param num_processes Pointer to an integer where the number of processes will be stored.
 * @param total_memory Pointer to an integer where the total memory used by the processes will be stored.
 * @param total_files Pointer to an integer where the total number of files across all processes will be stored.
 */
void read_process_file(const char *filename, PCB **processes, int *num_processes, int *total_memory, int *total_files) {
    // Print the filename being read for debugging purposes
    printf("Reading process file: %s\n", filename);

    // Open the file in binary read mode
    FILE *file = fopen(filename, "rb");
    if (!file) {
        // Print an error message and exit if the file cannot be opened
        perror("Failed to open process file");
        exit(EXIT_FAILURE);
    }

    // Move the file pointer to the end of the file to determine the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Calculate the number of processes in the file
    *num_processes = file_size / sizeof(PCB);
    // Allocate memory for the processes array
    *processes = malloc(*num_processes * sizeof(PCB));
    if (!*processes) {
        // Print an error message and exit if memory allocation fails
        fprintf(stderr, "Failed to allocate memory for processes\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Read the process data from the file into the processes array
    if (fread(*processes, sizeof(PCB), *num_processes, file) != *num_processes) {
        // Print an error message and clean up if reading from the file fails
        perror("Failed to read process file");
        free(*processes);
        fclose(file);
        exit(EXIT_FAILURE);
    }
    // Close the file as it is no longer needed
    fclose(file);

    *total_memory = *num_processes * sizeof(PCB);
    *total_files = 0;
    // Iterate over each process to count the total number of files
    for (int i = 0; i < *num_processes; i++) {
        *total_files += (*processes)[i].number_of_files;
    }

    // number of processes read from the file
    printf("Read %d processes from the file.\n", *num_processes);

    // for debugging
    for (int i = 0; i < (*num_processes < 5 ? *num_processes : 5); i++) {
        print_pcb(&(*processes)[i]);
    }
}

void print_pcb(PCB *process) {
    printf("Process Name: %s\n", process->process_name);
    printf("Process ID: %d\n", process->process_id);
    printf("Priority: %d\n", process->priority);
    printf("Activity Status: %d\n", process->activity_status);
    printf("CPU Burst Time: %d\n", process->cpu_burst_time);
    printf("Base Register: %d\n", process->base_register);
    printf("Limit Register: %ld\n", process->limit_register);
    printf("Number of Files: %d\n", process->number_of_files);
    printf("Check Sum: %ld\n", process->check_sum);
    printf("Process Type: %d\n", process->process_type);
}

/**
 * @brief Assigns processes to processors based on the load distribution and starts simulation threads.
 *
 * This function distributes the given processes among the available processors according to the specified load
 * distribution. It allocates memory for each processor's process list, sets up the scheduling parameters,
 * and creates a thread to simulate the CPU for each processor.
 *
 * @param processes Array of PCB structures representing all processes.
 * @param num_processes The total number of processes.
 * @param num_processors The number of processors.
 * @param loads Array of doubles representing the load distribution for each processor.
 * @param scheduling_algorithms Array of integers representing the scheduling algorithm for each processor.
 * @param all_processes Pointer to an array of PCB arrays where the processes for each processor will be stored.
 */
void assign_processes_to_processors(PCB *processes, int num_processes, int num_processors, double *loads, int *scheduling_algorithms, PCB ***all_processes) {
    int start = 0; // Start index for process distribution
    // Allocate memory for thread handles
    threads = malloc(num_processors * sizeof(pthread_t));

    // Loop through each processor
    for (int i = 0; i < num_processors; i++) {
        // Calculate the number of processes for this processor based on the load
        int count = (int)(loads[i] * num_processes);
        // Allocate memory for the processes assigned to this processor
        PCB *processor_processes = malloc(count * sizeof(PCB));
        if (!processor_processes) {
            // Print an error message and exit if memory allocation fails
            fprintf(stderr, "Failed to allocate memory for processor %d processes\n", i);
            exit(EXIT_FAILURE);
        }
        // Copy the processes to this processor's process list
        memcpy(processor_processes, &processes[start], count * sizeof(PCB));
        start += count; // Update the start index for the next processor

        // Store the process list in the all_processes array
        (*all_processes)[i] = processor_processes;

        // Print the initial processes for debugging purposes
        printf("Processor %d initial processes:\n", i);
        for (int j = 0; j < (count < 5 ? count : 5); j++) {
            print_pcb(&processor_processes[j]);
        }

        // Allocate memory for ProcessorArgs and set its values
        ProcessorArgs *args = malloc(sizeof(ProcessorArgs));
        if (!args) {
            // Print an error message and exit if memory allocation fails
            fprintf(stderr, "Failed to allocate memory for ProcessorArgs\n");
            exit(EXIT_FAILURE);
        }
        args->processes = processor_processes;
        args->num_processes = count;
        args->scheduling_algorithm = scheduling_algorithms[i];
        args->processor_index = i;

        // Set the initial queue size for this processor
        load_balancer.queue_sizes[i] = count;

        // Create a thread to simulate the CPU for this processor
        if (pthread_create(&threads[i], NULL, cpu_simulation, args) != 0) {
            // Print an error message and exit if thread creation fails
            fprintf(stderr, "Failed to create thread for processor %d\n", i);
            exit(EXIT_FAILURE);
        }
        // Print a message indicating the assignment details
        printf("Assigned %d processes to processor %d using scheduling algorithm %d.\n", count, i, scheduling_algorithms[i]);
    }
}