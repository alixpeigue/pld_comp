#include "ConstantPropagationVisitor.h"
#include "ir.h"
#include <memory>

void ConstantPropagationVisitor::visitCFG(ir::CFG &cfg) {
    for (const auto &block : cfg.getBlocks()) {
        for (auto &i : block->getInstructions()) {
            this->instr = &i;
            i->accept(*this);
        }
    }
}

void ConstantPropagationVisitor::visitAffectConst(ir::AffectConst &affect) {
    this->constants.emplace(affect.getTo(), affect.getValue());
}

void ConstantPropagationVisitor::visitAffect(ir::Affect &affect) {
    if (this->constants.find(affect.getFrom()) != this->constants.end()) {
        int value = this->constants.at(affect.getFrom());
        this->constants.emplace(affect.getTo(), value);
        ir::IRInstr *affectConst = new ir::AffectConst(affect.getTo(), value);
        affectConst->setBlock(&affect.getBlock());
        this->instr->reset(affectConst);
    }
}
