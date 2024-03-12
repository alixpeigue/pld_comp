#include "IRBaseVisitor.h"
#include "ir.h"

void ir::BasicBlock::addInstr(std::unique_ptr<IRInstr> instr) {
    this->instrs.push_back(std::move(instr));
}

void ir::BasicBlock::visit(IRBaseVisitor &visitor) {
    for (auto &instr : this->instrs) {
        instr->accept(visitor);
    }
}

void ir::CFG::addRoot(std::unique_ptr<BasicBlock> block) {
    this->roots.push_back(std::move(block));
}

void ir::CFG::visitRoots(IRBaseVisitor &visitor) {
    for (auto &root : roots) {
        root->visit(visitor);
    }
}

void ir::Affect::accept(IRBaseVisitor &visitor) { visitor.visitAffect(*this); }
