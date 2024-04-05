
#include "RemoveBlocksVisitor.h"
#include "ir.h"
#include <memory>

void RemoveBlocksVisitor::visitCFG(ir::CFG &cfg) {
    bool changed = true;
    while (changed) {
        cfg.getBlocks()[0]->accept(*this);
        std::vector<int> toRemove;
        for (int i = cfg.getBlocks().size() - 1; i >= 0; --i) {
            if (visited.find(cfg.getBlocks()[i].get()) == visited.end()) {
                toRemove.push_back(i);
            }
        }

        for (auto &el : toRemove) {
            cfg.getBlocks().erase(cfg.getBlocks().begin() + el);
        }
        changed = !toRemove.empty();
    }
}

void RemoveBlocksVisitor::visitBasicBlock(ir::BasicBlock &bb) {
    if (visited.find(&bb) != visited.end()) {
        return;
    }
    visited.insert(&bb);
    auto next = bb.getNext();
    next->accept(*this);
    bb.setNext(std::move(next));
}

void RemoveBlocksVisitor::visitUnconditionalJump(ir::UnconditionalJump &jump) {
    jump.getTo().accept(*this);
}

void RemoveBlocksVisitor::visitConditionalJump(ir::ConditionalJump &jump) {
    jump.getElse()->accept(*this);
    jump.getThen()->accept(*this);
}

void RemoveBlocksVisitor::visitSwitchJump(ir::SwitchJump &jump) {
    for (auto &[_, b] : jump.getCaseTests()) {
        b->accept(*this);
    }
    jump.getDefaultBlock()->accept(*this);
}
