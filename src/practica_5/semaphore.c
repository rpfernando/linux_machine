#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

void enqueue(struct QUEUE *q, int process)
{
    q -> queue[q -> start] = process;
    q -> start++;

    if (q -> start > 19)
        q -> start = 0;
}

int isEmpty(struct QUEUE q)
{
    return((q.in == q.out)? 1 : 0);
}

int dequeue(struct QUEUE *q)
{
    int process;
    
    process = q -> queue[q -> end];
    q -> end++;

    if (q -> end > 19)
        q -> end = 0;

    return(process);
}

void waitsem(struct SEMAPHORE *sem, int pid)
{
    //TODO
}

void signalsem(struct SEMAPHORE *sem, int pid)
{
    //TODO
}

void initsem(struct SEMAPHORE *sem, int counter)
{
    sem -> counter = counter;
}
