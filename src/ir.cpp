#include "IRBaseVisitor.h"
#include "ir.h"
#include <algorithm>
#include <cstdint>
#include <iostream>

void ir::BasicBlock::addInstr(std::unique_ptr<IRInstr> instr) {
    instr->block = this;
    this->instrs.push_back(std::move(instr));
}

void ir::BasicBlock::accept(IRBaseVisitor &visitor) {
    // visitor.visitEnterBlock(*this);
    // for (auto &instr : this->instrs) {
    //     instr->accept(visitor);
    // }
    // visitor.visitLeaveBlock(*this);
    visitor.visitBasicBlock(*this);
}

void ir::BasicBlock::setNext(std::unique_ptr<ir::Next> next) {
    next->block = this;
    this->next = std::move(next);
}

void ir::CFG::addBlock(std::unique_ptr<BasicBlock> block) {
    block->cfg = this;
    this->blocks.push_back(std::move(block));
}

void ir::CFG::addScope(std::unique_ptr<Scope> block) {
    // block->cfg = this;
    this->scopes.push_back(std::move(block));
}

void ir::CFG::visitBlocks(IRBaseVisitor &visitor) {
    // visitor.visitPrelude(*this);
    // for (auto &block : blocks) {
    //     block->accept(visitor);
    // }
    // visitor.visitEpilogue(*this);
    visitor.visitCFG(*this);
}

uint32_t ir::CFG::getSize() {
    uint32_t size = 0;
    
    for (const auto &scope: scopes) {
        size = std::max(size, scope->getSize());
    }

    return size;
}

void ir::Affect::accept(IRBaseVisitor &visitor) { visitor.visitAffect(*this); }
void ir::AffectConst::accept(IRBaseVisitor &visitor) {
    visitor.visitAffectConst(*this);
}
void ir::BinOp::accept(IRBaseVisitor &visitor) { visitor.visitBinOp(*this); }
void ir::UnaryOp::accept(IRBaseVisitor &visitor) {
    visitor.visitUnaryOp(*this);
}

void ir::UnconditionalJump::accept(IRBaseVisitor &visitor) {
    visitor.visitUnconditionalJump(*this);
}

void ir::ConditionalJump::accept(IRBaseVisitor &visitor) {
    visitor.visitConditionalJump(*this);
}

void::ir::SwitchJump::accept(IRBaseVisitor &visitor) {
    visitor.visitSwitchJump(*this);
}

void ir::Return::accept(IRBaseVisitor &visitor) { visitor.visitReturn(*this); }

void ir::Call::accept(IRBaseVisitor &visitor) { visitor.visitCall(*this); }
