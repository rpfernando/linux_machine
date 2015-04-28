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


// Rutina de fallos de página
int pagefault(char *vaddress)
{
    int frame;
    int pageIn;

    // Get process page
    pageIn = (int) vaddress >> 12;
  
    // Try to get physical frame first
    frame = getPhysicalFrame();

    // Physical space was not available try to get virtual space
    if (frame == NINGUNO) {
        frame = getVirtualFrame();

        // Out of memory error
        if (frame == NINGUNO)
            return NINGUNO;

        processpagetable[pageIn].framenumber = frame;
        swap(pageIn);

        return 1; // Success
    }

    // Update page values
    processpagetable[pageIn].presente = 1;
    processpagetable[pageIn].modificado = 0;
    processpagetable[pageIn].framenumber = frame;

    return 1; // Success
}

 
// --------------- FRAME SEARCH METHODS ---------------

int getPhysicalFrame()
{
    int i;
    // Look for a free physical frame
    for (i = framesbegin; i < systemframetablesize + framesbegin; i++)
    {
        if (!systemframetable[i].assigned)
        {
            systemframetable[i].assigned = 1;
            return i;
        }
    }

    return NINGUNO;
}

int getVirtualFrame() 
{
    struct SYSTEMFRAMETABLE frame;
    int framenumber = TOTFRAMES;

    for (framenumber = PHYSICALMEMORYSIZE; framenumber < TOTFRAMES * 2; framenumber++)
    {
        readSwapFile(&frame, framenumber);
        if (emptyFrame(frame))
        {
            frame.assigned = 1;
            writeSwapFile(&frame, framenumber);
            return framenumber;
        }
    }

   return NINGUNO;
}


// --------------- SWAPPING METHODS ---------------

void swap(int pageIn) 
{
    pageOut = calcLRU(); // Use Least Recent Access to find page to swap out

    // Get the frame number of each page
    int frameIn = processpagetable[pageIn].framenumber;
    int frameOut = processpagetable[pageOut].framenumber;

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
    struct SYSTEMFRAMETABLE frameInContent;
    readSwapFile(&frameInContent, frameIn);
    write(&frameInContent, frameOut);


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
            if (inVirtualMemory(i))
                minIdx = i;

    return minIdx;
}


// --------------- HELPER METHODS ---------------

int inVirtualMemory(int page) 
{
    return !processpagetable[page].presente;
}

int dirty(int page)
{
    return processpagetable[page].modificado;
}

int emptyFrame(struct SYSTEMFRAMETABLE frame)
{
    return !frame.assigned;
}


// --------------- FILE HANDLING METHODS ---------------

void readSwapFile(struct SYSTEMFRAMETABLE *frame, int framenumber) 
{
    // Open the swap file
    FILE *file = fopen("swapfile","rb");
    
    // Change position to according page
    fseek(file, framenumber - framesbegin, SEEK_SET);

    // Write page in current position
    fread(frame, sizeof(struct SYSTEMFRAMETABLE), 1, file);

    // Close the swapfile
    fclose(file);
}

void writeSwapFile(struct SYSTEMFRAMETABLE *frame, int framenumber) 
{
    // Open the swap file
    FILE *file = fopen("swapfile","wb");

    // Change position to according page
    fseek(file, framenumber - framesbegin, SEEK_SET);

    // Write page in current position
    fwrite(frame, sizeof(struct SYSTEMFRAMETABLE), 1, file);

    // Close the swapfile
    fclose(file);
}