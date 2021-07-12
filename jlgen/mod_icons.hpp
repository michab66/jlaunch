/* $Id$
 *
 * Icon routines.
 *
 * Copyright (c) 2019-2020 Michael G. Binz
 */

#pragma once

#include <filesystem>
#include <initializer_list>
#include <string>

#include "RtIcon.h"

namespace smack {
namespace util {
namespace icons {

using std::string;
using std::filesystem::path;

/**
 * Create a Windows .ico file from an input .png file.
 * Use the passed sizes to create a set of scaled, square images in the
 * supported dimensions.  It is recommended to pass a square image though
 * all image sizes will do.
 *
 * @param sourcePng The source .png file.  This must exist.
 * @param sizes A list of sizes.  Used to create square images.
 * @param targetIco The target file.  If this exists it gets overwritten.
 */
void CreateWindowsIcon(
    const path& sourcePng,
    const std::initializer_list<uint16_t> sizes,
    const path& targetIco);

/**
 * Create an MacOS .icns file from an input .png file.
 * Use the passed sizes to create a set of scaled, square images in the
 * supported dimensions.  It is recommended to pass a square image though
 * all image sizes will do.
 *
 * @param sourcePng The source .png file.  This must exist.
 * @param sizes A list of sizes.  Used to create square images.
 * @param targetIco The target file.  If this exists it gets overwritten.
 */
void CreateAppleIcon(
    const path& sourcePng,
    const std::initializer_list<uint16_t> sizes,
    const path& targetIco);

/**
 * Create a set of scaled icons and place them into the passed container.
 *
 * @param outHolder The target container.
 * @param sizes A list of sites.
 * @param sourcePng The input .png file.
 */
void CreateIcons(
    std::vector<std::unique_ptr<mob::windows::RtIcon>>& outHolder,
    const std::initializer_list<uint16_t> sizes,
    const path& sourcePng);

}
}
}
