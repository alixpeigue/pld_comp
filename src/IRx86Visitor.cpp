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
        std::string reg = (type == ir::BinOp::MOD) ? "edx" : "eax";
        std::cout << "    mov DWORD PTR -" << to.second << "[rbp], " << reg
                  << "\n";
        return;
    }
    if (type == ir::BinOp::SHIFT_L || type == ir::BinOp::SHIFT_R) {
        std::cout << "    mov ecx, DWORD PTR -" << right.second << "[rbp]\n";
        if (type == ir::BinOp::SHIFT_L) {
            std::cout << "    sal";
        } else {
            std::cout << "    sar";
        }
        std::cout << " eax, cl\n";
        std::cout << "    mov DWORD PTR -" << to.second << "[rbp], eax\n";
        return;
    }

    if (type >= ir::BinOp::GT && type <= ir::BinOp::NEQ) {
        std::cout << "    cmp eax, DWORD PTR -" << right.second << "[rbp]\n";
        switch (type) {
        case ir::BinOp::GT:
            std::cout << "    setg al\n";
            break;
        case ir::BinOp::LT:
            std::cout << "    setl al\n";
            break;
        case ir::BinOp::GTE:
            std::cout << "    setge al\n";
            break;
        case ir::BinOp::LTE:
            std::cout << "    setle al\n";
            break;
        case ir::BinOp::EQ:
            std::cout << "    sete al\n";
            break;
        case ir::BinOp::NEQ:
            std::cout << "    setne al\n";
            break;
        default:
            break;
        }
        std::cout << "    movzx   eax, al\n";
        std::cout << "    mov DWORD PTR -" << to.second << "[rbp], eax\n";
        return;
    }

    if (type == ir::BinOp::AND) {
        std::string block1 = ".AND" + std::to_string(currentCompBlock);
        currentCompBlock++;
        std::string block2 = ".AND" + std::to_string(currentCompBlock);
        currentCompBlock++;

        std::cout << "    cmp DWORD PTR -" << left.second << "[rbp], 0\n";
        std::cout << "    je " << block1 << "\n";
        std::cout << "    cmp DWORD PTR -" << right.second << "[rbp], 0\n";
        std::cout << "    je " << block1 << "\n";
        std::cout << "    mov   eax, 1\n";
        std::cout << "    jmp " << block2 << "\n";

        std::cout << block1 << ":\n";
        std::cout << "    mov   eax, 0\n";
        std::cout << "    jmp " << block2 << "\n";


        std::cout << block2 << ":\n";
        std::cout << "    movzx   eax, al\n";
        std::cout << "    mov DWORD PTR -" << to.second << "[rbp], eax\n";
        return;
    }

    if (type == ir::BinOp::OR) {
        std::string block1 = ".OR" + std::to_string(currentCompBlock);
        currentCompBlock++;
        std::string block2 = ".OR" + std::to_string(currentCompBlock);
        currentCompBlock++;
         std::string block3 = ".OR" + std::to_string(currentCompBlock);
        currentCompBlock++;

        std::cout << "    cmp DWORD PTR -" << left.second << "[rbp], 0\n";
        std::cout << "    jne " << block1 << "\n";
        std::cout << "    cmp DWORD PTR -" << right.second << "[rbp], 0\n";
        std::cout << "    je " << block2 << "\n";
        std::cout << "    jmp " << block1 << "\n";
      

        std::cout << block1 << ":\n";
        std::cout << "    mov   eax, 1\n";
        std::cout << "    jmp " << block3<< "\n";


        std::cout << block2 << ":\n";
        std::cout << "    mov   eax, 0\n";
        std::cout << "    jmp " << block3<< "\n";


        std::cout << block3 << ":\n";
        std::cout << "    movzx   eax, al\n";
        std::cout << "    mov DWORD PTR -" << to.second << "[rbp], eax\n";
        
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
    case ir::BinOp::OR_BIN:
        instr = "or";
        break;
    case ir::BinOp::XOR_BIN:
        instr = "xor";
        break;
    case ir::BinOp::AND_BIN:
        instr = "and";
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

    if (unaryop.getType() == ir::UnaryOp::NEG){
        std::cout << "    mov eax, DWORD PTR -" << from.second << "[rbp]\n";
        std::cout << "    neg eax\n";
        std::cout << "    mov DWORD PTR -" << to.second << "[rbp], eax\n";
    } else if (unaryop.getType() == ir::UnaryOp::PRE_DEC){
        std::cout << "    dec DWORD PTR -" << from.second << "[rbp]\n";
    } else if (unaryop.getType() == ir::UnaryOp::PRE_INC){
        std::cout << "    inc DWORD PTR -" << from.second << "[rbp]\n";
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
