#define HEADS 24
#define SECTORS 17
#define CYLINDERS 80 

int vdwritesl(int seclog, char *buffer);
int vdreadsl(int seclog, char *buffer);
int vdwritesector(int drive, int head, int cylinder, int sector, int nsecs, char *buffer);
int vdreadsector(int drive, int head, int cylinder, int sector, int nsecs, char *buffer);
int getcyl(int seclog);
int getsec(int seclog);
int gethead(int seclog);