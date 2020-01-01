/*
 * $Id$
 *
 * Copyright (c) 2019 Michael Binz
 */
#include <iostream>
#include <string>

#include "ResourceMgr.h"
#include "RtIconGroup.h"

namespace mob
{

ResourceMgr::ResourceMgr(std::string exeFileName) 
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
void ResourceMgr::addIcon(int resourceId, mob::windows::RtIconGroup& icon) {
    icon.update(handle_, resourceId);
}

} // mob
