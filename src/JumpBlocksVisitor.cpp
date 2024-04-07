#include "JumpBlocksVisitor.h"
#include "ir.h"
#include <memory>

void JumpBlocksVisitor::visitCFG(ir::CFG &cfg) {
    for (auto &block : cfg.getBlocks()) {
        block->accept(*this);
    }
}

void JumpBlocksVisitor::visitBasicBlock(ir::BasicBlock &bb) {
    auto next = bb.getNext();
    next->accept(*this);
    bb.setNext(std::move(next));
}

void JumpBlocksVisitor::visitUnconditionalJump(ir::UnconditionalJump &jump) {
    if (auto bb = this->canSkip(&jump.getTo())) {
        jump.setTo(bb);
    }
}

void JumpBlocksVisitor::visitConditionalJump(ir::ConditionalJump &jump) {
    if (auto bb = this->canSkip(jump.getThen())) {
        jump.setThen(bb);
    }
    if (auto bb = this->canSkip(jump.getElse())) {
        jump.setElse(bb);
    }
}

void JumpBlocksVisitor::visitSwitchJump(ir::SwitchJump &jump) {
    for (auto &[cond, block] : jump.getCaseTests()) {
        if (auto bb = this->canSkip(block)) {
            block = bb;
        }
    }
}

ir::BasicBlock *JumpBlocksVisitor::canSkip(ir::BasicBlock *bb) {
    if (bb->getInstructions().empty()) {
        std::unique_ptr<ir::Next> next = bb->getNext();
        if (auto j = dynamic_cast<ir::UnconditionalJump *>(next.get())) {
            bb->setNext(std::move(next));
            return &j->getTo();
        }
        bb->setNext(std::move(next));
    }
    return nullptr;
}
