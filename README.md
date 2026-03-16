# Unix-Systems-Internals-and-Concurrency
This repository contains a collection of C programs developed during my Computer Science studies, focusing on Operating Systems architecture, process synchronization, and Inter-Process Communication (IPC).

## Project Overview

### 1. Process Synchronization Sequencer
- **File:** `process_sync_sequencer_ByYUVAL.c`
- **Concepts:** System V Semaphores, `semop`, `fork`, Custom Synchronization Logic.
- **Description:** A highly synchronized multi-process system where 5 concurrent processes coordinate to print a sequence. It implements a non-trivial semaphore logic, using specific decrement (-4) and increment (+1) operations to ensure strict execution order.

### 2. Concurrent Shared Memory Summation
- **File:** `concurrent_shm_sum_ByYUVAL.c`
- **Concepts:** IPC Shared Memory (`shmget`/`shmat`), Binary Semaphores, Critical Sections.
- **Description:** Calculates the sum of integers from 1 to 100 by spawning 100 concurrent child processes. It utilizes shared memory for the total sum and semaphores to prevent race conditions during concurrent updates.

### 3. Custom PATH Shell
- **File:** `custom_path_shell_ByYUVAL.c`
- **Concepts:** Process management, `execv`, Environment Variables, `PATH` parsing.
- **Description:** A functional command-line interpreter (shell) that reads user commands and executes them by dynamically searching through the system's `PATH` directories using `strtok` and `getenv`.

## Compilation and Usage
To compile any of the programs, use the `gcc` compiler:

```bash
gcc filename.c -o output_name
./output_name
