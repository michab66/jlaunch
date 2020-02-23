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

std::string convert(std::wstring toConvert)
{
    // See https://en.cppreference.com/w/cpp/locale/wstring_convert/to_bytes
    std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt;
    return cvt.to_bytes(toConvert);
}

std::wstring convert(std::string toConvert)
{
    // See https://en.cppreference.com/w/cpp/locale/wstring_convert/to_bytes
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> cvt;
    return cvt.from_bytes(toConvert);
}

} // namespace util
} // namespace smack