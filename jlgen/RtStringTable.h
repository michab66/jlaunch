/* $Id$
 *
 * Copyright (c) 2019-2020 Michael Binz
 */

#pragma once

#include <map>
#include <string>

#include "RtBase.h"

namespace mob
{
namespace windows
{

/**
 * A string resource.
 */
class RtString : RtBase
{
    std::map<int, std::string> strings_;

public:
    RtString()
    {
    }

    void Add(int id, std::string value)
    {
        strings_[id] = value;
    }

    void update(HANDLE resourceHolder, int resourceId);
};

} // namespace windows
} // namespace mob
