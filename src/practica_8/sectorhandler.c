
int checkSecBoot()
{ 
    int result = SUCCESS

    // Check whether the superblock is in memory
    if (!secboot_in_memory)
    {
        // Load superblock
        result = vdreadsector(0, 0, 0, 1, 1, (char *) &secboot);
        secboot_in_memory = 1;
    }

    return result;
}

int checkINodesMap()
{
    int result = SUCCESS;

    // Check whether the "I Nodes Map" is in memory
    if (!inodesmap_in_memory)
    {
        // Load "I Nodes Map"
        int sl = getINodesMap();
        result = vdreadsl(0, sl, 1, (char *) &iNodesMap);
        inodesmap_in_memory = 1;
    }

    return result;
}

int checkDataMap()
{
  int result = SUCCESS;

    // Check whether the "Data Map" is in memory
    if (!datamap_in_memory)
    {
        // Load "Data Map"
        int sl = getDataMap();
        result = vdreadls(0, sl, 1, &dataMap);
        datamap_in_memory = 1;
    }

  return result;
}


int checkRootDir()
{
    int result = SUCCESS;

    // Check whether the "Root Dir" is in memory
    if (!rootdir_in_memory)
    {
        // Load "Root Dir"
        int sl = getINode();
        result = vdreadls(0, sl, (dataBlockLs() - iNodeLs()), &rootdir);
        rootdir_in_memory = 1;
    }

  return result;
}

int checkOpenFiles()
{
  int i;

    if (!openfiles_inicializada)
    {
        for (i = 3; i < NOPENFILES; i++)
        {
            openfiles[i].inuse = 0; // Archivo no está en uso
            openfiles[i].currbloqueenmemoria = -1; // Ningún bloque
        }

        openfiles_inicializada = 1;
    }

    return SUCCESS;
}


// ====== HELPER METHODS ======

int getINodesMap()
{
    checkSecBoot();
    return secBoot.sec_mapa_bits_nodo_i;
}

int getDataMap()
{
    checkSecBoot();
    return secBoot.sec_mapa_bits_bloques;
}

int getINode()
{
    checkSecBoot();
    return secBoot.sec_tabla_nodos_i;
}

int getDataBlock()
{
    checkSecBoot();
    return secBoot.sec_log_unidad;
}