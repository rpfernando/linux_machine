#include "vdisk.h"

extern short secBootInMemory;
extern struct SECBOOT secBoot;
extern short iNodesMapInMemory;
extern unsigned char iNodesMap[SECSIZE];
extern short dataMapInMemory;
extern unsigned char dataMap[SECSIZE];
extern short rootDirInMemory;
extern struct INODE rootDir[NINODES];
extern short openFilesInMemory;
extern struct OPENFILES openFiles[NOPENFILES];

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