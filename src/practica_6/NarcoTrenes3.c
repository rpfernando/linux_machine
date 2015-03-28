#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define CICLOS 10

char *pais[3] = { "Peru", "Bolivia", "Colombia" };

struct msgbuf {
    long mtype;       /* message type, must be > 0 */
    char mtext[1];    /* message data */
} msgp;

int msqid; // Buzon de mensajes

void proceso(int i)
{
    int k;

    for (k = 0; k < CICLOS; k++)
    {
        // Recibir mensaje
        msgrcv(msqid, &msgp, 1, 1, 0);

        // Sección crítica
        printf("Entra %s ", pais[i]);
        fflush(stdout);
        sleep(rand() % 3);
        printf("- %s Sale\n", pais[i]);
        // Sección crítica

        // Envia mensaje
        msgsnd(msqid, &msgp, 1, 0);

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

    // Declarar el buzón
    msqid = msgget(0x1234, 0666 | IPC_CREAT);
    if (msqid == -1) {
        perror("Error en la creacion del buzon\n");
        exit(1);
    }

    // Usar el canal tipo 1 para los mensajes
    msgp.mtype = 1;
    msgp.mtext[0] = 0;

    // Enviar un mensaje inicial
    if (msgsnd(msqid, &msgp, 1, 0) == -1) {
        perror("Error en el envio inicial");
        exit(2);
    }

    for (i = 0; i < 3; i++)
    {
        // Crea un nuevo proceso hijo que ejecuta la función proceso()
        pid = fork();
        if (pid == 0)
            proceso(i);
    }

    for (i = 0; i < 3; i++)
        pid = wait(&status);

    // Cerrar el buzón
    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        printf("Error en el cierre del buzon");
        exit(4);
    }
}
