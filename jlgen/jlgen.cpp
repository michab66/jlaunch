/* $Id$
 *
 * Generator cli implementation.
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
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

#include "util_console_app.hpp"

#include "winicon.h"
#include "ResourceMgr.h"
#include "RtIconGroup.h"
#include "RtStringTable.h"
#include "mod_icons.hpp"
#include "resource.h"
#include "../jlaunch/jlaunch_resource_ids.h"

using std::string;
using std::cerr;
using std::endl;
using std::experimental::filesystem::path;
using mob::windows::ResourceMgr;
using mob::windows::RtIconGroup;
using mob::windows::RtIcon;

namespace jlgen
{

    /**
     * The images sizes generated by this tool in the resources.
     */
    const std::initializer_list<uint16_t> IMAGE_SIZES =
        { 16, 32, 64, 128, 256 };

    /**
     * Update the icon with the given id in the executable.
     *
     * UpdateIcon C:\Users\Michael\svn\rep_github\jlaunch\x64\Debug\jlaunch.exe ..\mmt-icon-1024.png
     */
    void UpdateIconImpl(
        ResourceMgr& target,
        path& iconFile)
    {
        std::vector<std::unique_ptr<RtIcon>> outHolder;
        smack::util::icons::CreateIcons(
            outHolder,
            IMAGE_SIZES,
            iconFile);

        RtIconGroup iconGroup;

        for (std::unique_ptr<RtIcon>& a : outHolder)
            iconGroup.Add(a.get());

        target.add(IDI_ICON, iconGroup);
    }

    int WriteLauncher(
        path& targetFile)
    {
        HMODULE self = GetModuleHandleA(nullptr);

        // Locate the resource in the .EXE file.
        HRSRC hRes = FindResourceA(
            self,
            MAKEINTRESOURCE(IDR_BINARY1),
            RT_RCDATA);
        if (hRes == nullptr)
            throw std::invalid_argument("Could not locate binary resource.");

        // Load the resource.
        HGLOBAL hResLoad = LoadResource(
            self,
            hRes);
        if (hResLoad == nullptr)
            throw std::invalid_argument("Could not load resource.");

        // Lock the resource into global memory.
        void* dir = LockResource(hResLoad);
        if (dir == nullptr)
            throw std::invalid_argument("Could not lock resource.");

        int resourceSize =
            SizeofResource(nullptr, hRes);

        std::ofstream out;
        out.open(targetFile, std::ofstream::binary);
        if (out.fail())
            throw std::invalid_argument("Could not open target file.");

        out.write(
            static_cast<const char*>(dir), resourceSize);
        if (out.fail())
            throw std::invalid_argument("Could not write to target file.");

        out.close();
        if (out.fail())
            throw std::invalid_argument("Could not close target file.");

        return EXIT_SUCCESS;
    }

    /*
     * MakeLauncher C:\cygwin64\tmp\MMT.exe ..\mmt-icon-1024.png  mmt.app de/michab/app/mmt/Mmt
     */
    int MakeLauncher(
        path& targetFile,
        path& iconFile,
        string& moduleName,
        string& startClass
    )
    {
        cerr << "Creating launcher: " << targetFile << endl;
        WriteLauncher(targetFile);

        ResourceMgr target{ targetFile };

        mob::windows::RtString strings;
        cerr << "Adding main class name: " << startClass << endl;
        strings.Add(
            IDS_JAVA_MAIN_CLASS, 
            startClass);
        cerr << "Adding main module name: " << moduleName << endl;
        strings.Add(
            IDS_JAVA_MAIN_MODULE, 
            moduleName);
        cerr << "Adding info to string table."<< endl;
        target.add(
            IDS_STRINGS,
            strings);

        cerr << "Adding icons from: " << iconFile << endl;
        UpdateIconImpl(target, iconFile);

        cerr << "Committing resource additions." << endl;
        target.commit();

        return EXIT_SUCCESS;
    }

    int WriteImageSet(
        string& file)
    {
        smack::util::icons::WriteImageSet(
            file,
            IMAGE_SIZES );
        return EXIT_SUCCESS;
    }

    int execute(const std::vector<string>& argv) {
        using smack::util::Commands;

        auto cli = smack::util::makeCliApplication(

            Commands<path>::make(
                "WriteLauncher",
                WriteLauncher) ,
            Commands<path, path, string, string>::make(
                "MakeLauncher",
                MakeLauncher),
            Commands<string>::make(
                "WriteImageSet",
                WriteImageSet)
        );

        return cli.launch(argv);
    }
} // namespace unnamed

// MakeLauncher c:\cygwin64\tmp\xxx.exe C:\cygwin64\tmp\jlaunch\MMT.ico app.mmt de/michab/app/mmt/Mmt
int main(int argc, char** argv) {
    std::vector<string> cmdArgv(
        argv + 1,
        argv + argc);

    return jlgen::execute(cmdArgv);
}
