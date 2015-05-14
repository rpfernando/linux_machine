#include "vdisk.h"

#ifndef __SECTORHANDLER_LIB__

  #define __SECTORHANDLER_LIB__ 1
  // ====== MAIN CHECK FUNCTIONS ======

  int checkSecBoot();
  int checkINodesMap();
  int checkDataMap();
  int checkRootDir();
  int checkOpenFiles();

  // ====== HELPER METHODS ======

  int getINodesMap();
  int getDataMap();
  int getINodeTable();
  int getDataBlock();
#endif
