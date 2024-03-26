#include "ConstantPropagationVisitor.h"
#include "ir.h"

void ConstantPropagationVisitor::visitCFG(ir::CFG &cfg) {
    for (const auto &block : cfg.getBlocks()) {
        for (auto &i : block->getInstructions()) {
            this->instr = &i;
            i->accept(*this);
        }
        constants.clear();
    }
}

void ConstantPropagationVisitor::visitAffectConst(ir::AffectConst &affect) {
    this->constants[affect.getTo()] = affect.getValue();
}

void ConstantPropagationVisitor::visitAffect(ir::Affect &affect) {
    if (this->constants.find(affect.getFrom()) != this->constants.end()) {
        int value = this->constants.at(affect.getFrom());
        this->constants[affect.getTo()] = value;
        ir::IRInstr *affectConst = new ir::AffectConst(affect.getTo(), value);
        affectConst->setBlock(&affect.getBlock());
        this->instr->reset(affectConst);
    } else {
        constants.erase(affect.getTo());
    }
}

void ConstantPropagationVisitor::visitUnaryOp(ir::UnaryOp &unaryOp) {
    if (this->constants.find(unaryOp.getFrom()) != this->constants.end()) {
        int value = this->constants.at(unaryOp.getFrom());

        switch (unaryOp.getType()) {
        case ir::UnaryOp::NEG:
            value = -value;
            break;
        case ir::UnaryOp::NOT:
            value = !value;
            break;
        }

        this->constants[unaryOp.getTo()] = value;
        ir::IRInstr *unaryOpConst = new ir::AffectConst(unaryOp.getTo(), value);
        unaryOpConst->setBlock(&unaryOp.getBlock());
        this->instr->reset(unaryOpConst);
    } else {
        constants.erase(unaryOp.getTo());
    }

}

void ConstantPropagationVisitor::visitBinOp(ir::BinOp &binOp) {
    if (this->constants.find(binOp.getLeft()) != this->constants.end() &&
        this->constants.find(binOp.getRight()) != this->constants.end()) {
        int value = this->constants.at(binOp.getLeft());
        int rightValue = this->constants.at(binOp.getRight());

        switch (binOp.getType()) {
        case ir::BinOp::ADD:
            value += rightValue;
            break;
        case ir::BinOp::SUB:
            value -= rightValue;
            break;
        case ir::BinOp::MUL:
            value *= rightValue;
            break;
        case ir::BinOp::DIV:
            value /= rightValue;
            break;
        case ir::BinOp::MOD:
            value %= rightValue;
            break;
        }

        this->constants[binOp.getTo()] = value;
        ir::IRInstr *binOpConst = new ir::AffectConst(binOp.getTo(), value);
        binOpConst->setBlock(&binOp.getBlock());
        this->instr->reset(binOpConst);
    } else {
        constants.erase(binOp.getTo());
    }

}
