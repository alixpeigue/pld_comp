/**
 * @file ir.h
 * @author H4231
 * @brief Définition ddu namespace ir qui contient les classes nécessaires pour
 *        le développement de l'IR
 * @date 2024-04-02
 */

#pragma once

#include "Scope.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class IRBaseVisitor;

namespace ir {

class BasicBlock;

/**
 * @brief classe abstraite qui représente une instruction IR
 *
 */
class IRInstr {
    friend BasicBlock;

public:
    /**
     * @brief Accepte un visiteur d'IR afin de visiter l'IR de manière
     * polymorpĥique
     *
     * @param visitor
     */
    virtual void accept(IRBaseVisitor &visitor) = 0;

    /**
     * @brief Récupérer le BasicBlock dans lequel l'instruction se situe
     *
     * @return BasicBlock&
     */
    BasicBlock &getBlock() { return *block; }
    void setBlock(BasicBlock *bb) { block = bb; }
    virtual const std::string toString() const { return "IRInstr"; }
    virtual ~IRInstr() {}

protected:
    BasicBlock *block;
};

/**
 * @brief Instruction qui déplace vers un autre bloc
 *
 */
class Next : public IRInstr {};

class CFG;

/**
 * @brief bloc de code
 *
 */
class BasicBlock {
    friend CFG;

public:
    BasicBlock(Scope *scope, std::string name)
        : name(std::move(name)), scope(scope) {}
    void addInstr(std::unique_ptr<IRInstr> instr);
    void accept(IRBaseVisitor &visitor);
    /**
     * @brief Renvoie le scope contenant les variables auquel le bloc est
     * assicoé
     *
     * @return Scope&
     */
    Scope &getScope() { return *scope; }

    /**
     * @brief Nom du bloc
     *
     * @return const std::string&
     */
    const std::string &getName() const { return name; }

    /**
     * @brief Renvoie le vecteur d'instructions du bloc par référence cons
     *
     * @return const std::vector<std::unique_ptr<IRInstr>>&
     */
    std::vector<std::unique_ptr<IRInstr>> &getInstructions() { return instrs; }

    /**
     * @brief Renvoie l'objet représentant la suite du bloc
     *
     * @return std::unique_ptr<Next>
     */
    std::unique_ptr<Next> getNext() { return std::move(next); }

    /**
     * @brief Définit la fin du bloc
     *
     * @param next l'instruction de fin : saut absolu, saut conditionel...
     */
    void setNext(std::unique_ptr<Next> next);
    CFG &getCFG() { return *cfg; }
    void addInstr(size_t index, std::unique_ptr<IRInstr> instr) {
        this->instrs.emplace(this->instrs.begin() + index + 1,
                             std::move(instr));
    }
    void replaceInstr(size_t index, IRInstr *instr) {
        this->instrs[index].reset(instr);
    }

protected:
    std::string name;
    std::vector<std::unique_ptr<IRInstr>> instrs;
    Scope *scope;
    std::unique_ptr<Next> next;
    CFG *cfg;
};

/**
 * @brief Instruction qui déplace toujours vers un autre bloc
 *
 */
class UnconditionalJump : public Next {
public:
    UnconditionalJump(BasicBlock *to) : to(to) {}

    virtual void accept(IRBaseVisitor &visitor) override;
    void setTo(BasicBlock *block) { to = block; }
    BasicBlock &getTo() { return *to; }

protected:
    BasicBlock *to;
};

/**
 * @brief Instrution qui renvoie vers l'épilogue et place la valeur de retour
 * dans `#return`
 *
 */
class Return : public Next {
public:
    virtual void accept(IRBaseVisitor &visitor) override;
};

/**
 * @brief Instruction qui saute vers un bloc ou un autre
 *      en fonction d'une condition
 *
 */
class ConditionalJump : public Next {
public:
    /**
     * @brief Construit le saut conditionel
     *
     * @param condition la variable contenant la condition du saut
     * @param thenBlock le bloc où aller si la condition est vraie
     * @param elseBlock le bloc où aller si la condition est fausse
     */
    ConditionalJump(std::string condition, BasicBlock *thenBlock,
                    BasicBlock *elseBlock)
        : condition(std::move(condition)), thenBlock(thenBlock),
          elseBlock(elseBlock) {}

