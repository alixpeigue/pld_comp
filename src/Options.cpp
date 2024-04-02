#include "Options.h"
#include "unistd.h"
#include <iostream>

Options Options::fromArgs(int argn, char **argv) {
    int opt;
    Options options;
    options.out = "out.s";
    options.target = "x86-64";

    while ((opt = getopt(argn, argv, "t:o:")) != -1) {
        switch (opt) {
        case 't':
            options.target = optarg;
            break;
        case 'o':
            options.out = optarg;
            break;
        default:
            std::cerr << "usage :" << argv[0] << "[-t target] [-o out] file\n";
            exit(1);
        }
    }

    if (optind >= argn) {
        std::cerr << "usage :" << argv[0] << "[-t target] [-o out] file\n";
        exit(1);
    }

    options.in = argv[optind];

    return options;
}
