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
        // malloced
        PGRPICONDIR dir_;

        std::vector<RtIcon*> icons_;

        RtIconGroup() :
            dir_(nullptr) {};

        void Dump(int idx, PGRPICONDIRENTRY iconDirEntry);

        DWORD sizeofGroup();

    public:
        RtIconGroup(std::wstring iconFile);
        RtIconGroup(int resourceId, std::wstring exeName);
        ~RtIconGroup();

        void update(HANDLE resourceHolder, int resourceId);

        void Dump();
    };

} // namespace windows
} // namespace micbinz
