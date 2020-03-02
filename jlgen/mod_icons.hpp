/* $Id$
 *
 * Icon routines.
 *
 * Copyright (c) 2019-2020 Michael G. Binz
 */

#pragma once

#include <initializer_list>
#include <string>

#include "RtIcon.h"

namespace smack {
namespace util {
namespace icons {

using std::string;

/**
 * Use the passed image to create a set of scaled, square images in the
 * dimensions 16, 32, 64, 128, 256.  It is recommended to pass a square
 * image though all image sizes will do.
 */
void WriteImageSet(const string& sourceFile);

/**
 * Use the passed image to create a set of scaled, square images in the
 * dimensions 16, 32, 64, 128, 256.  It is recommended to pass a square
 * image though all image sizes will do.
 */
void WriteIconFile(const string& sourceFile);

/**
 * Create a set of scaled icons and place them into the passed container.
 */
void CreateIcons(
    std::vector<std::unique_ptr<mob::windows::RtIcon>>& outHolder,
    const std::initializer_list<uint16_t> sizes,
    const string& sourcePng);

}
}
}
