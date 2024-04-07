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
 * @brief Donne le code assembleur x86 correspondant à une instruction
 * d'affectation d'une variable à une autre
 *
 * @param affect Une instruction d'affectation variable à variable
 */
void IRx86Visitor::visitAffect(ir::Affect &affect) {
    Scope &scope = affect.getBlock().getScope();
    // scope.print(std::cerr);
    std::cerr << "TO : " << affect.getTo() << " FROM " << affect.getFrom()
              << "\n";
    Variable to = scope.getVariable(affect.getTo()).value();
    Variable from = scope.getVariable(affect.getFrom()).value();
    os << "    mov eax, DWORD PTR -" << from.second << "[rbp]\n";
    os << "    mov DWORD PTR -" << to.second << "[rbp], eax\n";
}

/**
 * @brief Donne le code assembleur x86 correspondant à une instruction
 * d'affectation d'une constante à une variable
 *
 * @param affectConst Une instruction d'affectation constante à variable
 */
void IRx86Visitor::visitAffectConst(ir::AffectConst &affectConst) {
    Variable to = affectConst.getBlock()
                      .getScope()
                      .getVariable(affectConst.getTo())
                      .value();
    os << "    mov DWORD PTR -" << to.second << "[rbp], "
       << affectConst.getValue() << "\n";
}

/**
 * @brief Donne le code assembleur x86 correspondant à une instruction
 * d'opération à deux opérandes, en fonction de l'opérateur
 *
 * @param binop Une instruction d'opération à deux opérandes
 */
void IRx86Visitor::visitBinOp(ir::BinOp &binop) {
    Scope &scope = binop.getBlock().getScope();
    Variable to = scope.getVariable(binop.getTo()).value();
    Variable left = scope.getVariable(binop.getLeft()).value();
    Variable right = scope.getVariable(binop.getRight()).value();

    ir::BinOp::BinOpType type = binop.getType();

    os << "    mov eax, DWORD PTR -" << left.second << "[rbp]\n";

    if (type == ir::BinOp::DIV || type == ir::BinOp::MOD) { // a ( / | % ) b
        os << "    cdq\n";
        os << "    idiv DWORD PTR -" << right.second << "[rbp]\n";
        std::string reg = (type == ir::BinOp::MOD) ? "edx" : "eax";
        os << "    mov DWORD PTR -" << to.second << "[rbp], " << reg << "\n";

    } else if (type == ir::BinOp::SHIFT_L ||
               type == ir::BinOp::SHIFT_R) { // a ( << | >> ) b
        os << "    mov ecx, DWORD PTR -" << right.second << "[rbp]\n";
        os << "    sa" << ((type == ir::BinOp::SHIFT_L) ? "l" : "r")
           << " eax, cl\n";
        os << "    mov DWORD PTR -" << to.second << "[rbp], eax\n";

    } else if (type >= ir::BinOp::GT &&
               type <= ir::BinOp::NEQ) { // a ( < | <= | > | >= | != | == ) b
        os << "    cmp eax, DWORD PTR -" << right.second << "[rbp]\n";
        std::string instr = getInstrFromOp(type);
        os << "    " << instr << " al\n";
        os << "    movzx   eax, al\n";
        os << "    mov DWORD PTR -" << to.second << "[rbp], eax\n";
    } else {
        std::string instr = getInstrFromOp(type);
        os << "    " << instr << " eax, DWORD PTR -" << right.second
           << "[rbp]\n";
        os << "    mov DWORD PTR -" << to.second << "[rbp], eax\n";
    }
}

/**
 * @brief Donne le code assembleur x86 correspondant à une instruction
 * d'opération à un opérande, en fonction de l'opérateur
 *
 * @param unaryop Une instruction d'opération à un opérande
 */
void IRx86Visitor::visitUnaryOp(ir::UnaryOp &unaryop) {
    Scope &scope = unaryop.getBlock().getScope();
    Variable to = scope.getVariable(unaryop.getTo()).value();
    Variable from = scope.getVariable(unaryop.getFrom()).value();

    switch (unaryop.getType()) {
    case ir::UnaryOp::NEG: // -a
        os << "    mov eax, DWORD PTR -" << from.second << "[rbp]\n";
        os << "    neg eax\n";
        os << "    mov DWORD PTR -" << to.second << "[rbp], eax\n";
        break;
    case ir::UnaryOp::PRE_DEC: // --a
        os << "    dec DWORD PTR -" << from.second << "[rbp]\n";
        break;
    case ir::UnaryOp::PRE_INC: // ++a
        os << "    inc DWORD PTR -" << from.second << "[rbp]\n";
        break;
    case ir::UnaryOp::POST_DEC: // a--
        os << "    mov eax, DWORD PTR -" << from.second
           << "[rbp]\n"; // On met a dans eax
        os << "    mov DWORD PTR -" << to.second
           << "[rbp], eax\n"; // on met eax dans to pour retour
        os << "    dec DWORD PTR -" << from.second
           << "[rbp]\n"; // On decremente a
        break;
    case ir::UnaryOp::POST_INC: // a++

        os << "    mov eax, DWORD PTR -" << from.second
           << "[rbp]\n"; // On met a dans eax
        os << "    mov DWORD PTR -" << to.second
           << "[rbp], eax\n"; // on met eax dans to pour retour
        os << "    inc DWORD PTR -" << from.second
           << "[rbp]\n"; // On incremente a
        break;
    case ir::UnaryOp::NOT: // !a
        os << "    cmp DWORD PTR -" << from.second << "[rbp], 0\n";
        os << "    setz al\n";
        os << "    movzx eax, al\n";
        os << "    mov DWORD PTR -" << to.second << "[rbp], eax\n";
        break;
    }
}

