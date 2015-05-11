#include <stdio.h>
#include "vdisk.h"

int main(int argc, char *argv[])
{
	int i;
    char filename[10];
    char buffer[SECSIZE];

    // Missing argument
	if (argc != 1)
	{
		printf("Expected 1 argument, example ./vdformat 0\n");
		return ERROR;
	}

	int diskNum = atoi(argv[1]);

	// Invalid argument
	if (diskNum < 0 || diskNum > 3)
    {
        fprintf(stderr, "Disk number not valid, must be between 0 and 3");
        exit(2);
    }

    // Display disk file to be formatted
    sprintf(filename, "disco%d.vd", diskNum);
    printf("Formatting %s", filename);

	// Initialize Master Boot Record
	struct SECBOOT mbr;

	for (i = 0; i < 4; i++)
	{
		mbr.jump[i] = 0;
	}

	mbr.sec_res = 0;
	mbr.sec_mapa_bits_nodo_i = 1;
	mbr.sec_mapa_bits_bloques = 2;
	mbr.sec_tabla_nodos_i = 3;
	mbr.sec_log_unidad = HEADS * CYLINDERS * SECTORS; 
	mbr.sec_x_bloque BLOCKSECT;
	mbr.heads = HEADS;
	mbr.cyls = CYLINDERS;
	mbr.secfis = SECTORS;

	for (i = 0; i < 487; i++)
	{
		mbr.restante[i] = 0;
	}

	// Write master boot record
	vdwritesector(diskNum, 0, 0, 1, 1, &mbr);

	for (i = 0; i < SECSIZE; i++)
		buffer[i] = 0;

	// Clean i nodes bitmap sector
	vdwritesector(diskNum, 0, 0, 2, 1, buffer);

	// Data bitmap first bit in 1
	buffer[0] = 1;

	// Clean data bitmap sector
	vdwritesector(diskNum, 0, 0, 3, 1, buffer);

	return SUCCESS;
}
