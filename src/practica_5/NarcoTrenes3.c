#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "semaphore.h"

#define CICLOS 10

char *pais[3] = { "Peru", "Bolvia", "Colombia" };
int *g;

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
    // Incializar el contador del semáforo en 1 una vez que esté
    // en memoria compartida, de manera que solo a un proceso se le
    // permitirá entrar a la sección crítica
    initsem(sem,1);
    
    //TODO
}
