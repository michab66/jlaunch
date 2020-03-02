/*
 * $Id$
 *
 * Copyright (c) 2019 Michael Binz
 */

#include <stdexcept>

#include "ResourceMgr.h"
#include "RtIconGroup.h"

namespace mob
{
namespace windows
{

ResourceMgr::ResourceMgr(string exeFileName) 
{
    handle_ = BeginUpdateResource(
        exeFileName.c_str(),
        TRUE);

    if (!handle_)
        throw std::invalid_argument("BeginUpdateResource() failed.");
}

ResourceMgr::~ResourceMgr() 
{
    if ( handle_ )
        commit();
}

void ResourceMgr::commit()
{
    BOOL success = EndUpdateResource(handle_, FALSE);
    if (!success)
        throw std::invalid_argument("EndUpdateResource() failed.");

    handle_ = nullptr;
}

/**
 * Update an icon resource.
 */
void ResourceMgr::addIcon(int resourceId, RtIconGroup& icon) {
    icon.update(handle_, resourceId);
}
void ResourceMgr::addString(int resourceId, RtString& icon) {
    icon.update(handle_, resourceId);
}

} // namespace windows
} // namespace mob
