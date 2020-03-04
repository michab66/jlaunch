/* $Id$
 *
 * Copyright (c) 2019 Michael Binz
 */

#pragma once

#include <string>

#include "winicon.h"
#include "RtBase.h"

namespace mob
{
namespace windows
{
    /**
     * Resource type icon.
     */
    class RtIcon : RtBase
    {
        GRPICONDIRENTRY directoryEntry_{};
        DWORD dataSize_ = 0;
        LPVOID iconData_ = 0;

    public:
        RtIcon(
            BYTE w,
            BYTE h,
            WORD bpp,
            std::vector<std::uint8_t> png);
        ~RtIcon();

        void update(HANDLE resourceHolder, int resourceId);

        GRPICONDIRENTRY GetDirectoryEntry()
        {
            return directoryEntry_;
        }
    };

} // namespace windows
} // namespace micbinz
