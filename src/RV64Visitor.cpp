#include "IRx86Visitor.h"
#include "RV64Visitor.h"
#include "Scope.h"
#include "ir.h"
#include <algorithm>
#include <iostream>
#include <sstream>

void RV64Visitor::visitAffect(ir::Affect &affect) {
    Scope &scope = affect.getBlock().getScope();
    Variable to = scope.getVariable(affect.getTo()).value();
    Variable from = scope.getVariable(affect.getFrom()).value();
    std::cout << "    lw a5, -" << from.second << "(s0)\n";
    std::cout << "    sw a5, -" << to.second << "(s0)\n";
}

void RV64Visitor::visitAffectConst(ir::AffectConst &affectConst) {
    Variable to = affectConst.getBlock()
                      .getScope()
                      .getVariable(affectConst.getTo())
                      .value();
    std::cout << "    li a5, " << affectConst.getValue() << "\n";
    std::cout << "    sw a5, -" << to.second << "(s0)\n";
}

void RV64Visitor::visitBinOp(ir::BinOp &binop) {
    Scope &scope = binop.getBlock().getScope();
    Variable to = scope.getVariable(binop.getTo()).value();
    Variable left = scope.getVariable(binop.getLeft()).value();
    Variable right = scope.getVariable(binop.getRight()).value();

    ir::BinOp::BinOpType type = binop.getType();

    std::cout << "    lw a5, -" << left.second << "(s0)\n";
    std::cout << "    lw a6, -" << right.second << "(s0)\n";

    switch (type) {
    case ir::BinOp::GT:
        std::cout << "    sgt a5, a5, a6\n";
        // std::cout << "    andi a5, a5, 1\n";
        break;
    case ir::BinOp::GTE:
        std::cout << "    slt a5, a5, a6\n";
        std::cout << "    seqz a5, a5\n";
        // std::cout << "    andi a5, a5, 1\n";
        break;
    case ir::BinOp::LT:
        std::cout << "    slt a5, a5, a6\n";
        // std::cout << "    andi a5, a5, 1\n";
        break;
    case ir::BinOp::LTE:
        std::cout << "    sgt a5, a5, a6\n";
        std::cout << "    seqz a5, a5\n";
        // std::cout << "    andi a5, a5, 1\n";
        break;
    case ir::BinOp::EQ:
        std::cout << "    sub a5, a5, a6\n";
        std::cout << "    seqz a5, a5\n";
        // std::cout << "    andi a5, a5, 1\n";
        break;
    case ir::BinOp::NEQ:
        std::cout << "    sub a5, a5, a6\n";
        std::cout << "    snez a5, a5\n";
        // std::cout << "    andi a5, a5, 1\n";
        break;
    default:
        std::cout << "    " << this->getInstrFromBinOp(type) << " a5, a5, a6\n";
    }

    std::cout << "    sw a5, -" << to.second << "(s0)\n";
}

void RV64Visitor::visitUnaryOp(ir::UnaryOp &unaryop) {
    Scope &scope = unaryop.getBlock().getScope();
    Variable to = scope.getVariable(unaryop.getTo()).value();
    Variable from = scope.getVariable(unaryop.getFrom()).value();

    ir::UnaryOp::UnaryOpType type = unaryop.getType();

    std::cout << "    lw a5, -" << from.second << "(s0)\n";

    if (type == ir::UnaryOp::POST_DEC) {
        std::cout << "    sw a5, -" << to.second << "(s0)\n";
        std::cout << "    addi a5, a5, -1\n";
        std::cout << "    sw a5, -" << from.second << "(s0)\n";
        return;
    }
    if (type == ir::UnaryOp::POST_INC) {
        std::cout << "    sw a5, -" << to.second << "(s0)\n";
        std::cout << "    addi a5, a5, 1\n";
        std::cout << "    sw a5, -" << from.second << "(s0)\n";
        return;
    }

    switch (unaryop.getType()) {
    case ir::UnaryOp::NEG:
        std::cout << "    subw a5, zero, a5\n";
        break;
    case ir::UnaryOp::PRE_DEC:
        std::cout << "    addi a5, a5, -1\n";
        std::cout << "    sw a5, -" << from.second << "(s0)\n";
        break;
    case ir::UnaryOp::PRE_INC:
        std::cout << "    addi a5, a5, 1\n";
        std::cout << "    sw a5, -" << from.second << "(s0)\n";
        break;
    case ir::UnaryOp::NOT:
        std::cout << "    seqz a5, a5\n";
        break;
    default:
        break;
    }

    std::cout << "    sw a5, -" << to.second << "(s0)\n";
}

