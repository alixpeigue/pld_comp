#pragma once

#include "IRBaseVisitor.h"
#include "ir.h"
#include <unordered_set>

/**
 * @brief Visiteur qui supprime les blocks inutilisés.
 * Les optimisations peuvent mener à avoir des blocks qui ne sont les suivants de personne. 
 * Ces blocks peuvent être éliminés pour ne pas générer leur code.
 */
class RemoveBlocksVisitor : public IRBaseVisitor {
    virtual void visitCFG(ir::CFG &cfg) override;
    virtual void visitBasicBlock(ir::BasicBlock &bb) override;
    virtual void visitUnconditionalJump(ir::UnconditionalJump &jump) override;
    virtual void visitConditionalJump(ir::ConditionalJump &jump) override;
    virtual void visitSwitchJump(ir::SwitchJump &jump) override;

    ir::BasicBlock *canSkip(ir::BasicBlock *bb);

    std::unordered_set<ir::BasicBlock *> visited;
};
