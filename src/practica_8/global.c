#include "global.h"

short secBootInMemory = 0;
struct SECBOOT secBoot;

short iNodesMapInMemory = 0;
unsigned char iNodesMap[SECSIZE];

short dataMapInMemory = 0;
unsigned char dataMap[SECSIZE];

short rootDirInMemory = 0;
struct INODE dirRaiz[NINODES];

short openFilesInMemory = 0;
struct OPENFILES openfiles[NOPENFILES];
