
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma once

#pragma pack(2)

// See https://devblogs.microsoft.com/oldnewthing/20120720-00/?p=7083

//icon file dir entry
typedef struct
{
    BYTE        bWidth;          // Width, in pixels, of the image
    BYTE        bHeight;         // Height, in pixels, of the image
    BYTE        bColorCount;     // Number of colors in image (0 if >=8bpp)
    BYTE        bReserved;       // Reserved ( must be 0)
    WORD        wPlanes;         // Color Planes
    WORD        wBitCount;       // Bits per pixel
    DWORD       dwBytesInRes;    // How many bytes in this resource?
    DWORD       dwImageOffset;   // Where in the file is this image?
} ICONDIRENTRY, * LPICONDIRENTRY;


//icon file header
typedef struct
{
    WORD           idReserved;   // Reserved (must be 0)
    WORD           idType;       // Resource Type (1 for icons)
    WORD           idCount;      // How many images?
    ICONDIRENTRY   idEntries[1]; // An entry for each image (idCount of 'em)
} ICONDIR, * LPICONDIR;

//icon file image
typedef struct
{
    BITMAPINFOHEADER   icHeader;      // DIB header
    RGBQUAD         icColors[1];   // Color table
    BYTE            icXOR[1];      // DIB bits for XOR mask
    BYTE            icAND[1];      // DIB bits for AND mask
} ICONIMAGE, * LPICONIMAGE;

typedef struct
{
    BYTE  bWidth;
    BYTE  bHeight;
    BYTE  bColorCount;
    BYTE  bReserved;
    WORD  wPlanes;
    WORD  wBitCount;
    DWORD dwBytesInRes;
    WORD  nId;
} GRPICONDIRENTRY, * PGRPICONDIRENTRY;

typedef struct
{
    WORD idReserved;
    WORD idType;
    WORD idCount;
    GRPICONDIRENTRY idEntries[1];
} GRPICONDIR, * PGRPICONDIR;
