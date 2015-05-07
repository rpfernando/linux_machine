#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "vdisk.h" 

int currentcyl[4] = {0, 0, 0, 0};
int currentsec[4] = {0, 0, 0, 0};

int vdwritesl(int seclog, char *buffer)
{
    int drive = 0;
    int nsec = getsec(seclog);
    int ncyl = getcyl(seclog);
    int nhead = gethead(seclog);

    vdwritesector(drive, nhead, ncyl, nsec, buffer);
}

int vdreadsl(int seclog, char *buffer)
{
    int drive = 0;
    int nsec = getsec(seclog);
    int ncyl = getcyl(seclog);
    int nhead = gethead(seclog);
    
    vdreadsector(drive, nhead, ncyl, nsec, buffer);
}

int vdwritesector(int drive, int head, int cylinder, int sector, int nsecs, char *buffer)
{
    char filename[20];
    int fp;
    int sl, offset;
    sprintf(filename, "disco%c.vd", (char) drive + '0');
    fp = open(filename, O_WRONLY);
    if (fp == -1)
        return(-1);

    // Valida parámetros
    if (validate(drive, head, cylinder, sector, nsecs) == 0)
        return -1;

    // Hace el retardo
    performDelay(drive, cylinder, sector);

    // Calcula la posición en el archivo
    sl = cylinder * SECTORS * HEADS + head * SECTORS + (sector - 1);
    offset = sl * 512;
    lseek(fp, offset, SEEK_SET);
    write(fp, buffer, 512 * nsecs);
    close(fp);
    return(nsecs);
}

int vdreadsector(int drive, int head, int cylinder, int sector, int nsecs, char *buffer)
{
    char filename[20];
    int fp;
    int sl, offset;
    sprintf(filename, "disco%c.vd", (char) drive + '0');
    fp = open(filename, O_RDONLY);
    if (fp == -1)
        return(-1);
                                                                                
    // Valida parámetros
    if (validate(drive, head, cylinder, sector, nsecs) == 0)
        return -1;
                                                                                
    // Hace el retardo
    performDelay(drive, cylinder, sector);
                                                                                
    // Calcula la posición en el archivo
    sl = cylinder * SECTORS * HEADS + head * SECTORS + (sector - 1);
    offset = sl * 512;
    lseek(fp, offset, SEEK_SET);
    read(fp, buffer, 512 * nsecs);
    close(fp);
    return(nsecs);
}

// ====== HELPER METHODS ======
int validate(int drive, int head, int cylinder, int sector, int nsecs) {
    // Valida parámetros
    if (drive < 0 || drive > 3)
        return 0;

    if (head < 0 || head >= HEADS)
        return 0;

    if (cylinder < 0 || cylinder >= CYLINDERS)
        return 0;

    if (sector < 1 || sector > SECTORS)
        return 0;

    if (sector + nsecs - 1 > SECTORS)
        return 0;

    return 1;
}

void performDelay(int drive, int cylinder, int sector) {
    int timecyl, timesec;

    // Hace el retardo
    timesec = sector - currentsec[drive];
    if (timesec < 0)
        timesec += SECTORS;
    usleep(timesec * 1000);
    currentsec[drive] = sector;
                                                                                
    timecyl = abs(currentcyl[drive] - cylinder);
    usleep(timecyl * 1000);
    currentcyl[drive] = cylinder;
}

// ====== LOGICAL SECTOR TRANSLATION HELPER METHODS =======

int getcyl(int seclog)
{
    return seclog / (SECTORS * HEADS);
}

int getsec(int seclog)
{
    return (seclog % SECTORS) + 1;
}

int gethead(int seclog)
{
    return (seclog / SECTORS) % HEADS;
}
