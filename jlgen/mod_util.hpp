/*
 * $Id$
 *
 * General utility routines.
 *
 * Copyright (c) 2019-2020 Michael G. Binz
 */

#pragma once

#include <codecvt>
#include <string>

namespace smack {
namespace util {

inline std::string convert(std::wstring toConvert)
{
    // C4996 'std::codecvt_utf8<wchar_t,1114111,(std::codecvt_mode)0>': warning STL4017 : std::wbuffer_convert, std::wstring_convert, and the <codecvt> header(containing std::codecvt_mode, std::codecvt_utf8, std::codecvt_utf16, and std::codecvt_utf8_utf16) are deprecated in C++17. (The std::codecvt class template is NOT deprecated.) The C++ Standard doesn't provide equivalent non-deprecated functionality; consider using MultiByteToWideChar() and WideCharToMultiByte() from <Windows.h> instead. You can define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.	jlgen	C:\Users\micbinz\svn\github\jlaunch\jlgen\mod_util.hpp
    // See https://en.cppreference.com/w/cpp/locale/wstring_convert/to_bytes
    std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt;
    return cvt.to_bytes(toConvert);
}

inline std::wstring convert(std::string toConvert)
{
    // C4996 'std::codecvt_utf8<wchar_t,1114111,(std::codecvt_mode)0>': warning STL4017 : std::wbuffer_convert, std::wstring_convert, and the <codecvt> header(containing std::codecvt_mode, std::codecvt_utf8, std::codecvt_utf16, and std::codecvt_utf8_utf16) are deprecated in C++17. (The std::codecvt class template is NOT deprecated.) The C++ Standard doesn't provide equivalent non-deprecated functionality; consider using MultiByteToWideChar() and WideCharToMultiByte() from <Windows.h> instead. You can define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.	jlgen	C:\Users\micbinz\svn\github\jlaunch\jlgen\mod_util.hpp
    // See https://en.cppreference.com/w/cpp/locale/wstring_convert/to_bytes
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> cvt;
    return cvt.from_bytes(toConvert);
}

template<typename T>
void rawAppend(
    std::vector<uint8_t>& v,
    const T* value)
{
    size_t size =
        sizeof(*value);
    uint8_t* pointer =
        (uint8_t*)value;
    for (int i = 0; i < size; ++i)
        v.push_back(pointer[i]);
}

template<typename T>
void rawAppend(
    std::vector<uint8_t>& v,
    const T* value,
    size_t size)
{
    uint8_t* pointer = 
        (uint8_t*)value;
    for (int i = 0; i < size; ++i)
        v.push_back(pointer[i]);
}

} // namespace util
} // namespace smack
