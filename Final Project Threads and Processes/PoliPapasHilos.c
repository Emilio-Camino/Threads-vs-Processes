// Proyecto IB de Sistemas Operativos
// GRUPO F - GR2SW_2025A - EPN

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

//Definitions of constants
//There will be 5 waiters that represents 5 threads
//Meanwhile, the maximum value of transactions a waiter can make is 500000 
#define MESERO 5
#define TRANSACCIONES_TOTALES 500000

//Global variables
int saldo = 1000;  //The restaurant balance is set to 1000 at the beginning
int indice_actual = 0;  //This variable saves how many transactions have been made
int aportes[MESERO] = {0};  //Every thread has an element in this array for it to save their subtotal

//Mutex initialized to control access to global variables between threads 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//This function gets the time in seconds and nanoseconds since the 
//CLOCK_MONOTONIC started counting and saves it as a double
double obtener_tiempo() {
    struct timespec ts;
     //This line gets how much time has passes since this system started running
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

//This function processes every transaction made in a thread, it receives a generic
// pointer and returns another one
void *procesar_transacciones(void *arg) {
    //The generic pointer is converted to an integer, and the value that is 
    //being pointed is set to the index variable
    int index = *(int *)arg;  //This index represent the thread that is making
                              //the transaction
    
    int subtotal = 0; //Every waiter subtotal starts with 0

    while (1) {
        //The thread needs the global variable, so the mutex is unlocked to this thread
        pthread_mutex_lock(&mutex);

        //The thread stops and unlocks the mutex when the maximum number of transactions is reached
        if (indice_actual >= TRANSACCIONES_TOTALES) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        //If there has been made an even number of transactions, 
        //8 is substracted from the balance of the restaurant and the thread's subtotal
        if (indice_actual % 2 == 0) {
            if (saldo >= 8) {
                saldo -= 8;
                subtotal -= 8;
            }
        //Otherwise, 15 is added to both variables
        } else {
            saldo += 15;
            subtotal += 15;
        }

        //The number of transactions is increased by 1
        indice_actual++;
        //The mutex is unlocked for now so other threads can access the global variables
        pthread_mutex_unlock(&mutex);
    }

    //The thread saves the final ammount of money after reaching the max transactions
    aportes[index] = subtotal;  
    return NULL;
}

//Main function of the program, it creates the threads, waits for them 
//to finish and document their results in a text file
int main() {

    //The results of every thread will be saved in "REPORTE_HILOS.text"
    FILE *fptr = fopen("REPORTE_HILOS.txt", "w");
    //Action in case of error
    if (!fptr) {
        perror("Error al abrir el archivo");
        return 1;
    }

    //Header of the final text file
    fprintf(fptr, "Movimientos de Caja en el Restaurante PoliPapas durante los últimos 3 anios\n");

    //An array of 5 threads is created for every waiter
    pthread_t hilos[MESERO];

    //Ar array of integers representing every thread: used to save the subtotal of every thread correctly
    int indices[MESERO];

    //The execution start time is saved here
    double inicio = obtener_tiempo();

    //Creation of every thread
    for (int i = 0; i < MESERO; i++) {
        indices[i] = i;  //In case of error, the index of the thread is saved
        //Every thread will execute the "procesar_transacciones"
        //function and save their subtotal into the array "aportes" using their index
        pthread_create(&hilos[i], NULL, procesar_transacciones, &indices[i]);
    }
    
    //The program waits for every thread to finish before continuing
    for (int i = 0; i < MESERO; i++) {
        pthread_join(hilos[i], NULL);
    }

    //The final balance after every thread execution is printed into the text file
    int saldoPorMesero = 0;
    for (int i = 0; i < MESERO; i++) {
        fprintf(fptr, "Mesero %d, saldo hasta el momento: %d\n", i + 1, aportes[i] + saldoPorMesero);
        saldoPorMesero += aportes[i];
    }

    //The execution end time is saved here
    double fin = obtener_tiempo();

    //Finally, the final balance value after the execution, the execution time, the 
    //number of threads and the number of transactions are printed into the text file
    fprintf(fptr, "Numero de meseros: %d\n", MESERO);
    fprintf(fptr, "Saldo total: %d\n", saldo);
    fprintf(fptr, "\nTotal de transacciones: %d\n", TRANSACCIONES_TOTALES);
    fprintf(fptr, "Tiempo de ejecucion: %.6f segundos\n", fin - inicio);

    //The program closes the text file, and destroy the mutex which is no longer needed
    fclose(fptr);
    pthread_mutex_destroy(&mutex);
    return 0;
}

