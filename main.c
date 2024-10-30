// William Sykes
// 5/26/2024
// Operating Systems
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "schedule.h"

/**
 * @brief The main function for the scheduling simulation.
 *
 * This function initializes the load balancer, reads processes from a file,
 * assigns processes to processors, and starts the scheduling simulation.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return int Returns 0 on successful completion.
 */
int main(int argc, char *argv[]) {
    // Check if the minimum number of command-line arguments is provided
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <process_file> <scheduler_algo> <load> ...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("Starting the scheduling simulation...\n");

    const char *process_file = argv[1]; // Process file name
    int num_processors = (argc - 2) / 2; // Calculate the number of processors
    double *loads = malloc(num_processors * sizeof(double));
    if (!loads) {
        // Print an error message and exit if memory allocation fails
        fprintf(stderr, "Failed to allocate memory for loads\n");
        exit(EXIT_FAILURE);
    }
    int *scheduling_algorithms = malloc(num_processors * sizeof(int));
    if (!scheduling_algorithms) {
        // Print an error message, free previously allocated memory, and exit if allocation fails
        fprintf(stderr, "Failed to allocate memory for scheduling_algorithms\n");
        free(loads);
        exit(EXIT_FAILURE);
    }

    // Initialize the load balancer
    load_balancer.num_processors = num_processors;
    load_balancer.queue_sizes = malloc(num_processors * sizeof(int));
    if (!load_balancer.queue_sizes) {
        // Print an error message, free previously allocated memory, and exit if allocation fails
        fprintf(stderr, "Failed to allocate memory for queue_sizes\n");
        free(loads);
        free(scheduling_algorithms);
        exit(EXIT_FAILURE);
    }
    pthread_mutex_init(&load_balancer.mutex, NULL);

    // Read scheduling algorithms and loads from command-line arguments
    for (int i = 0; i < num_processors; i++) {
        scheduling_algorithms[i] = atoi(argv[2 + i * 2]);
        loads[i] = atof(argv[3 + i * 2]);
        load_balancer.queue_sizes[i] = 0;
    }

    PCB *processes; // Array of process control blocks
    int num_processes; // Number of processes
    int total_memory = 0; // Total memory used by processes
    int total_files = 0; // Total number of open files
    read_process_file(process_file, &processes, &num_processes, &total_memory, &total_files);

    // Print summary information about the processes
    printf("Total number of processes: %d\n", num_processes);
    printf("Total memory allocated by processes: %d bytes\n", total_memory);
    printf("Total number of open files: %d\n", total_files);

    // Allocate memory for the array of process arrays (one per processor)
    all_processes = malloc(num_processors * sizeof(PCB *));
    if (!all_processes) {
        // Print an error message, free previously allocated memory, and exit if allocation fails
        fprintf(stderr, "Failed to allocate memory for all_processes\n");
        free(loads);
        free(scheduling_algorithms);
        free(load_balancer.queue_sizes);
        free(processes);
        exit(EXIT_FAILURE);
    }

    // Initialize all_processes pointers to NULL
    for (int i = 0; i < num_processors; i++) {
        all_processes[i] = NULL;
    }

    // Assign processes to processors based on the load distribution
    assign_processes_to_processors(processes, num_processes, num_processors, loads, scheduling_algorithms, &all_processes);

    // Free memory for loads and scheduling algorithms as no longer needed
    free(loads);
    free(scheduling_algorithms);
    free(processes);

    // Wait for all threads to complete
    for (int i = 0; i < num_processors; i++) {
        pthread_join(threads[i], NULL);
    }

    // Free memory for threads and all_processes arrays
    free(threads);
    for (int i = 0; i < num_processors; i++) {
        free(all_processes[i]);
    }
    free(all_processes);

    printf("Finished scheduling simulation.\n");

    return 0;
}