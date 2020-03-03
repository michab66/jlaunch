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
    // See https://en.cppreference.com/w/cpp/locale/wstring_convert/to_bytes
    std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt;
    return cvt.to_bytes(toConvert);
}

inline std::wstring convert(std::string toConvert)
{
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
