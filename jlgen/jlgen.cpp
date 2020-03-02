/* $Id$
 *
 * Copyright (c) 2019-2020 Michael Binz
 */

// See https://docs.microsoft.com/en-us/windows/uwp/design/globalizing/use-utf8-code-page
#undef UNICODE

#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "util_console_app.hpp"

#include "winicon.h"
#include "ResourceMgr.h"
#include "RtIconGroup.h"
#include "RtStringTable.h"
#include "mod_icons.hpp"
#include "resource.h"
#include "../jlaunch/jlaunch_resource_ids.h"

using std::string;
using mob::windows::ResourceMgr;
using mob::windows::RtIconGroup;
using mob::windows::RtIcon;

namespace jlgen
{

    /**
     * Update the icon with the given id in the executable.
     *
     * UpdateIcon C:\Users\Michael\svn\rep_github\jlaunch\x64\Debug\jlaunch.exe 1 ..\MMT.ico
     */
    int UpdateIcon(
        std::string exeFile,
        int resId,
        std::string iconFile)
    {
        ResourceMgr target{ exeFile };

        RtIconGroup icon{ iconFile };

        target.addIcon(resId, icon);

        target.commit();

        return 0;
    }

    /**
     * Update the icon with the given id in the executable.
     *
     * UpdateIcon C:\Users\Michael\svn\rep_github\jlaunch\x64\Debug\jlaunch.exe ..\mmt-icon-1024.png
     */
    void UpdateIconImpl(
        ResourceMgr& target,
        string iconFile)
    {
        std::vector<std::unique_ptr<RtIcon>> outHolder;
        smack::util::icons::CreateIcons(
            outHolder,
            { 16, 32, 64, 128, 256 },
            iconFile);

        RtIconGroup iconGroup;

        for (std::unique_ptr<RtIcon>& a : outHolder)
            iconGroup.Add(a.get());

        target.addIcon(IDI_ICON, iconGroup);
    }

    /**
     * Update the icon with the given id in the executable.
     *
     * UpdateIcon C:\Users\Michael\svn\rep_github\jlaunch\x64\Debug\jlaunch.exe ..\mmt-icon-1024.png
     */
    int UpdateIcon2(
        string exeFile,
        string iconFile)
    {
        ResourceMgr target{ exeFile };
        UpdateIconImpl(
            target,
            iconFile );
        target.commit();

        return 0;
    }

    int WriteLauncher(
        string targetFile)
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

    /*
     * MakeLauncher C:\cygwin64\tmp\MMT.exe ..\mmt-icon-1024.png  mmt.app de/michab/app/mmt/Mmt
     */
    int MakeLauncher(
        string targetFile,
        string iconFile,
        string moduleName,
        string startClass
    )
    {
        WriteLauncher(targetFile);

        ResourceMgr target{ targetFile };

        mob::windows::RtString strings;
        strings.Add(
            IDS_JAVA_MAIN_CLASS, 
            startClass);
        strings.Add(
            IDS_JAVA_MAIN_MODULE, 
            moduleName);
        target.addString(
            IDS_STRINGS,
            strings);

        UpdateIconImpl(target, iconFile);

        target.commit();

        return 0;
    }

    int WriteImageSet(string file)
    {
        smack::util::icons::WriteImageSet(file);
        return 0;
    }

    int WriteIconFile(string file)
    {
        smack::util::icons::WriteIconFile(file);
        return 0;
    }

    int execute(const std::vector<string>& argv) {
        using smack::util::Commands;

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
        auto cmd6 = Commands<string,string>::make(
            "UpdateIcon",
            UpdateIcon2);

        auto cli = smack::util::makeCliApplication(
            cmd0,
            cmd2,
            cmd3,
            cmd4,
            cmd5,
            cmd6
        );

        return cli.launch(argv);
    }
} // namespace unnamed

// MakeLauncher c:\cygwin64\tmp\xxx.exe C:\cygwin64\tmp\jlaunch\MMT.ico app.mmt de/michab/app/mmt/Mmt
int main(int argc, char** argv) {
    std::cout << argv[0] << std::endl;

    std::vector<string> cmdArgv(
        argv + 1,
        argv + argc);

    return jlgen::execute(cmdArgv);
}
