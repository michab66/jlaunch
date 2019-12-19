#pragma once

#include <string>

#include "winicon.h"

class GroupDir
{
    PGRPICONDIR dir_;

    void Dump(int idx, PGRPICONDIRENTRY iconDirEntry);

public:
    GroupDir(std::wstring exeName);
    ~GroupDir();

    void Dump();
};

