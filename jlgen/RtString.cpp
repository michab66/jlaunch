/*
 * $Id$
 *
 * Copyright (c) 2019-2020 Michael Binz
 */

#include <codecvt>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>

#include "RtString.h"

template<typename T>
void bang(std::vector<uint8_t>& v, size_t size, const T* value)
{
    uint8_t* pointer = (uint8_t*)value;

    for (int i = 0; i < size; ++i)
        v.push_back(pointer[i]);
}

// https://devblogs.microsoft.com/oldnewthing/20040130-00/?p=40813
// https://binaryworld.net/Main/CodeDetail.aspx?CodeId=3778
void mob::windows::RtString::update(HANDLE resourceHolder, int resourceId)
{
    if (strings_.empty())
        return;

    int bundle1 = (strings_.begin()->first / 16) + 1;
    int bundle2 = (strings_.rbegin()->first / 16) + 1;
    if (bundle1 != bundle2)
        return; // Error.

    std::vector<uint8_t> buffer;

    for (int i = 0; i < 16; ++i)
    {
        auto idx = ((bundle1 - 1) * 16) + i;
        auto c = strings_.find( idx );

        if ( c == strings_.end() )
        {
            WORD zero = 0;
            bang(buffer, sizeof(zero), &zero);
        }
        else
        {
            WORD len = (WORD)c->second.length();
            bang(buffer, sizeof(len), &len);
            
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            std::wstring wstr = converter.from_bytes(c->second);
            const wchar_t* contents = wstr.c_str();
            bang(buffer, wstr.length() * sizeof(wchar_t), contents);
        }
    }

    BOOLEAN result = UpdateResource(
        resourceHolder,
        RT_STRING,
        MAKEINTRESOURCE(bundle1),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
        (LPVOID*)buffer.data(),
        static_cast<DWORD>(buffer.size()));
    if (result == FALSE)
        throw std::invalid_argument("Could not add resource.");

    //for (auto i = strings_.begin(); i != strings_.end(); ++i)
    //{
    //    int x = i->first;
    //    std::cout << i->first << " = " << i->second << std::endl;
    //}
}
