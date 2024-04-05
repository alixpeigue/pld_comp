/**
 * @file RV64Visitor.cpp
 * @author H4231
 * @brief Génération du code assembleur RISC-V
 * @date 2024-04-05
 * 
 */


#include "IRx86Visitor.h"
#include "RV64Visitor.h"
#include "Scope.h"
#include "ir.h"
#include <algorithm>
#include <iostream>
#include <sstream>

/**
 * @brief Donne le code assembleur RISC-V correspondant à une instruction d'affectation d'une variable à une autre
 * 
 * @param affect Une instruction d'affectation variable à variable
 */

void RV64Visitor::visitAffect(ir::Affect &affect) {
    Scope &scope = affect.getBlock().getScope();
    Variable to = scope.getVariable(affect.getTo()).value();
    Variable from = scope.getVariable(affect.getFrom()).value();
    os << "    lw a5, -" << from.second << "(s0)\n";
    os << "    sw a5, -" << to.second << "(s0)\n";
}


/**
 * @brief Donne le code assembleur RISC-V correspondant à une instruction d'affectation d'une constante à une variable
 * 
 * @param affectConst Une instruction d'affectation constante à variable
 */
void RV64Visitor::visitAffectConst(ir::AffectConst &affectConst) {
    Variable to = affectConst.getBlock()
                      .getScope()
                      .getVariable(affectConst.getTo())
                      .value();
    os << "    li a5, " << affectConst.getValue() << "\n";
    os << "    sw a5, -" << to.second << "(s0)\n";
}

/**
 * @brief Donne le code assembleur RISC-V correspondant à une instruction d'opération à deux opérandes, en fonction de l'opérateur
 * 
 * @param binop Une instruction d'opération à deux opérandes
 */

void RV64Visitor::visitBinOp(ir::BinOp &binop) {
    Scope &scope = binop.getBlock().getScope();
    Variable to = scope.getVariable(binop.getTo()).value();
    Variable left = scope.getVariable(binop.getLeft()).value();
    Variable right = scope.getVariable(binop.getRight()).value();

    ir::BinOp::BinOpType type = binop.getType();

    os << "    lw a5, -" << left.second << "(s0)\n";
    os << "    lw a6, -" << right.second << "(s0)\n";

    switch (type) {
    case ir::BinOp::GT:
        os << "    sgt a5, a5, a6\n";
        // os << "    andi a5, a5, 1\n";
        break;
    case ir::BinOp::GTE:
        os << "    slt a5, a5, a6\n";
        os << "    seqz a5, a5\n";
        // os << "    andi a5, a5, 1\n";
        break;
    case ir::BinOp::LT:
        os << "    slt a5, a5, a6\n";
        // os << "    andi a5, a5, 1\n";
        break;
    case ir::BinOp::LTE:
        os << "    sgt a5, a5, a6\n";
        os << "    seqz a5, a5\n";
        // os << "    andi a5, a5, 1\n";
        break;
    case ir::BinOp::EQ:
        os << "    sub a5, a5, a6\n";
        os << "    seqz a5, a5\n";
        // os << "    andi a5, a5, 1\n";
        break;
    case ir::BinOp::NEQ:
        os << "    sub a5, a5, a6\n";
        os << "    snez a5, a5\n";
        // os << "    andi a5, a5, 1\n";
        break;
    default:
        os << "    " << this->getInstrFromBinOp(type) << " a5, a5, a6\n";
    }

    os << "    sw a5, -" << to.second << "(s0)\n";
}

/**
 * @brief Donne le code assembleur RISC-V correspondant à une instruction d'opération à un opérande, en fonction de l'opérateur 
 * 
 * @param unaryop Une instruction d'opération à un opérande
 */

void RV64Visitor::visitUnaryOp(ir::UnaryOp &unaryop) {
    Scope &scope = unaryop.getBlock().getScope();
    Variable to = scope.getVariable(unaryop.getTo()).value();
    Variable from = scope.getVariable(unaryop.getFrom()).value();

    ir::UnaryOp::UnaryOpType type = unaryop.getType();

    os << "    lw a5, -" << from.second << "(s0)\n";

    if (type == ir::UnaryOp::POST_DEC) {
        os << "    sw a5, -" << to.second << "(s0)\n";
        os << "    addi a5, a5, -1\n";
        os << "    sw a5, -" << from.second << "(s0)\n";
        return;
    }
    if (type == ir::UnaryOp::POST_INC) {
        os << "    sw a5, -" << to.second << "(s0)\n";
        os << "    addi a5, a5, 1\n";
        os << "    sw a5, -" << from.second << "(s0)\n";
        return;
    }

    switch (unaryop.getType()) {
    case ir::UnaryOp::NEG:
        os << "    subw a5, zero, a5\n";
        break;
    case ir::UnaryOp::PRE_DEC:
        os << "    addi a5, a5, -1\n";
        os << "    sw a5, -" << from.second << "(s0)\n";
        break;
    case ir::UnaryOp::PRE_INC:
        os << "    addi a5, a5, 1\n";
        os << "    sw a5, -" << from.second << "(s0)\n";
        break;
    case ir::UnaryOp::NOT:
        os << "    seqz a5, a5\n";
        break;
    default:
        break;
    }

    os << "    sw a5, -" << to.second << "(s0)\n";
}

/**
 * @brief Donne le code assembleur RISC-V correspondant à un block. Rajoute les instructions du block puis un jump vers le block suivant.
 * 
 * @param bb  Un block
 */