/**
 * @brief Donne le code assembleur x86 correspondant à un block. Rajoute les
 * instructions du block puis un jump vers le block suivant.
 *
 * @param bb Un block
 */
void IRx86Visitor::visitBasicBlock(ir::BasicBlock &bb) {
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
 * @brief Donne le code assembleur x86 correspondant au graphique de flot de
 * contrôle (CFG) donné .
 *
 * @param cfg Un graphique de flot de contrôle (CFG)
 */
void IRx86Visitor::visitCFG(ir::CFG &cfg) {

    std::vector<std::string> regs = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};
    Variable ret = cfg.getEpilogue().getScope().getVariable("#return").value();
    // prelude
    os << ".globl " << cfg.getName() << "\n";
    os << cfg.getName() << ":\n";
    os << "    push rbp\n";
    os << "    mov rbp, rsp\n";
    os << "    sub rsp, " << cfg.getSize() << '\n';
    if (cfg.getName() == "main") {
        os << "    mov DWORD PTR -" << ret.second << "[rbp], 0\n";
    }

    for (size_t i = 0; i < cfg.getArgs().size() && i < regs.size(); ++i) {
        os << "    mov DWORD PTR -"
           << cfg.getBlocks()[0]
                  ->getScope()
                  .getVariable(cfg.getArgs()[i].first)
                  .value()
                  .second
           << "[rbp], " << regs[i] << '\n';
    }

    for (size_t i = regs.size(); i < cfg.getArgs().size(); ++i) {
        os << "    mov eax, DWORD PTR " << (i - regs.size()) * 4 + 16
           << "[rbp]\n";
        os << "    mov DWORD PTR -"
           << cfg.getBlocks()[0]
                  ->getScope()
                  .getVariable(cfg.getArgs()[i].first)
                  .value()
                  .second
           << "[rbp], eax\n";
    }

    // blocks
    for (auto &block : cfg.getBlocks()) {
        block->accept(*this);
    }

    // epilogue
}

/**
 * @brief Donne le code assembleur x86 correspondant à une instruction de saut
 * absolu
 *
 * @param jump Une instruction de saut absolu
 */
void IRx86Visitor::visitUnconditionalJump(ir::UnconditionalJump &jump) {
    os << "    jmp " << jump.getTo().getName() << "\n";
}

/**
 * @brief Donne le code assembleur x86 correspondant à une instruction de saut
 * conditionnel
 *
 * @param jump Une instruction de saut conditionnel
 */
void IRx86Visitor::visitConditionalJump(ir::ConditionalJump &jump) {
    Variable cond =
        jump.getBlock().getScope().getVariable(jump.getCondition()).value();
    os << "    cmp DWORD PTR -" << cond.second << "[rbp], 0\n";
    os << "    je " << jump.getElse()->getName() << "\n";
    os << "    jmp " << jump.getThen()->getName() << "\n";
}

/**
 * @brief Donne le code assembleur x86 correspondant à une instruction switch
 * jump
 *
 * @param jump Une instruction switch jump
 */
void IRx86Visitor::visitSwitchJump(ir::SwitchJump &jump) {
    Variable expr = jump.getBlock()
                        .getScope()
                        .getVariable(jump.getExpressionTest())
                        .value();
    auto caseTests = jump.getCaseTests();
    auto defaultBlock = jump.getDefaultBlock();

    for (size_t i = 0; i < caseTests.size(); ++i) {
        auto caseTest = caseTests[i];
        os << "    cmp DWORD PTR -" << expr.second << "[rbp], "
           << caseTest.first << "\n";
        os << "    je " << caseTest.second->getName() << "\n";
    }
    os << "    jmp " << defaultBlock->getName() << "\n";
}

/**
 * @brief Donne le code assembleur x86 correspondant à une instruction de retour
 *
 * @param ret Une instruction de retour
 */
void IRx86Visitor::visitReturn(ir::Return &ret) {
    Variable retVar = ret.getBlock().getScope().getVariable("#return").value();

    os << "    mov eax, DWORD PTR -" << retVar.second << "[rbp]\n";
    os << "    mov rsp, rbp\n";
    os << "    pop rbp\n";
    os << "    ret\n";
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
        os << "    sub rsp, 8\n";
    }

    size_t i;
    for (i = 0; i < call.getNames().size() && i < regs.size(); ++i) {
        os << "    mov " << regs[i] << ", DWORD PTR -"
           << call.getBlock()
                  .getScope()
                  .getVariable(call.getNames()[i])
                  .value()
                  .second
           << "[rbp]\n";
    }

    for (; i >= regs.size() && i < call.getNames().size(); ++i) {
        os << "    push QWORD PTR -"
           << call.getBlock()
                  .getScope()
                  .getVariable(call.getNames()[i])
                  .value()
                  .second
           << "[rbp]\n";
    }

    os << "    call " << call.getFunc_name() << '\n';
    os << "    mov DWORD PTR -"
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
