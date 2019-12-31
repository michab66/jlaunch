/*
 * Simplest Java Launcher.
 *
 * Copyright (c) 2019 Michael G. Binz
 *
 * LGPL
 */

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "Image.h"
#include "RtIconGroup.h"

namespace mob {
    // https://github.com/BillyONeal/Instalog/blob/master/LogCommon/Win32Exception.hpp

    class ResourceMgr
    {
    private:
        HANDLE handle_;

    public:
        ResourceMgr(std::wstring exeFileName);
        ~ResourceMgr();

        void commit();

        /**
         * Update a string resource.
         */
        BOOL updateString(int resourceId, std::wstring value);

        /**
         * Update an icon resource.
         */
        void addIcon(int resourceId, mob::windows::RtIconGroup icon);
        BOOL updateIcon(int resourceId, std::wstring iconPathName);


    };
} // micbinz
