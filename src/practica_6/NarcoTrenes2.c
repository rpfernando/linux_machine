#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "semaphore.h"

#define CICLOS 10

char *pais[3] = { "Peru", "Bolivia", "Colombia" };

int semid; // Semaforo

void proceso(int i)
{
    int k;

    for (k = 0; k < CICLOS; k++)
    {
        // Checar el semaforo
        printf("Espera %s\n", pais[i]);
        waitsem(semid);
        printf("\tEntra %s\n", pais[i]);
        fflush(stdout);
        sleep(rand() % 3);
        printf("\t\tSale %s\n", pais[i]);

        // Señalar al semaforo
        signalsem(semid);

        // Espera aleatoria fuera de la sección crítica
        sleep(rand() % 3);
    }        

    exit(0); // Termina el proceso
}


int main()
{
    int pid;
    int status;
    int i;

    srand(getpid());

    // Incializar el contador del semáforo en 1 una vez que esté
    // en memoria compartida, de manera que solo a un proceso se le
    // permitirá entrar a la sección crítica
    initsem(&semid, 1);

    for (i = 0; i < 3; i++)
    {
        // Crea un nuevo proceso hijo que ejecuta la función proceso()
        pid = fork();
        if (pid == 0)
            proceso(i);
    }

    for (i = 0; i < 3; i++)
        pid = wait(&status);

    // Cerrar el semaforo
    closesem(semid);
}
