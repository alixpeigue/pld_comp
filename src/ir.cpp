/**
 * @file ir.cpp
 * @author H4231
 * @brief Définition ddu namespace ir qui contient les classes nécessaires pour
 *        le développement de l'IR
 * @date 2024-04-02
 * 
 */

#include "IRBaseVisitor.h"
#include "ir.h"
#include <algorithm>
#include <cstdint>
#include <iostream>

/**
 * @brief ajouté une instruction IR au bloc
 * 
 * @param instr l'instruction IR a ajouté
 */
void ir::BasicBlock::addInstr(std::unique_ptr<IRInstr> instr) {
    instr->block = this;
    this->instrs.push_back(std::move(instr));
}

void ir::BasicBlock::accept(IRBaseVisitor &visitor) {
    visitor.visitBasicBlock(*this);
}

void ir::BasicBlock::setNext(std::unique_ptr<ir::Next> next) {
    next->block = this;
    this->next = std::move(next);
}

/**
 * @brief ajoute un bloc au CFG
 * 
 * @param block le bloc ajouté
 */
void ir::CFG::addBlock(std::unique_ptr<BasicBlock> block) {
    block->cfg = this;
    this->blocks.push_back(std::move(block));
}

/**
 * @brief ajoute un scope au CGF
 * 
 * @param scope Le scope a ajouté
 */
void ir::CFG::addScope(std::unique_ptr<Scope> scope) {
    this->scopes.push_back(std::move(scope));
}

void ir::CFG::visitBlocks(IRBaseVisitor &visitor) {
    visitor.visitCFG(*this);
}

/**
 * @brief Retourne la taille totale du CFG
 * 
 * @return la taille du CFG
 */
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
