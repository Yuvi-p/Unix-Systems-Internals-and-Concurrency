#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>

#define NUM_PROCESSES 100
#define SHM_KEY 1234
#define SEM_KEY 5678

// function to initialize semaphore
int init_semaphore(int sem_id) {
    union {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
    } argument;
    
    argument.val = 1;  // initial value of semaphore => available
    return semctl(sem_id, 0, SETVAL, argument);
}

// function to perform semaphore wait operation
void sem_wait(int sem_id) {
    struct sembuf operation;
    operation.sem_num = 0;
    operation.sem_op = -1;  // decrement by 1
    operation.sem_flg = 0;
    
    if (semop(sem_id, &operation, 1) == -1) {
        perror("semop wait failed");
        exit(1);
    }
}

// function to perform semaphore signal operation
void sem_signal(int sem_id) {
    struct sembuf operation;
    operation.sem_num = 0;
    operation.sem_op = 1;  // increment by 1
    operation.sem_flg = 0;
    
    if (semop(sem_id, &operation, 1) == -1) {
        perror("semop signal failed");
        exit(1);
    }
}

int main() {
    int i;
    pid_t pid;
    int shmid, semid;
    int *shared_sum;
    int status;
    int total_processes = 0;
    
    // create shared memory segment
    shmid = shmget(SHM_KEY, sizeof(int), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }
    
    // attach shared memory segment
    shared_sum = (int *) shmat(shmid, NULL, 0);
    if (shared_sum == (int *) -1) {
        perror("shmat failed");
        exit(1);
    }
    
    // initialize shared memory
    *shared_sum = 0;
    
    // creates semaphore
    semid = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget failed");
        exit(1);
    }
    
    // initialize semaphore
    if (init_semaphore(semid) == -1) {
        perror("semaphore initialization failed");
        exit(1);
    }
    
    printf("Starting %d processes to calculate sum from 1 to %d\n", NUM_PROCESSES, NUM_PROCESSES);
    
    // create child processes
    for (i = 1; i <= NUM_PROCESSES; i++) {
        pid = fork();
        
        if (pid < 0) {
            // the fork failed
            perror("fork failed");
            exit(1);
        } 
        else if (pid == 0) { // child process
            sem_wait(semid);

            // add number to shared sum
            *shared_sum += i;
            
            // print within the section to ensure the order
            printf("Process %d adding number %d to sum\n", getpid(), i);
            
            // exit section
            sem_signal(semid);
            exit(0);
        } else {
            // parent process
            total_processes++;
        }
    }
    
    // parent waits for all child processes to finish
    for (i = 0; i < total_processes; i++) {
        wait(&status);
    }
    
    // prints result
    printf("The sum of numbers from 1 to %d is: %d\n", NUM_PROCESSES, *shared_sum);
    
    //  result using formula of sum
    int theoretical_sum = NUM_PROCESSES * (NUM_PROCESSES + 1) / 2;
    printf("Theoretical sum: %d\n", theoretical_sum);
    printf("Results match: %s\n", (*shared_sum == theoretical_sum) ? "Yes" : "No");
    
    // detach shared memory
    if (shmdt(shared_sum) == -1) {
        perror("shmdt failed");
    }
    
    // remove shared memory
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl failed");
    }
    
    // remove semaphore
    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror("semctl failed");
    }
    
    return 0;
}