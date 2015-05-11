#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "global.h"

// ======= FUNCTIONS ======

// Logic sector read/write
int vdwritesl(int drive, int seclog, int nsecs, char *buffer);
int vdreadsl(int drive, int seclog, int nsecs, char *buffer);

// Sector read/write
int vdwritesector(int drive, int head, int cylinder, int sector, int nsecs, char *buffer);
int vdreadsector(int drive, int head, int cylinder, int sector, int nsecs, char *buffer);

// Helper methods
int getcyl(int seclog);
int getsec(int seclog);
int gethead(int seclog);