void RV64Visitor::visitBasicBlock(ir::BasicBlock &bb) {
    std::cout << bb.getName() << ":\n";
    for (auto &instruction : bb.getInstructions()) {
        instruction->accept(*this);
    }
    // Call end block here :
    auto next = bb.getNext();
    next->accept(*this);
    bb.setNext(std::move(next));
}

void RV64Visitor::visitCFG(ir::CFG &cfg) {

    std::vector<std::string> regs = {"a0", "a1", "a2", "a3",
                                     "a4", "a5", "a6", "a7"};

    int size = ((cfg.getSize() + 16 + 15) / 16) * 16;

    // prelude
    std::cout << ".globl " << cfg.getName() << "\n";
    std::cout << cfg.getName() << ":\n";
    std::cout << "    addi sp, sp, -" << size << "\n";
    std::cout << "    sd ra, 24(sp)\n";
    std::cout << "    sd s0, 16(sp)\n";
    std::cout << "    addi s0, sp, -" << size << '\n';
    if (cfg.getName() == "main") {
        std::cout << "    mv a5, zero\n";
        std::cout << "    sw a5, -"
                  << cfg.getEpilogue()
                         .getScope()
                         .getVariable("#return")
                         .value()
                         .second
                  << "(s0)\n";
    }

    for (size_t i = 0; i < cfg.getArgs().size() && i < regs.size(); ++i) {
        std::cout << "    mv a5, " << regs[i] << "\n";
        std::cout << "    sw a5, -"
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

void RV64Visitor::visitUnconditionalJump(ir::UnconditionalJump &jump) {
    std::cout << "    j " << jump.getTo().getName() << "\n";
}

void RV64Visitor::visitConditionalJump(ir::ConditionalJump &jump) {
    Variable cond =
        jump.getBlock().getScope().getVariable(jump.getCondition()).value();
    std::cout << "    lw a5, -" << cond.second << "(s0)\n";
    std::cout << "    beq a5, zero, " << jump.getElse()->getName() << "\n";
    std::cout << "    j " << jump.getThen()->getName() << "\n";
}

void RV64Visitor::visitSwitchJump(ir::SwitchJump &jump) {
    Variable expr = jump.getBlock()
                        .getScope()
                        .getVariable(jump.getExpressionTest())
                        .value();
    auto caseTests = jump.getCaseTests();
    for (auto &caseTest : caseTests) {
        std::cout << "    li a6, " << caseTest.first << "\n";
        std::cout << "    lw a5, -" << expr.second << "(s0)\n";
        std::cout << "    beq a5, a6, " << caseTest.second->getName() << "\n";
    }
};

void RV64Visitor::visitReturn(ir::Return &ret) {
    Variable retVar = ret.getBlock().getScope().getVariable("#return").value();

    int size = ret.getBlock().getCFG().getSize() + 16;
    size = ((size + 15) / 16) * 16;

    std::cout << "    lw a0, -" << retVar.second << "(s0)\n";
    std::cout << "    ld ra, 24(sp)\n";
    std::cout << "    ld s0, 16(sp)\n";
    std::cout << "    addi sp, sp, " << size << "\n";
    std::cout << "    jr ra\n";
}

void RV64Visitor::visitCall(ir::Call &call) {

    std::vector<std::string> regs = {"a0", "a1", "a2", "a3",
                                     "a4", "a5", "a6", "a7"};

    size_t i;
    for (i = 0; i < call.getNames().size() && i < regs.size(); ++i) {
        std::cout << "    lw " << regs[i] << ", -"
                  << call.getBlock()
                         .getScope()
                         .getVariable(call.getNames()[i])
                         .value()
                         .second
                  << "(s0)\n";
    }

    std::cout << "    call " << call.getFunc_name() << "\n";
    Variable to = call.getBlock().getScope().getVariable(call.getRet()).value();
    std::cout << "    sw a0, -" << to.second << "(s0)\n";
}

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
