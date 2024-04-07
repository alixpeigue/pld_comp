#include "UnusedTempVarRemoverVisitor.h"
#include "ir.h"
#include <algorithm>

void UnusedTempVarRemoverVisitor::visitCFG(ir::CFG &cfg) {
    for (const auto &block : cfg.getBlocks()) {
        for (size_t i = 0; i < block->getInstructions().size(); ++i) {
            instr_index = i;
            block->getInstructions()[i]->accept(*this);
        }
        auto next = block->getNext();
        next->accept(*this);
        block->setNext(std::move(next));
        std::vector<int> indexes;
        for (const auto &[_, index] : tempAffects) {
            indexes.push_back(index);
        }
        std::sort(indexes.begin(), indexes.end(), std::greater<>());
        for (auto i : indexes) {
            block->getInstructions().erase(block->getInstructions().begin() +
                                           i);
        }
        tempAffects.clear();
    }
}

void UnusedTempVarRemoverVisitor::visitAffectConst(ir::AffectConst &affect) {
    if (affect.getTo()[0] == '#' && affect.getTo() != "#return") {
        this->tempAffects[affect.getTo()] = instr_index;
    }
}

void UnusedTempVarRemoverVisitor::visitAffect(ir::Affect &affect) {
    if (affect.getFrom()[0] == '#') {
        this->tempAffects.erase(affect.getFrom());
    }
}

void UnusedTempVarRemoverVisitor::visitUnaryOp(ir::UnaryOp &unaryOp) {
    if (unaryOp.getFrom()[0] == '#') {
        this->tempAffects.erase(unaryOp.getFrom());
    }
}

void UnusedTempVarRemoverVisitor::visitBinOp(ir::BinOp &binOp) {
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

void UnusedTempVarRemoverVisitor::visitCall(ir::Call &call) {
    for (const auto &arg : call.getNames()) {
        this->tempAffects.erase(arg);
    }
}

void UnusedTempVarRemoverVisitor::visitConditionalJump(
    ir::ConditionalJump &jump) {

    this->tempAffects.erase(jump.getCondition());
}

void UnusedTempVarRemoverVisitor::visitSwitchJump(ir::SwitchJump &jump) {
    this->tempAffects.erase(jump.getExpressionTest());
}
