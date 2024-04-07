#include "UnusedTempVarRemoverVisitor.h"
#include "ir.h"
#include <algorithm>

void UnusedAffectRemoverVisitor::visitCFG(ir::CFG &cfg) {
    for (const auto &block : cfg.getBlocks()) {
        for (size_t i = 0; i < block->getInstructions().size(); ++i) {
            instr_index = i;
            block->getInstructions()[i]->accept(*this);
        }
        auto next = block->getNext();
        next->accept(*this);
        block->setNext(std::move(next));
        for (const auto &[name, index] : tempAffects) {
            // block->getScope().removeVariable(name);
            indexes.push_back(index);
        }
        std::sort(indexes.begin(), indexes.end(), std::greater<>());
        for (auto i : indexes) {
            block->getInstructions().erase(block->getInstructions().begin() +
                                           i);
        }
        tempAffects.clear();
        indexes.clear();
    }
}

void UnusedAffectRemoverVisitor::visitAffectConst(ir::AffectConst &affect) {
    if (affect.getTo()[0] == '#' && affect.getTo() != "#return") {
        if (tempAffects.find(affect.getTo()) != tempAffects.end()) {
            indexes.push_back(this->tempAffects[affect.getTo()]);
        }
        this->tempAffects[affect.getTo()] = instr_index;
    }
}

void UnusedAffectRemoverVisitor::visitAffect(ir::Affect &affect) {
    if (affect.getFrom()[0] == '#') {
        this->tempAffects.erase(affect.getFrom());
    }
    if (affect.getTo()[0] == '#' && affect.getTo() != "#return") {
        if (tempAffects.find(affect.getTo()) != tempAffects.end()) {
            indexes.push_back(this->tempAffects[affect.getTo()]);
        }
        this->tempAffects[affect.getTo()] = instr_index;
    }
}

void UnusedAffectRemoverVisitor::visitUnaryOp(ir::UnaryOp &unaryOp) {
    if (unaryOp.getFrom()[0] == '#') {
        this->tempAffects.erase(unaryOp.getFrom());
    }
}

void UnusedAffectRemoverVisitor::visitBinOp(ir::BinOp &binOp) {
    std::string tempVarName;
    if (binOp.getLeft()[0] == '#') {
        tempVarName = binOp.getLeft();
    } else if (binOp.getRight()[0] == '#') {
        tempVarName = binOp.getRight();
    } else {
        return;
    }

    this->tempAffects.erase(tempVarName);
}

void UnusedAffectRemoverVisitor::visitCall(ir::Call &call) {
    for (const auto &arg : call.getNames()) {
        this->tempAffects.erase(arg);
    }
}

void UnusedAffectRemoverVisitor::visitConditionalJump(
    ir::ConditionalJump &jump) {

    this->tempAffects.erase(jump.getCondition());
}

void UnusedAffectRemoverVisitor::visitSwitchJump(ir::SwitchJump &jump) {
    this->tempAffects.erase(jump.getExpressionTest());
}
