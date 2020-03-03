/*
 * $Id$
 *
 * Copyright (c) 2019-2020 Michael Binz
 */

#include <iostream>
#include <stdexcept>
#include <vector>

#include "RtIconGroup.h"
#include "mod_util.hpp"

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

    smack::util::rawAppend(
        data,
        &dir,
        sizeof(dir) - sizeof(dir.idEntries));

    int idCounter = 0;
    for (auto& c : icons_)
    {
        auto dirEntry =
            c->GetDirectoryEntry();
        dirEntry.nId = 
            ++idCounter;
        smack::util::rawAppend(
            data,
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

} // namespace windows
} // namespace mob
