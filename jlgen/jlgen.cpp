/*
 * $Id$
 *
 * Copyright (c) 2019 Michael Binz
 */

#include <cstdio>
#include <string>
#include <iostream>

#include "util_console_app.hpp"

#include "winicon.h"
#include "ResourceMgr.h"
#include "Image.h"
#include "RtIconGroup.h"

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

static int execute(const std::vector<std::string>& argv) {
    using smack::util::Commands;
    using std::string;

    auto cmd1 = Commands<string, int, string>::make(
            "UpdateIcon",
            UpdateIcon);

    auto cli = smack::util::makeCliApplication(
        cmd1
    );

    return cli.launch(argv);
}

int main(int argc, char** argv) {
    std::cout << argv[0] << std::endl;

    std::vector<std::string> cmdArgv(
        argv + 1,
        argv + argc);

    return execute(cmdArgv);
}
