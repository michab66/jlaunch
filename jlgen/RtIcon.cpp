
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

    DWORD dwBytesRead;

    // Read the image data
    ReadFile(
        file,
        iconData_,
        entry->dwBytesInRes,
        &dwBytesRead, 
        NULL);
    // Result check TODO
}

RtIcon::~RtIcon()
{
    free(iconData_);
}

} // namespace windows
} // namespace micbinz
