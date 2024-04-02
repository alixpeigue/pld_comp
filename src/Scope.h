#pragma once

#include <cstdint>
#include <optional>
#include <ostream>
#include <string>
#include <unordered_map>

/**
 * @brief Classe représentant les types de variables en C
 *
 */
class VarType {

public:
    enum Type { VOID, INT };

    VarType() = default;

    /**
     * @brief Construit à partit de l'enum Type
     *
     * @param type
     */
    VarType(Type type) : type(type){};

    /**
     * @brief Construit à partir du nom du type
     *
     * @param name nom c du type (ex : `int` ou `void`)
     */
    VarType(const std::string &name);

    /**
     * @brief Conversion implicite en enum type
     *
     * @return Type
     */
    operator Type() const { return type; }

    /**
     * @brief retourne la taille correspondant au type, exemple 4 pour int
     *
     * @return int
     */
    int getSize() const { return sizes[type]; }

protected:
    Type type;
    static constexpr int sizes[] = {0, 4};
};

using Variable = std::pair<VarType, uint32_t>;

/**
 * @brief Scope représente la table des symboles des variables en prenant en
 * compte leur portée
 *
 */
class Scope {
public:
    Scope(Scope &&scope)
        : parent(scope.parent), vars(std::move(scope.vars)), size(scope.size) {}
    /**
     * @brief Construit un scope vide avec `parent` comme parent. Le scope
     parent est le scope dans lequel il est impriqué :
     ```
     a : {
        b : {

        }
     }
     ```
     a est le parent de b
     * @param parent le parent du scope
     */
    Scope(Scope *parent) : parent(parent), size(0){};

    /**
     * @brief Cherche une variable dans ce scope, si elle n'est pas trouvée dans
     * ce scope, on cherche récursivement dans le parent.
     *
     * @param varName le nom de la variable
     * @return la variable si elle é été trouvée, sinon la variante vide de
     * std::optional
     */
    std::optional<Variable> getVariable(const std::string &varName);
    uint32_t getSize();

    /**
     * @brief Ajoute une variable à ce scope
     *
     * @param name nom de la variable
     * @param type type de la variable
     */
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
