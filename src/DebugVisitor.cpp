
#include "DebugVisitor.h"
#include "ir.h"
#include <iostream>

void DebugVisitor::visitAffect(ir::Affect &affect) {
    std::cerr << "    " << affect.getTo() << " = " << affect.getFrom() << "\n";
}
void DebugVisitor::visitAffectConst(ir::AffectConst &affectConst) {
    std::cerr << "    " << affectConst.getTo() << " = "
              << affectConst.getValue() << "\n";
}
void DebugVisitor::visitBinOp(ir::BinOp &binop) {
    std::cerr << "    " << binop.getTo() << " = (" << binop.getLeft() << ", "
              << binop.getRight() << ")\n";
}
void DebugVisitor::visitUnaryOp(ir::UnaryOp &unaryop) {
    std::cerr << "    " << unaryop.getTo() << " = (" << unaryop.getFrom()
              << ")\n";
}
void DebugVisitor::visitBasicBlock(ir::BasicBlock &bb) {
    std::cerr << "    " << bb.getName() << ":\n";
    for (auto &instr : bb.getInstructions()) {
        instr->accept(*this);
    }
    auto next = bb.getNext();
    next->accept(*this);
    bb.setNext(std::move(next));
    std::cerr << "\n";
}
void DebugVisitor::visitCFG(ir::CFG &cfg) {
    std::cerr << cfg.getName() << "(";
    for (auto &[name, _] : cfg.getArgs()) {
        std::cerr << name << ", ";
    }
    std::cerr << ") {\n";
    for (auto &bb : cfg.getBlocks()) {
        bb->accept(*this);
    }
    std::cerr << "}\n";
}
void DebugVisitor::visitUnconditionalJump(ir::UnconditionalJump &jump) {
    std::cerr << "    "
              << "j " << jump.getTo().getName() << "\n";
}
void DebugVisitor::visitConditionalJump(ir::ConditionalJump &jump) {
    std::cerr << "    "
              << "j " << jump.getThen()->getName() << " if "
              << jump.getCondition() << " else " << jump.getElse()->getName()
              << "\n";
}
void DebugVisitor::visitSwitchJump(ir::SwitchJump &jump) {
    std::cerr << "    switch (" << jump.getExpressionTest() << ") {\n";
    for (auto &[k, v] : jump.getCaseTests()) {
        std::cerr << "        " << k << " : " << v->getName() << "\n";
    }
    std::cerr << "        default : " << jump.getDefaultBlock()->getName()
              << "\n";
    std::cerr << "    }\n";
}
void DebugVisitor::visitReturn(ir::Return &jump) {
    std::cerr << "    "
              << "ret\n";
}
void DebugVisitor::visitCall(ir::Call &call) {
    std::cerr << "    " << call.getRet() << " = " << call.getFunc_name() << "(";
    for (auto &name : call.getNames()) {
        std::cerr << name << ", ";
    }
    std::cerr << ")\n";
}
