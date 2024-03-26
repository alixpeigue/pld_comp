#pragma once

#include "IRBaseVisitor.h"
#include "ir.h"

class IRx86Visitor : public IRBaseVisitor {
private:
    virtual void visitAffect(ir::Affect &affect) override;
    virtual void visitAffectConst(ir::AffectConst &effectConst) override;
    virtual void visitBinOp(ir::BinOp &binop) override;
    virtual void visitUnaryOp(ir::UnaryOp &unaryop) override;
    virtual void visitBasicBlock(ir::BasicBlock &bb) override;
    virtual void visitCFG(ir::CFG &cfg) override;
    virtual void visitUnconditionalJump(ir::UnconditionalJump &jump) override;
    virtual void visitConditionalJump(ir::ConditionalJump &jump) override;
    virtual void visitSwitchJump(ir::SwitchJump &jump) override;
    virtual void visitReturn(ir::Return &ret) override;
    virtual void visitCall(ir::Call &call) override;
    
    inline std::string getInstrFromOp(ir::BinOp::BinOpType op);
};
