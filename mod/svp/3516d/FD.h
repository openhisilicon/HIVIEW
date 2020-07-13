#ifndef __FD_H
#define __FD_H


#pragma pack(1)
 
typedef struct{
    short type;
    int size;
    short reserved1;
    short reserved2;
    int offset;
} BMPHeader;
 
typedef struct{
    int size;
    int width;
    int height;
    short planes;
    short bitsPerPixel;
    unsigned compression;
    unsigned imageSize;
    int xPelsPerMeter;
    int yPelsPerMeter;
    int clrUsed;
    int clrImportant;
} BMPInfoHeader;
 
#pragma pack()


int saveBMPFile(unsigned char* src, int width, int height, const char* name);

int fd_start();
int fd_stop();



#endif 
