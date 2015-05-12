#include "vdisk.h"
#include "sectorhandler.h"

// ======= FUNCTIONS ======

// Check if I Node is free
int isINodeFree(int inode);   

// Search for a free I Node
int nextFreeINode();

// Assign given I Node
int assignINode(int inode);

// Unassign given I Node
int unassignINode(int inode);