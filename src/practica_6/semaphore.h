#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

void waitsem(int semid);
void signalsem(int semid);
void initsem(int *semid, int counter);
void closesem(int semid);
