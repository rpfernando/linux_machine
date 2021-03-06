#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "semaphore.h"

int g = 0;

void enqueue(struct QUEUE *q, int process)
{
    q -> items[q -> start] = process;
    q -> start++;

    if (q -> start > 19)
        q -> start = 0;
}

int isEmpty(struct QUEUE *q)
{
    return((q -> start == q -> end)? 1 : 0);
}

int dequeue(struct QUEUE *q)
{
    int process;
    
    process = q -> items[q -> end];
    q -> end++;

    if (q -> end > 19)
        q -> end = 0;

    return(process);
}

void waitsem(struct SEMAPHORE *sem)
{    
    int process = getpid();
    int l = 1;
    do { atomic_xchg(l, g); } while(l != 0);
   
    // Critical section
    sem -> counter--;
    if (sem -> counter < 0)
    {
        // Enqueue in blocked queue
        enqueue(&(sem -> blocked_queue), process);

        // Lock process
        kill(process, SIGSTOP);
    }

    // End of critical section
    g = 0;

}

void signalsem(struct SEMAPHORE *sem)
{  
    int process;
    int l = 1;

    do { atomic_xchg(l, g); } while(l != 0);
   
    // Critical section
    sem -> counter++;
    if (sem -> counter <= 0)
    {
        // Get pid from blocked queue
        process = dequeue(&(sem -> blocked_queue));

        // Unlock process
        kill(process, SIGCONT);
    }

    // End of critical section
    g = 0;
}

void initsem(struct SEMAPHORE *sem, int counter)
{
    int l = 1;
    do { atomic_xchg(l, g); } while(l != 0);
   
    // Critical section
    sem -> counter = counter;
    sem -> blocked_queue.start = 0;
    sem -> blocked_queue.end = 0;
    // End of critical section
    g = 0;
}
