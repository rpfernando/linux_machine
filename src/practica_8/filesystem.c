#include "filesystem.h"

unsigned short *postoptr(int fd, int pos);
unsigned short *currpostoptr(int fd);

// Create a new file (file remains open)
int vdcreat(char *filename, unsigned short mode)
{
    int numinode;
    int i;

    // Ver si ya existe el archivo
    numinode = searchinode(filename);

    // Si no existe
    if(numinode == -1)
    {
        // Buscar un inodo en blanco en el mapa de bits (nodos i)
        numinode = nextFreeINode();
        if(numinode == -1)
        {
            return -1; // No hay espacio para más archivos
        }
    } else { // Si ya existe, elimina el inodo
        removeinode(numinode);
    }


    // Escribir el archivo en el inodo encontrado
    setninode(numinode, filename, 0, getuid(), getgid());
    assignINode(numinode);

    // Poner el archivo en la tabla de archivos abiertos

    checkOpenFiles();

    // Buscar si hay lugar en la tabla de archivos abiertos
    // Si no hay lugar, regresa -1
    for(i = 3; openfiles[i].inUse && i < NOPENFILES; i++);

    // Si no hay lugar en la tabla de archivos para
    // el que queremos abrir, regresamos error (-1)
    if(i >= NOPENFILES) return -1;

    // Si hay lugar, vamos a poner la entrada en uso
    // Ponemos en la tabla el inodo que corresponde al
    // archivo
    // Poner la posición actual del archivo en 0.
    openfiles[i].inUse = 1;
    openfiles[i].iNode = numinode;
    openfiles[i].currPos = 0;
    return i;
}

// Open an existing file
int vdopen(char *filename, unsigned short mode)
{
    int numinode;
    int i;
    unsigned short currblock;


    // Ver si ya existe el archivo
    // Si no existe regresa con un error
    numinode = searchinode(filename);
    if(numinode == -1) return(-1);

    checkOpenFiles();

    // Buscar si hay lugar en la tabla de archivos abiertos
    // Si no hay lugar, regresa -1
    for(i = 3; openfiles[i].inUse && i < NOPENFILES; i++);
    if(i >= NOPENFILES) return -1;

    openfiles[i].inUse = 1;
    openfiles[i].iNode = numinode;
    openfiles[i].currPos = 0;

    // Si hay apuntador indirecto, leerlo en el buffer
    if(rootDir[numinode].indirect != 0) {
        readBlock(rootDir[numinode].indirect,(char *) openfiles[i].indirectBuff);
    }

    // Cargar el buffer con el bloque actual del archivo (primer bloque)
    currblock = *currpostoptr(i);
    readBlock(currblock, openfiles[i].buffer);
    return i;
}

// Deletes a file
int vdunlink(char *filename)
{
    int numinode;
    int i;

    // Busca el inodo del archivo
    numinode = searchinode(filename);
    if(numinode == -1) return -1; // No existe

    removeinode(numinode);
    setninode(numinode, "", 0, getuid(), getgid());
    removeinode(numinode);
    return 0;
}

