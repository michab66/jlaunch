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
    class RtGroupDir
    {
        PGRPICONDIR dir_;

        void Dump(int idx, PGRPICONDIRENTRY iconDirEntry);

    public:
        RtGroupDir(std::wstring exeName);
        RtGroupDir(int id, std::wstring exeName);
        ~RtGroupDir();

        void Dump();

        static RtGroupDir fromFile();
    };

} // namespace windows
} // namespace micbinz
