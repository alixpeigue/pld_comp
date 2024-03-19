#include "IRx86Visitor.h"
#include "Scope.h"
#include "ir.h"
#include <iostream>

void IRx86Visitor::visitAffect(ir::Affect &affect) {
    Scope &scope = affect.getBlock().getScope();
    // scope.print(std::cout);
    // std::cout << "TO : " << affect.getTo() << " FROM " << affect.getFrom()
    //           << "\n";
    Variable to = scope.getVariable(affect.getTo()).value();
    Variable from = scope.getVariable(affect.getFrom()).value();
    std::cout << "    mov eax, DWORD PTR -" << from.second << "[rbp]\n";
    std::cout << "    mov DWORD PTR -" << to.second << "[rbp], eax\n";
}

void IRx86Visitor::visitAffectConst(ir::AffectConst &affectConst) {
    Variable to = affectConst.getBlock()
                      .getScope()
                      .getVariable(affectConst.getTo())
                      .value();
    std::cout << "    mov DWORD PTR -" << to.second << "[rbp], "
              << affectConst.getValue() << "\n";
}

void IRx86Visitor::visitBinOp(ir::BinOp &binop) {
    Scope &scope = binop.getBlock().getScope();
    Variable to = scope.getVariable(binop.getTo()).value();
    Variable left = scope.getVariable(binop.getLeft()).value();
    Variable right = scope.getVariable(binop.getRight()).value();

    std::cout << "    mov eax, DWORD PTR -" << left.second << "[rbp]\n";

    ir::BinOp::BinOpType type = binop.getType();

    if (type == ir::BinOp::DIV || type == ir::BinOp::MOD) {
        std::cout << "    cdq\n";
        std::cout << "    idiv DWORD PTR -" << right.second << "[rbp]\n";
        std::string reg = type == ir::BinOp::MOD ? "edx" : "eax";
        std::cout << "    mov DWORD PTR -" << to.second << "[rbp], " << reg
                  << "\n";
        return;
    }

    std::string instr = "ERR";
    switch (type) {
    case ir::BinOp::ADD:
        instr = "add";
        break;
    case ir::BinOp::SUB:
        instr = "sub";
        break;
    case ir::BinOp::MUL:
        instr = "imul";
        break;
    }

    std::cout << "    " << instr << " eax, DWORD PTR -" << right.second
              << "[rbp]\n";
    std::cout << "    mov DWORD PTR -" << to.second << "[rbp], eax\n";
}

void IRx86Visitor::visitUnaryOp(ir::UnaryOp &unaryop) {
    Scope &scope = unaryop.getBlock().getScope();
    Variable to = scope.getVariable(unaryop.getTo()).value();
    Variable from = scope.getVariable(unaryop.getFrom()).value();

    switch (unaryop.getType()) {
    case ir::UnaryOp::NEG:
        std::cout << "    mov eax, DWORD PTR -" << from.second << "[rbp]\n";
        std::cout << "    neg eax\n";
        break;
    case ir::UnaryOp::NOT:
        std::cout << "    cmp DWORD PTR -" << from.second << "[rbp], 0\n";
        std::cout << "    setz al\n";
        std::cout << "    movzx eax, al\n";
        break;
    }

    std::cout << "    mov DWORD PTR -" << to.second << "[rbp], eax\n";
}

void IRx86Visitor::visitBasicBlock(ir::BasicBlock &bb) {
    std::cout << bb.getName() << ":\n";
    for (auto &instruction : bb.getInstructions()) {
        instruction->accept(*this);
    }
    // Call end block here :
    auto next = bb.getNext();
    next->accept(*this);
    bb.setNext(std::move(next));
}

void IRx86Visitor::visitCFG(ir::CFG &cfg) {

    // prelude
    std::cout << ".globl " << cfg.getName() << "\n";
    std::cout << cfg.getName() << ":\n";
    std::cout << "    push rbp\n";
    std::cout << "    mov rbp, rsp\n";
    std::cout << "    sub rsp, " << cfg.getSize() << '\n';

    // blocks
    for (auto &block : cfg.getBlocks()) {
        block->accept(*this);
    }

    // epilogue
}

void IRx86Visitor::visitUnconditionalJump(ir::UnconditionalJump &jump) {
    std::cout << "    jmp " << jump.getTo().getName() << "\n";
}

void IRx86Visitor::visitConditionalJump(ir::ConditionalJump &jump) {
    Variable cond =
        jump.getBlock().getScope().getVariable(jump.getCondition()).value();
    std::cout << "    cmp DWORD PTR -" << cond.second << "[rbp], 0\n";
    std::cout << "    je " << jump.getElse()->getName() << "\n";
    std::cout << "    jmp " << jump.getThen()->getName() << "\n";
}

void IRx86Visitor::visitReturn(ir::Return &ret) {
    Variable retVar = ret.getBlock().getScope().getVariable("#return").value();

    std::cout << "    mov eax, DWORD PTR -" << retVar.second << "[rbp]\n";
    std::cout << "    mov rsp, rbp\n";
    std::cout << "    pop rbp\n";
    std::cout << "    ret\n";
}

void IRx86Visitor::visitCall(ir::Call &call) {
    std::vector<std::string> regs = { "rdi", "rsi", "rdx", "rcx", "r8", "r9" };

    if (call.getNames().size() > regs.size() && ((call.getNames().size() * 4) % 16) != 0) {
        std::cout << "    sub rsp, 8\n";
    }

    size_t i;
    for (i = 0; i < call.getNames().size() && i < regs.size(); ++i) {
        std::cout << "    mov " << regs[i] << ", QWORD PTR -" << call.getBlock().getScope().getVariable(call.getNames()[i]).value().second << "[rbp]\n";
    }

    for (; i >= regs.size() && i < call.getNames().size(); ++i) {
        std::cout << "    push QWORD PTR -" << call.getBlock().getScope().getVariable(call.getNames()[i]).value().second << "[rbp]\n";
    }

    std::cout << "    call " << call.getFunc_name() << '\n';
    std::cout << "    mov DWORD PTR -" << call.getBlock().getScope().getVariable(call.getRet()).value().second << "[rbp], eax\n";
}
