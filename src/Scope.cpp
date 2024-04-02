/**
 * @file Scope.cpp
 * @author H4231
 * @brief Gestion variables (type) d'une scope
 * @date 2024-04-02
 * 
 */

#include "Scope.h"
#include <iostream>
#include <optional>
#include <stdexcept>


/**
 * @brief Construct a new Var Type:: Var Type object
 * 
 * @param name nom du type
 */

VarType::VarType(const std::string &name) {
    if (name == "void") {
        type = VOID;
    } else if (name == "int") {
        type = INT;
    } else {
        type = VOID;
    }
}

/**
 * @brief Recupere la variable stocke dans la map 
 * 
 * @param varName nom de la variable quand veut recuperer 
 * @return std::optional<Variable>  
 */

std::optional<Variable> Scope::getVariable(const std::string &varName) {
    try {
        Variable var = vars.at(varName);
        var.second += parent ? parent->getSize() : 0;
        return var;
    } catch (std::out_of_range &) {
        return parent ? parent->getVariable(varName) : std::nullopt;
    }
}

uint32_t Scope::getSize() { return parent ? size + parent->getSize() : size; }
