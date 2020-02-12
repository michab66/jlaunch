#pragma once

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
    std::string value_;

public:
    RtString(std::string value) : value_(value) 
    {
    }

    void update(HANDLE resourceHolder, int resourceId);
};

} // namespace windows
} // namespace mob
