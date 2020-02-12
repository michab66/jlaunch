/*
 * $Id$
 *
 * Copyright (c) 2019-2020 Michael Binz
 */

#include <stdexcept>
#include "RtString.h"

void mob::windows::RtString::update(HANDLE resourceHolder, int resourceId)
{
    BOOLEAN result = UpdateResource(
        resourceHolder,
        RT_STRING,
        MAKEINTRESOURCE(resourceId),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
        (LPVOID*)value_.c_str(),
        // Care for the terminating \0.
        static_cast<DWORD>(value_.length() +1));
    if (result == FALSE)
        throw std::invalid_argument("Could not add resource.");
}
