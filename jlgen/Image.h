#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

namespace micbinz {
    class Image
    {
        HANDLE handle_;

    public:
        Image(std::wstring exeFileName);

        ~Image();
    };

} // micbinz

