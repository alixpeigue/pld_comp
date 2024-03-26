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
    virtual void visitAffect(ir::Affect &affect) {}
    virtual void visitAffectConst(ir::AffectConst &affectConst) {}
    virtual void visitBinOp(ir::BinOp &binop) {}
    virtual void visitUnaryOp(ir::UnaryOp &unaryop) {}
    virtual void visitBasicBlock(ir::BasicBlock &bb) {}
    virtual void visitCFG(ir::CFG &cfg) {}
    virtual void visitUnconditionalJump(ir::UnconditionalJump &jump) {}
    virtual void visitConditionalJump(ir::ConditionalJump &jump) {}
    virtual void visitReturn(ir::Return &jump) {}
    virtual void visitCall(ir::Call &call) {}
};
