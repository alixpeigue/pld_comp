#pragma once

#include <cstdint>
#include <optional>
#include <ostream>
#include <string>
#include <unordered_map>

class VarType {

public:
    enum Type { VOID, INT, CHAR };

    VarType() = default;
    VarType(Type type) : type(type){};
    VarType(const std::string &name);
    operator Type() const { return type; }

    int getSize() const { return sizes[type]; }

protected:
    Type type;
    static constexpr int sizes[] = {0, 4, 1};
};

using Variable = std::pair<VarType, uint32_t>;

class Scope {
public:
    Scope(Scope &&scope)
        : parent(scope.parent), vars(std::move(scope.vars)), size(scope.size) {}
    Scope(Scope *parent) : size(0), parent(parent){};

    std::optional<Variable> getVariable(const std::string &varName);
    uint32_t getSize();

    void addVariable(std::string name, VarType type) {
        size += type.getSize();
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
