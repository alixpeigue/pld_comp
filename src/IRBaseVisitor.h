#pragma once

#include "ir.h"
class IRBaseVisitor {
public:
    virtual void visitAffect(ir::Affect &affect) = 0;
    virtual void visitAffectConst(ir::AffectConst &affectConst) = 0;
    virtual void visitBinOp(ir::BinOp &binop) = 0;
    virtual void visitUnaryOp(ir::UnaryOp &unaryop) = 0;
    virtual void visitBasicBlock(ir::BasicBlock &bb) = 0;
    virtual void visitCFG(ir::CFG &cfg) = 0;
    virtual void visitUnconditionalJump(ir::UnconditionalJump &jump) = 0;
    virtual void visitConditionalJump(ir::ConditionalJump &jump) = 0;
    virtual void visitReturn(ir::Return &jump) = 0;
    virtual void visitCall(ir::Call &call) = 0;
};