void RV64Visitor::visitBasicBlock(ir::BasicBlock &bb) {
    os << bb.getName() << ":\n";
    for (auto &instruction : bb.getInstructions()) {
        instruction->accept(*this);
    }
    // Call end block here :
    auto next = bb.getNext();
    next->accept(*this);
    bb.setNext(std::move(next));
}


/**
 * @brief Donne le code assembleur RISC-V correspondant au graphique de flot de contrôle (CFG) donné .
 * 
 * @param cfg Un graphique de flot de contrôle (CFG)
 */
void RV64Visitor::visitCFG(ir::CFG &cfg) {

    std::vector<std::string> regs = {"a0", "a1", "a2", "a3",
                                     "a4", "a5", "a6", "a7"};

    int size = ((cfg.getSize() + 16 + 15) / 16) * 16;

    // prelude
    os << ".globl " << cfg.getName() << "\n";
    os << cfg.getName() << ":\n";
    os << "    addi sp, sp, -" << size << "\n";
    os << "    sd ra, 24(sp)\n";
    os << "    sd s0, 16(sp)\n";
    os << "    addi s0, sp, -" << size << '\n';
    if (cfg.getName() == "main") {
        os << "    mv a5, zero\n";
        os << "    sw a5, -"
           << cfg.getEpilogue().getScope().getVariable("#return").value().second
           << "(s0)\n";
    }

    for (size_t i = 0; i < cfg.getArgs().size() && i < regs.size(); ++i) {
        os << "    mv a5, " << regs[i] << "\n";
        os << "    sw a5, -"
           << cfg.getBlocks()[0]
                  ->getScope()
                  .getVariable(cfg.getArgs()[i].first)
                  .value()
                  .second
           << "(s0)\n";
    }

    // blocks
    for (auto &block : cfg.getBlocks()) {
        block->accept(*this);
    }

    // epilogue
}


/**
 * @brief Donne le code assembleur RISC-V correspondant à une instruction de saut absolu
 * 
 * @param jump Une instruction de saut absolu
 */
void RV64Visitor::visitUnconditionalJump(ir::UnconditionalJump &jump) {
    os << "    j " << jump.getTo().getName() << "\n";
}

void RV64Visitor::visitConditionalJump(ir::ConditionalJump &jump) {
    Variable cond =
        jump.getBlock().getScope().getVariable(jump.getCondition()).value();
    os << "    lw a5, -" << cond.second << "(s0)\n";
    os << "    beq a5, zero, " << jump.getElse()->getName() << "\n";
    os << "    j " << jump.getThen()->getName() << "\n";
}

/**
 * @brief Donne le code assembleur RISC-V  correspondant à une instruction switch jump
 * 
 * @param jump Une instruction switch jump
 */

void RV64Visitor::visitSwitchJump(ir::SwitchJump &jump) {
    Variable expr = jump.getBlock()
                        .getScope()
                        .getVariable(jump.getExpressionTest())
                        .value();
    auto caseTests = jump.getCaseTests();
    for (auto &caseTest : caseTests) {
        os << "    li a6, " << caseTest.first << "\n";
        os << "    lw a5, -" << expr.second << "(s0)\n";
        os << "    beq a5, a6, " << caseTest.second->getName() << "\n";
    }
};

/**
 * @brief Donne le code assembleur RISC-V correspondant à une instruction de retour
 * 
 * @param ret Une instruction de retour
 */
void RV64Visitor::visitReturn(ir::Return &ret) {
    Variable retVar = ret.getBlock().getScope().getVariable("#return").value();

    int size = ret.getBlock().getCFG().getSize() + 16;
    size = ((size + 15) / 16) * 16;

    os << "    lw a0, -" << retVar.second << "(s0)\n";
    os << "    ld ra, 24(sp)\n";
    os << "    ld s0, 16(sp)\n";
    os << "    addi sp, sp, " << size << "\n";
    os << "    jr ra\n";
}


/**
 * @brief Donne le code assembleur RISC-V correspondant à un appel de fonction
 * 
 * @param call Un appel de fonction
 */

void RV64Visitor::visitCall(ir::Call &call) {

    std::vector<std::string> regs = {"a0", "a1", "a2", "a3",
                                     "a4", "a5", "a6", "a7"};

    size_t i;
    for (i = 0; i < call.getNames().size() && i < regs.size(); ++i) {
        os << "    lw " << regs[i] << ", -"
           << call.getBlock()
                  .getScope()
                  .getVariable(call.getNames()[i])
                  .value()
                  .second
           << "(s0)\n";
    }

    os << "    call " << call.getFunc_name() << "\n";
    Variable to = call.getBlock().getScope().getVariable(call.getRet()).value();
    os << "    sw a0, -" << to.second << "(s0)\n";
}



/**
 * @brief Renvoie l'instruction RISC-V correspondant a l'opérateur en paramètre
 * 
 * @param op 
 * @return Retour possible : div, rem, mulw, addw, subw, sllw, sraw, and, or, xor (std::string)
 */
inline std::string RV64Visitor::getInstrFromBinOp(ir::BinOp::BinOpType op) {
    switch (op) {
    case ir::BinOp::DIV:
        return "div";
    case ir::BinOp::MOD:
        return "rem";
    case ir::BinOp::MUL:
        return "mulw";
    case ir::BinOp::ADD:
        return "addw";
    case ir::BinOp::SUB:
        return "subw";
    case ir::BinOp::SHIFT_L:
        return "sllw";
    case ir::BinOp::SHIFT_R:
        return "sraw";
    case ir::BinOp::AND_BIN:
        return "and";
    case ir::BinOp::OR_BIN:
        return "or";
    case ir::BinOp::XOR_BIN:
        return "xor";
    default:
        return "";
    }
}
