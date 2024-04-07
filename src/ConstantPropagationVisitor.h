#pragma once

#include "IRBaseVisitor.h"
#include "ir.h"
#include <unordered_map>

class ConstantPropagationVisitor : public IRBaseVisitor {
    virtual void visitAffectConst(ir::AffectConst &affect) override;
    virtual void visitAffect(ir::Affect &affect) override;
    virtual void visitUnaryOp(ir::UnaryOp &unaryOp) override;
    virtual void visitBinOp(ir::BinOp &binOp) override;
    virtual void visitCFG(ir::CFG &cfg) override;
    virtual void visitConditionalJump(ir::ConditionalJump &jump) override;
    virtual void visitUnconditionalJump(ir::UnconditionalJump &jump) override;
    virtual void visitSwitchJump(ir::SwitchJump &jump) override;
    virtual void visitReturn(ir::Return &ret) override;
    virtual void visitCall(ir::Call &call) override;

protected:
    std::unordered_map<std::string, int> constants;
    size_t instr_index;
};
