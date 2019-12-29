/*
 * $Id$
 *
 * Copyright (c) 2019 Michael Binz
 */
#pragma once

#include <string>
#include <vector>

#include "winicon.h"
#include "RtIcon.h"

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

        std::vector<RtIcon> icons_;

        RtIconGroup(PGRPICONDIR dir, std::vector<RtIcon> icons) :
            dir_(dir),
            icons_(icons) {};

        void Dump(int idx, PGRPICONDIRENTRY iconDirEntry);

    public:
        RtIconGroup(std::wstring exeName);
        RtIconGroup(int id, std::wstring exeName);
        ~RtIconGroup();

        void Dump();

        static RtIconGroup fromFile(std::wstring iconFile);
    };

} // namespace windows
} // namespace micbinz
