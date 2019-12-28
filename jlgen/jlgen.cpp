// jlgen.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdio>
#include <string>
#include <iostream>

#include "winicon.h"
#include "ResourceMgr.h"
#include "Image.h"
#include "GroupDir.h"


static void ErrorHandler( const char* msg )
{
    printf("%s\n", msg);
    exit(1);
}

static void dump(int idx, LPICONDIRENTRY iconDirEntry)
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

static void dump(LPICONDIR iconDirEntry)
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

//Update the application icon in an exe. file
//iconFile: the path of the icon file
//iconIndex: the index of the image in the icon file
//resId: resource id, you can see it when you open a exe.file with visual studio 2008
//exeFile: the path of exe. file
static void UpdateIcon(LPCTSTR iconFile, unsigned int iconIndex, unsigned int resId, LPCTSTR exeFile)
{
    // We need an ICONDIR to hold the data
    LPICONDIR pIconDir = (LPICONDIR)malloc(sizeof(ICONDIR));

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
    pIconDir = (LPICONDIR)malloc(
        sizeof(ICONDIR) + ((tmpIdCount - 1) * sizeof(ICONDIRENTRY) ));
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

    // Loop through and read in each image
    for (int i = 0; i < pIconDir->idCount; i++)
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
        if (i == iconIndex)
        {
            HGLOBAL hResLoad;   // handle to loaded resource
            HMODULE hExe;       // handle to existing .EXE file
            HRSRC hRes;         // handle/ptr. to res. info. in hExe
            HANDLE hUpdateRes;  // update resource handle
            LPVOID lpResLock;   // pointer to resource data
            BOOL result;
            //LPVOID lpResource;   // pointer to resource data
            HRSRC hResource;    // handle to FindResource 
            //HGLOBAL hMem;         // handle to LoadResource
            int nID;            // ID of resource that best fits current screen

            // Load the .EXE file that contains the dialog box you want to copy.
            hExe = LoadLibrary(exeFile);
            if (hExe == NULL)
            {
                ErrorHandler("Could not load exe.");
                return;
            }

            // Locate the dialog box resource in the .EXE file.
            hRes = FindResource(hExe, MAKEINTRESOURCE(resId), RT_GROUP_ICON);
            //hRes = FindResource(hExe, resName, RT_GROUP_ICON);
            if (hRes == NULL)
            {
                ErrorHandler("Could not locate icon.");
                return;
            }

            // Load the dialog box into global memory.
            hResLoad = LoadResource(hExe, hRes);
            if (hResLoad == NULL)
            {
                ErrorHandler("Could not load icon.");
                return;
            }

            //Lock the dialog box into global memory.
            lpResLock = LockResource(hResLoad);
            if (lpResLock == NULL)
            {
                ErrorHandler("Could not lock icon.");
                return;
            }

            // Get the identifier of the icon that is most appropriate
            // for the video display.                  
            nID = LookupIconIdFromDirectoryEx((PBYTE)lpResLock, TRUE,
                16, 16, LR_DEFAULTCOLOR);

            // Find the bits for the nID icon.               
            hResource = FindResource(hExe,
                MAKEINTRESOURCE(nID),
                MAKEINTRESOURCE(RT_ICON));
            int updateSize = SizeofResource(hExe, hResource);

            // Clean up.
            if (!FreeLibrary(hExe))
            {
                ErrorHandler("Could not free executable.");
                return;
            }

            // Open the file to which you want to add the dialog box resource.
            hUpdateRes = BeginUpdateResource(exeFile, FALSE);
            if (hUpdateRes == NULL)
            {
                ErrorHandler("Could not open file for writing.");
                return;
            }

            // update the icon.
            result = UpdateResource(hUpdateRes,    // update resource handle
                RT_ICON,                         // change icon
                MAKEINTRESOURCE(nID),         // icon id
                MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),  // neutral language
                pIconImage,                         // ptr to resource info      
                updateSize);       // size of resource info                

            if (result == FALSE)
            {
                ErrorHandler("Could not add resource.");
                return;
            }

            // Write changes to FOOT.EXE and then close it.
            if (!EndUpdateResource(hUpdateRes, FALSE))
            {
                ErrorHandler("Could not write changes to file.");
                return;
            }
        }

        // Then, free the associated memory
        free(pIconImage);
    }

    // Clean up the ICONDIR memory
    free(pIconDir);
}

