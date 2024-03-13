#pragma once

#include <cstdint>
#include <optional>
#include <ostream>
#include <string>
#include <unordered_map>

enum VarType { INT };

static uint32_t typeSize[] = {4};

using Variable = std::pair<VarType, uint32_t>;

class Scope {
public:
    Scope(Scope &&scope)
        : parent(scope.parent), vars(std::move(scope.vars)), size(scope.size) {}
    Scope(Scope *parent) : size(0), parent(parent){};

    std::optional<Variable> getVariable(const std::string &varName);
    uint32_t getSize();

    void addVariable(std::string name, VarType type) {
        size += 4;
        vars.insert(std::make_pair(name, std::make_pair(type, size)));
    }

    std::ostream &print(std::ostream &os) {
        for (auto &pair : vars) {
            os << pair.first << " at " << pair.second.second << "\n";
        }
        return os;
    }

protected:
    Scope *parent;
    std::unordered_map<std::string, Variable> vars;
    uint32_t size;
};
