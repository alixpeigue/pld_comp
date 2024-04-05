/**
 * @file Options.h
 * @author H4231
 * @brief Analyse des arguments en entrée du programme
 * @date 2024-04-05
 * 
 */

#include "Options.h"
#include "unistd.h"
#include <iostream>

/**
 * @brief analyse les arguments en entrée du programme pour choisir
 *        l'architecture cible et les noms des fichiers a lire et a générer
 *        Ferme le programme en cas de mauvaises entrées
 * 
 * @param argn le nombre d'argument dans argv
 * @param argv les paramètres entrés par l'utilisateur dans l'appel du
 *          compilateur
 * @return Options contenant l'architecture cible et le noms des fichiers
 *         a lire et générer
 */
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
