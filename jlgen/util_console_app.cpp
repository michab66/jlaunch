/* $Id$
 *
 * Console application helper.
 *
 * Copyright (c) 2019-2020 Michael Binz
 */

#include "util_console_app.hpp"

namespace smack {
namespace util {

void transform(const char* in, std::experimental::filesystem::path& out) {
    out = std::experimental::filesystem::path(in);
}

void transform(const char* in, int& out) {
    std::size_t pos;
    out = std::stoi(in, &pos, 0);
    if (in[pos]) {
        throw std::invalid_argument(in);
    }
}

void transform(const char* in, long& out) {
    std::size_t pos;
    out = std::stol(in, &pos, 0);
    if (in[pos]) {
        throw std::invalid_argument(in);
    }
}

void transform(const char* in, float& out) {
    std::size_t pos;
    out = std::stof(in, &pos);
    if (in[pos]) {
        throw std::invalid_argument(in);
    }
}

void transform(const char* in, double& out) {
    std::size_t pos;
    out = std::stod(in, &pos);
    if (in[pos]) {
        throw std::invalid_argument(in);
    }
}

void transform(const char* in, bool& out) {
    std::string parameter = in;

    if (parameter == "true") {
        out = true;
        return;
    }
    if (parameter == "false") {
        out = false;
        return;
    }

    throw std::invalid_argument(in);
}

} // namespace util
} // namespace smack
