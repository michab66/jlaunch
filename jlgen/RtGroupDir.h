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
    class RtGroupDir
    {
        PGRPICONDIR dir_;

        void Dump(int idx, PGRPICONDIRENTRY iconDirEntry);

    public:
        RtGroupDir(std::wstring exeName);
        ~RtGroupDir();

        void Dump();
    };

} // namespace windows
} // namespace micbinz
