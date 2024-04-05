/**
 * @file Options.h
 * @author H4231
 * @brief Analyse des arguments en entrée du programme
 * @date 2024-04-05
 * 
 */

#pragma once

#include <string>

struct Options {
    std::string out;
    std::string in;
    std::string target;

    static Options fromArgs(int argn, char **argv);
};
