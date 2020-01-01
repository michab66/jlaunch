/*
 * $Id$
 *
 * Copyright (c) 2019 Michael Binz
 */
#pragma once

#include <string>
#include <vector>

#include "winicon.h"
#include "RtBase.h"
#include "RtIcon.h"

namespace mob
{
namespace windows
{
    /**
     * Resource type icon group.
     */
    class RtIconGroup : RtBase
    {
        RtIconGroup(RtIconGroup const&) = delete;

        // malloced
        PGRPICONDIR dir_;

        std::vector<RtIcon*> icons_;

        RtIconGroup() :
            dir_(nullptr) {};

        void Dump(int idx, PGRPICONDIRENTRY iconDirEntry);

        DWORD sizeofGroup();

    public:
        RtIconGroup(std::string iconFile);
        RtIconGroup(int resourceId, std::string exeName);
        ~RtIconGroup();

        void update(HANDLE resourceHolder, int resourceId);

        void Dump();
    };

} // namespace windows
} // namespace micbinz
