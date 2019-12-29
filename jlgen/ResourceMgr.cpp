
#include <iostream>
#include <string>

#include "ResourceMgr.h"

namespace mob
{

ResourceMgr::ResourceMgr(std::wstring exeFileName) {
    handle_ = BeginUpdateResource(
        exeFileName.c_str(),
        TRUE);
    // TODO error handling exc
    std::cout << "handle is " << handle_ << std::endl;
}

ResourceMgr::~ResourceMgr() {
    // TODO error handling exc
    BOOL success = EndUpdateResource(handle_, FALSE);

    std::cout << "dtor " << success << std::endl;

    handle_ = nullptr;
}

/**
 * Update a string resource.
 */
BOOL ResourceMgr::updateString(int resourceId, std::wstring value) {
    return UpdateResource(
        handle_,
        RT_STRING,
        MAKEINTRESOURCE(resourceId),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
        (LPVOID*)value.c_str(),
        (DWORD)value.length());
    // TODO error handling exc
}

std::string get_file_string2( std::wstring name ) {
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
BOOL ResourceMgr::updateIcon(int resourceId, std::wstring iconPathName) {
    
    std::string iconBuffer =
        get_file_string2(iconPathName);

    std::cout << "Length is " << iconBuffer.length() << std::endl;

    return UpdateResource(
        handle_,
        RT_ICON,
        MAKEINTRESOURCE(resourceId),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
        (LPVOID*)iconBuffer.c_str()+22,
        (DWORD)iconBuffer.length()-22);
    // TODO error handling exc
}

/**
 * Update an icon resource.
 */
BOOL ResourceMgr::updateIcon(int resourceId, mob::Image icon) {
    return 1;
}

} // micbinz
