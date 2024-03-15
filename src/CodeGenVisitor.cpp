#include "CodeGenVisitor.h"

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext *ctx) {
    std::cout << ".intel_syntax noprefix\n";
    std::cout << ".globl main\n";
    std::cout << " main: \n";

    // Prologue
    std::cout << "    push rbp\n";
    std::cout << "    mov rbp, rsp\n";

    // Génère le code correspondants aux enfants
    this->visitChildren(ctx);

    // Épilogue
    std::cout << "    pop rbp\n";
    std::cout << "    ret\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitReturn_stmt(
    ifccParser::Return_stmtContext *ctx) {
    // On récupère l'index dans la pile du résultat de l'expression à return
    int expr_index = (int)this->visit(ctx->expression());

    // On met le contenu de la variable temporaire de l'expression dans eax
    // (registre du return)
    std::cout << "    mov eax, DWORD PTR -" << expr_index * 4 << "[rbp]\n";
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitDeclaration(
    ifccParser::DeclarationContext *ctx) {
    // On génère le code pour chaque DeclaAffect de la déclaration
    visitChildren(ctx);
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitDeclaAffect(
    ifccParser::DeclaAffectContext *ctx) {

    // Si on affecte une expression à la variable lors de la déclaration
    if (ctx->expression()) {
        std::string variable = ctx->VARIABLE()->getText();

        // On génère le code de l'expression et on récupère l'index dans la pile
        // de son résultat
        int expr_index = (int)visit(ctx->expression());

        // On met le contenu de la variable temporaire de l'expression dans la
        // variable à initialiser (en passant par eax)
        std::cout << "    mov eax, DWORD PTR -" << expr_index * 4 << "[rbp]\n";
        std::cout << "    mov DWORD PTR -" << symbolsTable.at(variable) * 4
                  << "[rbp], eax\n";
    }

    // Sinon on a juste déclaré une variable sans initialisation, on ne fait
    // rien

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitIntConst(ifccParser::IntConstContext *ctx) {
    // On récupère la valeur de la constante
    int value = stoi(ctx->INT_CONST()->getText());

    // On ajoute une variable temporaire dans la table des symboles
    // correspondant à la constante
    ++counterTempVariables;
    symbolsTable.insert(std::make_pair(
        "#" + std::to_string(counterTempVariables), symbolsTable.size() + 1));

    // On met la valeur de la constante dans la variable temporaire
    std::cout << "    mov DWORD PTR -" << (symbolsTable.size()) * 4 << "[rbp], "
              << value << "\n";

    // On renvoit la position de la variable temporaire
    return (int)symbolsTable.size();
}

antlrcpp::Any CodeGenVisitor::visitCharConst(
    ifccParser::CharConstContext *ctx) {
    // On récupère la valeur de la constante
    int value = ctx->CHAR_CONST()->getText()[1];

    // On ajoute une variable temporaire dans la table des symboles
    // correspondant à la constante
    ++counterTempVariables;
    symbolsTable.insert(std::make_pair(
        "#" + std::to_string(counterTempVariables), symbolsTable.size() + 1));

    // On met la valeur de la constante dans la variable temporaire
    std::cout << "    mov DWORD PTR -" << (symbolsTable.size()) * 4 << "[rbp], "
              << value << "\n";

    // On renvoit la position de la variable temporaire
    return (int)symbolsTable.size();
}

antlrcpp::Any CodeGenVisitor::visitVariable(ifccParser::VariableContext *ctx) {
    // On récupère le nom de la variable visitée
    std::string variable = ctx->VARIABLE()->getText();

    // On renvoit la position de la variable dans la pile
    return (int)symbolsTable.at(variable);
}

antlrcpp::Any CodeGenVisitor::visitAffect(ifccParser::AffectContext *ctx) {
    // On récupère le nom de la variable à affecter
    std::string variable = ctx->VARIABLE()->getText();

    // On génère le code de l'expression et on récupère l'index dans la pile de
    // son résultat
    int expr_index = (int)this->visit(ctx->expression());

    // On met le contenu de la variable temporaire de l'expression dans la
    // variable à initialiser (en passant par eax)
    std::cout << "    mov eax, DWORD PTR -" << expr_index * 4 << "[rbp]\n";
    std::cout << "    mov DWORD PTR -" << symbolsTable.at(variable) * 4
              << "[rbp], eax\n";

    // On renvoit la position de la variable dans la pile
    return (int)symbolsTable.at(variable);
}

antlrcpp::Any CodeGenVisitor::visitParen(ifccParser::ParenContext *ctx) {
    // On génère le code de l'expression entre parenthèses
    return (int)visit(ctx->expression());
}

antlrcpp::Any CodeGenVisitor::visitMult(ifccParser::MultContext *ctx) {
    // On génère le code des expressions à multiplier/diviser et on récupère
    // l'index dans la pile de leur résultat
    int expr1_index = (int)visit(ctx->expression(0));
    int expr2_index = (int)visit(ctx->expression(1));

    // On bouge le résultat de la première expression dans eax
    std::cout << "    mov eax, DWORD PTR -" << expr1_index * 4 << "[rbp]\n";

    if (ctx->op->getText()[0] == '*') {
        // Multiplication
        std::cout << "    imul eax, DWORD PTR -" << expr2_index * 4
                  << "[rbp]\n";
    } else if (ctx->op->getText()[0] == '/') {
        // Division
        std::cout << "    cdq\n";
        std::cout << "    idiv DWORD PTR -" << expr2_index * 4 << "[rbp]\n";
    } else if (ctx->op->getText()[0] == '%') {
        // Modulo
        std::cout << "    cdq\n";
        std::cout << "    idiv DWORD PTR -" << expr2_index * 4 << "[rbp]\n";
        std::cout << "    mov eax, edx\n";
    }

    // On ajoute une variable temporaire dans la table des symboles
    // correspondant au résultat de l'opération
    ++counterTempVariables;
    symbolsTable.insert(std::make_pair(
        "#" + std::to_string(counterTempVariables), symbolsTable.size() + 1));

    // On ajoute de résultat de l'opération dans la variable temporaire
    std::cout << "    mov DWORD PTR -" << (symbolsTable.size()) * 4
              << "[rbp], eax\n";

    // On renvoit la position de la variable temporaire
    return (int)symbolsTable.size();
}

antlrcpp::Any CodeGenVisitor::visitAdd(ifccParser::AddContext *ctx) {
    // On génère le code des expressions à additionner/soustraire et on récupère
    // l'index dans la pile de leur résultat
    int expr1_index = (int)visit(ctx->expression(0));
    int expr2_index = (int)visit(ctx->expression(1));

    // On bouge le résultat de la première expression dans eax
    std::cout << "    mov eax, DWORD PTR -" << expr1_index * 4 << "[rbp]\n";

    if (ctx->op->getText()[0] == '+') {
        // Addition
        std::cout << "    add eax, DWORD PTR -" << expr2_index * 4 << "[rbp]\n";
    } else if (ctx->op->getText()[0] == '-') {
        // Soustraire
        std::cout << "    sub eax, DWORD PTR -" << expr2_index * 4 << "[rbp]\n";
    }

    // On ajoute une variable temporaire dans la table des symboles
    // correspondant au résultat de l'opération
    ++counterTempVariables;
    symbolsTable.insert(std::make_pair(
        "#" + std::to_string(counterTempVariables), symbolsTable.size() + 1));

    // On ajoute de résultat de l'opération dans la variable temporaire
    std::cout << "    mov DWORD PTR -" << (symbolsTable.size()) * 4
              << "[rbp], eax\n";

    // On renvoit la position de la variable temporaire
    return (int)symbolsTable.size();
}

antlrcpp::Any CodeGenVisitor::visitUnaryAdd(ifccParser::UnaryAddContext *ctx) {
    // On génère le code de l'expression et on récupère l'index dans la pile de
    // son résultat
    int expr_index = (int)visit(ctx->expression());

    // Si l'opération unaire est un plus, on ne fait rien et on renvoit la
    // position du résultat de l'expression
    if (ctx->op->getText()[0] == '+')
        return (int)expr_index;

    // Opération unaire moins
    std::cout << "    mov eax, DWORD PTR -" << expr_index * 4 << "[rbp]\n";
    std::cout << "    neg eax\n";

    // On ajoute une variable temporaire dans la table des symboles
    // correspondant au résultat de l'opération
    ++counterTempVariables;
    symbolsTable.insert(std::make_pair(
        "#" + std::to_string(counterTempVariables), symbolsTable.size() + 1));

    // On ajoute de résultat de l'opération dans la variable temporaire
    std::cout << "    mov DWORD PTR -" << (symbolsTable.size()) * 4
              << "[rbp], eax\n";

    // On renvoit la position de la variable temporaire
    return (int)symbolsTable.size();
}
