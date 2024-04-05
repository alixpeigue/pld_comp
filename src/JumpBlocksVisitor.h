#pragma once

#include "IRBaseVisitor.h"
#include "ir.h"
class JumpBlocksVisitor : public IRBaseVisitor {
    virtual void visitCFG(ir::CFG &cfg) override;
    virtual void visitBasicBlock(ir::BasicBlock &bb) override;
    virtual void visitUnconditionalJump(ir::UnconditionalJump &jump) override;
    virtual void visitConditionalJump(ir::ConditionalJump &jump) override;
    // virtual void visitSwitchJump(ir::SwitchJump &jump) override;

    ir::BasicBlock *canSkip(ir::BasicBlock *bb);
};
