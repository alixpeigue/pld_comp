#pragma once

#include <memory>
#include <string>
#include <vector>

class IRBaseVisitor;

namespace ir {

class IRInstr {
public:
    virtual void accept(IRBaseVisitor &visitor) = 0;
};

class BasicBlock {
public:
    void addInstr(std::unique_ptr<IRInstr> instr);
    void visit(IRBaseVisitor &visitor);

protected:
    std::vector<std::unique_ptr<IRInstr>> instrs;
};

class CFG {
public:
    void addRoot(std::unique_ptr<BasicBlock> block);

    void visitRoots(IRBaseVisitor &visitor);

protected:
    std::vector<std::unique_ptr<BasicBlock>> roots;
};

class Affect : public IRInstr {
public:
    Affect(std::string left, std::string right)
        : leftVar(left), rightVar(right){};
    virtual void accept(IRBaseVisitor &visitor) override;
    const std::string &getLeft() const { return leftVar; };
    const std::string &getRight() const { return rightVar; };

protected:
    std::string leftVar;
    std::string rightVar;
};

class AffectConst : public IRInstr {
    public: 
        AffectConst(std::string left, std::string right)
        : leftVar(left), rightVar(right){};
    virtual void accept(IRBaseVisitor &visitor) override;
    const std::string &getLeft() const { return leftVar; };
    const std::string &getRight() const { return rightVar; };

protected:
    std::string leftVar;
    std::string rightVar;

};

} // namespace ir
