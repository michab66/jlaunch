/*
 * Copyright Microsoft
 *
 * See The format of icon resources 
 * https://devblogs.microsoft.com/oldnewthing/20120720-00/?p=7083
 * See Part 1 - Monochrome beginnings
 * https://devblogs.microsoft.com/oldnewthing/20101018-00/?p=12513
 * See Part 2 - Now in color
 * https://devblogs.microsoft.com/oldnewthing/20101019-00/?p=12503
 * See Part 3 - Apha blended
 * https://devblogs.microsoft.com/oldnewthing/20101021-00/?p=12483
 * See Part 4 - Png images
 * https://devblogs.microsoft.com/oldnewthing/20101022-00/?p=12473
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#pragma once
#pragma pack(2)

// icon file dir entry
typedef struct ICONDIRENTRY_
{
    /* The bWidth and bHeight are the dimensions of the image.
     * Originally, the supported range was 1 through 255, but starting in
     * Windows 95 (and Windows NT 4), the value 0 is accepted as 
     * representing a width or height of 256.
     */
    BYTE        bWidth;          // Width, in pixels, of the image
    BYTE        bHeight;         // Height, in pixels, of the image
    BYTE        bColorCount;     // Number of colors in image (0 if >=8bpp)
    BYTE        bReserved;       // Reserved ( must be 0)
    WORD        wPlanes;         // Color Planes
    WORD        wBitCount;       // Bits per pixel
    DWORD       dwBytesInRes;    // How many bytes in this resource?
    DWORD       dwImageOffset;   // Where in the file is this image?
} ICONDIRENTRY, *PICONDIRENTRY;

// icon file header
typedef struct ICONDIR_
{
    WORD           idReserved;   // Reserved (must be 0)
    WORD           idType;       // Resource Type (1 for icons)
    WORD           idCount;      // How many images?
    ICONDIRENTRY   idEntries[1]; // An entry for each image (idCount of 'em)
} ICONDIR, *PICONDIR;

// icon file image
typedef struct ICONIMAGE_
{
    BITMAPINFOHEADER icHeader;   // DIB header
    RGBQUAD         icColors[1]; // Color table
    BYTE            icXOR[1];    // DIB bits for XOR mask
    BYTE            icAND[1];    // DIB bits for AND mask
} ICONIMAGE, *PICONIMAGE;

// icon resource group directory entry
typedef struct GRPICONDIRENTRY_
{
    BYTE  bWidth;
    BYTE  bHeight;
    BYTE  bColorCount;
    BYTE  bReserved;
    WORD  wPlanes;
    WORD  wBitCount;
    DWORD dwBytesInRes;
    WORD  nId;                   // icon resource id
} GRPICONDIRENTRY, *PGRPICONDIRENTRY;

// icon resource directory entry
typedef struct GRPICONDIR_
{
    WORD idReserved;
    WORD idType;
    WORD idCount;
    GRPICONDIRENTRY idEntries[1];
} GRPICONDIR, *PGRPICONDIR;
