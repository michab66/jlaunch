/* $Id$
 *
 * Copyright (c) 2019 Michael Binz
 */

#pragma once

#include <filesystem>
#include <string>

#define WIN32_LEAN_AND_MEAN
#undef UNICODE
#include <windows.h>

#include "RtIconGroup.h"
#include "RtStringTable.h"

namespace mob {
namespace windows {

using std::filesystem::path;
using std::string;
using mob::windows::RtIconGroup;
using mob::windows::RtString;

/**
 * The handle to resources in an executable.
 */
class ResourceMgr
{
private:
    HANDLE handle_;

public:
    ResourceMgr(path exeFileName);
    ~ResourceMgr();

    void commit();

    /**
        * Update a string resource.
        */
    void add(int resourceId, RtString& string);

    /**
        * Update an icon resource.
        */
    void add(int resourceId, RtIconGroup& icon);
};

} // namespace windows
} // namespace mob