    virtual void accept(IRBaseVisitor &visitor) override;
    const std::string &getCondition() const { return condition; }
    BasicBlock *getThen() { return thenBlock; }
    BasicBlock *getElse() { return elseBlock; }
    void setThen(BasicBlock *bb) { thenBlock = bb; }
    void setElse(BasicBlock *bb) { elseBlock = bb; }

protected:
    std::string condition;
    BasicBlock *thenBlock;
    BasicBlock *elseBlock;
};

/**
 * @brief Dans le cas d'un switch, Instruction qui saute vers le bloc
 * approprié
 *
 */
class SwitchJump : public Next {
public:
    /**
     * @brief Construit un SwitchJump vers les blocs appropriés
     *
     * @param expressionTest la variable devant être testée
     * @param caseTests un vecteur de pairs, chaque paire représente un cas du
     * switch sous la forme (valeur, block)
     */
    SwitchJump(std::string expressionTest,
               std::vector<std::pair<std::string, BasicBlock *>> caseTests,
               BasicBlock *defaultBlock)
        : expressionTest(std::move(expressionTest)),
          caseTests(std::move(caseTests)),
          defaultBlock(std::move(defaultBlock)) {}
    virtual void accept(IRBaseVisitor &visitor) override;
    const std::string &getExpressionTest() const { return expressionTest; }
    const std::vector<std::pair<std::string, BasicBlock *>> &getCaseTests()
        const {
        return caseTests;
    }
    BasicBlock *getDefaultBlock() { return defaultBlock; }

protected:
    std::string expressionTest;
    std::vector<std::pair<std::string, BasicBlock *>> caseTests;
    BasicBlock *defaultBlock;
};

/**
 * @brief Control Flow Graph qui représente une fonction sous la forme
 *        d'un arbre de bloc
 *
 */
class CFG {
public:
    /**
     * @brief Construit un Control Flow Graph
     *
     * @param name nom de la fonction représentée par le CFG
     * @param returnType type de retour de la fonction représentée par le CFG
     */
    CFG(std::string name, VarType returnType)
        : returnType(returnType), name(std::move(name)) {}

    /**
     * @brief Ajoute un bloc dans le CFG. Prend l'ownership du bloc via le
     * unique_ptr
     *
     * @param block le bloc à ajouter dans le CFG
     */
    void addBlock(std::unique_ptr<BasicBlock> block);

    /**
     * @brief Ajoute un scope dans le CFG. Prend l'ownership du scope via le
     * unique_ptr
     *
     * @param scope le scope à ajouter dans le CFG
     */
    void addScope(std::unique_ptr<Scope> scope);

    /**
     * @brief Place le bloc passé comme épilogue du bloc. L'épilogue est un bloc
     * normal et doit donc être ajouté au CFG afin d'être possédé par le CFG.
     *
     * @param epilogue le bloc épilogue
     */
    void setEpilogue(BasicBlock *epilogue) { this->epilogue = epilogue; }
    BasicBlock &getEpilogue() { return *epilogue; }

    /**
     * @brief Accepte un visiteur d'IR pour visiter de CFG.
     *
     * @param visitor le visiteur d'IR
     */
    void visitBlocks(IRBaseVisitor &visitor);
    const std::string &getName() { return name; }
    std::vector<std::unique_ptr<BasicBlock>> &getBlocks() { return blocks; }
    uint32_t getSize();

    /**
     * @brief Ajoute un argument à la fonction représentée par le cfg
     *
     * @param name le nom du paramètre
     * @param type le type du paramètre
     */
    void addArg(const std::string &name, VarType type) {
        args.push_back(std::make_pair(name, type));
    }

