#pragma once

#include <string>

struct Options {
    std::string out;
    std::string in;
    std::string target;

    static Options fromArgs(int argn, char **argv);
};
