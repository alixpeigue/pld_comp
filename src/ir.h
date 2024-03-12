#pragma once

#include "IRBaseVisitor.h"
#include <memory>
#include <vector>

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

} // namespace ir
