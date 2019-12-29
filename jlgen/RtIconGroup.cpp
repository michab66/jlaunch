
#include <iostream>
#include <stdexcept>
#include "RtIconGroup.h"

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

RtIconGroup RtIconGroup::fromFile(std::wstring iconFile)
{
    HANDLE hFile = CreateFile(
        iconFile.c_str(), 
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_SEQUENTIAL_SCAN, 
        0);

    if (hFile == INVALID_HANDLE_VALUE)
        throw std::invalid_argument("Load file error!");

    // We need an ICONDIR to hold the data
    PICONDIR pIconDir = (PICONDIR)malloc(sizeof(ICONDIR));

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

    // Convert.

    PGRPICONDIR pIconGrp =
        (PGRPICONDIR)malloc(sizeof(GRPICONDIR) + ((pIconDir->idCount - 1) * sizeof(GRPICONDIRENTRY)));
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

    std::vector<RtIcon> icons;

    // Loop through and read in each image
    for (WORD i = 0; i < pIconDir->idCount; i++)
    {
        RtIcon c{ hFile, &pIconDir->idEntries[i] };
        icons.push_back(c);
    }

    RtIconGroup result{ pIconGrp, icons };

    return result;
}

} // namespace windows
} // namespace micbinz
