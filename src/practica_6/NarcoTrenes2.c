#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "semaphore.h"

#define CICLOS 10

char *pais[3] = { "Peru", "Bolivia", "Colombia" };
struct SEMAPHORE *sem;

void proceso(int i)
{
    int k;

    for (k = 0; k < CICLOS; k++)
    {
        // Llamada waitsem implementada en la parte 3
        waitsem(sem);
        printf("Entra %s ", pais[i]);
        fflush(stdout);
        sleep(rand() % 3);
        printf("- %s Sale\n", pais[i]);

        // Llamada waitsignal implementada en la parte 3
        signalsem(sem);

        // Espera aleatoria fuera de la sección crítica
        sleep(rand() % 3);
    }

    exit(0); // Termina el proceso
}


int main()
{
    int pid;
    int status;
    int shmid;
    int i;

    // Declarar memoria compartida
    shmid = shmget(0x1234, sizeof(sem), 0666 | IPC_CREAT);
    if (shmid == -1)
    {
        perror("Error en la memoria compartida\n");
        exit(1);
    }

    sem = shmat(shmid, NULL, 0);

    if (sem == NULL)
    {
        perror("Error en el shmat\n");
        exit(2);
    }

    // Incializar el contador del semáforo en 1 una vez que esté
    // en memoria compartida, de manera que solo a un proceso se le
    // permitirá entrar a la sección crítica
    initsem(sem, 1);

    srand(getpid());

    for (i = 0; i < 3; i++)
    {
        // Crea un nuevo proceso hijo que ejecuta la función proceso()
        pid = fork();
        if (pid == 0)
            proceso(i);
    }

    for (i = 0; i < 3; i++)
        pid = wait(&status);

    // Eliminar la memoria compartida
    shmdt(sem);
}
