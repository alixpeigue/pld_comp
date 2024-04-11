#include "UnusedVarsRemoverVisitor.h"
#include "ir.h"

void UnusedVarsRemoverVisitor::visitCFG(ir::CFG &cfg) {
    for (const auto &block : cfg.getBlocks()) {
        for (size_t i = 0; i < block->getInstructions().size(); ++i) {
            block->getInstructions()[i]->accept(*this);
        }
    }
    for (const auto &scope : cfg.getScopes()) {
        usedVars[scope.get()].insert("#return");
        std::vector<std::string> toRemove;
        auto &vars = scope->getVariables();
        for (const auto &[var, _] : vars) {
            if (usedVars[scope.get()].find(var) ==
                usedVars[scope.get()].end()) // var not in usedVars
                toRemove.push_back(var);
            // scope->removeVariable(var);
        }
        for (const auto &var : toRemove) {
            if (var[0] == '#') {
                scope->removeVariable(var);
            }
        }
    }
}

void UnusedVarsRemoverVisitor::visitAffect(ir::Affect &affect) {
    usedVars[&affect.getBlock().getScope()].insert(affect.getFrom());
}

void UnusedVarsRemoverVisitor::visitUnaryOp(ir::UnaryOp &unaryOp) {
    usedVars[&unaryOp.getBlock().getScope()].insert(unaryOp.getFrom());
}

void UnusedVarsRemoverVisitor::visitBinOp(ir::BinOp &binOp) {
    usedVars[&binOp.getBlock().getScope()].insert(binOp.getLeft());
    usedVars[&binOp.getBlock().getScope()].insert(binOp.getRight());
}

void UnusedVarsRemoverVisitor::visitCall(ir::Call &call) {
    auto &scope = call.getBlock().getScope();
    for (const auto &arg : call.getNames()) {
        usedVars[&scope].insert(arg);
    }
}

void UnusedVarsRemoverVisitor::visitConditionalJump(ir::ConditionalJump &jump) {

    usedVars[&jump.getBlock().getScope()].insert(jump.getCondition());
}

void UnusedVarsRemoverVisitor::visitSwitchJump(ir::SwitchJump &jump) {
    usedVars[&jump.getBlock().getScope()].insert(jump.getExpressionTest());
}
