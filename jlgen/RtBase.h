/*
 * $Id$
 *
 * Copyright (c) 2019-2020 Michael Binz
 */

#pragma once

#define WIN32_LEAN_AND_MEAN
#undef UNICODE
#include <windows.h>

namespace mob
{
namespace windows
{

/**
 * An abstract base class for all resource types.
 */
class RtBase
{
public:
    virtual void update( HANDLE resourceHolder, int resourceId ) = 0;
};

} // namespace windows
} // namespace micbinz
