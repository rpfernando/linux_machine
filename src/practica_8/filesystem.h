#include "inode.h"

// Create a new file (file remains open)
int vdcreat(char *filename, unsigned short mode);

// Open an existing file
int vdopen(char *filename, int flags);

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

VDDIR* vdopendir(char*);

struct vddirent* vdreaddir(VDDIR*);

int vdclosedir(VDDIR*);