// Changes position within the file
int vdseek(int fd, int offset, int whence)
{
    unsigned short oldblock, newblock;

    // Si no está abierto regresa error
    if(openfiles[fd].inUse == 0)
        return -1;

    oldblock = *currpostoptr(fd);

    if(whence == WHENCE_BEG) // A partir del inicio
    {
        // Si el desplazamiento es desde el inicio
        // el offset no puede ser negativo
        // el offset no debe exceder el tamaño del archivo
        if(offset < 0 ||
            openfiles[fd].currPos + offset > rootDir[openfiles[fd].iNode].size)
            return -1;
        // Poner el cursor con su nuevo valor
        openfiles[fd].currPos = offset;

    }
    else if(whence == WHENCE_CUR) // A partir de la posición actual
    {
        // Si me muevo hacia atrás no llegue hasta antes
        // del inicio del archivp
        // Si me muevo hacia adelante, no pasar el final
        // del archivo.
        if(openfiles[fd].currPos + offset > rootDir[openfiles[fd].iNode].size ||
            openfiles[fd].currPos + offset < 0)
            return -1;

        // Poner el cursor con su nuevo valor
        openfiles[fd].currPos += offset;

    }
    else if(whence == WHENCE_END) // A partir del final
    {
        // Si me muevo hacia atrás no llegue hasta antes
        // del inicio del archivp
        // Si me muevo hacia adelante, no pasar el final
        // del archivo.

        if(offset > rootDir[openfiles[fd].iNode].size ||
            openfiles[fd].currPos-offset < 0)
            return -1;

        // Poner el cursor con su nuevo valor
        openfiles[fd].currPos = rootDir[openfiles[fd].iNode].size - offset;
    } else {
        return -1;
    }

    newblock = *currpostoptr(fd);

    if(newblock != oldblock)
    {
        writeBlock(oldblock, openfiles[fd].buffer);
        readBlock(newblock, openfiles[fd].buffer);
        openfiles[fd].currBlockInMemory = newblock;
    }

    return openfiles[fd].currPos;
}

// Read from an open file
int vdread(int fd, char *buffer, int size)
{
    int currblock;
    int currinode;
    int cont = 0;
    int i;
    unsigned short *currptr;

    // Si no está abierto, regresa error
    if(openfiles[fd].inUse == 0) return -1;
    currinode = openfiles[fd].iNode;

    // Cada iteración del ciclo copia un caracter
    // de la entrada buffer al buffer del bloque
    while(cont < size)
    {
        // Obtener la dirección de donde está el bloque que corresponde
        // a la posición actual
        currptr = currpostoptr(fd);
        if(currptr == NULL)
            return -1;

        currblock = *currptr;

        // Si el bloque está en blanco, dale uno
        if(currblock == 0)
        {
            return cont;
        }

        // Si el bloque de la posición actual no está en memoria
        // Lee el bloque al buffer del archivo
        if(openfiles[fd].currBlockInMemory != currblock)
        {
            // Leer el bloque actual hacia el buffer que
            // está en la tabla de archivos abiertos
            readBlock(currblock,openfiles[fd].buffer);
            openfiles[fd].currBlockInMemory = currblock;
        }

        // Copia el caracter al buffer
        buffer[cont] = openfiles[fd].buffer[openfiles[fd].currPos % BLOCKSIZE];

        // Incrementa posición
        openfiles[fd].currPos++;

        // Incrementa el contador
        cont++;

        if (rootDir[openfiles[fd].iNode].size <= openfiles[fd].currPos) return cont;
    }
    return size;
}

// Write into an open file
int vdwrite(int fd, char *buffer, int size)
{
    int currblock;
    int currinode;
    int inicio_nodos_i = getINodeTable();
    int cont = 0;
    int sector;
    int i;
    int result;
    unsigned short *currptr;

    // Si no está abierto, regresa error
    if(openfiles[fd].inUse==0)
        return(-1);

    currinode = openfiles[fd].iNode;

    // Cada iteración del ciclo copia un caracter
    // de la entrada buffer al buffer del bloque
    while(cont < size)
    {
        // Obtener la dirección de donde está el bloque que corresponde
        // a la posición actual
        currptr = currpostoptr(fd);
        if(currptr == NULL)
            return -1;

        currblock = *currptr;

        // Si el bloque está en blanco, dale uno
        if(currblock == 0)
        {
            currblock = nextFreeBlock();
            // El bloque encontrado ponerlo en donde
            // apunta el apuntador al bloque actual
            *currptr = currblock;
            assignBlock(currblock);

            // Escribir el sector de la tabla de nodos i
            // En el disco
            sector = (currinode/8)*8;
            result = vdwritesl(0, inicio_nodos_i+sector, 1, (char *) &rootDir[sector*8]);
        }

        // Si el bloque de la posición actual no está en memoria
        // Lee el bloque al buffer del archivo
        if(openfiles[fd].currBlockInMemory != currblock)
        {
            // Leer el bloque actual hacia el buffer que
            // está en la tabla de archivos abiertos
            readBlock(currblock,openfiles[fd].buffer);
            openfiles[fd].currBlockInMemory = currblock;
        }

        // Copia el caracter al buffer
        openfiles[fd].buffer[openfiles[fd].currPos % BLOCKSIZE] = buffer[cont];

        // Incrementa posición
        openfiles[fd].currPos++;

        // Si la posición es mayor que el tamaño, modifica el tamaño
        if(openfiles[fd].currPos > rootDir[currinode].size)
            rootDir[openfiles[fd].iNode].size = openfiles[fd].currPos;

        // Incrementa el contador
        cont++;

        // Si se llena el buffer, escríbelo
        if(openfiles[fd].currPos % BLOCKSIZE == 0)
            writeBlock(currblock,openfiles[fd].buffer);
    }
    if(cont % 4096 != 0) {
        writeBlock(currblock, openfiles[fd].buffer);
    }
    return(cont);
}


