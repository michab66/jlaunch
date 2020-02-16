#pragma once

#include <map>
#include <string>

#include "RtBase.h"

namespace mob
{
namespace windows
{

/**
 * A binary resource.
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
