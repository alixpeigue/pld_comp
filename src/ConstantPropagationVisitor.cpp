#include "ConstantPropagationVisitor.h"
#include "ir.h"
#include <memory>

void ConstantPropagationVisitor::visitCFG(ir::CFG &cfg) {
    for (const auto &block : cfg.getBlocks()) {
        for (size_t i = 0; i < block->getInstructions().size(); ++i) {
            this->instr_index = i;
            block->getInstructions()[i]->accept(*this);
        }
        // we do not propagate beyond blocks
        constants.clear();
    }
}

void ConstantPropagationVisitor::visitAffectConst(ir::AffectConst &affect) {
    // after that, we know that to = value
    this->constants[affect.getTo()] = affect.getValue();
}

void ConstantPropagationVisitor::visitAffect(ir::Affect &affect) {
    if (this->constants.find(affect.getFrom()) != this->constants.end()) {
        // if we have a value for "from", we can replace the Affect by an AffectConst
        // we now also have a value for "to"
        int value = this->constants.at(affect.getFrom());
        this->constants[affect.getTo()] = value;
        ir::IRInstr *affectConst = new ir::AffectConst(affect.getTo(), value);
        affectConst->setBlock(&affect.getBlock());
        affectConst->getBlock().replaceInstr(instr_index, affectConst);
    } else {
        // if we don't have a value, it kills any previously know falue for "to"
        constants.erase(affect.getTo());
    }
}

void ConstantPropagationVisitor::visitUnaryOp(ir::UnaryOp &unaryOp) {
    if (this->constants.find(unaryOp.getFrom()) != this->constants.end()) {
        // if we know the value, we can calculate the result
        int value = this->constants.at(unaryOp.getFrom());

        switch (unaryOp.getType()) {
        case ir::UnaryOp::NEG:
            value = -value;
            break;
        case ir::UnaryOp::NOT:
            value = !value;
            break;
        case ir::UnaryOp::PRE_INC:
            this->constants[unaryOp.getFrom()] = ++value;
            break;
        case ir::UnaryOp::PRE_DEC:
            this->constants[unaryOp.getFrom()] = --value;
            break;
        case ir::UnaryOp::POST_INC: {
            this->constants[unaryOp.getFrom()] = value + 1;
            std::unique_ptr<ir::AffectConst> instr =
                std::make_unique<ir::AffectConst>(unaryOp.getFrom(), value + 1);
            instr->setBlock(&unaryOp.getBlock());
            instr->getBlock().addInstr(instr_index, std::move(instr));
            break;
        }
        case ir::UnaryOp::POST_DEC: {
            this->constants[unaryOp.getFrom()] = value - 1;
            std::unique_ptr<ir::AffectConst> instr =
                std::make_unique<ir::AffectConst>(unaryOp.getFrom(), value - 1);
            instr->setBlock(&unaryOp.getBlock());
            instr->getBlock().addInstr(instr_index, std::move(instr));
            break;
        }
        default:
            return;
        }

        this->constants[unaryOp.getTo()] = value;
        ir::IRInstr *unaryOpConst = new ir::AffectConst(unaryOp.getTo(), value);
        unaryOpConst->setBlock(&unaryOp.getBlock());
        unaryOpConst->getBlock().replaceInstr(instr_index, unaryOpConst);
    } else {
        // if we don't have a value, it kills any previously know falue for "to"
        constants.erase(unaryOp.getTo());
    }
}

