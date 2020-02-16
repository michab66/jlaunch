/*
 * Simplest Java Launcher.
 *
 * Copyright (c) 2019 Michael G. Binz
 *
 * LGPL
 */

#pragma once

#define WIN32_LEAN_AND_MEAN
#undef UNICODE
#include <windows.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "Image.h"
#include "RtIconGroup.h"
#include "RtStringTable.h"

namespace mob {
    // https://github.com/BillyONeal/Instalog/blob/master/LogCommon/Win32Exception.hpp

    class ResourceMgr
    {
    private:
        HANDLE handle_;

    public:
        ResourceMgr(std::string exeFileName);
        ~ResourceMgr();

        void commit();

        /**
         * Update a string resource.
         */
        void addString(int resourceId, mob::windows::RtString& string);

        /**
         * Update an icon resource.
         */
        void addIcon(int resourceId, mob::windows::RtIconGroup& icon);
    };
} // mob
