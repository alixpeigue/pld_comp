/**
 * @file IRBaseVisitor.h
 * @author H4231
 * @brief Un visiteur abstrait pour l'IR
 * @date 2024-04-02
 */

#pragma once

#include "ir.h"
/**
 * @brief Base des visiteurs d'IR.
 * On peut visiter l'IR pour passer des couches d'optimisation ou pour générer du code
 * 
 */
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
    virtual void visitSwitchJump(ir::SwitchJump &jump) = 0;
    virtual void visitReturn(ir::Return &jump) = 0;
    virtual void visitCall(ir::Call &call) = 0;
};