// Close an open file
int vdclose(int fd)
{
    if(fd >= NOPENFILES || fd < 0) {
        return ERROR;
    }

    openfiles[fd].inUse = 0;
    openfiles[fd].iNode = 0;
    openfiles[fd].currPos = 0;

    return SUCCESS;
}

VDDIR* vdopendir(char* dir)
{
    if (checkRootDir() == ERROR)
        return ERROR;

    if (strcmp(dir, ".") != 0)
        return NULL;

    int i = 0;

    while (dirs[i] != -1 && i < 2) i++;

    if (i == 2)
        return NULL;

    dirs[i] = 0;

    return &dirs[i];
}

struct VDDIRENT* vdreaddir(VDDIR* dir)
{
    if (checkRootDir() == ERROR)
        return ERROR;

    // Mientras no haya nodo i, avanza
    while(isINodeFree(*dir) && *dir < 4096) (*dir)++;

    // Apunta a donde está el nombre en el inodo
    currDir.d_name = rootDir[*dir].name;

    (*dir)++;

    if (*dir >= 4096)
        return NULL;

    return &currDir;
}

int vdclosedir(VDDIR* dir)
{
    dir = -1;
    return 0;
}

// Funciones auxiliares en el manejo de archivos
// A partir de un archivo abierto y la posición del cursor
// del archivo, me devuelve un apuntador al campo del inodo
// o del bloque de apuntadores que contiene el bloque que
// le corresponde a esa posición.
unsigned short *postoptr(int fd, int pos)
{
    int currinode;
    int sector;
    int currblock = (pos/BLOCKSIZE);
    unsigned short *currptr;
    unsigned short indirect1;

    currinode = openfiles[fd].iNode;

    // Está en los primeros 10 K
    if(currblock < 10)
        // Está entre los 10 apuntadores directos
        currptr = &rootDir[currinode].blocks[currblock];
    else if(currblock < (PTRxBLOCK+10))
    {
        // Si el indirecto está vacío, asígnale un bloque
        if(rootDir[currinode].indirect == 0)
        {
            // El primer bloque disponible
            indirect1 = nextFreeBlock();
            assignBlock(indirect1); // Asígnalo
            rootDir[currinode].indirect = indirect1;
            sector = (currinode/8)*8;
            vdwritesl(0, getINodeTable()+sector, 1, (char *) &rootDir[sector*8]);
            readBlock(indirect1, (char *) openfiles[fd].indirectBuff);
        }
        currptr = &openfiles[fd].indirectBuff[currblock-10];
    }
    else {
        return NULL;
    }

    return currptr;
}


// Toma la posición del cursor del archivo como valor
// y a partir de ahí nos devuelve la dirección de memoria
// donde está el bloque que le corresponde.
unsigned short *currpostoptr(int fd)
{
    unsigned short *currptr;

    currptr=postoptr(fd,openfiles[fd].currPos);

    return(currptr);
}