void ConstantPropagationVisitor::visitBinOp(ir::BinOp &binOp) {
    // we propagate, compute and/or simplify expressions
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
        case ir::BinOp::SHIFT_R:
            value >>= rightValue;
            break;
        case ir::BinOp::SHIFT_L:
            value <<= rightValue;
            break;
        case ir::BinOp::GT:
            value = value > rightValue;
            break;
        case ir::BinOp::LT:
            value = value < rightValue;
            break;
        case ir::BinOp::GTE:
            value = value >= rightValue;
            break;
        case ir::BinOp::LTE:
            value = value <= rightValue;
            break;
        case ir::BinOp::EQ:
            value = value == rightValue;
            break;
        case ir::BinOp::NEQ:
            value = value != rightValue;
            break;
        case ir::BinOp::AND_BIN:
            value = value & rightValue;
            break;
        case ir::BinOp::OR_BIN:
            value = value | rightValue;
            break;
        case ir::BinOp::XOR_BIN:
            value = value ^ rightValue;
            break;
        default:
            return;
        }

        this->constants[binOp.getTo()] = value;
        ir::IRInstr *binOpConst = new ir::AffectConst(binOp.getTo(), value);
        binOpConst->setBlock(&binOp.getBlock());
        binOpConst->getBlock().replaceInstr(instr_index, binOpConst);
    } else if (this->constants.find(binOp.getLeft()) != this->constants.end() &&
               this->constants.at(binOp.getLeft()) == 0) {
        ir::IRInstr *instr;
        if (binOp.getType() == ir::BinOp::ADD ||
            binOp.getType() == ir::BinOp::SUB ||
            binOp.getType() == ir::BinOp::OR_BIN ||
            binOp.getType() == ir::BinOp::XOR_BIN) {
            instr = new ir::Affect(binOp.getTo(), binOp.getRight());
        } else if (binOp.getType() == ir::BinOp::MOD ||
                   binOp.getType() == ir::BinOp::DIV ||
                   binOp.getType() == ir::BinOp::SHIFT_R ||
                   binOp.getType() == ir::BinOp::SHIFT_L ||
                   binOp.getType() == ir::BinOp::AND_BIN) {
            instr = new ir::AffectConst(binOp.getTo(), 0);
        } else {
            constants.erase(binOp.getTo());
            return;
        }
        instr->setBlock(&binOp.getBlock());
        instr->getBlock().replaceInstr(instr_index, instr);
    } else if (this->constants.find(binOp.getLeft()) != this->constants.end() &&
               this->constants.at(binOp.getLeft()) == 1 &&
               binOp.getType() == ir::BinOp::MUL) {
        ir::IRInstr *affect = new ir::Affect(binOp.getTo(), binOp.getRight());
        affect->setBlock(&binOp.getBlock());
        affect->getBlock().replaceInstr(instr_index, affect);
    } else if (this->constants.find(binOp.getRight()) !=
                   this->constants.end() &&
               this->constants.at(binOp.getRight()) == 0 &&
               (binOp.getType() == ir::BinOp::ADD ||
                binOp.getType() == ir::BinOp::SUB ||
                binOp.getType() == ir::BinOp::OR_BIN ||
                binOp.getType() == ir::BinOp::XOR_BIN ||
                binOp.getType() == ir::BinOp::SHIFT_L ||
                binOp.getType() == ir::BinOp::SHIFT_R)) {
        ir::IRInstr *affect = new ir::Affect(binOp.getTo(), binOp.getLeft());
        affect->setBlock(&binOp.getBlock());
        affect->getBlock().replaceInstr(instr_index, affect);
    } else if (this->constants.find(binOp.getRight()) !=
                   this->constants.end() &&
               this->constants.at(binOp.getRight()) == 1) {
        ir::IRInstr *instr;
        if (binOp.getType() == ir::BinOp::MUL ||
            binOp.getType() == ir::BinOp::DIV) {
            instr = new ir::Affect(binOp.getTo(), binOp.getLeft());
        } else if (binOp.getType() == ir::BinOp::MOD) {
            instr = new ir::AffectConst(binOp.getTo(), 0);
        } else {
            constants.erase(binOp.getTo());
            return;
        }
        instr->setBlock(&binOp.getBlock());
        instr->getBlock().replaceInstr(instr_index, instr);
    } else {
        // if we don't have a value, it kills any previously know falue for "to"
        constants.erase(binOp.getTo());
    }
}
