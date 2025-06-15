# Threads-vs-Processes
## First Bimenster Project for Operating Systems Class

## Sales Management for PoliPapas Restaurant

This is the first semester Operating Systems project for Group 2, parallel GR2SW, Semester 2025A - EPN. The project consists of two programs, both calculating 500,000 transactions in a sales system for PoliPapas Restaurant. The first program performs the calculation using threads that run in parallel; the second uses processes that run simultaneously. The program code is C.

#### How to execute

Please run the command `./start.sh` to run the script that will execute both programs at the same time and open both final text files. Run the command `chmod +x start.sh ` before executing the script if there is an execution permission issue. 

#### Members

- Bryan Ayala
- Emilio Camino
- Juan Chugá
- Jeimy Sánchez

### Study Case

PoliPapas Restaurant, a branch of PoliBurguer, offers a variety of combinations of fries for its customers from the EPN, mostly college students. Due to how quick their orders are, the restaurant needs to know what is the most efficient way to collect payments. Based on that idea, the restaurant has asked the members of group F to create a solution that calculates a large number of transactions, similar to the number they receive each day. The restaurant specifies that the solution should somehow emulate the collection of payments directly from the waiter who serves customers at each table orders, but at the same time it should emulate collection from cashiers. This way, it should be possible to observe which collection method is more efficient.

### Procedure

To meet the requirement, each case was transformed into a separate program. The first program calculates transactions through parallel threads, while the second does so through processes. This way, a difference can be seen between the collection of waiters and cashiers. Both programs follow the same conditions and logic.

    1. The restaurant's balance starts at $1,000.
    2. Each case has 5 waiters (threads) or 5 cashiers (processes) available.
    3. Each program simulates 500,000 transactions, both income and expenses.
    4. When the number of transactions is an even number, $8 is subtracted from the restaurant's balance.
    5. When the number of transactions is an odd number, $15 is added to the restaurant's balance.
    6. Both programs terminate when all transactions have been completed.
    7. Both programs record the final balances of each waiter or cashier in a text file along with the final execution time.

Both programs use a function to determine the start and end times of the program's execution; this function is called `obtener_tiempo()`. This function also uses the `clock_gettime()` function from the `<time.h>` library, which obtains the time at which the program is executed based on how much time has passed since the system was powered on. At the end of execution, the execution time interval is found.

In addition, both programs use the `FILE` data type to generate text files. Both programs also print the total number of transactions, the execution time, and the restaurant's total balance.

#### Program Using Threads

Briefly, this program uses a mutex to restrict each thread's access to the global variables, `saldo`, `indice_actual` (which stores the number of transactions made), and an array of integers that store the subtotal amount of money handled by each thread during its execution based on its index. For this purpose, the `pthread_mutex_lock()` and `pthread_mutex_unlock()` functions were used, which lock and unlock access to the critical memory section after each transaction. To create multiple simultaneous threads, the `<pthread.h>` library was used along with the `pthread_create()` and `pthread_join()` functions. The first function is responsible for starting the execution of each thread, while the second makes the main program wait for each thread to finish its execution before continuing. Finally, the program progressively adds up all the balances for each waiter and writes each result to a text file called `REPORTE_HILOS.txt`.

#### Program that uses processes

Similarly, this program uses a semaphore to restrict each thread's access to the global variables `saldo` and `total_transacciones`. To do this, the `<semaphore.h>` library was used, creating a semaphore initialized with a value of 1. Additionally, the `sem_wait()` and `sem_post()` functions were used, which cause the programs that invoke them to wait for access to critical sections of the code. To create multiple simultaneous processes, the `fork()` function was used along with the `wait(NULL)` function. The latter is responsible for ensuring the final synchronization of the processes by waiting for the last child process to finish executing in order to continue. When a process finishes calculating transactions, it prints its latest balance value to a text file called `REPORTE_PROCESOS.txt`, to which more data will be added at the end of the execution. Each process can perform a maximum of 100,000 transactions.

Unlike threads, processes require a shared memory mapping to store global variables between them. To create this, the `<sys/mman.h>` library and the `mmap()` and `munmap()` functions were used, which respectively allocate or deallocate these memory spaces.

## Conclusions

Performance tests were attempted on different virtual machines and the following results were obtained. The following table illustrates the execution times in seconds for various tests.

<div align="center">
  
| Threads | Processes |
|---------------|---------------|
| 0.031925 | 0.047390 |
| 0.249066 | 0.043174 |
| 0.234231 | 0.785658 |
| 0.013787 | 0.049326 |
| 0.364921 | 0.652178 |

</div>

Execution time differences may be due to differences in hardware or resources provided to the system, in addition to the scheduling of processes and threads by the operating system. However, a trend can be observed that indicates greater efficiency for programs using threads. Analogously to the study case, this means that charging food purchases directly from the table through a waiter is much more efficient than going to a checkout to pay.

The reason why threads are more efficient in solving the study case may be due to the fact that threads automatically reserve shared memory space for their execution, while processes do not, and this memory space must be assigned manually, which can lead to a slight execution delay. Furthermore, creating processes takes more time due to the operating system loading the context or data required for scheduling each process, which may take longer to switch execution between processes than between threads.

#### Synchronization Problems

At the beginning of the implementation, the shared memory space between processes was not being allocated correctly, so each process could not access global variables. It should be noted that the `fork()` function creates a process from the point in the code where it is invoked. Therefore, it was decided to implement manual memory mapping that includes the `saldo` and `total_transacciones` variables, in addition to the semaphore. The latter, along with the `wait(NULL)` function, are the strongest program synchronization mechanisms, as they prevent conflicts when accessing the created memory mappings and ensure that program execution continues at the same time when all child processes terminate. Removing either of these two functions would cause calculation errors, false readings, or incorrect execution, which could cause the loss of information from each process.

The printing of results in threads also had to be synchronized. Therefore, it was decided to first save the final balance of each thread in a global array and then print these values ​​to the resulting file in a controlled manner. This avoided problems with opening and closing files or the creation of too many unnecessary instructions.

### Threads vs. Processes

The use of threads is more efficient when the tasks performed by a program are dependent on each other or, as demonstrated in this project, require faster or parallel communication. Processes can be used for tasks that do not require communication or require low communication security, but in favor of high security for the data they handle. That is, since threads use shared memory by default, unforeseen synchronization errors can arise that are not as urgent with processes that work independently.

Finally, the presented project demonstrated the superior efficiency between creating thread in a program, without detracting from the additional benefits and disadvantages of using processes instead. It also shows some synchronization techniques used to ensure the safety of the calculations. 
