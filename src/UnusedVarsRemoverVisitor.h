#pragma once

#include "IRBaseVisitor.h"
#include "ir.h"
#include <unordered_map>
#include <unordered_set>

class UnusedVarsRemoverVisitor : public IRBaseVisitor {
    virtual void visitAffect(ir::Affect &affect) override;
    virtual void visitUnaryOp(ir::UnaryOp &unaryOp) override;
    virtual void visitBinOp(ir::BinOp &binOp) override;
    virtual void visitCFG(ir::CFG &cfg) override;
    virtual void visitCall(ir::Call &call) override;
    virtual void visitConditionalJump(ir::ConditionalJump &jump) override;
    virtual void visitSwitchJump(ir::SwitchJump &jump) override;

protected:
    std::unordered_map<Scope *, std::unordered_set<std::string>> usedVars;
};
