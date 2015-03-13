#include <stdio.h>
#include <stdlib.h>
#include "virtual_processor.h"

extern struct PROCESO proceso[];
extern struct COLAPROC listos, bloqueados;
extern int tiempo;
extern int pars[];

#define COLAS_RETROALIMENTACION 5
#define QUANTUM 1

struct COLAPROC retroalimentacion[5];
int InicializaColasRetroalimentacion = 0;
int min_priority = 0;

// =============================================================================
// ESTE ES EL SCHEDULER
// ============================================================================

void mete_a_cola_prioridad(int proceso_n) {
    printf("Metiendo proceso %d a cola %d\n", proceso_n, proceso[proceso_n].prioridad);
    if( proceso[proceso_n].prioridad >= COLAS_RETROALIMENTACION) {
        proceso[proceso_n].prioridad = COLAS_RETROALIMENTACION-1 ;
    }
    mete_a_cola(&retroalimentacion[proceso[proceso_n].prioridad], proceso_n);
}

int scheduler(int evento)
{
    int cambia_proceso = 0;
    int prox_proceso_a_ejecutar;

    if (InicializaColasRetroalimentacion == 0) {
        int i;
        InicializaColasRetroalimentacion = 1;
        for(i = 0; i < COLAS_RETROALIMENTACION; i++) {
            retroalimentacion[i].ent = 0;
            retroalimentacion[i].sal = 0;
        }
    }

    prox_proceso_a_ejecutar = pars[1]; // pars[1] = proceso en ejecución

    if (evento == PROCESO_NUEVO)
    {
        // Agregar el nuevo proceso a la cola de listos
        // pars[0] es el proceso nuevo
        proceso[pars[0]].estado = LISTO;
        if(min_priority > proceso[pars[0]].prioridad)
        {
            min_priority = proceso[pars[0]].prioridad;
        }
        mete_a_cola_prioridad(pars[0]);

        // pars[1] es el proceso en ejecución
        if (tiempo == 0) //if (pars[1] == NINGUNO)
            cambia_proceso = 1;
    }

    if (evento == TIMER)
    {
        printf("Llega interrupcion del Timer\n");
        if((tiempo + 1) % QUANTUM == 0 && pars[1] != NINGUNO)
        {
            proceso[pars[1]].prioridad++;
            mete_a_cola_prioridad(pars[1]);
            proceso[pars[1]].estado = LISTO;
        }
        cambia_proceso = 1;
    }

    if (evento == SOLICITA_E_S)
    {
        proceso[pars[1]].estado = BLOQUEADO;
        printf("Solicita E/S Proceso %d\n", pars[1]);
        cambia_proceso = 1;
    }

    if (evento == TERMINA_E_S)
    {
        // Saber cual proceso terminó E/S
        // pars0 es el proceso desbloqueado
        proceso[pars[0]].estado = LISTO;
        mete_a_cola_prioridad(pars[0]);
        // prox_proceso_a_ejecutar = pars[0];
        printf("Termina E/S Proceso desbloqueado %d\n", pars[0]);
    }

    if (evento == PROCESO_TERMINADO)
    {
        // pars0 = proceso terminado
        proceso[pars[0]].estado = TERMINADO;
        cambia_proceso = 1; // Indíca que puede poner un proceso nuevo en ejecucion
    }

    if (cambia_proceso)
    {
        // Si la cola no esta vacia obtener de la cola el siguiente proceso listo
        prox_proceso_a_ejecutar = NINGUNO;
        while(1) {
            printf("Revisando cola %d\n", min_priority);
            if (!cola_vacia(retroalimentacion[min_priority]))
            {
                prox_proceso_a_ejecutar = sacar_de_cola(&retroalimentacion[min_priority]);
                proceso[prox_proceso_a_ejecutar].estado = EJECUCION;
                cambia_proceso = 0;
                return(prox_proceso_a_ejecutar);
            }
            else
            {
                printf("No hay procesos en cola %d\n", min_priority);
                prox_proceso_a_ejecutar = NINGUNO;
            }
            min_priority ++;
            if(min_priority >= COLAS_RETROALIMENTACION) {
                min_priority %= COLAS_RETROALIMENTACION;
                return(NINGUNO);
            }
        }
    }

    return(prox_proceso_a_ejecutar);
}

// =================================================================
