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

static void UpdateIcon(
    LPCTSTR iconFile,
    unsigned int resId,
    LPCTSTR exeFile)
{
    HANDLE hUpdateRes;  // update resource handle
    hUpdateRes = BeginUpdateResource(exeFile, FALSE);
    if (hUpdateRes == NULL)
    {
        ErrorHandler("Could not open file for writing.");
        return;
    }

    // We need an ICONDIR to hold the data
    PICONDIR pIconDir = (PICONDIR)malloc(sizeof(ICONDIR));

    HANDLE hFile = CreateFile(iconFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        ErrorHandler("Load file error!");
        return;
    }

    DWORD dwBytesRead;

    // Read the Reserved word
    WORD tmpReserved;
    ReadFile(hFile, &tmpReserved, sizeof(WORD), &dwBytesRead, NULL);
    // Read the Type word - make sure it is 1 for icons
    WORD tmpIdType;
    ReadFile(hFile, &tmpIdType, sizeof(WORD), &dwBytesRead, NULL);
    // Read the count - how many images in this file?
    WORD tmpIdCount;
    ReadFile(hFile, &tmpIdCount, sizeof(WORD), &dwBytesRead, NULL);
    // Allocate IconDir so that idEntries has enough room for idCount elements
    pIconDir = (PICONDIR)malloc(
        sizeof(ICONDIR) + ((tmpIdCount - 1) * sizeof(ICONDIRENTRY)));
    pIconDir->idReserved = 0;
    pIconDir->idType = tmpIdType;
    pIconDir->idCount = tmpIdCount;
    // Read the ICONDIRENTRY elements
    ReadFile(
        hFile,
        pIconDir->idEntries,
        pIconDir->idCount * sizeof(ICONDIRENTRY),
        &dwBytesRead,
        NULL);

    dump(pIconDir);

    PGRPICONDIR pIconGrp = (PGRPICONDIR)malloc(sizeof(GRPICONDIR) + ((pIconDir->idCount - 1) * sizeof(GRPICONDIRENTRY)));
    pIconGrp->idReserved = pIconDir->idReserved;
    pIconGrp->idCount = pIconDir->idCount;
    pIconGrp->idType = pIconDir->idType;

    for (int i = 0; i < pIconGrp->idCount; ++i)
    {
        (pIconGrp->idEntries[i]).bWidth = (pIconDir->idEntries[i]).bWidth;
        (pIconGrp->idEntries[i]).bHeight = (pIconDir->idEntries[i]).bHeight;
        (pIconGrp->idEntries[i]).bColorCount = (pIconDir->idEntries[i]).bColorCount;
        (pIconGrp->idEntries[i]).bReserved = (pIconDir->idEntries[i]).bReserved;
        (pIconGrp->idEntries[i]).wPlanes = (pIconDir->idEntries[i]).wPlanes;
        (pIconGrp->idEntries[i]).wBitCount = (pIconDir->idEntries[i]).wBitCount;
        (pIconGrp->idEntries[i]).dwBytesInRes = (pIconDir->idEntries[i]).dwBytesInRes;
        // Note that we must not generate zero-based ids.
        (pIconGrp->idEntries[i]).nId = i + 1;
    }

    dump(pIconGrp);

    // Write the group entry.
    BOOL result = UpdateResource(hUpdateRes,    // update resource handle
        RT_GROUP_ICON,                         // change dialog box resource
        MAKEINTRESOURCE(resId),         // dialog box id
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),  // neutral language
        pIconGrp,                         // ptr to resource info
        sizeofGroup(pIconGrp));       // size of resource info

    // Loop through and read in each image
    for (WORD i = 0; i < pIconDir->idCount; i++)
    {
        // Allocate memory to hold the image
        LPVOID pIconImage = malloc(pIconDir->idEntries[i].dwBytesInRes);
        // Seek to the location in the file that has the image
        SetFilePointer(hFile, pIconDir->idEntries[i].dwImageOffset,
            NULL, FILE_BEGIN);
        // Read the image data
        ReadFile(hFile, pIconImage, pIconDir->idEntries[i].dwBytesInRes,
            &dwBytesRead, NULL);
        // Here, pIconImage is an ICONIMAGE structure.
        //update resource in exe
        {
            BOOL result;

            std::cout <<
                "Adding icon " <<
                (int)i <<
                " size is: " <<
                (int)(pIconDir->idEntries[i].dwBytesInRes) <<
                std::endl;

            // update the icon.
            result = UpdateResource(
                hUpdateRes,    // update resource handle
                RT_ICON,                         // change icon
                MAKEINTRESOURCE(pIconGrp->idEntries[i].nId),         // icon id
                MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),  // neutral language
                pIconImage,                         // ptr to resource info      
                pIconDir->idEntries[i].dwBytesInRes);       // size of resource info                

            if (result == FALSE)
            {
                ErrorHandler("Could not add resource.");
                return;
            }
        }

        // Then, free the associated memory
        free(pIconImage);
    }

    // Write changes to FOOT.EXE and then close it.
    if (!EndUpdateResource(hUpdateRes, FALSE))
    {
        ErrorHandler("Could not write changes to file.");
        return;
    }


    // Clean up the ICONDIR memory
    free(pIconDir);
    free(pIconGrp);
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

        //micbinz::Image icon(iconName);
        //micbinz::ResourceMgr resourceMgr( exeName );

        //std::wcout << L"Set icon to: " << iconName << std::endl;
        //std::wcout << L"Target is: " << exeName << std::endl;

        //BOOL success = 
        //    resourceMgr.updateIcon(312, iconName);
    //    std::cout << "Hello World! " << success << std::endl ;
        //UpdateIcon(iconName.c_str(), 0, 312, exeName.c_str());
    //    updateResource_1(L"C:\\cygwin64\\tmp\\jlaunch\\x64\\Debug\\jlaunchKopie.exe", 312, exeName.c_str());
        UpdateIcon(
            iconName.c_str(),
            312,
            exeName.c_str());
    }
    catch (std::invalid_argument & e) {
        std::cout << "Error: " << e.what() << std::endl;

        return EXIT_FAILURE;
    }
}
