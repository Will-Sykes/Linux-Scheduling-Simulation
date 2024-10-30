**Scheduling Simulation Project**

**Overview**

This project is a scheduling simulation for operating systems that manages processes and simulates their execution across multiple processors. The simulation supports various scheduling algorithms and implements a load-balancing mechanism to distribute the processes among processors.

**Features**

Multithreaded Simulation: Utilizes POSIX threads (pthreads) to simulate multiple processors running concurrently.

Scheduling Algorithms: Supports different scheduling algorithms, such as priority scheduling and round-robin.

Process Control Block (PCB): Simulates real-world processes with detailed metadata like CPU burst time, priority, base and limit registers, and process type.

Load Balancer: Distributes processes across multiple processors to balance the workload dynamically.

**Files**

main.c: The entry point of the scheduling simulation. It initializes the load balancer, reads processes from a file, assigns them to processors, and starts the simulation.

schedule.c: Contains the core CPU simulation logic, including the implementation of the scheduling algorithms and multithreading.

schedule.h: Defines the data structures and function prototypes used throughout the project, including the PCB structure, processor arguments, and the load balancer.

PCB.bin: A binary file containing the details of the processes to be scheduled. It stores an array of Process Control Blocks (PCBs) for the simulation.

**How to Run**

Compile the Program:

Ensure you have gcc installed and available in your environment.

Compile the program using the following command:
bash
gcc -pthread main.c schedule.c -o scheduling_simulation

Run the Simulation:

The program requires a process file, scheduler algorithm, and other parameters to run. For example:

bash

./scheduling_simulation processes.txt priority 2

Here, processes.txt is the file containing process information, priority is the scheduling algorithm, and 2 indicates the number of processors.
Process Binary File (PCB.bin):

The PCB.bin file is read by the simulation to initialize the processes with their associated attributes (priority, CPU burst time, etc.).

**Data Structures**

Process Control Block (PCB):

Represents a process with attributes such as process_id, priority, cpu_burst_time, base_register, limit_register, and more.

LoadBalancer:

A shared structure that manages the load distribution among processors and uses mutexes to ensure thread-safe operations.

**Scheduling Algorithms Supported**

Priority Scheduling: Processes are scheduled based on their priority. The highest-priority process gets executed first.

Round-Robin Scheduling: Each process is assigned a time slice (quantum), and the scheduler rotates through the processes in the queue.

**Future Improvements**

Implement additional scheduling algorithms, such as Shortest Job First (SJF) or Multilevel Queue Scheduling.

Add more detailed process state transitions (e.g., I/O waiting, blocked, etc.).

Improve the visualization of process execution on each processor.

**Notes**
Ensure that the PCB.bin file is properly formatted and accessible when running the simulation, as it contains crucial process metadata.
