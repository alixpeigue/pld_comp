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

    ir::BinOp::BinOpType type = binop.getType();

    std::cout << "    mov eax, DWORD PTR -" << left.second << "[rbp]\n";

    if (type == ir::BinOp::DIV || type == ir::BinOp::MOD) { // a ( / | % ) b
        std::cout << "    cdq\n";
        std::cout << "    idiv DWORD PTR -" << right.second << "[rbp]\n";
        std::string reg = (type == ir::BinOp::MOD) ? "edx" : "eax";
        std::cout << "    mov DWORD PTR -" << to.second << "[rbp], " << reg
                  << "\n";

    } else if (type == ir::BinOp::SHIFT_L ||
               type == ir::BinOp::SHIFT_R) { // a ( << | >> ) b
        std::cout << "    mov ecx, DWORD PTR -" << right.second << "[rbp]\n";
        std::cout << "    sa" << ((type == ir::BinOp::SHIFT_L) ? "l" : "r")
                  << " eax, cl\n";
        std::cout << "    mov DWORD PTR -" << to.second << "[rbp], eax\n";

    } else if (type >= ir::BinOp::GT &&
               type <= ir::BinOp::NEQ) { // a ( < | <= | > | >= | != | == ) b
        std::cout << "    cmp eax, DWORD PTR -" << right.second << "[rbp]\n";
        std::string instr = getInstrFromOp(type);
        std::cout << "    " << instr << " al\n";
        std::cout << "    movzx   eax, al\n";
        std::cout << "    mov DWORD PTR -" << to.second << "[rbp], eax\n";
    } else if (type == ir::BinOp::AND) { // a && b
        writeAndCond(left.second, right.second, to.second);

    } else if (type == ir::BinOp::OR) { // a || b
        writeOrCond(left.second, right.second, to.second);

    } else {
        std::string instr = getInstrFromOp(type);
        std::cout << "    " << instr << " eax, DWORD PTR -" << right.second
                  << "[rbp]\n";
        std::cout << "    mov DWORD PTR -" << to.second << "[rbp], eax\n";
    }
}

void IRx86Visitor::visitUnaryOp(ir::UnaryOp &unaryop) {
    Scope &scope = unaryop.getBlock().getScope();
    Variable to = scope.getVariable(unaryop.getTo()).value();
    Variable from = scope.getVariable(unaryop.getFrom()).value();

    switch (unaryop.getType()) {
    case ir::UnaryOp::NEG: // -a
        std::cout << "    mov eax, DWORD PTR -" << from.second << "[rbp]\n";
        std::cout << "    neg eax\n";
        std::cout << "    mov DWORD PTR -" << to.second << "[rbp], eax\n";
        break;
    case ir::UnaryOp::PRE_DEC: // --a
        std::cout << "    dec DWORD PTR -" << from.second << "[rbp]\n";
        break;
    case ir::UnaryOp::PRE_INC: // ++a
        std::cout << "    inc DWORD PTR -" << from.second << "[rbp]\n";
        break;
    case ir::UnaryOp::POST_DEC: // a--
        std::cout << "    mov eax, DWORD PTR -" << from.second
                  << "[rbp]\n"; // On met a dans eax
        std::cout << "    mov DWORD PTR -" << to.second
                  << "[rbp], eax\n"; // on met eax dans to pour retour
        std::cout << "    dec DWORD PTR -" << from.second
                  << "[rbp]\n"; // On decremente a
        break;
    case ir::UnaryOp::POST_INC: // a++

        std::cout << "    mov eax, DWORD PTR -" << from.second
                  << "[rbp]\n"; // On met a dans eax
        std::cout << "    mov DWORD PTR -" << to.second
                  << "[rbp], eax\n"; // on met eax dans to pour retour
        std::cout << "    inc DWORD PTR -" << from.second
                  << "[rbp]\n"; // On incremente a
        break;
    }
}

void IRx86Visitor::visitBasicBlock(ir::BasicBlock &bb) {
    std::cout << bb.getName() << ":\n";
    for (auto &instruction : bb.getInstructions()) {
        instruction->accept(*this);
    }
    // Call end block here :
    bb.getNext().accept(*this);
}

