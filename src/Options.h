#pragma once

#include <string>

/**
 * @brief Struct qui représente les arguments passés au programme
 * 
 */
struct Options {
    std::string out; /**< Le fichier de sortie */
    std::string in; /**< Le fichier source */
    std::string target; /**< L'architecture ciblée */

    /**
     * @brief Analyse lar arguments et construit la struct
     * 
     * @param argn le nombre d'arguments
     * @param argv le tableau d'arguemnts
     * @return la structure construite
     */
    static Options fromArgs(int argn, char **argv);
};
