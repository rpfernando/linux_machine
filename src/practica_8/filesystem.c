#include "filesystem.h"

// Create a new file (file remains open)
int vdcreat(char *filename, unsigned short mode)
{
    int numinode;
    int i;

    // Ver si ya existe el archivo
    numinode=searchinode(filename);

    // Si no existe
    if(numinode==-1)
    {
        // Buscar un inodo en blanco en el mapa de bits (nodos i)
        numinode=nextfreeinode();
        if(numinode==-1)
        {
            return(-1); // No hay espacio para más archivos
        }
    } else  // Si ya existe, elimina el inodo
        removeinode(numinode);


    // Escribir el archivo en el inodo encontrado

    setninode(numinode,filename,perms,getuid(),getgid());
    assigninode(numinode);

    // Poner el archivo en la tabla de archivos abiertos

    if(!openfiles_inicializada)
    {
        for(i=3;i<16;i++)
        {
            openfiles[i].inuse=0;
            openfiles[i].currbloqueenmemoria=-1;
        }
        openfiles_inicializada=1;
    }

    // Buscar si hay lugar en la tabla de archivos abiertos
    // Si no hay lugar, regresa -1
    i=3;
    while(openfiles[i].inuse && i<16)
        i++;

    // Si no hay lugar en la tabla de archivos para
    // el que queremos abrir, regresamos error (-1)
    if(i>=16)
        return(-1);

    // Si hay lugar, vamos a poner la entrada en uso
    // Ponemos en la tabla el inodo que corresponde al
    // archivo
    // Poner la posición actual del archivo en 0.
    openfiles[i].inuse=1;
    openfiles[i].inode=numinode;
    openfiles[i].currpos=0;
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

    // Si no está inicializada la tabla de archivos abiertos inicialízala
    if(!openfiles_inicializada)
    {
        for(i=3; i<16; i++)
        {
            openfiles[i].inuse=0;
            openfiles[i].currbloqueenmemoria=-1;
        }
        openfiles_inicializada=1;
    }

    // Buscar si hay lugar en la tabla de archivos abiertos
    // Si no hay lugar, regresa -1
    for(i=3; openfiles[i].inuse && i<16; i++);
    if(i>=16) return -1;

    openfiles[i].inuse=1;
    openfiles[i].inode=numinode;
    openfiles[i].currpos=0;

    // Si hay apuntador indirecto, leerlo en el buffer
    if(inode[numinode].indirect!=0) {
        readblock(inode[numinode].indirect,(char *) openfiles[i].buffindirect);
    }

    // Cargar el buffer con el bloque actual del archivo (primer bloque)
    currblock=*currpostoptr(i);
    readblock(currblock,openfiles[i].buffer);
    return i;
}

// Deletes a file
int vdunlink(char *filename)
{
    int numinode;
    int i;

    // Busca el inodo del archivo
    numinode=searchinode(filename);
    if(numinode==-1) return -1; // No existe

    removeinode(numinode);
    return 0;
}

// Changes position within the file
int vdseek(int fd, int offset, int whence)
{
    unsigned short oldblock,newblock;

    // Si no está abierto regresa error
    if(openfiles[fd].inuse==0)
        return(-1);

    oldblock=*currpostoptr(fd);

    if(whence==0) // A partir del inicio
    {
        // Si el desplazamiento es desde el inicio
        // el offset no puede ser negativo
        // el offset no debe exceder el tamaño del archivo
        if(offset<0 ||
            openfiles[fd].currpos+offset>inode[openfiles[fd].inode].size)
            return(-1);
        // Poner el cursor con su nuevo valor
        openfiles[fd].currpos=offset;

    } else if(whence==1) // A partir de la posición actual
    {
        // Si me muevo hacia atrás no llegue hasta antes
        // del inicio del archivp
        // Si me muevo hacia adelante, no pasar el final
        // del archivo.
    if(openfiles[fd].currpos+offset>inode[openfiles[fd].inode].size ||
            openfiles[fd].currpos+offset<0)
            return(-1);

        // Poner el cursor con su nuevo valor
        openfiles[fd].currpos+=offset;

    } else if(whence==2) // A partir del final
    {
        // Si me muevo hacia atrás no llegue hasta antes
        // del inicio del archivp
        // Si me muevo hacia adelante, no pasar el final
        // del archivo.

        if(offset>inode[openfiles[fd].inode].size ||
            openfiles[fd].currpos-offset<0)
            return(-1);

        // Poner el cursor con su nuevo valor
    openfiles[fd].currpos=inode[openfiles[fd].inode].size-offset;
    } else {
        return(-1);
    }

    newblock = *currpostoptr(fd);

    if(newblock != oldblock)
    {
        writeblock(oldblock,openfiles[fd].buffer);
        readblock(newblock,openfiles[fd].buffer);
        openfiles[fd].currbloqueenmemoria=newblock;
    }

    return openfiles[fd].currpos;
}

