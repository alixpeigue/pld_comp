#pragma once

#include "Scope.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class IRBaseVisitor;

namespace ir {

class BasicBlock;

class IRInstr {
    friend BasicBlock;

public:
    virtual void accept(IRBaseVisitor &visitor) = 0;
    BasicBlock &getBlock() { return *block; };

protected:
    BasicBlock *block;
};

class Next : public IRInstr {};

class CFG;

class BasicBlock {
    friend CFG;

public:
    BasicBlock(Scope *scope, std::string name)
        : scope(scope), name(std::move(name)) {}
    void addInstr(std::unique_ptr<IRInstr> instr);
    void accept(IRBaseVisitor &visitor);
    Scope &getScope() { return *scope; }
    const std::string &getName() const { return name; }
    const std::vector<std::unique_ptr<IRInstr>> &getInstructions() const {
        return instrs;
    }
    Next &getNext() const { return *next; }
    void setNext(std::unique_ptr<Next> next);
    CFG &getCFG() { return *cfg; }

protected:
    std::string name;
    std::vector<std::unique_ptr<IRInstr>> instrs;
    Scope *scope;
    std::unique_ptr<Next> next;
    CFG *cfg;
};

class UnconditionalJump : public Next {
public:
    UnconditionalJump(BasicBlock *to) : to(to) {}

    virtual void accept(IRBaseVisitor &visitor) override;
    void setTo(BasicBlock *block) { to = block; }
    BasicBlock &getTo() { return *to; }

protected:
    BasicBlock *to;
};

class Return : public Next {
public:
    virtual void accept(IRBaseVisitor &visitor) override;
};

class CFG {
public:
    CFG(std::string name) : name(std::move(name)) {}
    void addBlock(std::unique_ptr<BasicBlock> block);
    void addScope(std::unique_ptr<Scope> scope);
    void setEpilogue(BasicBlock *epilogue) { this->epilogue = epilogue; }
    BasicBlock &getEpilogue() { return *epilogue; }

    void visitBlocks(IRBaseVisitor &visitor);
    const std::string &getName() { return name; }
    const std::vector<std::unique_ptr<BasicBlock>> &getBlocks() const {
        return blocks;
    }
    // Scope *mcainScope;

protected:
    std::vector<std::unique_ptr<BasicBlock>> blocks;
    std::vector<std::unique_ptr<Scope>> scopes;
    BasicBlock *epilogue;
    std::string name;
};

using Prog = std::vector<std::unique_ptr<CFG>>;

class Affect : public IRInstr {
public:
    Affect(std::string to, std::string from)
        : to(std::move(to)), from(std::move(from)){};
    virtual void accept(IRBaseVisitor &visitor) override;
    const std::string &getTo() const { return to; };
    const std::string &getFrom() const { return from; };

protected:
    std::string to;
    std::string from;
};

class AffectConst : public IRInstr {
public:
    AffectConst(std::string left, int value)
        : to(std::move(left)), value(value){};
    virtual void accept(IRBaseVisitor &visitor) override;
    const std::string &getTo() const { return to; };
    int getValue() const { return value; };

protected:
    std::string to;
    int value;
};

class UnaryOp : public IRInstr {
public:
    enum UnaryOpType { NEG, NOT };

    UnaryOp(UnaryOpType type, std::string to, std::string from)
        : type(type), to(to), from(from) {}

    const std::string &getTo() const { return to; }
    const std::string &getFrom() const { return from; };
    UnaryOpType getType() const { return type; }
    virtual void accept(IRBaseVisitor &visitor) override;

protected:
    UnaryOpType type;
    std::string to;
    std::string from;
};

class BinOp : public IRInstr {
public:
    enum BinOpType { ADD, SUB, DIV, MUL, MOD };

    BinOp(BinOpType type, std::string to, std::string from, std::string right)
        : type(type), to(to), left(from), right(right) {}
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

} // namespace ir
