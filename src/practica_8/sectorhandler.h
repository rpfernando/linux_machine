#include "vdisk.h"

// ====== MAIN CHECK FUNCTIONS ======

int checkSecBoot();
int checkINodesMap();
int checkDataMap();
int checkRootDir();
int checkOpenFiles();

// ====== HELPER METHODS ======

int getINodesMap();
int getDataMap();
int getINode();
int getDataBlock();