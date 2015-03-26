#define atomic_xchg(A,B) __asm__ __volatile__( \
    " lock xchg %1,%0 ;\n" \
    : "=ir" (A) \
    : "m" (B), "ir" (A) \
);

struct QUEUE {
    int items[20];
    int start;
    int end;
};

struct SEMAPHORE {
    int counter;
    struct QUEUE blocked_queue;
};

void enqueue(struct QUEUE *q, int process);
int isEmpty(struct QUEUE *q);
int dequeue(struct QUEUE *q);

void waitsem(struct SEMAPHORE *sem);
void signalsem(struct SEMAPHORE *sem);
void initsem(struct SEMAPHORE *sem, int counter);
