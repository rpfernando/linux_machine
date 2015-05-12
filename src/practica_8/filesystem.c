#include "filesystem.h"

// Create a new file (file remains open)
int vdcreat(char *filename, unsigned short mode)
{
    // TODO
}

// Open an existing file
int vdopen(char *filename, int flags)
{
    // TODO
}

// Deletes a file
int vdunlink(char *filename)
{
    // TODO
}

// Changes position within the file
int vdseek(int fd, int offset, int whence)
{
    // TODO
}

// Read from an open file
int vdread(int fd, char *buffer, int size)
{
    // TODO
}

// Write into an open file
int vdwrite(int fd, char *buffer, int size)
{
    // TODO
}

// Close an open file
int vdclose(int fd)
{
    // TODO
}