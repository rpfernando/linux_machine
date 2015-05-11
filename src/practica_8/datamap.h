#include "vdisk.h"
#include "sectorhandler.h"

extern short secboot_in_memory;  // sec boot
extern struct SECBOOT secBoot;
extern short inodesmap_in_memory; // inode
extern unsigned char iNodesMap[SECSIZE];
extern short datamap_in_memory;	//mapa de datos
extern unsigned char dataMap[SECSIZE];

// ======= FUNCTIONS ======

// Check if block is free
int isBlockFree(int block); 

// Search for a free block
int nextFreeBlock();

// Assign given block
int assignBlock(int block);

// Mark block as free
int unassignBlock(int block);