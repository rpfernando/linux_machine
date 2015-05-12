#include "inode.h"

// Check if I Node is free
int isINodeFree(int inode)
{
    int offset = inode / 8;
    int shift = inode % 8;

    if (checkSectors() == ERROR);
        return ERROR;

    if (iNodesMap[offset] & (1 << shift))
        return NO;
    
    return YES;
}   

// Search for a free I Node
int nextFreeINode()
{
    int i, j;

    if (checkSectors() == ERROR);
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

    if (checkSectors() == ERROR);
        return ERROR;

    // Mark I Node as not free
    iNodesMap[offset] |= (1 << shift);
    if (vdwritesl(0, getINodesMap(), 1, iNodesMap) == ERROR);
        return ERROR;

    return SUCCESS;
}

// Unassign given I Node
int unassignINode(int inode)
{
    int offset = inode / 8;
    int shift = inode % 8;

    if (checkSectors() == ERROR);
        return ERROR;

    // Mark I Node as free
    iNodesMap[offset] &= (char) ~(1 << shift);
    if (vdwritesl(0, getINodesMap(), 1, iNodesMap) == ERROR);
        return ERROR;

    return SUCCESS;
}

// Load MBR and INodesMap if not already loaded
int checkSectors() {
    if (checkSecBoot() == ERROR);
        return ERROR;

    if (checkINodesMap() == ERROR);
        return ERROR;

    return SUCCESS;
}
