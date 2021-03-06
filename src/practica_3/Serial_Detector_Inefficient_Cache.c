#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

#define DIF 16
// NOMBRE DEL ARCHIVO A PROCESAR
char filename[]="ferrari.bmp";
char namedest[]="ferrari_P.bmp";

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
IMAGE src_image, dst_image;

// Load the file in bmp format
int loadBMP(char *filename, IMAGE *image)
{
	FILE *fin;
	int i = 0;
	int totpixs = 0;
	fin = fopen(filename, "rb+");

	// If file doesn't exist
	if (fin == NULL)
		return(-1);

	// Load header
	fread(&image->header, sizeof(HEADER), 1, fin);

	// Check to see if valid bmp format
	if ((image->header.magic1 != 'B') || (image->header.magic2 != 'M'))
		return(-1);

	// Load header info
	fread(&image->infoheader, sizeof(INFOHEADER), 1, fin);

	// Check to see if format is 24 bits not compressed
	if ((image->infoheader.bitsPerPixel != 24) || image->infoheader.compression)
		return(-1);

	image->pixel = (PIXEL *)malloc(sizeof(PIXEL) * image->infoheader.cols * image->infoheader.rows);
	totpixs = image->infoheader.rows * image->infoheader.cols;

	// Load pixels
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

// Save the file in bmp format
int saveBMP(char *filename, IMAGE *image)
{
	FILE *fout;
	int i = 0, totpixs = 0;

	fout = fopen(filename, "wb");

	// If file doesn't exist
	if (fout == NULL)
		return(-1); // Error

	// Write header
	fwrite(&image->header, sizeof(HEADER), 1, fout);

	// Write header info
	fwrite(&image->infoheader, sizeof(INFOHEADER), 1, fout);

	totpixs = image->infoheader.rows * image->infoheader.cols;

	// Write pixels
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

// Change to gray scale
unsigned char blackandwhite(PIXEL p)
{
	return ((unsigned char) (0.3 * ((float)p.red) + 0.59 * ((float)p.green) + 0.11 * ((float)p.blue)));
}

// Find borders using neighborhood contrast algorithm
void processBMP(IMAGE *src_image, IMAGE *dst_image)
{
	int i, j;					// Used in "for" loops
	PIXEL *psrc, *pdst;			// Pixel pointers
	PIXEL *v0, *v1, *v2, *v3, *v4, *v5, *v6, *v7;	// Neighbors
	
	unsigned char gray_psrc;	// Source pixel changed to gray scale

	// Image size
	int imageRows = src_image->infoheader.rows;
	int imageCols = src_image->infoheader.cols;	
	
	// Initiliaze destination image
	memcpy(dst_image, src_image, sizeof(IMAGE) - sizeof(PIXEL *));

	imageRows = src_image->infoheader.rows;
	imageCols = src_image->infoheader.cols;
	dst_image->pixel = (PIXEL *)malloc(sizeof(PIXEL) * imageRows * imageCols);

	for (i = 1; i < imageRows - 1; i++)
	{	
		for (j = 1; j < imageCols - 1; j++)
		{			
			psrc = src_image->pixel + imageCols * i + j;
			v0 = psrc - imageCols-1;
			v1 = psrc - imageCols;
			v2 = psrc - imageCols+1;
			v3 = psrc - 1;
			v4 = psrc + 1;
			v5 = psrc + imageCols - 1;
			v6 = psrc + imageCols;
			v7 = psrc + imageCols + 1;
			pdst = dst_image->pixel + imageCols * i + j;

			// Detect contrast against neighbors
			if (abs(blackandwhite(*psrc) - blackandwhite(*v0)) > DIF ||
				abs(blackandwhite(*psrc) - blackandwhite(*v1)) > DIF ||
				abs(blackandwhite(*psrc) - blackandwhite(*v2)) > DIF ||
				abs(blackandwhite(*psrc) - blackandwhite(*v3)) > DIF ||
				abs(blackandwhite(*psrc) - blackandwhite(*v4)) > DIF ||
				abs(blackandwhite(*psrc) - blackandwhite(*v5)) > DIF ||
				abs(blackandwhite(*psrc) - blackandwhite(*v6)) > DIF ||
				abs(blackandwhite(*psrc) - blackandwhite(*v7)) > DIF)
			{
				// Border found
				pdst->red = 0;
				pdst->green = 0;
				pdst->blue = 0;
			}
			else
			{
				// No border found
				pdst->red = 255;
				pdst->green = 255;
				pdst->blue = 255;
			}
		}
	}
}

int main()
{
	clock_t initial_t, final_t;		// Variable to count running time
	
	// Start clock
	initial_t = clock();			

	printf("Archivo fuente %s\n", filename);
	printf("Archivo destino %s\n", namedest);

	// Load the image in bmp format
	if (loadBMP(filename, &src_image) == -1)
	{
		fprintf(stderr, "Error al abrir imagen\n");
		exit(1);
	}

	// Image size variables
	int imageRows = src_image.infoheader.rows;
	int imageCols = src_image.infoheader.cols;

	printf("Procesando imagen de: Renglones = %d, Columnas = %d\n", imageRows, imageCols);	
	processBMP(&src_image, &dst_image);

	// Save the image in bmp format	
	if (saveBMP(namedest, &dst_image) == -1)
	{
		fprintf(stderr, "Error al escribir imagen\n");
		exit(1);
	}

	// Stop clock
	final_t = clock();

	printf("Tiempo %3.6f segundos\n", ((float)final_t - (float)initial_t) / CLOCKS_PER_SEC);
    return 0;
}
