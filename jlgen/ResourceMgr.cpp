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

ResourceMgr::ResourceMgr(std::wstring exeFileName) 
{
    handle_ = BeginUpdateResource(
        exeFileName.c_str(),
        TRUE);
    // TODO error handling exc
    std::cout << "handle is " << handle_ << std::endl;
}

ResourceMgr::~ResourceMgr() 
{
    if ( handle_ )
        commit();
}

void ResourceMgr::commit()
{
    // TODO error handling exc
    BOOL success = EndUpdateResource(handle_, FALSE);

    std::cout << "dtor " << success << std::endl;

    handle_ = nullptr;
}

/**
 * Update a string resource.
 */
BOOL ResourceMgr::updateString(int resourceId, std::wstring value) 
{
    return UpdateResource(
        handle_,
        RT_STRING,
        MAKEINTRESOURCE(resourceId),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
        (LPVOID*)value.c_str(),
        (DWORD)value.length());
    // TODO error handling exc
}

std::string get_file_string2( std::wstring name ) 
{
    std::ifstream       file(name);
    /*
        * Get the size of the file
        */
    file.seekg(0, std::ios::end);
    std::streampos          length = file.tellg();
    file.seekg(0, std::ios::beg);
    std::string       buffer(length, 0);
    file.read(&buffer[0], length);

    return buffer;
}

/**
 * Update an icon resource.
 */
void ResourceMgr::addIcon(int resourceId, mob::windows::RtIconGroup icon) {
    icon.update(handle_, resourceId);
}

} // mob
