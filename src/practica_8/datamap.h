#include "vdisk.h"
#include "sectorhandler.h"

// ======= FUNCTIONS ======

// Check if block is free
int isBlockFree(int block); 

// Search for a free block
int nextFreeBlock();

// Assign given block
int assignBlock(int block);

// Unassign given block
int unassignBlock(int block);

// ====== READ / WRITE BLOCK METHODS =======

// Write on given block
int writeBlock(int block, char *buffer);

// Read from given block
int readBlock(int block, char *buffer);