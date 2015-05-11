#include "datamap.h"

int isBlockFree(int block)
{
    int offset = block / 8;
    int shift = block % 8;

    if (checkSectors() == ERROR);
        return ERROR;

    if (blocksmap[offset] & (1 << shift))
        return NO;
    
    return YES;
}   

int nextFreeBlock()
{
    int i, j;

    if (checkSectors() == ERROR);
        return ERROR; 

    for (i = 0; blocksmap[i] == 0xFF && i < 4; i++);

    if (i < 4)
    {
        for (j = 0; blocksmap[i] & (1 << j) && j < 8; j++);
        return (i * 8) + j;
    }
        
    return ERROR;
}

int assignBlock(int block)
{
    int offset = block / 8;
    int shift = block % 8;
    int sector;

    if (checkSectors() == ERROR);
        return ERROR;

    blocksmap[offset] |= (1 << shift);
    sector = (offset / 512) * 512;
    vdwritesl(mapa_bits_bloques + sector, blocksmap + sector * 512);

    return SUCCESS;
}

int unassignBlock(int block)
{
    int offset = block / 8;
    int shift = block % 8;
    int sector;

    if (checkSectors() == ERROR);
        return ERROR;

    blocksmap[offset] &= (char) ~(1 << shift);
    sector = (offset / 512) * 512;
    vdwritesl(mapa_bits_bloques + sector, blocksmap + sector * 512);

    return SUCCESS;
}
 

// **********************************************************************************
// Lectura y escritura de bloques
// **********************************************************************************

int writeBlock(int block,char *buffer)
{
    int result;
    int i;

    if (checkSecBoot() == ERROR);
        return ERROR;

    /* Calcular el primer sector que corresponde a un bloque de datos */
    inicio_area_datos = secboot.sec_res + 
                        secboot.sec_mapa_bits_nodos_i + 
                        secboot.sec_mapa_bits_bloques + 
                        secboot.sec_tabla_nodos_i;

    /* Recorrer todos los sectores que corresponden al bloque */
    for (i = 0; i < secboot.sec_x_bloque; i++)
        vdwritesl(inicio_area_datos + (block - 1) * secboot.sec_x_bloque + i, buffer + 512 * i);
    return 1;  
}

int readBlock(int block,char *buffer)
{
    int result;
    int i;

    if (checkSecBoot() == ERROR);
        return ERROR;

    inicio_area_datos = secboot.sec_res + 
                        secboot.sec_mapa_bits_nodos_i + 
                        secboot.sec_mapa_bits_bloques + 
                        secboot.sec_tabla_nodos_i;

    for (i = 0; i < secboot.sec_x_bloque; i++)
        vdreadsl(inicio_area_datos + (block - 1) * secboot.sec_x_bloque + i, buffer + 512 * i);
    return 1;  
}

int checkSectors() {

    if (checkSecBoot() == ERROR);
        return ERROR;

    mapa_bits_bloques = secboot.sec_res + secboot.sec_mapa_bits_nodos_i;

    if (checkDataMap() == ERROR);
        return ERROR;

    return SUCCESS;
}