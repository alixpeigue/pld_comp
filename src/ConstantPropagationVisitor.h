#pragma once

#include "IRBaseVisitor.h"
#include "ir.h"
#include <unordered_map>

class ConstantPropagationVisitor : public IRBaseVisitor {
    virtual void visitAffectConst(ir::AffectConst &affect) override;
    virtual void visitAffect(ir::Affect &affect) override;
    virtual void visitUnaryOp(ir::UnaryOp &unaryop) override;
    virtual void visitCFG(ir::CFG &cfg) override;

protected:
    std::unordered_map<std::string, int> constants;
    std::unique_ptr<ir::IRInstr> *instr;
};