// Read from an open file
int vdread(int fd, char *buffer, int size)
{
    // TODO
    return 0;
}

// Write into an open file
int vdwrite(int fd, char *buffer, int size)
{
    int currblock;
    int currinode;
    int cont=0;
    int sector;
    int i;
    int result;
    unsigned short *currptr;

    // Si no está abierto, regresa error
    if(openfiles[fd].inuse==0)
        return(-1);

    currinode=openfiles[fd].inode;

    // Cada iteración del ciclo copia un caracter
    // de la entrada buffer al buffer del bloque
    while(cont<bytes)
    {
        // Obtener la dirección de donde está el bloque que corresponde
        // a la posición actual
        currptr=currpostoptr(fd);
        if(currptr==NULL)
            return(-1);

        currblock=*currptr;

        // Si el bloque está en blanco, dale uno
        if(currblock==0)
        {
            currblock=nextfreeblock();
            // El bloque encontrado ponerlo en donde
            // apunta el apuntador al bloque actual
            *currptr=currblock;
            assignblock(currblock);

            // Escribir el sector de la tabla de nodos i
            // En el disco
            sector=(currinode/8)*8;
            result=vdwriteseclog(inicio_nodos_i+sector,&inode[sector*8]);
        }

        // Si el bloque de la posición actual no está en memoria
        // Lee el bloque al buffer del archivo
        if(openfiles[fd].currbloqueenmemoria!=currblock)
        {
            // Leer el bloque actual hacia el buffer que
            // está en la tabla de archivos abiertos
            readblock(currblock,openfiles[fd].buffer);
            openfiles[fd].currbloqueenmemoria=currblock;
        }

        // Copia el caracter al buffer
        openfiles[fd].buffer[openfiles[fd].currpos%4096]=buffer[cont];

        // Incrementa posición
        openfiles[fd].currpos++;

        // Si la posición es mayor que el tamaño, modifica el tamaño
        if(openfiles[fd].currpos>inode[currinode].size)
            inode[openfiles[fd].inode].size=openfiles[fd].currpos;

        // Incrementa el contador
        cont++;

        // Si se llena el buffer, escríbelo
        if(openfiles[fd].currpos%4096==0)
            writeblock(currblock,openfiles[fd].buffer);
    }
    return(cont);
}


// Close an open file
int vdclose(int fd)
{
    // TODO
    return 0;
}

VDDIR* vdopendir(char* dir)
{
    // TODO
    return 0;
}

struct vddirent* vdreaddir(VDDIR* dir)
{
    // TODO
    return 0;
}

int vdclosedir(VDDIR* dir)
{
    // TODO
    return 0;
}


Funciones para el manejo de fechas
unsigned int datetoint(struct DATE date)
{
    unsigned int val=0;

    val=date.year-1970;
    val<<=4;
    val|=date.month;
    val<<=5;
    val|=date.day;
    val<<=5;
    val|=date.hour;
    val<<=6;
    val|=date.min;
    val<<=6;
    val|=date.sec;

    return(val);
}

int inttodate(struct DATE *date,unsigned int val)
{
    date->sec=val&0x3F;
    val>>=6;
    date->min=val&0x3F;
    val>>=6;
    date->hour=val&0x1F;
    val>>=5;
    date->day=val&0x1F;
    val>>=5;
    date->month=val&0x0F;
    val>>=4;
    date->year=(val&0x3F) + 1970;
    return(1);
}

// Obtiene la fecha actual del sistema
// La convierte a un entero de 32 bits
unsigned int currdatetimetoint()
{
    struct tm *tm_ptr;
    time_t the_time;

    struct DATE now;

    (void) time(&the_time);
    tm_ptr=gmtime(&the_time);

    now.year=tm_ptr->tm_year-70;
    now.month=tm_ptr->tm_mon+1;
    now.day=tm_ptr->tm_mday;
    now.hour=tm_ptr->tm_hour;
    now.min=tm_ptr->tm_min;
    now.sec=tm_ptr->tm_sec;
    return(datetoint(now));
}

