#include "vdisk.h"

void performDelay(int drive, int cylinder, int sector);

int currentcyl[4] = {0, 0, 0, 0};
int currentsec[4] = {0, 0, 0, 0};

int vdwritesl(int drive, int seclog, int nsecs, char *buffer)
{
    int nsec = getsec(seclog);
    int ncyl = getcyl(seclog);
    int nhead = gethead(seclog);

    return vdwritesector(drive, nhead, ncyl, nsec, nsecs, buffer);
}

int vdreadsl(int drive, int seclog, int nsecs, char *buffer)
{
    // int drive = 0;
    int nsec = getsec(seclog);
    int ncyl = getcyl(seclog);
    int nhead = gethead(seclog);

    return vdreadsector(drive, nhead, ncyl, nsec, nsecs, buffer);
}

int vdwritesector(int drive, int head, int cylinder, int sector, int nsecs, char *buffer)
{
    char filename[20];
    int fp;
    int sl, offset;
    sprintf(filename, "disco%c.vd", (char) drive + '0');
    fp = open(filename, O_WRONLY);
    if (fp == -1)
        return ERROR;

    // Valida parámetros
    if (isValid(drive, head, cylinder, sector, nsecs) == NO)
        return ERROR;

    // Hace el retardo
    performDelay(drive, cylinder, sector);

    // Calcula la posición en el archivo
    sl = cylinder * SECTORS * HEADS + head * SECTORS + (sector - 1);
    offset = sl * 512;
    lseek(fp, offset, SEEK_SET);
    write(fp, buffer, 512 * nsecs);
    close(fp);
    return nsecs;
}

int vdreadsector(int drive, int head, int cylinder, int sector, int nsecs, char *buffer)
{
    char filename[20];
    int fp;
    int sl, offset;
    sprintf(filename, "disco%c.vd", (char) drive + '0');
    fp = open(filename, O_RDONLY);
    if (fp == -1)
        return ERROR;

    // Valida parámetros
    if (isValid(drive, head, cylinder, sector, nsecs) == NO)
        return ERROR;

    // Hace el retardo
    performDelay(drive, cylinder, sector);

    // Calcula la posición en el archivo
    sl = cylinder * SECTORS * HEADS + head * SECTORS + (sector - 1);
    offset = sl * 512;
    lseek(fp, offset, SEEK_SET);
    read(fp, buffer, 512 * nsecs);
    close(fp);
    return nsecs;
}

// ====== HELPER METHODS ======

int isValid(int drive, int head, int cylinder, int sector, int nsecs) {
    // Valida parámetros
    if (drive < 0 || drive > 3)
        return NO;

    if (head < 0 || head >= HEADS)
        return NO;

    if (cylinder < 0 || cylinder >= CYLINDERS)
        return NO;

    if (sector < 1 || sector > SECTORS)
        return NO;

    if (sector + nsecs - 1 > SECTORS)
        return NO;

    return YES;
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
