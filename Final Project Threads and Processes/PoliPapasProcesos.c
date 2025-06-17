// Proyecto IB de Sistemas Operativos
// GRUPO F - GR2SW_2025A - EPN

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <time.h>

//Definitions of constants
//There will be 5 cashiers that represents 5 new processes
//Meanwhile, the maximum value of transactions a cashier can make is 500000
#define CAJERAS 5
#define TRANSACCIONES_TOTALES 500000

//Global variables
int *saldo; //This is a pointer to balance which will be set to 1000 at the beginning
int *total_transacciones; //This variable points to a integer that saves how many transactions have been made
sem_t *mutex; //Mutex or semaphore declared to control access to global variables between processes

//This function gets the time in seconds and nanoseconds since the 
//CLOCK_MONOTONIC started counting and saves it as a double
double obtener_tiempo() {
    struct timespec ts;
    //This line gets how much time has passes since this system started running
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

//This function processes every transaction made in a process, it receives the 
//identifier of each process
void procesar_transacciones() {

    //The results of every process will be saved in "REPORTE_PROCESOS.text"
    FILE *fptr = fopen("REPORTE_PROCESOS.txt", "a");
    //Action in case of error
    if (!fptr) {
        perror("fopen proceso");
        exit(1);
    }

    //Every cashier can make up to 100000 transactions
    int transacciones_por_cajera = TRANSACCIONES_TOTALES / CAJERAS;

    //Loop for every transaction a cashier makes
    for (int i = 0; i < transacciones_por_cajera; i++) {
        //The process wait for the global variables to be unlocked with a sem_post
        sem_wait(mutex);
        //If the process makes more than 100000 transactions, it stops and unlock the global variables
        if ((*total_transacciones) >= TRANSACCIONES_TOTALES) {
            sem_post(mutex);
            break;
        }

        //If there has been made an even number of transactions, 
        //8 is substracted from the balance of the restaurant
        if ((*total_transacciones) % 2 == 0) {
            if (*saldo >= 8) {
                *saldo -= 8;
            }
        } else {
            //Otherwise, 15 is added to the balance
            *saldo += 15;
        }

        //The ammount of transactions is increased by 1 and the global variables are unlocked
        (*total_transacciones)++;
        sem_post(mutex);
    } 

    //After the cashier has made all the transactions, it writes its final balance in the final document
    fprintf(fptr, "Cajera %d, saldo: %d \n", getpid(), *saldo);
    fclose(fptr);
}

int main() {
    //Every global variable needs a space in memory for it to exist and be accessed by every process (cashier)
    //NULL: the OS decides what is the direction of this memory space
    //sizeof(data type): The OS has to reserve a space corresponding to the size of the variable
    //PROT_READ | PROT_WRITE: Every process can read and write into this memory space
    //MAP_SHARED | MAP_ANONYMOUS: Every process shares this same memory space
    saldo = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                 MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    total_transacciones = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                              MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    mutex = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,
                 MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *saldo = 1000; //The balance of the restaurant is initialized with 1000
    *total_transacciones = 0;
    sem_init(mutex, 1, 1); //The semaphore is initialized as 1 for all the processes

    //The final document is opened to save the results of execution
    FILE *fptr = fopen("REPORTE_PROCESOS.txt", "w");
    //Action in case of error
    if (!fptr) {
        perror("fopen");
        return 1;
    }

    //Header of the final document
    fprintf(fptr, "Movimientos de Caja en el Restaurante PoliPapas durante los últimos 3 anios\n");
    fclose(fptr);

    //The execution start time is saved here
    double inicio = obtener_tiempo();

    //A child process for every cashier is created
    for (int i = 0; i < CAJERAS; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            //Every child process, a cashier executes "procesar_transacciones()"
            procesar_transacciones();
            //Then every child eliminates its addressability to the memory spaces 
            //reserved for the global variables
            munmap(saldo, sizeof(int));
            munmap(total_transacciones, sizeof(int));
            munmap(mutex, sizeof(sem_t));
            //Every child process finishes
            exit(0);
        }
    }

    //The parent process waits for all the child processes to finish to guarantee synchronization
    for (int i = 0; i < CAJERAS; i++) {
        wait(NULL);
    }

    //The execution end time is saved here
    double fin = obtener_tiempo();

    //The program reopens the final document to save the results of execution
    fptr = fopen("REPORTE_PROCESOS.txt", "a");
    if (!fptr) {
        perror("fopen final");
        return 1;
    }

    //Finally, the final balance value after the execution, the execution time, the 
    //number of threads and the number of transactions are printed into the text file
    fprintf(fptr, "Total de cajeras: %d\n", CAJERAS);
    fprintf(fptr, "Saldo final: %d\n", *saldo);
    fprintf(fptr, "\nTotal de transacciones: %d\n", TRANSACCIONES_TOTALES);
    fprintf(fptr, "Tiempo total de ejecucion: %.6f segundos\n", fin - inicio);
    fclose(fptr);

    //Finally, the father process eliminates the semaphore created 
    //it also eliminates the addressability to the memory spaces reserved for the global variables
    sem_destroy(mutex);
    munmap(saldo, sizeof(int));
    munmap(total_transacciones, sizeof(int));
    munmap(mutex, sizeof(sem_t));

    return 0;
}
