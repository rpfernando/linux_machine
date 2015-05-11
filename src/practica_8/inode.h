#include "vdisk.h"
#include "sectorhandler.h"

extern short secboot_in_memory;  // sec boot
extern struct SECBOOT secBoot;
extern short inodesmap_in_memory; // inode
extern unsigned char iNodesMap[SECSIZE];
extern short datamap_in_memory;	//mapa de datos
extern unsigned char dataMap[SECSIZE];

// ======= FUNCTIONS ======

// Check if I Node is free
int isINodeFree(int inode);   

// Search for a free I Node
int nextFreeINode();

// Assign given I Node
int assignINode(int inode);

// Mark I Node as free
int unassignINode(int inode);