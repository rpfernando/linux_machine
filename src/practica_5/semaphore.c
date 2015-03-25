#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

void waitsem(struct SEMAPHORE *sem)
{    
    int process; = getpid();
    int l = 1;
    do { atomic_xchg(l, *g); } while(l != 0);
   
    // Critical section
    sem -> counter--;
    if (sem -> counter < 0)
    {
        // Enqueue in blocked queue
        enqueue(sem -> blocked_queue, process);

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

    do { atomic_xchg(l, *g); } while(l != 0);
   
    // Critical section
    sem -> counter++;
    if (sem -> counter <= 0)
    {
        // Get pid from blocked queue
        process = dequeue(sem -> blocked_queue);

        // Unlock process
        kill(proccess, SIGCONT);
    }

    // End of critical section
    g = 0;
}

void initsem(struct SEMAPHORE *sem, int counter)
{
    int l = 1;
    do { atomic_xchg(l, *g); } while(l != 0);
   
    // Critical section
    sem -> counter = counter;

    // End of critical section
    g = 0;
}
