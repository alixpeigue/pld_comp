/**
 * @file UnusedTempVarRemoverVisitor.h
 * @date 2024-04-02
 */

#pragma once

#include "IRBaseVisitor.h"
#include "ir.h"

/**
 * @brief Visiteur qui supprime les assignations inutiles
 *
 */
class UnusedAffectsRemoverVisitor : public IRBaseVisitor {
    virtual void visitAffectConst(ir::AffectConst &affect) override;
    virtual void visitAffect(ir::Affect &affect) override;
    virtual void visitUnaryOp(ir::UnaryOp &unaryOp) override;
    virtual void visitBinOp(ir::BinOp &binOp) override;
    virtual void visitCFG(ir::CFG &cfg) override;
    virtual void visitCall(ir::Call &call) override;
    virtual void visitConditionalJump(ir::ConditionalJump &jump) override;
    virtual void visitSwitchJump(ir::SwitchJump &jump) override;

protected:
    std::unordered_map<std::string, int> tempAffects;
    std::vector<int> indexes;
    size_t instr_index;
};
