#include "Scope.h"
#include <iostream>
#include <optional>
#include <stdexcept>

VarType::VarType(const std::string &name) {
    if (name == "void") {
        type = VOID;
    } else if (name == "int") {
        type = INT;
    } else {
        type = VOID;
    }
}

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
