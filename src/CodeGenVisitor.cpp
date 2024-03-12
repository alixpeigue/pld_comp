#include "CodeGenVisitor.h"

antlrcpp::Any IRGenVisitor::visitProg(ifccParser::ProgContext *ctx) {
    std::cout << ".intel_syntax noprefix\n";
    std::cout << ".globl main\n";
    std::cout << " main: \n";

    std::cout << "    push rbp\n";
    std::cout << "    mov rbp, rsp\n";

    this->visitChildren(ctx);

    std::cout << "    pop rbp\n";
    std::cout << "    ret\n";

    return 0;
}

antlrcpp::Any
IRGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx) {
    int expr_index = (int)this->visit(ctx->expression());
    std::cout << "    mov eax, DWORD PTR -" << expr_index * 4 << "[rbp]\n";
    return 0;
}

antlrcpp::Any
IRGenVisitor::visitDeclaration(ifccParser::DeclarationContext *ctx) {
    visitChildren(ctx);
    return 0;
}

antlrcpp::Any
IRGenVisitor::visitDeclaAffect(ifccParser::DeclaAffectContext *ctx) {

    if (ctx->expression()) {
        std::string variable = ctx->VARIABLE()->getText();
        int expr_index = (int)visit(ctx->expression());
        std::cout << "    mov eax, DWORD PTR -" << expr_index * 4 << "[rbp]\n";
        std::cout << "    mov DWORD PTR -" << symbolsTable.at(variable) * 4
                  << "[rbp], eax\n";
    }

    return 0;
}

antlrcpp::Any IRGenVisitor::visitIntConst(ifccParser::IntConstContext *ctx) {
    int value = stoi(ctx->INT_CONST()->getText());

    ++counterTempVariables;
    symbolsTable.insert(std::make_pair(
        "#" + std::to_string(counterTempVariables), symbolsTable.size() + 1));

    std::cout << "    mov DWORD PTR -" << (symbolsTable.size()) * 4 << "[rbp], "
              << value << "\n";

    return (int)symbolsTable.size();
}

antlrcpp::Any IRGenVisitor::visitCharConst(ifccParser::CharConstContext *ctx) {
    int value = ctx->CHAR_CONST()->getText()[1];

    ++counterTempVariables;
    symbolsTable.insert(std::make_pair(
        "#" + std::to_string(counterTempVariables), symbolsTable.size() + 1));
    std::cout << "    mov DWORD PTR -" << (symbolsTable.size()) * 4 << "[rbp], "
              << value << "\n";

    return (int)symbolsTable.size();
}

antlrcpp::Any IRGenVisitor::visitVariable(ifccParser::VariableContext *ctx) {
    std::string variable = ctx->VARIABLE()->getText();
    return (int)symbolsTable.at(variable);
}

antlrcpp::Any IRGenVisitor::visitAffect(ifccParser::AffectContext *ctx) {
    std::string variable = ctx->VARIABLE()->getText();

    int expr_index = (int)this->visit(ctx->expression());
    std::cout << "    mov eax, DWORD PTR -" << expr_index * 4 << "[rbp]\n";
    std::cout << "    mov DWORD PTR -" << symbolsTable.at(variable) * 4
              << "[rbp], eax\n";

    return (int)symbolsTable.at(variable);
}

antlrcpp::Any IRGenVisitor::visitParen(ifccParser::ParenContext *ctx) {
    return (int)visit(ctx->expression());
}

antlrcpp::Any IRGenVisitor::visitMult(ifccParser::MultContext *ctx) {
    int expr1_index = (int)visit(ctx->expression(0));
    int expr2_index = (int)visit(ctx->expression(1));
    std::cout << "    mov eax, DWORD PTR -" << expr1_index * 4 << "[rbp]\n";

    if (ctx->op->getText()[0] == '*') {
        std::cout << "    imul eax, DWORD PTR -" << expr2_index * 4
                  << "[rbp]\n";

    } else if (ctx->op->getText()[0] == '/') {
        std::cout << "    cdq\n";
        std::cout << "    idiv DWORD PTR -" << expr2_index * 4 << "[rbp]\n";
    } else if (ctx->op->getText()[0] == '%') {
        std::cout << "    cdq\n";
        std::cout << "    idiv DWORD PTR -" << expr2_index * 4 << "[rbp]\n";
        std::cout << "    mov eax, edx\n";
    }

    ++counterTempVariables;
    symbolsTable.insert(std::make_pair(
        "#" + std::to_string(counterTempVariables), symbolsTable.size() + 1));

    std::cout << "    mov DWORD PTR -" << (symbolsTable.size()) * 4
              << "[rbp], eax\n";

    return (int)symbolsTable.size();
}

antlrcpp::Any IRGenVisitor::visitAdd(ifccParser::AddContext *ctx) {
    int expr1_index = (int)visit(ctx->expression(0));
    int expr2_index = (int)visit(ctx->expression(1));
    std::cout << "    mov eax, DWORD PTR -" << expr1_index * 4 << "[rbp]\n";

    if (ctx->op->getText()[0] == '+') {
        std::cout << "    add eax, DWORD PTR -" << expr2_index * 4 << "[rbp]\n";
    } else if (ctx->op->getText()[0] == '-') {
        std::cout << "    sub eax, DWORD PTR -" << expr2_index * 4 << "[rbp]\n";
    }

    ++counterTempVariables;
    symbolsTable.insert(std::make_pair(
        "#" + std::to_string(counterTempVariables), symbolsTable.size() + 1));

    std::cout << "    mov DWORD PTR -" << (symbolsTable.size()) * 4
              << "[rbp], eax\n";

    return (int)symbolsTable.size();
}

antlrcpp::Any IRGenVisitor::visitUnaryAdd(ifccParser::UnaryAddContext *ctx) {
    int expr_index = (int)visit(ctx->expression());

    if (ctx->op->getText()[0] == '+')
        return (int)expr_index;

    std::cout << "    mov eax, DWORD PTR -" << expr_index * 4 << "[rbp]\n";
    std::cout << "    neg eax\n";

    ++counterTempVariables;
    symbolsTable.insert(std::make_pair(
        "#" + std::to_string(counterTempVariables), symbolsTable.size() + 1));

    std::cout << "    mov DWORD PTR -" << (symbolsTable.size()) * 4
              << "[rbp], eax\n";

    return (int)symbolsTable.size();
}
