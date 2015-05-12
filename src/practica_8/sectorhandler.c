#include "sectorhandler.h"

int checkSecBoot()
{ 
    int result = SUCCESS

    // Check whether the superblock is in memory
    if (!secBootInMemory)
    {
        // Load superblock
        result = vdreadsl(0, 0, 1, (char *) &secBoot);
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
        result = vdreadsl(0, sl, 1, (char *) &iNodesMap);
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
        result = vdreadsl(0, sl, 1, &dataMap);
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
        result = vdreadsl(0, sl, nsecs, &rootDir);
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
            openFiles[i].inUse = 0; // File is not been used
            openFiles[i].currBlockInMemory = -1; // No block assigned yet
        }

        openFilesInMemory = 1;
    }

    return SUCCESS;
}


// ====== HELPER METHODS ======

int getINodesMap()
{
    return secBoot.sec_mapa_bits_nodo_i;
}

int getDataMap()
{
    return secBoot.sec_mapa_bits_bloques;
}

int getINodeTable()
{
    return secBoot.sec_tabla_nodos_i;
}

int getDataBlock()
{
    return secBoot.sec_log_unidad;
}