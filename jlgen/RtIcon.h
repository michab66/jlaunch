/*
 * $Id$
 *
 * Copyright (c) 2019 Michael Binz
 */
#pragma once

#include <string>

#include "winicon.h"

namespace mob
{
namespace windows
{
    /**
     * Resource type icon.
     */
    class RtIcon
    {
        LPVOID iconData_;

    public:
        RtIcon(HANDLE file, PICONDIRENTRY);
        ~RtIcon();

    };

} // namespace windows
} // namespace micbinz
