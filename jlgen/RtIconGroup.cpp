/*
 * $Id$
 *
 * Copyright (c) 2019-2020 Michael Binz
 */

#include <iostream>
#include <stdexcept>
#include <vector>

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

namespace
{
    template<typename T>
    void bang(std::vector<uint8_t>& v, size_t size, const T* value)
    {
        uint8_t* pointer = (uint8_t*)value;

        for (int i = 0; i < size; ++i)
            v.push_back(pointer[i]);
    }
}

namespace mob
{
namespace windows
{

RtIconGroup::RtIconGroup(int id, std::string executableName)
{
    // Load the executable.
    HMODULE hExe = LoadLibraryA(executableName.c_str());
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
    PGRPICONDIR dir = (PGRPICONDIR)LockResource(hResLoad);
    if (dir == NULL)
        throw std::invalid_argument("Could not lock dialog box.");

    // Perform a copy to be in-line with the icon file reader.
    int resourceSize = 
        SizeofResource(hExe, hRes);
    auto dir_ = (PGRPICONDIR)malloc(resourceSize);
    memcpy(dir_, dir, resourceSize);

    for (int i = 0; i < dir_->idCount; ++i)
    {
        RtIcon* c = new RtIcon(&dir_->idEntries[i], hExe);
        icons_.push_back(c);
    }
}

RtIconGroup::RtIconGroup(std::string iconFile)
{
    HANDLE hFile = CreateFileA(
        iconFile.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_SEQUENTIAL_SCAN,
        0);

    if (hFile == INVALID_HANDLE_VALUE)
        throw std::invalid_argument("Load file error!");

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

    // We need an ICONDIR to hold the data
    // Allocate IconDir so that idEntries has enough room for idCount elements
    PICONDIR pIconDir = (PICONDIR)malloc(
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

    auto dir_ =
        (PGRPICONDIR)malloc(sizeof(GRPICONDIR) + ((pIconDir->idCount - 1) * sizeof(GRPICONDIRENTRY)));
    dir_->idReserved = pIconDir->idReserved;
    dir_->idCount = pIconDir->idCount;
    dir_->idType = pIconDir->idType;

    for (int i = 0; i < dir_->idCount; ++i)
    {
        (dir_->idEntries[i]).bWidth = (pIconDir->idEntries[i]).bWidth;
        (dir_->idEntries[i]).bHeight = (pIconDir->idEntries[i]).bHeight;
        (dir_->idEntries[i]).bColorCount = (pIconDir->idEntries[i]).bColorCount;
        (dir_->idEntries[i]).bReserved = (pIconDir->idEntries[i]).bReserved;
        (dir_->idEntries[i]).wPlanes = (pIconDir->idEntries[i]).wPlanes;
        (dir_->idEntries[i]).wBitCount = (pIconDir->idEntries[i]).wBitCount;
        (dir_->idEntries[i]).dwBytesInRes = (pIconDir->idEntries[i]).dwBytesInRes;
        // Note that we must not generate zero-based ids.
        (dir_->idEntries[i]).nId = i + 1;
    }

    dump(dir_);

    // Loop through and read in each image
    for (WORD i = 0; i < pIconDir->idCount; i++)
    {
        RtIcon* c = new RtIcon{ 
            hFile,
            pIconDir->idEntries[i].dwImageOffset,
            &dir_->idEntries[i] };
        icons_.push_back(c);
    }
}

RtIconGroup::~RtIconGroup()
{
}

DWORD RtIconGroup::sizeofGroup()
{
    auto result =
        sizeof(GRPICONDIR);
    result +=
        (icons_.size() - 1) *
        sizeof(GRPICONDIRENTRY);
    return static_cast<DWORD>(result);
}

void RtIconGroup::update(HANDLE resourceHolder, int resourceId)
{
    std::vector<uint8_t> data;

    GRPICONDIR dir{ 0, 1, static_cast<WORD>(icons_.size()) };

    bang(
        data,
        sizeof( dir) - sizeof(dir.idEntries),
        &dir );

    int idCounter = 0;
    for (auto& c : icons_)
    {
        auto dirEntry =
            c->GetDirectoryEntry();
        dirEntry.nId = 
            ++idCounter;
        bang(
            data,
            sizeof(dirEntry),
            &dirEntry);
    }

    BOOL result = UpdateResource(
        resourceHolder,
        RT_GROUP_ICON,
        MAKEINTRESOURCE(resourceId),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
        // Resource data.
        &data[0],
        // Resource size.
        static_cast<DWORD>(data.size()));

    if (!result)
        throw std::invalid_argument("Update failed.");

    int idCount = 0;
    for (auto& c : icons_)
        c->update(resourceHolder, ++idCount);
}

void RtIconGroup::Dump()
{
    size_t idx = 0;
    for (auto& c : icons_)
    {
        auto dirEntry = c->GetDirectoryEntry();
        Dump(idx++, &dirEntry);
    }
}

void RtIconGroup::Dump(size_t idx, PGRPICONDIRENTRY iconDirEntry)
{
    std::cout << "ICONDIRENTRY[" << idx << "] " <<
        " w=" << (int)iconDirEntry->bWidth <<
        " h=" << (int)iconDirEntry->bHeight <<
        " colorCount=" << (int)iconDirEntry->bColorCount <<
        " byteCount=" << iconDirEntry->dwBytesInRes <<
        " id=" << iconDirEntry->nId <<
        std::endl;
}

} // namespace windows
} // namespace mob
