/*
 * $Id$
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
        DWORD dataSize_ = 0;
        LPVOID iconData_ = 0;

    public:
        RtIcon(HANDLE file, PICONDIRENTRY entry);
        RtIcon(int id, HMODULE module);
        ~RtIcon();

        void update(HANDLE resourceHolder, int resourceId);
    };

} // namespace windows
} // namespace micbinz