Funciones básicas para el manejo de i-nodos
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

    int result;

    if(!secboot_en_memoria)
    {
        result=vdreadsector(0,0,0,1,1,(char *) &secboot);
        secboot_en_memoria=1;
    }
    inicio_nodos_i=secboot.sec_res+secboot.sec_mapa_bits_nodos_i+secboot.sec_mapa_bits_bloques;

    if(!nodos_i_en_memoria)
    {
        for(i=0;i<secboot.sec_tabla_nodos_i;i++)
            result=vdreadseclog(inicio_nodos_i+i,&inode[i*8]);

        nodos_i_en_memoria=1;
    }


    strncpy(inode[num].name,filename,20);

    if(strlen(inode[num].name)>19)
        inode[num].name[19]='\0';

    inode[num].datetimecreat=currdatetimetoint();
    inode[num].datetimemodif=currdatetimetoint();
    inode[num].uid=uid;
    inode[num].gid=gid;
    inode[num].perms=atribs;
    inode[num].size=0;

    for(i=0;i<10;i++)
        inode[num].blocks[i]=0;

    inode[num].indirect=0;
    inode[num].indirect2=0;

    // Optimizar la escritura escribiendo solo el sector lógico que
    // corresponde al inodo que estamos asignando.
    // i=num/8;
    // result=vdwriteseclog(inicio_nodos_i+i,&inode[i*8]);
    for(i=0;i<secboot.sec_tabla_nodos_i;i++)
        result=vdwriteseclog(inicio_nodos_i+i,&inode[i*8]);

    return(num);
}

// Buscar un archivo por nombre en la tabla de nodos i
// Devuelve la posición del archivo en la tabla de nodos i
// Si no se encuentra devuelve -1
int searchinode(char *filename)
{
    int i;
    int free;
    int result;

    if(!secboot_en_memoria)
    {
        result=vdreadsector(0,0,0,1,1,(char *) &secboot);
        secboot_en_memoria=1;
    }
    inicio_nodos_i=secboot.sec_res+secboot.sec_mapa_bits_nodos_i+secboot.sec_mapa_bits_bloques;

    if(!nodos_i_en_memoria)
    {
        for(i=0;i<secboot.sec_tabla_nodos_i;i++)
            result=vdreadseclog(inicio_nodos_i+i,&inode[i*8]);

        nodos_i_en_memoria=1;
    }

    if(strlen(filename)>19)
        filename[19]='\0';

    i=0;
    while(strcmp(inode[i].name,filename) && i<32)
        i++;

    if(i>=32)
        return(-1);
    else
        return(i);
}

// Borrar un inodo de la tabla de nodos i
// Se va a utilizar para borrar un archivo
int removeinode(int numinode)
{
    int i;

    unsigned short temp[2048];

    // Recorrer los apuntadores directos del inodo
    // Poner en 0 su bit correspondiente en el mapa
    // de bits
    for(i=0;i<10;i++)
        if(inode[numinode].blocks[i]!=0)
            unassignblock(inode[numinode].blocks[i]);

    // Recorrer los apuntadores indirectos
    if(inode[numinode].indirect!=0)
    {
        // Leer el bloque indirecto
        // el que contiene los apuntadores después del 10
        readblock(inode[numinode].indirect,(char *) temp);

        // Recorrer todos los apuntadores y poner en 0
        // el bit correspondiente en el mapa de bits
        for(i=0;i<2048;i++)
            if(temp[i]!=0)
                unassignblock(temp[i]);

        // Desasignar el bloque que contiene los
        // apuntadores.
        unassignblock(inode[numinode].indirect);
        inode[numinode].indirect=0;
    }

    // Desasignar el inodo
    unassigninode(numinode);
    return(1);
}

Funciones del sistema de archivos

// **********************************************************************
// Funciones del sistema de archivos
// **********************************************************************

// Tabla de archivos abiertos
int openfiles_inicializada=0;
struct OPENFILES openfiles[16];


// Funciones auxiliares en el manejo de archivos

// A partir de un archivo abierto y la posición del cursor
// del archivo, me devuelve un apuntador al campo del inodo
// o del bloque de apuntadores que contiene el bloque que
// le corresponde a esa posición.
unsigned short *postoptr(int fd,int pos)
{
    int currinode;
    unsigned short *currptr;
    unsigned short indirect1;

    currinode=openfiles[fd].inode;

    // Está en los primeros 10 K
    if((pos/4096)<10)
        // Está entre los 10 apuntadores directos
        currptr=&inode[currinode].blocks[pos/4096];
    else if((pos/4096)<(2048+10))
    {
        // Si el indirecto está vacío, asígnale un bloque
        if(inode[currinode].indirect==0)
        {
            // El primer bloque disponible
            indirect1=nextfreeblock();
            assignblock(indirect1); // Asígnalo
            inode[currinode].indirect=indirect1;
        }
        currptr=&openfiles[fd].buffindirect[pos/4096-10];
    }
    else
        return(NULL);

    return(currptr);
}


// Toma la posición del cursor del archivo como valor
// y a partir de ahí nos devuelve la dirección de memoria
// donde está el bloque que le corresponde.
unsigned short *currpostoptr(int fd)
{
    unsigned short *currptr;

    currptr=postoptr(fd,openfiles[fd].currpos);

    return(currptr);
}
