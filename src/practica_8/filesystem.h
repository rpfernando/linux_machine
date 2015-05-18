#include "inode.h"

// Create a new file (file remains open)
int vdcreat(char *filename, unsigned short mode);

// Open an existing file
int vdopen(char *filename, unsigned short mode);

// Deletes a file
int vdunlink(char *filename);

// Changes position within the file
int vdseek(int fd, int offset, int whence);

// Read from an open file
int vdread(int fd, char *buffer, int size);

// Write into an open file
int vdwrite(int fd, char *buffer, int size);

// Close an open file
int vdclose(int fd);

// Open an existing dir
VDDIR* vdopendir(char*);

// Read from an open dir
struct VDDIRENT* vdreaddir(VDDIR*);

// Close an open dir
int vdclosedir(VDDIR*);
