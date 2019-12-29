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
     * Resource type icon group.
     */
    class RtIconGroup
    {
        PGRPICONDIR dir_;

        void Dump(int idx, PGRPICONDIRENTRY iconDirEntry);

    public:
        RtIconGroup(std::wstring exeName);
        RtIconGroup(int id, std::wstring exeName);
        ~RtIconGroup();

        void Dump();

        static RtIconGroup fromFile();
    };

} // namespace windows
} // namespace micbinz
