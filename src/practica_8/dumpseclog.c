#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "vdisk.h"

#define LINESIZE 16
#define SECSIZE 512

int main(int argc, char *argv[])
{
	int seclog = atoi(argv[1]);
	unsigned char buffer[SECSIZE];
	int i, j;
	unsigned char c;

	if ( seclog > HEADS * SECTORS * CYLINDERS )
	{
		fprintf(stderr, "Posición invalida\n");
		exit(1);
	}
	
	if (vdreadsl(seclog, buffer) == -1)
	{
		fprintf(stderr,"Error al abrir disco virtual\n");
		exit(1);
	}

	for (i = 0; i < SECSIZE / LINESIZE; i++)
	{
		printf("\n %3X -->", i * LINESIZE);
		for (j = 0; j < LINESIZE; j++)
		{
			c = buffer[i * LINESIZE + j];
			printf("%2X ", c);
		}

		printf("  |  ");
		for (j = 0; j < LINESIZE; j++)
		{
			c = buffer[i * LINESIZE + j] % 256;
			if (c > 0x1F && c < 127)
				printf("%c", c);
			else
				printf(".");
		}
	}

	printf("\n");
}

