#include "inode.h"

// Check if I Node is free
int isINodeFree(int inode)
{
    int offset = inode / 8;
    int shift = inode % 8;

    if (checkSectors() == ERROR);
        return ERROR;

    if (inodesmap[offset] & (1 << shift))
        return NO;
    
    return YES;
}   

// Search for a free I Node
int nextFreeINode()
{
    int i, j;

    if (checkSectors() == ERROR);
        return ERROR;

    // Recorrer byte por byte mientras sea 0xFF sigo recorriendo
    for (i = 0; inodesmap[i] == 0xFF && i < 4; i++);

    if (i < 4)
    {
        for (j = 0; inodesmap[i] & (1 << j) && j < 8; j++);
        return (i * 8) + j;
    }

    return ERROR;
}

// Assign given I Node
int assignINode(int inode)
{
    int offset = inode / 8;
    int shift = inode % 8;

    if (checkSectors() == ERROR);
        return ERROR;

    inodesmap[offset] |= (1 << shift);
    if (vdwritesl(mapa_bits_nodos_i, inodesmap) == ERROR);
        return ERROR;

    return SUCCESS;
}

// Mark I Node as free
int unassignINode(int inode)
{
    int offset = inode / 8;
    int shift = inode % 8;

    if (checkSectors() == ERROR);
        return ERROR;

    inodesmap[offset] &= (char) ~(1 << shift);
    vdwritesl(mapa_bits_nodos_i, inodesmap);
    return SUCCESS;
}

int checkSectors() {
    if (checkSecBoot() == ERROR);
        return ERROR;

    if (checkINodesMap() == ERROR);
        return ERROR;

    return SUCCESS;
}
