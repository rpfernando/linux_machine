#include "sectorhandler.h"

int checkSecBoot()
{
    int result = SUCCESS;

    // Check whether the superblock is in memory
    if (!secBootInMemory)
    {
        // Load superblock
        result = vdreadsl(0, 0, (char *) &secBoot);
        secBootInMemory = 1;
    }

    return result;
}

int checkINodesMap()
{
    int result = SUCCESS;

    // Check whether the "I Nodes Map" is in memory
    if (!iNodesMapInMemory)
    {
        // Load "I Nodes Map"
        int sl = getINodesMap();
        result = vdreadsl(0, sl, (char *) &iNodesMap);
        iNodesMapInMemory = 1;
    }

    return result;
}

int checkDataMap()
{
  int result = SUCCESS;

    // Check whether the "Data Map" is in memory
    if (!dataMapInMemory)
    {
        // Load "Data Map"
        int sl = getDataMap();
        result = vdreadsl(0, sl, (char *) &dataMap);
        dataMapInMemory = 1;
    }

  return result;
}


int checkRootDir()
{
    int result = SUCCESS;

    // Check whether the "Root Dir" is in memory
    if (!rootDirInMemory)
    {
        // Load "Root Dir"
        int sl = getINodeTable();
        int nsecs = getDataBlock() - getINodeTable();
        int i;

        for (i = 0; i < nsecs; i++)
            result = vdreadsl(0, sl + i, (char *) &rootDir);

        rootDirInMemory = 1;
    }

  return result;
}

int checkOpenFiles()
{
  int i;

    if (!openFilesInMemory)
    {
        for (i = 3; i < NOPENFILES; i++)
        {
            openfiles[i].inUse = 0; // File is not been used
            openfiles[i].currBlockInMemory = 0; // No block assigned yet
        }

        openFilesInMemory = 1;
    }

    return SUCCESS;
}


// ====== HELPER METHODS ======

int getINodesMap()
{
    checkSecBoot();
    return secBoot.sec_res;
}

int getDataMap()
{
    checkSecBoot();
    return secBoot.sec_res +
            secBoot.sec_mapa_bits_nodo_i;
}

int getINodeTable()
{
    checkSecBoot();
    return secBoot.sec_res +
            secBoot.sec_mapa_bits_nodo_i +
            secBoot.sec_mapa_bits_bloques;
}

int getDataBlock()
{
    checkSecBoot();
    return secBoot.sec_res +
            secBoot.sec_mapa_bits_nodo_i +
            secBoot.sec_mapa_bits_bloques +
            secBoot.sec_tabla_nodos_i;
}
