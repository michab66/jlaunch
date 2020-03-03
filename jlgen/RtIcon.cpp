/*
 * $Id$
 *
 * Copyright (c) 2019-2020 Michael Binz
 */

#include <iostream>
#include <stdexcept>
#include "RtIconGroup.h"
#include "RtIcon.h"

namespace mob
{
namespace windows
{

RtIcon::RtIcon(BYTE w, BYTE h, WORD bpp, std::vector<std::uint8_t> png)
{
    dataSize_ =
        static_cast<DWORD>(png.size());
    iconData_ = malloc(dataSize_);
    if (iconData_ == nullptr)
        throw new std::bad_alloc();
    memcpy(iconData_, &png[0], dataSize_);
    directoryEntry_.bWidth =
        w;
    directoryEntry_.bHeight =
        h;
    directoryEntry_.wPlanes =
        1;
    directoryEntry_.wBitCount =
        32;
    directoryEntry_.dwBytesInRes =
        dataSize_;
}

RtIcon::~RtIcon()
{
    free(iconData_);
}

void RtIcon::update(HANDLE resourceHolder, int resourceId)
{
    BOOLEAN result = UpdateResource(
        resourceHolder,
        RT_ICON,                         // change icon
        MAKEINTRESOURCE(resourceId),         // icon id
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),  // neutral language
        iconData_,                         // ptr to resource info      
        dataSize_);       // size of resource info                

    if (result == FALSE)
        throw std::invalid_argument("Could not add resource.");
}

} // namespace windows
} // namespace micbinz
