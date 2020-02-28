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
#include "RtStringTable.h"

#include "mod_icons.hpp"

#include "resource.h"

namespace jlgen
{
    /**
     * Update the icon with the given id in the executable.
     */
    int UpdateIcon(
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

    int WriteLauncher(
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
        if (out.fail())
            throw std::invalid_argument("Could not open target file.");

        out.write((const char*)dir, resourceSize);
        if (out.fail())
            throw std::invalid_argument("Could not write to target file.");

        out.close();
        if (out.fail())
            throw std::invalid_argument("Could not close target file.");

        return 0;
    }

    int MakeLauncher(
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

        UpdateIcon(targetFile, 312, iconFile);

        return 0;
    }

    int WriteImageSet(std::string file)
    {
        smack::util::icons::WriteImageSet(file);
        return 0;
    }

    int WriteIconFile(std::string file)
    {
        smack::util::icons::WriteIconFile(file);
        return 0;
    }

    int execute(const std::vector<std::string>& argv) {
        using smack::util::Commands;
        using std::string;

        auto cmd0 = Commands<string, int, string>::make(
            "UpdateIcon",
            UpdateIcon);
        auto cmd2 = Commands<string>::make(
            "WriteLauncher",
            WriteLauncher);
        auto cmd3 = Commands<string, string, string, string>::make(
            "MakeLauncher",
            MakeLauncher);
        auto cmd4 = Commands<string>::make(
            "WriteImageSet",
            WriteImageSet);
        auto cmd5 = Commands<string>::make(
            "WriteIconFile",
            WriteIconFile);

        auto cli = smack::util::makeCliApplication(
            cmd0,
            cmd2,
            cmd3,
            cmd4,
            cmd5
        );

        return cli.launch(argv);
    }
} // namespace unnamed

// MakeLauncher c:\cygwin64\tmp\xxx.exe C:\cygwin64\tmp\jlaunch\MMT.ico app.mmt de/michab/app/mmt/Mmt
int main(int argc, char** argv) {
    std::cout << argv[0] << std::endl;

    std::vector<std::string> cmdArgv(
        argv + 1,
        argv + argc);

    return jlgen::execute(cmdArgv);
}
