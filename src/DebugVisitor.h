#pragma once

#include "IRBaseVisitor.h"
class DebugVisitor : public IRBaseVisitor {
    virtual void visitAffect(ir::Affect &affect) override;
    virtual void visitAffectConst(ir::AffectConst &affectConst) override;
    virtual void visitBinOp(ir::BinOp &binop) override;
    virtual void visitUnaryOp(ir::UnaryOp &unaryop) override;
    virtual void visitBasicBlock(ir::BasicBlock &bb) override;
    virtual void visitCFG(ir::CFG &cfg) override;
    virtual void visitUnconditionalJump(ir::UnconditionalJump &jump) override;
    virtual void visitConditionalJump(ir::ConditionalJump &jump) override;
    virtual void visitSwitchJump(ir::SwitchJump &jump) override;
    virtual void visitReturn(ir::Return &jump) override;
    virtual void visitCall(ir::Call &call) override;
};
