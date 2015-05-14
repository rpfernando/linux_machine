#include <string.h>
#include "sectorhandler.h"
#include "datamap.h"
#include "date.h"

#ifndef __INODE_LIB__

  #define __INODE_LIB__ 1

  // ======= FUNCTIONS ======

  // Check if I Node is free
  int isINodeFree(int inode);

  // Search for a free I Node
  int nextFreeINode();

  // Assign given I Node
  int assignINode(int inode);

  // Unassign given I Node
  int unassignINode(int inode);

  int setninode(int num, char *filename,unsigned short atribs, int uid, int gid);

  int searchinode(char *filename);

  int removeinode(int numinode);

#endif
