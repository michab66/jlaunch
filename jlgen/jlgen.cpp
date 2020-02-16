/*
 * $Id$
 *
 * Copyright (c) 2019-2020 Michael Binz
 */

// See https://docs.microsoft.com/en-us/windows/uwp/design/globalizing/use-utf8-code-page
#undef UNICODE

#include <cstdio>
#include <string>
#include <iostream>

#include "util_console_app.hpp"

#include "winicon.h"
#include "ResourceMgr.h"
#include "Image.h"
#include "RtIconGroup.h"
#include "RtString.h"
#include "resource.h"

static int UpdateIcon(
    std::string exeFile,
    int resId,
    std::string iconFile)
{
    using mob::windows::RtIconGroup;

    mob::ResourceMgr target{ exeFile };

    RtIconGroup icon{ iconFile };

    target.addIcon(resId, icon);

    target.commit();

    return 0;
}

static int WriteLauncher(
    std::string targetFile)
{
    HMODULE self = GetModuleHandleA(nullptr);

    // Locate the resource in the .EXE file.
    HRSRC hRes = FindResourceA(
        self,
        MAKEINTRESOURCE(IDR_BINARY1),
        RT_RCDATA);
    if (hRes == NULL)
        throw std::invalid_argument("Could not locate binary resource.");

    // Load the resource.
    HGLOBAL hResLoad = LoadResource(
        self,
        hRes);
    if (hResLoad == NULL)
        throw std::invalid_argument("Could not load resource.");

    // Lock the resource into global memory.
    PGRPICONDIR dir = (PGRPICONDIR)LockResource(hResLoad);
    if (dir == NULL)
        throw std::invalid_argument("Could not lock resource.");

    // Perform a copy to be in-line with the icon file reader.
    int resourceSize =
        SizeofResource(nullptr, hRes);

    std::ofstream out;
    out.open(targetFile.c_str(), std::ofstream::binary);
    if ( out.fail() )
        throw std::invalid_argument("Could not open target file.");

    out.write((const char*)dir, resourceSize);
    if (out.fail())
        throw std::invalid_argument("Could not write to target file.");

    out.close();
    if (out.fail())
        throw std::invalid_argument("Could not close target file.");

    return 0;
}

static int MakeLauncher(
    std::string targetFile,
    std::string iconFile,
    std::string moduleName,
    std::string startClass
)
{
    WriteLauncher(targetFile);

    mob::ResourceMgr target{ targetFile };

    mob::windows::RtString strings;
    strings.Add(314, startClass);
    strings.Add(313, moduleName);

    target.addString(313, strings);

    target.commit();

    //UpdateString(targetFile, 313, moduleName);
    //UpdateString(targetFile, 314, startClass);
 //   UpdateIcon(targetFile, 312, iconFile);

    return 0;
}

static int execute(const std::vector<std::string>& argv) {
    using smack::util::Commands;
    using std::string;

    auto cmd0 = Commands<string, int, string>::make(
        "UpdateIcon",
        UpdateIcon);
    auto cmd2 = Commands<string>::make(
        "WriteLauncher",
        WriteLauncher);
    auto cmd3 = Commands<string,string,string,string>::make(
        "MakeLauncher",
        MakeLauncher);

    auto cli = smack::util::makeCliApplication(
        cmd0,
        cmd2,
        cmd3
    );

    return cli.launch(argv);
}

// MakeLauncher c:\cygwin64\tmp\xxx.exe C:\cygwin64\tmp\jlaunch\MMT.ico app.mmt de/michab/app/mmt/Mmt
int main(int argc, char** argv) {
    std::cout << argv[0] << std::endl;

    std::vector<std::string> cmdArgv(
        argv + 1,
        argv + argc);

    return execute(cmdArgv);
}
