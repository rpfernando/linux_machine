#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

#define DIF 16
#define NUMTHREADS 4

// NOMBRE DEL ARCHIVO A PROCESAR
char filename[]="/home/usuario/ferrari.bmp";
char namedest[]="/home/usuario/ferrari_P.bmp";

#pragma pack(2) // Empaquetado de 2 bytes
typedef struct {
	unsigned char magic1; // 'B'
	unsigned char magic2; // 'M'
	unsigned int size; // Tamaño
	unsigned short int reserved1, reserved2;
	unsigned int pixelOffset; // offset a la imagen
} HEADER;

#pragma pack() // Empaquetamiento por default
typedef struct {
	unsigned int size; // Tamaño de este encabezado INFOHEADER
	int cols, rows; // Renglones y columnas de la imagen
	unsigned short int planes;
	unsigned short int bitsPerPixel; // Bits por pixel
	unsigned int compression;
	unsigned int cmpSize;
	int xScale, yScale;
	unsigned int numColors;
	unsigned int importantColors;
} INFOHEADER;

typedef struct {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} PIXEL;

typedef struct {
	HEADER header;
	INFOHEADER infoheader;
	PIXEL *pixel;
} IMAGE;
IMAGE imagenfte, imagendst;

int loadBMP(char *filename, IMAGE *image)
{
	FILE *fin;
	int i = 0;
	int totpixs = 0;
	fin = fopen(filename, "rb+");

	// Si el archivo no existe
	if (fin == NULL)
		return(-1);

	// Leer encabezado
	fread(&image->header, sizeof(HEADER), 1, fin);

	// Probar si es un archivo BMP
	if ((image->header.magic1 != 'B') || (image->header.magic2 != 'M'))
		return(-1);

	fread(&image->infoheader, sizeof(INFOHEADER), 1, fin);

	// Probar si es un archivo BMP 24 bits no compactado
	if ((image->infoheader.bitsPerPixel != 24) || image->infoheader.compression)
		return(-1);

	image->pixel = (PIXEL *)malloc(sizeof(PIXEL) * image->infoheader.cols * image->infoheader.rows);
	totpixs = image->infoheader.rows * image->infoheader.cols;

	while (i < totpixs)
	{
		if (i + 512 < totpixs) 
          fread(image->pixel + i, sizeof(PIXEL), 512, fin);
        else 
          fread(image->pixel + i, sizeof(PIXEL), (i + 512) - totpixs, fin);        

        i += 512;
	}
	
	fclose(fin);
}

int saveBMP(char *filename, IMAGE *image)
{
	FILE *fout;
	int i, totpixs;

	fout = fopen(filename, "wb");
	if (fout == NULL)
		return(-1); // Error

	// Escribe encabezado
	fwrite(&image->header, sizeof(HEADER), 1, fout);

	// Escribe información del encabezado
	fwrite(&image->infoheader, sizeof(INFOHEADER), 1, fout);

	i = 0;
	totpixs = image->infoheader.rows * image->infoheader.cols;

	while (i < totpixs)
	{
		if (i + 512 < totpixs) 
          fwrite(image->pixel + i, sizeof(PIXEL), 512, fout);
        else 
          fwrite(image->pixel + i, sizeof(PIXEL), (i + 512) - totpixs, fout);        

        i += 512;
	}

	fclose(fout);
}

unsigned char blackandwhite(PIXEL p)
{
	return ((unsigned char) (0.3 * ((float)p.red) + 0.59 * ((float)p.green) + 0.11 * ((float)p.blue)));
}

void *processBMP(void *arg)
{
	int i, j;
	int tid = *((int *)arg);
	PIXEL *pfte, *pdst;
	PIXEL *v0, *v1, *v2, *v3, *v4, *v5, *v6, *v7;
	int imageRows, imageCols;
	unsigned char bandw;
	int slice;

	imageRows = imagenfte.infoheader.rows;
	imageCols = imagenfte.infoheader.cols;
	imagendst.pixel = (PIXEL *)malloc(sizeof(PIXEL) * imageRows * imageCols);
	
	slice = (imageRows - 1) / NUMTHREADS;

	pfte = imagenfte.pixel + imageCols + 1;
	v0 = pfte - imageCols - 1;
	v1 = pfte - imageCols;
	v5 = pfte + imageCols - 1;
	v6 = pfte + imageCols;

	for (i = (slice * tid) + 1; i <= (slice * (tid + 1)) && i < imageRows - 1; i++)
	{	
		for (j = 1; j < imageCols - 1; j++)
		{			
			pfte = imagenfte.pixel + imageCols * i + j;
			v3 = pfte - 1;
			v4 = pfte + 1;

			if (j % 3 == 1)
			{
				v2 = pfte - imageCols + 1;
				v7 = pfte + imageCols + 1;
			}
			else if (j % 3 == 2)
			{
				v0 = pfte - imageCols + 1;
				v5 = pfte + imageCols + 1;
			}
			else
			{
				v1 = pfte - imageCols + 1;
				v6 = pfte + imageCols + 1;
			}

			pdst = imagendst.pixel + imageCols * i + j;
			bandw = blackandwhite(*pfte);
			
			if (abs(bandw - blackandwhite(*v0)) > DIF ||
				abs(bandw - blackandwhite(*v1)) > DIF ||
				abs(bandw - blackandwhite(*v2)) > DIF ||
				abs(bandw - blackandwhite(*v3)) > DIF ||
				abs(bandw - blackandwhite(*v4)) > DIF ||
				abs(bandw - blackandwhite(*v5)) > DIF ||
				abs(bandw - blackandwhite(*v6)) > DIF ||
				abs(bandw - blackandwhite(*v7)) > DIF)
			{
				pdst->red = 0;
				pdst->green = 0;
				pdst->blue = 0;
			}
			else
			{
				pdst->red = 255;
				pdst->green = 255;
				pdst->blue = 255;
			}
		}
	}
}

int main()
{
	clock_t t_inicial, t_final;
	int i;
	int tids[NUMTHREADS];
	pthread_t threads[NUMTHREADS];
	t_inicial = clock();

	printf("Archivo fuente %s\n", filename);
	printf("Archivo destino %s\n", namedest);

	if (loadBMP(filename, &imagenfte) == -1)
	{
		fprintf(stderr, "Error al abrir imagen\n");
		exit(1);
	}

	// Creación de hilos para procesar la imagen
	printf("Procesando imagen de: Renglones = %d, Columnas = %d\n", imagenfte.infoheader.rows, imagenfte.infoheader.cols);	
	memcpy(&imagendst, &imagenfte, sizeof(IMAGE) - sizeof(PIXEL *));

	for (i = 0; i < NUMTHREADS; i++)
	{
		tids[i] = i;
		pthread_create(&threads[i], NULL, processBMP, (void *) &tids[i]);
	}

	for (i = 0; i < NUMTHREADS; i++)
		pthread_join(threads[i], NULL);

	if (saveBMP(namedest, &imagendst) == -1)
	{
		fprintf(stderr, "Error al escribir imagen\n");
		exit(1);
	}

	t_final = clock();

	printf("Tiempo %3.6f segundos\n", ((float)t_final - (float)t_inicial) / CLOCKS_PER_SEC);
}