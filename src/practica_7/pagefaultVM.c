#include <stdio.h>
#include "mmu.h"

#define RESIDENTSETSIZE 24

extern char *base;
extern int framesbegin;
extern int idproc;
extern int systemframetablesize;
extern int processpagetablesize;

extern struct SYSTEMFRAMETABLE *systemframetable;
extern struct PROCESSPAGETABLE processpagetable[];

int pagefault(char *vaddress);

// FRAME SEARCH METHODS
int getPhysicalFrame();
int getVirtualFrame();

// SWAPPING METHODS
void performSwap(int pageIn);
int calcLRU();

// HELPER METHODS
int inPhysicalMemory(int page);
int dirty(int page);

// FILE HANDLING METHODS
void readSwapFile(struct SYSTEMFRAMETABLE *frame, int framenumber);
void writeSwapFile(struct SYSTEMFRAMETABLE *frame, int framenumber);


// --------------- PAGE FAULT ROUTINE ---------------
int pagefault(char *vaddress)
{
    int frame;
    int pageIn;

    // Get process page
    pageIn = (int) vaddress >> 12;

    // Try to get physical frame first
    if (countframesassigned() < FRAMESPROC)
    {
        frame = getPhysicalFrame();

        // Out of memory error
        if (frame == NINGUNO)
            return NINGUNO;

        processpagetable[pageIn].framenumber = frame;
    }
    // Physical space was not available try to get virtual space
    else
    {
        // Has no virtual frame assigned, get a free one
        if (!hasVirtualFrame(pageIn))
        {
            frame = getVirtualFrame();

            // Out of memory error
            if (frame == NINGUNO)
                return NINGUNO;

            processpagetable[pageIn].framenumber = frame;
        }

        performSwap(pageIn);
    }

    // Update page values
    processpagetable[pageIn].presente = 1;

    return 1; // Success
}

 
// --------------- FRAME SEARCH METHODS ---------------

int getPhysicalFrame()
{
    int start = framesbegin;
    int end = start + systemframetablesize;

    return getFrame(start, end);
}

int getVirtualFrame() 
{
    int start = systemframetablesize + framesbegin + (idproc * FRAMESPROC);
    int end = start + FRAMESPROC;

    return getFrame(start, end);
}

int getFrame(int start, int end) {
    int i;

    for (i = start; i < end; i++)
    {
        if (!systemframetable[i].assigned)
        {
            systemframetable[i].assigned = 1;
            return i;
        }
    }

   return NINGUNO;
}

// --------------- SWAPPING METHODS ---------------

void performSwap(int pageIn)
{
    // Use Least Recent Access to find page to swap out
    int pageOut = calcLRU(); 

    // Get the frame number of each page
    int frameIn = processpagetable[pageIn].framenumber;
    int frameOut = processpagetable[pageOut].framenumber;

    /*// Read frame content from swap virtual space
    struct SYSTEMFRAMETABLE frameInContent;
    readSwapFile(&frameInContent, frameIn);

    // Swap out the old frame
    if (dirty(pageOut))
    {
        // Page was modified perform swap directly from system frame table
        writeSwapFile(&systemframetable[frameOut], frameIn);
    }
    else
    {
        // Page was not modified perform swap directly from file
        struct SYSTEMFRAMETABLE frameOutContent;
        readSwapFile(&frameOutContent, frameOut);
        writeSwapFile(&frameOutContent, frameIn);
    }

    // Swap in the new frame
    writeSwapFile(&frameInContent, frameOut);*/

    // Page were swapped, update respective values
    processpagetable[pageOut].presente = 0;
    processpagetable[pageOut].modificado = 0;
    processpagetable[pageOut].framenumber = frameIn;

    processpagetable[pageIn].presente = 1;
    processpagetable[pageIn].modificado = 0;
    processpagetable[pageIn].framenumber = frameOut;
}


int calcLRU() 
{
    int i = 0;
    int minIdx = 0; // Index of page with least recent access

    // Find page with least recent access
    for (i = 0; i < processpagetablesize; i++)
        if (processpagetable[i].tlastaccess < processpagetable[minIdx].tlastaccess)
            if (inPhysicalMemory(i))
                minIdx = i;

    return minIdx;
}


// --------------- HELPER METHODS ---------------

int inPhysicalMemory(int page) 
{
    return processpagetable[page].presente;
}

int dirty(int page)
{
    return processpagetable[page].modificado;
}

int hasVirtualFrame(int page)
{
    int min = framesbegin + systemframetablesize;
    int max = min + systemframetablesize;
    int framenumber = processpagetable[page].framenumber;

    return framenumber >= min && framenumber < max;
}


// --------------- FILE HANDLING METHODS ---------------

void readSwapFile(struct SYSTEMFRAMETABLE *frame, int framenumber) 
{
    // Open the swap file
    FILE *file = fopen("swap","rb");

    // Calculate initial position
    int position = (framenumber - framesbegin) * FRAMESIZE;

    // Change position to according frame
    fseek(file, position, SEEK_SET);

    // Read frame in current position
    fread(frame, FRAMESIZE, 1, file);

    // Close the swap file
    fclose(file);
}

void writeSwapFile(struct SYSTEMFRAMETABLE *frame, int framenumber) 
{
    // Open the swap file
    FILE *file = fopen("swap","wb");

    // Calculate initial position
    int position = (framenumber - framesbegin) * FRAMESIZE;

    // Change position to according fram
    fseek(file, position, SEEK_SET);

    // Write frame in current position
    fwrite(frame, FRAMESIZE, 1, file);

    // Close the swap file
    fclose(file);
}