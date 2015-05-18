#include "inode.h"

// Check if I Node is free
int isINodeFree(int inode)
{
    int offset = inode / 8;
    int shift = inode % 8;

    if (checkINodesMap() == ERROR)
        return ERROR;

    if (iNodesMap[offset] & (1 << shift))
        return NO;

    return YES;
}

// Search for a free I Node
int nextFreeINode()
{
    int i, j;

    if (checkINodesMap() == ERROR)
        return ERROR;

    // Check byte by byte
    for (i = 0; iNodesMap[i] == 0xFF && i < 4; i++);

    if (i < 4)
    {
        // Check bit by bit
        for (j = 0; iNodesMap[i] & (1 << j) && j < 8; j++);
        return (i * 8) + j;
    }

    return ERROR;
}

// Assign given I Node
int assignINode(int inode)
{
    int offset = inode / 8;
    int shift = inode % 8;

    if (checkINodesMap() == ERROR)
        return ERROR;

    // Mark I Node as not free
    iNodesMap[offset] |= (1 << shift);
    if (vdwritesl(0, getINodesMap(), 1, iNodesMap) == ERROR)
        return ERROR;

    return SUCCESS;
}

// Unassign given I Node
int unassignINode(int inode)
{
    int offset = inode / 8;
    int shift = inode % 8;

    if (checkINodesMap() == ERROR)
        return ERROR;

    // Mark I Node as free
    iNodesMap[offset] &= (char) ~(1 << shift);
    if (vdwritesl(0, getINodesMap(), 1, iNodesMap) == ERROR)
        return ERROR;

    return SUCCESS;
}

// Funciones básicas para el manejo de i-nodos
// Escribe en el nodo i indicado en num del directorio raíz
// (tabla de nodos i), los datos:
//  filename (nombre del archivo)
//  atribs  (permisos)
//  uid (id del usuario dueño del archive)
//  gid (id del grupo al que pertenece el usuario dueño
// Esta función va a ser utilizada para implementar la llamada vdcreat
int setninode(int num, char *filename,unsigned short atribs, int uid, int gid)
{
    int i;
    int inicio_nodos_i = getINodeTable();
    int result;

    checkSecBoot();
    checkRootDir();


    strncpy(rootDir[num].name, filename, 20);

    if(strlen(rootDir[num].name) > 19)
        rootDir[num].name[19] = '\0';

    rootDir[num].datetimecreate = currDateTime2Int();
    rootDir[num].datetimemodif = currDateTime2Int();
    rootDir[num].uid = uid;
    rootDir[num].gid = gid;
    rootDir[num].perms = atribs;
    rootDir[num].size = 0;

    for(i = 0; i < 10; i++)
        rootDir[num].blocks[i] = 0;

    rootDir[num].indirect = 0;
    rootDir[num].indirect2 = 0;

    // Optimizar la escritura escribiendo solo el sector lógico que
    // corresponde al inodo que estamos asignando.
    // i=num/8;
    // result=vdwritesl(0, inicio_nodos_i+i, 1,&rootDir[i*8]);
    for(i = 0; i < secBoot.sec_tabla_nodos_i; i++)
        result=vdwritesl(0, inicio_nodos_i+i, 1, (char *) &rootDir[i*8]);

    return num;
}

// Buscar un archivo por nombre en la tabla de nodos i
// Devuelve la posición del archivo en la tabla de nodos i
// Si no se encuentra devuelve -1
int searchinode(char *filename)
{
    int i;
    int inicio_nodos_i = getINodeTable();
    int result;

    checkSecBoot();
    checkRootDir();

    if(strlen(filename) > 19) filename[19]='\0';


    for(i=0; strcmp(rootDir[i].name,filename) && i < 32; i++);

    if(i >= 32)
        return -1;
    else
        return i;
}

// Borrar un inodo de la tabla de nodos i
// Se va a utilizar para borrar un archivo
int removeinode(int numinode)
{
    int i;

    unsigned short temp[PTRxBLOCK];

    // Recorrer los apuntadores directos del inodo
    // Poner en 0 su bit correspondiente en el mapa
    // de bits
    for(i = 0; i < 10; i++) {
        if(rootDir[numinode].blocks[i] != 0) {
            unassignBlock(rootDir[numinode].blocks[i]);
        }
    }

    // Recorrer los apuntadores indirectos
    if(rootDir[numinode].indirect!=0)
    {
        // Leer el bloque indirecto
        // el que contiene los apuntadores después del 10
        readBlock(rootDir[numinode].indirect,(char *) temp);

        // Recorrer todos los apuntadores y poner en 0
        // el bit correspondiente en el mapa de bits
        for(i = 0; i < PTRxBLOCK; i++) {
            if(temp[i] != 0) {
                unassignBlock(temp[i]);
            }
        }

        // Desasignar el bloque que contiene los
        // apuntadores.
        unassignBlock(rootDir[numinode].indirect);
        rootDir[numinode].indirect = 0;
    }

    // Desasignar el inodo
    unassignINode(numinode);
    return 1;
}
