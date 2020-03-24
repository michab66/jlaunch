/* $Id$
 *
 * Copyright (c) 2019-2020 Michael Binz
 */

#include <stdexcept>
#include "RtIconGroup.h"
#include "RtIcon.h"

namespace mob
{
namespace windows
{

RtIcon::RtIcon(BYTE w, BYTE h, WORD bpp, std::vector<std::uint8_t>& png)
{
    data_ = 
        png;
    directoryEntry_.bWidth =
        w;
    directoryEntry_.bHeight =
        h;
    directoryEntry_.wPlanes =
        1;
    directoryEntry_.wBitCount =
        32;
    directoryEntry_.dwBytesInRes =
        static_cast<DWORD>(data_.size());
}

void RtIcon::update(HANDLE resourceHolder, int resourceId)
{
    BOOLEAN result = UpdateResource(
        resourceHolder,
        RT_ICON,
        MAKEINTRESOURCE(resourceId),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
        &data_[0],
        directoryEntry_.dwBytesInRes);

    if (result == FALSE)
        throw std::invalid_argument("Could not add resource.");
}

} // namespace windows
} // namespace mob
