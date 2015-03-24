#define atomic_xchg(A,B) __asm__ __volatile__( \
    " lock xchg %1,%0 ;\n" \
    : "=ir" (A) \
    : "m" (B), "ir" (A) \
);

struct SEMAPHORE {
    int counter;
    struct QUEUE blocked_queue;
};

struct QUEUE {
    int queue[20];
    int start;
    int end;
};

void enqueue(struct QUEUE *q, int process);
int isEmpty(struct QUEUE q);
int dequeue(struct QUEUE *q);

void waitsem(struct SEMAPHORE *sem, int pid);
void signalsem(struct SEMAPHORE *sem, int pid);
void initsem(struct SEMAPHORE *sem, int counter);
