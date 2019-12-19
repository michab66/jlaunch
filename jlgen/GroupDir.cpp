
#include <iostream>
#include <stdexcept>
#include "GroupDir.h"

GroupDir::GroupDir(std::wstring iconExe)
{
    HGLOBAL hResLoad;   // handle to loaded resource
    HMODULE hExe;       // handle to existing .EXE file
    HRSRC hRes;         // handle/ptr. to res. info. in hExe
    HANDLE hUpdateRes;  // update resource handle
    BOOL result;

    // Load the .EXE file.
    hExe = LoadLibrary(iconExe.c_str());
    if (hExe == NULL)
        throw std::invalid_argument("Could not load exe.");

    // Locate the resource in the .EXE file.
    hRes = FindResource(hExe, MAKEINTRESOURCE(312), RT_GROUP_ICON);
    if (hRes == NULL)
        throw std::invalid_argument("Could not locate icons.");

    // Load the resource into global memory.
    hResLoad = LoadResource(hExe, hRes);
    if (hResLoad == NULL)
        throw std::invalid_argument("Could not load dialog box.");

    // Lock the resource into global memory.
    dir_ = (PGRPICONDIR)LockResource(hResLoad);
    if (dir_ == NULL)
        throw std::invalid_argument("Could not lock dialog box.");
}

void GroupDir::Dump()
{
    if (dir_->idReserved != 0 || dir_->idType != 1)
    {
        std::cout <<
            "Not an ICONDIR. idReserved= " <<
            dir_->idReserved <<
            ", idType=" <<
            dir_->idType <<
            std::endl;
        return;
    }

    std::cout <<
        "ICONDIR contains " <<
        dir_->idCount <<
        " icons." <<
        std::endl;

    for (int i = 0; i < dir_->idCount; ++i)
        Dump(i, &dir_->idEntries[i]);
}

void GroupDir::Dump(int idx, PGRPICONDIRENTRY iconDirEntry)
{
    std::cout << "ICONDIRENTRY[" << idx << "] " <<
        " w=" << (int)iconDirEntry->bWidth <<
        " h=" << (int)iconDirEntry->bHeight <<
        " colorCount=" << (int)iconDirEntry->bColorCount <<
        " byteCount=" << iconDirEntry->dwBytesInRes <<
        " id=" << iconDirEntry->nId <<
        std::endl;
}