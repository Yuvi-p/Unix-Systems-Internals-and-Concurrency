#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>

#define NUM_PROCESSES 5
#define MAX_NUMBER 100

// We'll completely remove the union semun definition and instead use inline declarations
// when needed

// Function to initialize semaphores
void init_semaphores(int sem_id, int values[]) {
    // Use direct union declaration
    union {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
    } arg;
    
    unsigned short semaphore_values[NUM_PROCESSES];
    
    for (int i = 0; i < NUM_PROCESSES; i++) {
        semaphore_values[i] = values[i];
    }
    
    arg.array = semaphore_values;
    if (semctl(sem_id, 0, SETALL, arg) == -1) {
        perror("semctl SETALL");
        exit(1);
    }
}

// Function to perform wait operation (decrement by 4)
void sem_wait_4(int sem_id, int sem_num) {
    struct sembuf sb;
    sb.sem_num = sem_num;
    sb.sem_op = -4;  // Decrement by 4
    sb.sem_flg = 0;
    
    if (semop(sem_id, &sb, 1) == -1) {
        perror("semop wait");
        exit(1);
    }
}

// Function to perform signal operation (increment by 1)
void sem_signal_1(int sem_id, int sem_num) {
    struct sembuf sb;
    sb.sem_num = sem_num;
    sb.sem_op = 1;  // Increment by 1
    sb.sem_flg = 0;
    
    if (semop(sem_id, &sb, 1) == -1) {
        perror("semop signal");
        exit(1);
    }
}

// Process function
void process_task(int process_id, int sem_id) {

    int start = process_id + 1;  // Starting number for this process
    
    for (int i = start; i <= MAX_NUMBER; i += NUM_PROCESSES) {
        // Wait on own semaphore (decrement by 4)
        sem_wait_4(sem_id, process_id);
        
        // Print the number
        printf("Process %d: %d\n", process_id + 1, i);
        fflush(stdout);
        
        // Signal all other semaphores (increment by 1)
        for (int j = 0; j < NUM_PROCESSES; j++) {
            if (j != process_id) {
                sem_signal_1(sem_id, j);
            }
        }
    }
    
    exit(0);
}

int main() {
    int sem_id;
    pid_t pid[NUM_PROCESSES];
    // Initialize the semaphores with opposite values
    int initial_values[NUM_PROCESSES] = {4, 3, 2, 1, 0};
    
    // Create set with NUM_PROCESSES semaphores
    sem_id = semget(IPC_PRIVATE, NUM_PROCESSES, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("semget");
        exit(1);
    }
    
    // Initialize semaphores
    init_semaphores(sem_id, initial_values);
    
    // Create child processes
    for (int i = 0; i < NUM_PROCESSES; i++) {
        pid[i] = fork();
        
        if (pid[i] < 0) {
            perror("fork");
            exit(1);
        } else if (pid[i] == 0) {
            process_task(i, sem_id); // Child process
        }
    }
    
    // Parent process waits for all children to finish
    for (int i = 0; i < NUM_PROCESSES; i++) {
        waitpid(pid[i], NULL, 0);
    }
    
    // Remove semaphore set
    if (semctl(sem_id, 0, IPC_RMID) == -1) {
        perror("semctl IPC_RMID");
        exit(1);
    }
    
    printf("All processes completed\n");
    
    return 0;
}
