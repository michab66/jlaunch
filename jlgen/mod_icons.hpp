/*
 * $Id$
 *
 * Icon routines.
 *
 * Copyright (c) 2019-2020 Michael G. Binz
 */

#pragma once

#include <string>

namespace smack {
namespace util {
namespace icons {

/**
    * Use the passed image to create a set of scaled, square images in the
    * dimensions 16, 32, 64, 128, 256.  It is recommended to pass a square
    * image though all image sizes will do.
    */
void WriteImageSet(const std::string& sourceFile);

/**
    * Use the passed image to create a set of scaled, square images in the
    * dimensions 16, 32, 64, 128, 256.  It is recommended to pass a square
    * image though all image sizes will do.
    */
void WriteIconFile(const std::string& sourceFile);

}
}
}