void IRx86Visitor::visitCFG(ir::CFG &cfg) {
    // prelude
    std::cout << ".globl " << cfg.getName() << "\n";
    std::cout << cfg.getName() << ":\n";
    std::cout << "    push rbp\n";
    std::cout << "    mov rbp, rsp\n";

    // blocks
    for (auto &block : cfg.getBlocks()) {
        block->accept(*this);
    }

    // epilogue
}

void IRx86Visitor::visitUnconditionalJump(ir::UnconditionalJump &jump) {
    std::cout << "    jmp " << jump.getTo().getName() << "\n";
}

void IRx86Visitor::visitReturn(ir::Return &ret) {
    Variable retVar = ret.getBlock().getScope().getVariable("#return").value();

    std::cout << "    mov eax, DWORD PTR -" << retVar.second << "[rbp]\n";
    std::cout << "    pop rbp\n";
    std::cout << "    ret\n";
}

inline void IRx86Visitor::writeOrCond(uint32_t left, uint32_t right,
                                      uint32_t to) {
    std::string block1 = ".OR" + std::to_string(currentCompBlock);
    currentCompBlock++;
    std::string block2 = ".OR" + std::to_string(currentCompBlock);
    currentCompBlock++;
    std::string block3 = ".OR" + std::to_string(currentCompBlock);
    currentCompBlock++;

    std::cout << "    cmp DWORD PTR -" << left << "[rbp], 0\n";
    std::cout << "    jne " << block1 << "\n";
    std::cout << "    cmp DWORD PTR -" << right << "[rbp], 0\n";
    std::cout << "    je " << block2 << "\n";
    std::cout << "    jmp " << block1 << "\n";

    std::cout << block1 << ":\n";
    std::cout << "    mov   eax, 1\n";
    std::cout << "    jmp " << block3 << "\n";

    std::cout << block2 << ":\n";
    std::cout << "    mov   eax, 0\n";
    std::cout << "    jmp " << block3 << "\n";

    std::cout << block3 << ":\n";
    std::cout << "    movzx   eax, al\n";
    std::cout << "    mov DWORD PTR -" << to << "[rbp], eax\n";
}

inline void IRx86Visitor::writeAndCond(uint32_t left, uint32_t right,
                                       uint32_t to) {
    std::string block1 = ".AND" + std::to_string(currentCompBlock);
    currentCompBlock++;
    std::string block2 = ".AND" + std::to_string(currentCompBlock);
    currentCompBlock++;

    std::cout << "    cmp DWORD PTR -" << left << "[rbp], 0\n";
    std::cout << "    je " << block1 << "\n";
    std::cout << "    cmp DWORD PTR -" << right << "[rbp], 0\n";
    std::cout << "    je " << block1 << "\n";
    std::cout << "    mov   eax, 1\n";
    std::cout << "    jmp " << block2 << "\n";

    std::cout << block1 << ":\n";
    std::cout << "    mov   eax, 0\n";
    std::cout << "    jmp " << block2 << "\n";

    std::cout << block2 << ":\n";
    std::cout << "    movzx   eax, al\n";
    std::cout << "    mov DWORD PTR -" << to << "[rbp], eax\n";
}

/*
Renvoie l'instruction x86 correspondant a l'opérateur en parametre
Retour possible : add, sub, imul, or, xor, and, setXX
*/
inline std::string IRx86Visitor::getInstrFromOp(ir::BinOp::BinOpType op) {
    std::string instr = "ERR";
    switch (op) {
    case ir::BinOp::ADD:
        instr = "add";
        break;
    case ir::BinOp::SUB:
        instr = "sub";
        break;
    case ir::BinOp::MUL:
        instr = "imul";
        break;
    case ir::BinOp::OR_BIN:
        instr = "or";
        break;
    case ir::BinOp::XOR_BIN:
        instr = "xor";
        break;
    case ir::BinOp::AND_BIN:
        instr = "and";
        break;
    case ir::BinOp::GT:
        instr = "setg";
        break;
    case ir::BinOp::LT:
        instr = "setl";
        break;
    case ir::BinOp::GTE:
        instr = "setge";
        break;
    case ir::BinOp::LTE:
        instr = "setle";
        break;
    case ir::BinOp::EQ:
        instr = "sete";
        break;
    case ir::BinOp::NEQ:
        instr = "setne";
        break;
    }
    return instr;
}