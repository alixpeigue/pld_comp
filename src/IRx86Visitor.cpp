/**
 * @file IRx86Visitor.cpp
 * @author H4231
 * @brief Génération du code assembleur x86
 * @date 2024-04-02
 * 
 * 
 */
#include "IRx86Visitor.h"
#include "Scope.h"
#include "ir.h"
#include <iostream>

/**
 * @brief Donne le code assembleur x86 correspondant à une instruction d'affectation d'une variable à une autre
 * 
 * @param affect Une instruction d'affectation variable à variable
 */
void IRx86Visitor::visitAffect(ir::Affect &affect) {
    Scope &scope = affect.getBlock().getScope();
    scope.print(std::cerr);
    std::cerr << "TO : " << affect.getTo() << " FROM " << affect.getFrom()
              << "\n";
    Variable to = scope.getVariable(affect.getTo()).value();
    Variable from = scope.getVariable(affect.getFrom()).value();
    std::cout << "    mov eax, DWORD PTR -" << from.second << "[rbp]\n";
    std::cout << "    mov DWORD PTR -" << to.second << "[rbp], eax\n";
}

/**
 * @brief Donne le code assembleur x86 correspondant à une instruction d'affectation d'une constante à une variable
 * 
 * @param affectConst Une instruction d'affectation constante à variable
 */
void IRx86Visitor::visitAffectConst(ir::AffectConst &affectConst) {
    Variable to = affectConst.getBlock()
                      .getScope()
                      .getVariable(affectConst.getTo())
                      .value();
    std::cout << "    mov DWORD PTR -" << to.second << "[rbp], "
              << affectConst.getValue() << "\n";
}

/**
 * @brief Donne le code assembleur x86 correspondant à une instruction d'opération à deux opérandes, en fonction de l'opérateur
 * 
 * @param binop Une instruction d'opération à deux opérandes
 */
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
    } else {
        std::string instr = getInstrFromOp(type);
        std::cout << "    " << instr << " eax, DWORD PTR -" << right.second
                  << "[rbp]\n";
        std::cout << "    mov DWORD PTR -" << to.second << "[rbp], eax\n";
    }
}

/**
 * @brief Donne le code assembleur x86 correspondant à une instruction d'opération à un opérande, en fonction de l'opérateur
 * 
 * @param unaryop Une instruction d'opération à un opérande
 */
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
    case ir::UnaryOp::NOT: // !a
        std::cout << "    cmp DWORD PTR -" << from.second << "[rbp], 0\n";
        std::cout << "    setz al\n";
        std::cout << "    movzx eax, al\n";
        std::cout << "    mov DWORD PTR -" << to.second << "[rbp], eax\n";
        break;
    }
}

/**
 * @brief Donne le code assembleur x86 correspondant à un block. Rajoute les instructions du block puis un jump vers le block suivant.
 * 
 * @param bb Un block
 */
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

/**
 * @brief Donne le code assembleur x86 correspondant au graphique de flot de contrôle (CFG) donné .
 * 
 * @param cfg Un graphique de flot de contrôle (CFG)
 */
void IRx86Visitor::visitCFG(ir::CFG &cfg) {

    std::vector<std::string> regs = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};
    // prelude
    std::cout << ".globl " << cfg.getName() << "\n";
    std::cout << cfg.getName() << ":\n";
    std::cout << "    push rbp\n";
    std::cout << "    mov rbp, rsp\n";
    std::cout << "    sub rsp, " << cfg.getSize() << '\n';

    for (size_t i = 0; i < cfg.getArgs().size() && i < regs.size(); ++i) {
        std::cout << "    mov DWORD PTR -"
                  << cfg.getBlocks()[0]
                         ->getScope()
                         .getVariable(cfg.getArgs()[i].first)
                         .value()
                         .second
                  << "[rbp], " << regs[i] << '\n';
    }

    // blocks
    for (auto &block : cfg.getBlocks()) {
        block->accept(*this);
    }

    // epilogue
}

/**
 * @brief Donne le code assembleur x86 correspondant à une instruction de saut absolu
 * 
 * @param jump Une instruction de saut absolu
 */
void IRx86Visitor::visitUnconditionalJump(ir::UnconditionalJump &jump) {
    std::cout << "    jmp " << jump.getTo().getName() << "\n";
}

/**
 * @brief Donne le code assembleur x86 correspondant à une instruction de saut conditionnel
 * 
 * @param jump Une instruction de saut conditionnel
 */
void IRx86Visitor::visitConditionalJump(ir::ConditionalJump &jump) {
    Variable cond =
        jump.getBlock().getScope().getVariable(jump.getCondition()).value();
    std::cout << "    cmp DWORD PTR -" << cond.second << "[rbp], 0\n";
    std::cout << "    je " << jump.getElse()->getName() << "\n";
    std::cout << "    jmp " << jump.getThen()->getName() << "\n";
}

/**
 * @brief Donne le code assembleur x86 correspondant à une instruction switch jump
 * 
 * @param jump Une instruction switch jump
 */
void IRx86Visitor::visitSwitchJump(ir::SwitchJump &jump) {
    Variable expr = jump.getBlock()
                        .getScope()
                        .getVariable(jump.getExpressionTest())
                        .value();
    auto caseTests = jump.getCaseTests();

    for (size_t i = 0; i < caseTests.size(); ++i) {
        auto caseTest = caseTests[i];
        std::cout << "    cmp DWORD PTR -" << expr.second << "[rbp], "
                  << caseTest.first << "\n";
        std::cout << "    je " << caseTest.second->getName() << "\n";
    }
}

/**
 * @brief Donne le code assembleur x86 correspondant à une instruction de retour
 * 
 * @param ret Une instruction de retour
 */
void IRx86Visitor::visitReturn(ir::Return &ret) {
    Variable retVar = ret.getBlock().getScope().getVariable("#return").value();

    std::cout << "    mov eax, DWORD PTR -" << retVar.second << "[rbp]\n";
    std::cout << "    mov rsp, rbp\n";
    std::cout << "    pop rbp\n";
    std::cout << "    ret\n";
}


/**
 * @brief Donne le code assembleur x86 correspondant à un appel de fonction
 * 
 * @param call Un appel de fonction
 */
void IRx86Visitor::visitCall(ir::Call &call) {
    std::vector<std::string> regs = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};

    if (call.getNames().size() > regs.size() &&
        ((call.getNames().size() * 4) % 16) != 0) {
        std::cout << "    sub rsp, 8\n";
    }

    size_t i;
    for (i = 0; i < call.getNames().size() && i < regs.size(); ++i) {
        std::cout << "    mov " << regs[i] << ", DWORD PTR -"
                  << call.getBlock()
                         .getScope()
                         .getVariable(call.getNames()[i])
                         .value()
                         .second
                  << "[rbp]\n";
    }

    for (; i >= regs.size() && i < call.getNames().size(); ++i) {
        std::cout << "    push DWORD PTR -"
                  << call.getBlock()
                         .getScope()
                         .getVariable(call.getNames()[i])
                         .value()
                         .second
                  << "[rbp]\n";
    }

    std::cout << "    call " << call.getFunc_name() << '\n';
    std::cout
        << "    mov DWORD PTR -"
        << call.getBlock().getScope().getVariable(call.getRet()).value().second
        << "[rbp], eax\n";
}


/**
 * @brief Renvoie l'instruction x86 correspondant a l'opérateur en paramètre
 * 
 * @param op 
 * @return Retour possible : add, sub, imul, or, xor, and, setXX (std::string)
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
    default:
        break;
    }
    return instr;
}
