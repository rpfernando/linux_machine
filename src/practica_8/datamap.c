#include "datamap.h"

int isBlockFree(int block)
{
    int offset = block / 8;
    int shift = block % 8;

    if (checkSectors() == ERROR)
        return ERROR;

    if (dataMap[offset] & (1 << shift))
        return NO;

    return YES;
}

int nextFreeBlock()
{
    int i, j;

    if (checkSectors() == ERROR)
        return ERROR;

    // Check byte by byte
    for (i = 0; dataMap[i] == 0xFF && i < secBoot.sec_mapa_bits_bloques * SECSIZE; i++);

    if (i < secBoot.sec_mapa_bits_bloques * SECSIZE)
    {
        // Check bit by bit
        for (j = 0; dataMap[i] & (1 << j) && j < 8; j++);
        return (i * 8) + j;
    }

    return ERROR;
}

// Assign given block
int assignBlock(int block)
{
    int offset = block / 8;
    int shift = block % 8;

    if (checkSectors() == ERROR)
        return ERROR;

    // Mark block as not free
    dataMap[offset] |= (1 << shift);

    if (vdwritesl(0, getDataMap(), dataMap) == ERROR)
        return ERROR;

    return SUCCESS;
}

// Unassign given block
int unassignBlock(int block)
{
    int offset = block / 8;
    int shift = block % 8;

    if (checkSectors() == ERROR)
        return ERROR;

    // Mark block as free
    dataMap[offset] &= (char) ~(1 << shift);

    if (vdwritesl(0, getDataMap(), dataMap) == ERROR)
        return ERROR;

    return SUCCESS;
}

// ====== READ / WRITE BLOCK METHODS =======

int writeBlock(int block, char *buffer)
{
    int dataAreaStart = getDataBlock();
    int i;

    if (checkSecBoot() == ERROR)
        return ERROR;

    /* Calcular el primer sector que corresponde a un bloque de datos */
    for (i = 0; i < secBoot.sec_x_bloque; i++)
    {        
        if (vdwritesl(0, dataAreaStart + (block - 1) * secBoot.sec_x_bloque + i, buffer + 512 * i) == ERROR)
            return ERROR;
    }

    return SUCCESS;
}

int readBlock(int block, char *buffer)
{
    int dataAreaStart = getDataBlock();
    int i;

    if (checkSecBoot() == ERROR)
        return ERROR;

    for (i = 0; i < secBoot.sec_x_bloque; i++)
    {        
        if (vdreadsl(0, dataAreaStart + (block - 1) * secBoot.sec_x_bloque + i, buffer + 512 * i) == ERROR)
            return ERROR;
    }

    return SUCCESS;
}

// Load MBR and DataMap if not already loaded
int checkSectors() {

    if (checkSecBoot() == ERROR)
        return ERROR;

    if (checkDataMap() == ERROR)
        return ERROR;

    return SUCCESS;
}
