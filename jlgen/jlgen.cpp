/*
 * $Id$
 *
 * Copyright (c) 2019 Michael Binz
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdio>
#include <string>
#include <iostream>

#include "winicon.h"
#include "ResourceMgr.h"
#include "Image.h"
#include "RtIconGroup.h"


static void ErrorHandler( const char* msg )
{
    printf("%s\n", msg);
    exit(1);
}

static void dump(int idx, PICONDIRENTRY iconDirEntry)
{
    std::cout << "ICONDIRENTRY[" << idx << "] " << 
        " w=" << (int)iconDirEntry->bWidth <<
        " h=" << (int)iconDirEntry->bHeight <<
        " colorCount=" << (int)iconDirEntry->bColorCount <<
        " byteCount=" << iconDirEntry->dwBytesInRes <<
        " offset=" << iconDirEntry->dwImageOffset <<
        std::endl;
}
static void dump(int idx, PGRPICONDIRENTRY iconDirEntry)
{
    std::cout << "ICONDIRENTRY[" << idx << "] " <<
        " w=" << (int)iconDirEntry->bWidth <<
        " h=" << (int)iconDirEntry->bHeight <<
        " colorCount=" << (int)iconDirEntry->bColorCount <<
        " byteCount=" << iconDirEntry->dwBytesInRes <<
        " id=" << iconDirEntry->nId <<
        std::endl;
}

static void dump(PICONDIR iconDirEntry)
{
    if (iconDirEntry->idReserved != 0 || iconDirEntry->idType != 1)
    {
        std::cout <<
            "Not an ICONDIR. idReserved= " <<
            iconDirEntry->idReserved <<
            ", idType=" <<
            iconDirEntry->idType <<
            std::endl;
        return;
    }

    std::cout <<
        "ICONDIR contains " <<
        iconDirEntry->idCount <<
        " icons." <<
        std::endl;

    for (int i = 0; i < iconDirEntry->idCount; ++i)
        dump(i, &iconDirEntry->idEntries[i]);
}

static void dump(PGRPICONDIR iconDirEntry)
{
    if (iconDirEntry->idReserved != 0 || iconDirEntry->idType != 1)
    {
        std::cout <<
            "Not an ICONDIR. idReserved= " <<
            iconDirEntry->idReserved <<
            ", idType=" <<
            iconDirEntry->idType <<
            std::endl;
        return;
    }

    std::cout <<
        "ICONDIR contains " <<
        iconDirEntry->idCount <<
        " icons." <<
        std::endl;

    for (int i = 0; i < iconDirEntry->idCount; ++i)
        dump(i, &iconDirEntry->idEntries[i]);
}

static DWORD sizeofGroup(PGRPICONDIR dir)
{
    DWORD result = 
        sizeof(GRPICONDIR);
    result += 
        (dir->idCount - 1) * 
        sizeof(GRPICONDIRENTRY);
    return result;
}

static void UpdateIcon_2(
    LPCTSTR iconFile,
    unsigned int resId,
    LPCTSTR exeFile)
{
    using mob::windows::RtIconGroup;

    mob::ResourceMgr target{ exeFile };

    RtIconGroup icon{ iconFile };

    target.addIcon(resId, icon);

    target.commit();
}

int wmain( int argc, wchar_t** argv )
{
    if (argc != 3)
    {
        std::cout << "Not three" << std::endl;
        return 1;
    }
    try
    {
        std::wstring exeName{ argv[1] };
        std::wstring iconName{ argv[2] };

        UpdateIcon_2(
            iconName.c_str(),
            312,
            exeName.c_str());
    }
    catch (std::invalid_argument & e) {
        std::cout << "Error: " << e.what() << std::endl;

        return EXIT_FAILURE;
    }
}