    /**
     * @brief Retourne tous les arguments pric par la fonction représentée par
     * le cfg sous forme d'un vecteur de pairs (nom, type)
     *
     * @return const std::vector<std::pair<std::string, VarType>>&
     */
    const std::vector<std::pair<std::string, VarType>> &getArgs() const {
        return args;
    }
    // Scope *mcainScope;

protected:
    std::vector<std::unique_ptr<BasicBlock>> blocks;
    std::vector<std::unique_ptr<Scope>> scopes;
    std::vector<std::pair<std::string, VarType>> args;
    VarType returnType;
    BasicBlock *epilogue;
    std::string name;
};

using Prog = std::vector<std::unique_ptr<CFG>>;

/**
 * @brief Instruction qui déplace une valeur d'une case mémoire vers une autre
 *
 */
class Affect : public IRInstr {
public:
    /**
     * @brief Crée un objet Affect représentant un affectation de la forme `to =
     * from`
     */
    Affect(std::string to, std::string from)
        : to(std::move(to)), from(std::move(from)){};
    virtual void accept(IRBaseVisitor &visitor) override;
    const std::string &getTo() const { return to; }
    const std::string &getFrom() const { return from; }
    virtual const std::string toString() const override {
        return "Affect: " + getTo() + '=' + getFrom();
    }

protected:
    std::string to;
    std::string from;
};

/**
 * @brief Instruction qui déplace une valeur constante vers un espace mémoire
 *
 */
class AffectConst : public IRInstr {
public:
    /**
     * @brief CCrée un objet AffectConst représentent une affectation d'une
     * constante à une variable de la forme `to = value`
     */
    AffectConst(std::string left, int value)
        : to(std::move(left)), value(value){};
    virtual void accept(IRBaseVisitor &visitor) override;
    const std::string &getTo() const { return to; }
    int getValue() const { return value; }
    virtual const std::string toString() const override {
        return "AffectConst: " + getTo() + '=' + std::to_string(getValue());
    }

protected:
    std::string to;
    int value;
};

/**
 * @brief Instruction qui effectue une opération a une opérande
 *
 */
class UnaryOp : public IRInstr {
public:
    enum UnaryOpType { NEG, NOT, PRE_INC, PRE_DEC, POST_INC, POST_DEC };

    /**
     * @brief Crée un objet représentant une opération unaire de la forme `to =
     * type from` Par exemple, si type est NOT, cela représente `to = !from`.
     */
    UnaryOp(UnaryOpType type, std::string to, std::string from)
        : type(type), to(to), from(from) {}

    const std::string &getTo() const { return to; }
    const std::string &getFrom() const { return from; }
    UnaryOpType getType() const { return type; }
    virtual void accept(IRBaseVisitor &visitor) override;

protected:
    UnaryOpType type;
    std::string to;
    std::string from;
};

/**
 * @brief Instruction qui effectue une opération a deux opérandes
 *
 */
class BinOp : public IRInstr {
public:
    enum BinOpType {
        ADD,
        SUB,
        DIV,
        MUL,
        MOD,
        SHIFT_R,
        SHIFT_L,
        GT,
        LT,
        GTE,
        LTE,
        EQ,
        NEQ,
        AND_BIN,
        OR_BIN,
        XOR_BIN
    };

    /**
     * @brief Construit une opération binaire représentant une opéation du type
     * `to = left op right`
     */
    BinOp(BinOpType op, std::string to, std::string left, std::string right)
        : type(op), to(to), left(left), right(right) {}
    const std::string &getTo() const { return to; }
    const std::string &getLeft() const { return left; }
    const std::string &getRight() const { return right; }
    BinOpType getType() const { return type; }
    virtual void accept(IRBaseVisitor &visitor) override;

protected:
    BinOpType type;
    std::string to;
    std::string left;
    std::string right;
};

/**
 * @brief Instruction qui fait appel a une autre fonction
 *
 */
class Call : public IRInstr {
public:
    Call(const std::string &func_name, const std::string &ret)
        : ret(ret), func_name(func_name) {}
    virtual void accept(IRBaseVisitor &visitor) override;
    void addName(const std::string &name) { names.push_back(name); }
    const std::string &getFunc_name() const { return func_name; }
    const std::vector<std::string> &getNames() const { return names; }
    const std::string &getRet() { return ret; }

protected:
    std::string ret;
    std::string func_name;
    std::vector<std::string> names;
};

} // namespace ir
