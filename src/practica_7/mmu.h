// Definición de constantes

#define NINGUNO -1

#define PAGESIZE 4096
#define FRAMESIZE PAGESIZE
#define PHYSICALMEMORYSIZE 32*1024
#define SYSTEMFRAMETABLESIZE PHYSICALMEMORYSIZE/PAGESIZE
#define TOTFRAMES SYSTEMFRAMETABLESIZE
#define MAXPROC 4
#define PROCESSPAGETABLESIZE 2*SYSTEMFRAMETABLESIZE/MAXPROC 
#define TABLESSIZE 2*SYSTEMFRAMETABLESIZE*sizeof(struct SYSTEMFRAMETABLE)
#define FRAMESPROC SYSTEMFRAMETABLESIZE / MAXPROC

// Definición de estructuras

struct SYSTEMFRAMETABLE {
    int assigned;

    char *paddress; // No modificar
    int shmidframe;  // No modificar
};

struct PROCESSPAGETABLE {
    int presente;
    int modificado;
    int framenumber;
    unsigned long tarrived;
    unsigned long tlastaccess;

    int attached;   // No modificar
};


