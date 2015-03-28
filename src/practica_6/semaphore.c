#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "semaphore.h"

void waitsem(int semid)
{   
    struct sembuf sop;
    sop.sem_num = 0;
    sop.sem_op = -1;
    sop.sem_flg = 0;

    if (semop(semid, &sop, 1) == -1) 
    {
       perror("Error en semop");
       exit(2);
    }
}

void signalsem(int semid)
{   
    struct sembuf sop;
    sop.sem_num = 0;
    sop.sem_op = 1;
    sop.sem_flg = 0;

    if (semop(semid, &sop, 1) == -1) 
    {
       perror("Error en semop");
       exit(2);
    }
}

void initsem(int *semid, int counter)
{
    *semid = semget(0x1234, 1, 0666 | IPC_CREAT);
    if (*semid == -1) 
    {
        perror("Error en la creacion del buzon\n");
        exit(1);
    }

    // Inicializar en 1
    struct sembuf sop;
    sop.sem_num = 0;
    sop.sem_op = 1;
    sop.sem_flg = 0;

    if (semop(*semid, &sop, 1) == -1) {
       perror("Error en semop");
       exit(2);
    }
}

void closesem(int semid)
{
    if (semctl(semid, 0, IPC_RMID) == -1) {
        printf("Error en el cierre del semaforo");
        exit(3);
    }
}
