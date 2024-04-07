#include "Options.h"
#include "unistd.h"
#include <iostream>

Options Options::fromArgs(int argn, char **argv) {
    int opt;
    Options options;
    options.out = "out.s";
    options.target = "x86-64";
    options.debug = false;

    while ((opt = getopt(argn, argv, "sdt:o:")) != -1) {
        switch (opt) {
        case 't':
            options.target = optarg;
            break;
        case 'o':
            options.out = optarg;
            break;
        case 's':
            options.out = "stdout";
            break;
        case 'd':
            options.debug = true;
            break;
        default:
            std::cerr << "usage :" << argv[0]
                      << "[-t target] [-o out] [-s] [-d] file\n";
            exit(1);
        }
    }

    if (optind >= argn) {
        std::cerr << "usage :" << argv[0]
                  << "[-t target] [-o out] [-s] [-d] file\n";
        exit(1);
    }

    options.in = argv[optind];

    return options;
}
