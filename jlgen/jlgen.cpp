// jlgen.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdio>
#include <string>
#include <iostream>

#include "ResourceMgr.h"
#include "Image.h"


static void ErrorHandler( const char* msg )
{
    //const wchar_t* wCharString = msg.GetString();

    //size_t origsize = wcslen(wCharString) + 1;

    //size_t convertedChars = 0;

    //char* CharString;

    //CharString = new char(origsize);

    //wcstombs_s(&convertedChars, CharString, origsize, wCharString, _TRUNCATE);

    printf("%s\n", msg);
    exit(1);
}

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
    ReadFile(hFile, &(pIconDir->idReserved), sizeof(WORD), &dwBytesRead, NULL);
    // Read the Type word - make sure it is 1 for icons
    ReadFile(hFile, &(pIconDir->idType), sizeof(WORD), &dwBytesRead, NULL);
    // Read the count - how many images in this file?
    ReadFile(hFile, &(pIconDir->idCount), sizeof(WORD), &dwBytesRead, NULL);
    // Reallocate IconDir so that idEntries has enough room for idCount elements
    pIconDir = (LPICONDIR)realloc(
        pIconDir,
        (sizeof(WORD) * 3) + (sizeof(ICONDIRENTRY) * pIconDir->idCount));
    // Read the ICONDIRENTRY elements
    ReadFile(
        hFile,
        pIconDir->idEntries,
        pIconDir->idCount * sizeof(ICONDIRENTRY),
        &dwBytesRead, 
        NULL);
    free(pIconDir);
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
                32, 32, LR_DEFAULTCOLOR);

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

//static void ErrorHandler( std::wstring msg )
//{
//    std::wcerr << msg << std::endl;
//
//    std::exit(1);
//}
//
//static void generate()
//{
//    std::exit(0);
//
//    HGLOBAL hResLoad;   // handle to loaded resource
//    HMODULE hExe;       // handle to existing .EXE file
//    HRSRC hRes;         // handle/ptr. to res. info. in hExe
//    HANDLE hUpdateRes;  // update resource handle
//    LPVOID lpResLock;   // pointer to resource data
//    BOOL result;
//#define IDD_HAND_ABOUTBOX   103
//#define IDD_FOOT_ABOUTBOX   110
//
//    // Load the .EXE file that contains the dialog box you want to copy.
//    hExe = LoadLibrary(TEXT("hand.exe"));
//    if (hExe == NULL)
//    {
//        ErrorHandler(TEXT("Could not load exe."));
//        return;
//    }
//
//    // Locate the dialog box resource in the .EXE file.
//    hRes = FindResource(hExe, MAKEINTRESOURCE(IDD_HAND_ABOUTBOX), RT_DIALOG);
//    if (hRes == NULL)
//    {
//        ErrorHandler(TEXT("Could not locate dialog box."));
//        return;
//    }
//
//    // Load the dialog box into global memory.
//    hResLoad = LoadResource(hExe, hRes);
//    if (hResLoad == NULL)
//    {
//        ErrorHandler(TEXT("Could not load dialog box."));
//        return;
//    }
//
//    // Lock the dialog box into global memory.
//    lpResLock = LockResource(hResLoad);
//    if (lpResLock == NULL)
//    {
//        ErrorHandler(TEXT("Could not lock dialog box."));
//        return;
//    }
//
//    // Open the file to which you want to add the dialog box resource.
//    hUpdateRes = BeginUpdateResource(TEXT("foot.exe"), FALSE);
//    if (hUpdateRes == NULL)
//    {
//        ErrorHandler(TEXT("Could not open file for writing."));
//        return;
//    }
//
//    // Add the dialog box resource to the update list.
//    result = UpdateResource(hUpdateRes,    // update resource handle
//        RT_DIALOG,                         // change dialog box resource
//        MAKEINTRESOURCE(IDD_FOOT_ABOUTBOX),         // dialog box id
//        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),  // neutral language
//        lpResLock,                         // ptr to resource info
//        SizeofResource(hExe, hRes));       // size of resource info
//
//    if (result == FALSE)
//    {
//        ErrorHandler(TEXT("Could not add resource."));
//        return;
//    }
//
//    // Write changes to FOOT.EXE and then close it.
//    if (!EndUpdateResource(hUpdateRes, FALSE))
//    {
//        ErrorHandler(TEXT("Could not write changes to file."));
//        return;
//    }
//
//    // Clean up.
//    if (!FreeLibrary(hExe))
//    {
//        ErrorHandler(TEXT("Could not free executable."));
//        return;
//    }
//}

int wmain( int argc, wchar_t** argv )
{
    if (argc != 3)
    {
        std::cout << "Not three" << std::endl;
        return 1;
    }

    std::wstring exeName{ argv[1] };
    std::wstring iconName{ argv[2] };

    //micbinz::Image icon(iconName);
    //micbinz::ResourceMgr resourceMgr( exeName );

    //std::wcout << L"Set icon to: " << iconName << std::endl;
    //std::wcout << L"Target is: " << exeName << std::endl;

    //BOOL success = 
    //    resourceMgr.updateIcon(312, iconName);
//    std::cout << "Hello World! " << success << std::endl ;
    UpdateIcon(iconName.c_str(), 1, 312, exeName.c_str());
}
