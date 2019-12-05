
#include <iostream>
#include <string>

#include "ResourceMgr.h"

namespace micbinz
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
        value.length());
    // TODO error handling exc
}

/**
 * Update an icon resource.
 */
BOOL ResourceMgr::updateIcon(int resourceId, std::wstring iconPathName) {
    // Read the icon into a buffer.
    std::ifstream is;
    try
    {
        // Set to throw on failure
        is.exceptions(std::fstream::failbit | std::fstream::badbit);
        is.open(iconPathName);
    }
    catch (std::system_error & error)
    {
        //TODO check message of system error.
        std::cerr << "Failed to open '" << iconPathName.c_str() << "'\n" << error.code().message() << std::endl;
        return false;
    }

    std::stringstream out;
    out << is.rdbuf();
    std::string iconBuffer = out.str();

    std::cout << "Lnegth is " << iconBuffer.length() << std::endl;

    return UpdateResource(
        handle_,
        RT_ICON,
        MAKEINTRESOURCE(resourceId),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
        (LPVOID*)iconBuffer.c_str(),
        iconBuffer.length());
    // TODO error handling exc
}
} // micbinz
