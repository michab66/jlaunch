/*
 * $Id$
 *
 * Copyright (c) 2019-2020 Michael Binz
 */

#include <iostream>
#include <stdexcept>
#include "RtIconGroup.h"
#include "RtIcon.h"

namespace mob
{
namespace windows
{

RtIcon::RtIcon(HANDLE file, PICONDIRENTRY entry)
{
    // Allocate memory to hold the image
    iconData_ = malloc(
        entry->dwBytesInRes);
    // Seek to the location in the file that has the image
    SetFilePointer(
        file, 
        entry->dwImageOffset,
        NULL, 
        FILE_BEGIN);

    // Read the image data
    ReadFile(
        file,
        iconData_,
        entry->dwBytesInRes,
        &dataSize_, 
        NULL);
    // Result check TODO
}

RtIcon::RtIcon(int id, HMODULE module)
{
    // Locate the resource in the .EXE file.
    HRSRC hRes = FindResource(
        module, 
        MAKEINTRESOURCE(id), 
        RT_ICON);
    if (hRes == NULL)
        throw std::invalid_argument("Could not locate icons.");

    // Load the resource.
    HGLOBAL hResLoad = LoadResource(module, hRes);
    if (hResLoad == NULL)
        throw std::invalid_argument("Could not load resource.");

    // Lock the resource into global memory.
    PGRPICONDIR dir = (PGRPICONDIR)LockResource(hResLoad);
    if (dir == NULL)
        throw std::invalid_argument("Could not lock resource.");

    // Perform a copy to be in-line with the icon file reader.
    int resourceSize =
        SizeofResource(module, hRes);
    iconData_ = malloc(resourceSize);
    memcpy(iconData_, dir, resourceSize);
}

RtIcon::~RtIcon()
{
    free(iconData_);
}

void RtIcon::update(HANDLE resourceHolder, int resourceId)
{
    BOOLEAN result = UpdateResource(
        resourceHolder,
        RT_ICON,                         // change icon
        MAKEINTRESOURCE(resourceId),         // icon id
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),  // neutral language
        iconData_,                         // ptr to resource info      
        dataSize_);       // size of resource info                

    if (result == FALSE)
        throw std::invalid_argument("Could not add resource.");
}

} // namespace windows
} // namespace micbinz
