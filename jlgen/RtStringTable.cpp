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

#include "RtStringTable.h"
#include "mod_util.hpp"

using std::string;
using std::wstring;

/**
 * Make a resource string table.
 * https://devblogs.microsoft.com/oldnewthing/20040130-00/?p=40813
 * https://binaryworld.net/Main/CodeDetail.aspx?CodeId=3778
 */
void mob::windows::RtString::update(HANDLE resourceHolder, int resourceId)
{
    if (strings_.empty())
        return;

    int bundle1 = (strings_.begin()->first / 16) + 1;
    int bundle2 = (strings_.rbegin()->first / 16) + 1;
    // Currently the code writes only a single bundle of string entries.
    if (bundle1 != bundle2)
        throw std::invalid_argument("Only single bundle supported.");

    std::vector<uint8_t> buffer;

    for (int i = 0; i < 16; ++i)
    {
        auto idx = ((bundle1 - 1) * 16) + i;
        auto c = strings_.find( idx );

        if ( c == strings_.end() )
        {
            WORD zero = 0;
            smack::util::rawAppend(buffer, &zero);
        }
        else
        {
            WORD len = 
                static_cast<WORD>(c->second.length());
            smack::util::rawAppend(
                buffer,
                &len);
            auto wstr = 
                smack::util::convert(c->second);
            smack::util::rawAppend(
                buffer, 
                wstr.c_str(),
                wstr.length() * sizeof(wchar_t) );
        }
    }

    BOOLEAN result = UpdateResource(
        resourceHolder,
        RT_STRING,
        MAKEINTRESOURCE(bundle1),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
        static_cast<LPVOID>(buffer.data()),
        static_cast<DWORD>(buffer.size()));
    if (result == FALSE)
        throw std::invalid_argument("Could not add resource.");
}