#if 1
static void updateResource_1(LPCTSTR iconExe, unsigned int resId, LPCTSTR targetExe)
{
    HGLOBAL hResLoad;   // handle to loaded resource
    HMODULE hExe;       // handle to existing .EXE file
    HRSRC hRes;         // handle/ptr. to res. info. in hExe
    HANDLE hUpdateRes;  // update resource handle
    //LPVOID lpResLock;   // pointer to resource data
    BOOL result;
#define IDD_HAND_ABOUTBOX   103
#define IDD_FOOT_ABOUTBOX   110

    // Load the .EXE file that contains the dialog box you want to copy.
    hExe = LoadLibrary(iconExe);
    if (hExe == NULL)
    {
        ErrorHandler("Could not load exe.");
        return;
    }

    // Locate the dialog box resource in the .EXE file.
    hRes = FindResource(hExe, MAKEINTRESOURCE(312), RT_GROUP_ICON);
    if (hRes == NULL)
    {
        ErrorHandler("Could not locate icons.");
        return;
    }

    // Load the dialog box into global memory.
    hResLoad = LoadResource(hExe, hRes);
    if (hResLoad == NULL)
    {
        ErrorHandler("Could not load dialog box.");
        return;
    }

    // Lock the dialog box into global memory.
    PGRPICONDIR lpResLock = (PGRPICONDIR)LockResource(hResLoad);
    if (lpResLock == NULL)
    {
        ErrorHandler("Could not lock dialog box.");
        return;
    }

    dump(lpResLock);

    // Open the file to which you want to add the dialog box resource.
    hUpdateRes = BeginUpdateResource(targetExe, FALSE);
    if (hUpdateRes == NULL)
    {
        ErrorHandler("Could not open file for writing.");
        return;
    }

    DWORD sor = 
        SizeofResource(hExe, hRes);
    DWORD check = sizeof(GRPICONDIR) + sizeof(GRPICONDIRENTRY);

    // Add the dialog box resource to the update list.
    result = UpdateResource(hUpdateRes,    // update resource handle
        RT_GROUP_ICON,                         // change dialog box resource
        MAKEINTRESOURCE(resId),         // dialog box id
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),  // neutral language
        lpResLock,                         // ptr to resource info
        SizeofResource(hExe, hRes));       // size of resource info

    if (result == FALSE)
    {
        ErrorHandler("Could not add resource.");
        return;
    }

    // Write changes to FOOT.EXE and then close it.
    if (!EndUpdateResource(hUpdateRes, FALSE))
    {
        ErrorHandler("Could not write changes to file.");
        return;
    }

    // Clean up.
    if (!FreeLibrary(hExe))
    {
        ErrorHandler("Could not free executable.");
        return;
    }
}
#endif

#if 1
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
    // Open the file to which you want to add the dialog box resource.
    HANDLE hUpdateRes;  // update resource handle
    hUpdateRes = BeginUpdateResource(exeFile, FALSE);
    if (hUpdateRes == NULL)
    {
        ErrorHandler("Could not open file for writing.");
        return;
    }

    // We need an ICONDIR to hold the data
    LPICONDIR pIconDir = (LPICONDIR)malloc(sizeof(ICONDIR));

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
    pIconDir = (LPICONDIR)malloc(
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
        (pIconGrp->idEntries[i]).nId = i+1;
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
                pIconDir->idEntries[i].dwBytesInRes );       // size of resource info                

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
#endif

static void UpdateIcon_3(
    LPCTSTR iconFile,
    unsigned int resId,
    LPCTSTR exeFile)
{
    // Open the file to which you want to add the dialog box resource.
    HANDLE hUpdateRes;  // update resource handle
    hUpdateRes = BeginUpdateResource(exeFile, FALSE);
    if (hUpdateRes == NULL)
    {
        ErrorHandler("Could not open file for writing.");
        return;
    }

    // We need an ICONDIR to hold the data
    LPICONDIR pIconDir = (LPICONDIR)malloc(sizeof(ICONDIR));

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
    pIconDir = (LPICONDIR)malloc(
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
        (pIconGrp->idEntries[i]).nId = i;
    }

    dump(pIconGrp);

    // Write the group entry.
    BOOL result = UpdateResource(hUpdateRes,    // update resource handle
        RT_GROUP_ICON,                         // change dialog box resource
        MAKEINTRESOURCE(resId),         // dialog box id
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),  // neutral language
        pIconGrp,                         // ptr to resource info
        sizeofGroup(pIconGrp));       // size of resource info

    std::cout << "Wrote size: " << sizeofGroup(pIconGrp) << std::endl;

    // Write changes to FOOT.EXE and then close it.
    if (!EndUpdateResource(hUpdateRes, FALSE))
    {
        ErrorHandler("Could not write changes to file.");
        return;
    }


    // Clean up the ICONDIR memory
    free(pIconDir);
    free(pIconGrp);

    GroupDir gd{ exeFile };

    gd.Dump();
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
        UpdateIcon_2(
            iconName.c_str(),
            312,
            exeName.c_str());
        //UpdateIcon_3(
        //    iconName.c_str(),
        //    312,
        //    exeName.c_str());
    }
    catch (std::invalid_argument & e) {
        std::cout << "Error: " << e.what() << std::endl;

        return EXIT_FAILURE;
    }
}
