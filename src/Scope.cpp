/**
 * @file Scope.cpp
 * @author H4231
 * @brief Gestion variables (type) d'une scope
 * @date 2024-04-02
 *
 */

#include "Scope.h"
#include <cstdint>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <utility>

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
    VarType var;
    try {
        var = this->vars.at(varName);
    } catch (std::out_of_range &) {
        return parent ? parent->getVariable(varName) : std::nullopt;
    }
    uint32_t pos = 0;
    for (const auto &name : this->order) {
        pos += vars[name].getSize();
        if (name == varName) {
            break;
        }
    }
    pos += parent ? parent->getSize() : 0;
    return std::make_pair(var, pos);
}

void Scope::removeVariable(const std::string &name) {
    auto pos = std::find(order.begin(), order.end(), name);
    order.erase(pos);
    vars.erase(name);
}

uint32_t Scope::getSize() {
    int size = parent ? parent->getSize() : 0;
    for (const auto &[_, type] : this->vars) {
        size += type.getSize();
    }
    return size;
}
