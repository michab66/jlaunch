/* $Id$
 *
 * Icon routines.
 *
 * Copyright (c) 2019-2020 Michael G. Binz
 */

#pragma once

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
#include <initializer_list>
#include <string>

#include "RtIcon.h"

namespace smack {
namespace util {
namespace icons {

using std::string;
using std::experimental::filesystem::path;

/**
 * Use the passed image to create a set of scaled, square images in the
 * supported dimensions.  It is recommended to pass a square image though
 * all image sizes will do.
 */
void WriteImageSet(
    const path& sourceFile,
    const std::initializer_list<uint16_t> sizes);

/**
 * Create a set of scaled icons and place them into the passed container.
 */
void CreateIcons(
    std::vector<std::unique_ptr<mob::windows::RtIcon>>& outHolder,
    const std::initializer_list<uint16_t> sizes,
    const path& sourcePng);

}
}
}
