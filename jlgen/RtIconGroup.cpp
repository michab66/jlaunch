
#include <iostream>
#include <stdexcept>
#include "RtIconGroup.h"

namespace mob
{
namespace windows
{

RtIconGroup::RtIconGroup(std::wstring executableName)
{
    // Load the executable.
    HMODULE hExe = LoadLibrary(executableName.c_str());
    if (hExe == NULL)
        throw std::invalid_argument("Could not load exe.");

    // Locate the resource in the .EXE file.
    HRSRC hRes = FindResource(hExe, MAKEINTRESOURCE(312), RT_GROUP_ICON);
    if (hRes == NULL)
        throw std::invalid_argument("Could not locate icons.");

    // Load the resource.
    HGLOBAL hResLoad = LoadResource(hExe, hRes);
    if (hResLoad == NULL)
        throw std::invalid_argument("Could not load dialog box.");

    // Lock the resource into global memory.
    dir_ = (PGRPICONDIR)LockResource(hResLoad);
    if (dir_ == NULL)
        throw std::invalid_argument("Could not lock dialog box.");
}

RtIconGroup::RtIconGroup(int id, std::wstring executableName)
{
    // Load the executable.
    HMODULE hExe = LoadLibrary(executableName.c_str());
    if (hExe == NULL)
        throw std::invalid_argument("Could not load exe.");

    // Locate the resource in the .EXE file.
    HRSRC hRes = FindResource(hExe, MAKEINTRESOURCE(id), RT_GROUP_ICON);
    if (hRes == NULL)
        throw std::invalid_argument("Could not locate icons.");

    // Load the resource.
    HGLOBAL hResLoad = LoadResource(hExe, hRes);
    if (hResLoad == NULL)
        throw std::invalid_argument("Could not load dialog box.");

    // Lock the resource into global memory.
    dir_ = (PGRPICONDIR)LockResource(hResLoad);
    if (dir_ == NULL)
        throw std::invalid_argument("Could not lock dialog box.");
}

RtIconGroup::~RtIconGroup()
{

}

void RtIconGroup::Dump()
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

void RtIconGroup::Dump(int idx, PGRPICONDIRENTRY iconDirEntry)
{
    std::cout << "ICONDIRENTRY[" << idx << "] " <<
        " w=" << (int)iconDirEntry->bWidth <<
        " h=" << (int)iconDirEntry->bHeight <<
        " colorCount=" << (int)iconDirEntry->bColorCount <<
        " byteCount=" << iconDirEntry->dwBytesInRes <<
        " id=" << iconDirEntry->nId <<
        std::endl;
}

RtIconGroup RtIconGroup::fromFile()
{
    return RtIconGroup( L"bad" );
}

} // namespace windows
} // namespace micbinz
