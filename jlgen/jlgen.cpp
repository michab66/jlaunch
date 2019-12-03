// jlgen.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>
#include <iostream>

static void ErrorHandler( std::wstring msg )
{
    std::wcerr << msg << std::endl;

    std::exit(1);
}

static void generate()
{
    std::exit(0);

    HGLOBAL hResLoad;   // handle to loaded resource
    HMODULE hExe;       // handle to existing .EXE file
    HRSRC hRes;         // handle/ptr. to res. info. in hExe
    HANDLE hUpdateRes;  // update resource handle
    LPVOID lpResLock;   // pointer to resource data
    BOOL result;
#define IDD_HAND_ABOUTBOX   103
#define IDD_FOOT_ABOUTBOX   110

    // Load the .EXE file that contains the dialog box you want to copy.
    hExe = LoadLibrary(TEXT("hand.exe"));
    if (hExe == NULL)
    {
        ErrorHandler(TEXT("Could not load exe."));
        return;
    }

    // Locate the dialog box resource in the .EXE file.
    hRes = FindResource(hExe, MAKEINTRESOURCE(IDD_HAND_ABOUTBOX), RT_DIALOG);
    if (hRes == NULL)
    {
        ErrorHandler(TEXT("Could not locate dialog box."));
        return;
    }

    // Load the dialog box into global memory.
    hResLoad = LoadResource(hExe, hRes);
    if (hResLoad == NULL)
    {
        ErrorHandler(TEXT("Could not load dialog box."));
        return;
    }

    // Lock the dialog box into global memory.
    lpResLock = LockResource(hResLoad);
    if (lpResLock == NULL)
    {
        ErrorHandler(TEXT("Could not lock dialog box."));
        return;
    }

    // Open the file to which you want to add the dialog box resource.
    hUpdateRes = BeginUpdateResource(TEXT("foot.exe"), FALSE);
    if (hUpdateRes == NULL)
    {
        ErrorHandler(TEXT("Could not open file for writing."));
        return;
    }

    // Add the dialog box resource to the update list.
    result = UpdateResource(hUpdateRes,    // update resource handle
        RT_DIALOG,                         // change dialog box resource
        MAKEINTRESOURCE(IDD_FOOT_ABOUTBOX),         // dialog box id
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),  // neutral language
        lpResLock,                         // ptr to resource info
        SizeofResource(hExe, hRes));       // size of resource info

    if (result == FALSE)
    {
        ErrorHandler(TEXT("Could not add resource."));
        return;
    }

    // Write changes to FOOT.EXE and then close it.
    if (!EndUpdateResource(hUpdateRes, FALSE))
    {
        ErrorHandler(TEXT("Could not write changes to file."));
        return;
    }

    // Clean up.
    if (!FreeLibrary(hExe))
    {
        ErrorHandler(TEXT("Could not free executable."));
        return;
    }
}

int main()
{
    std::cout << "Hello World!\n";

}
