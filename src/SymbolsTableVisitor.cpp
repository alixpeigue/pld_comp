#include "SymbolsTableVisitor.h"

antlrcpp::Any SymbolsTableVisitor::visitAxiom(ifccParser::AxiomContext *ctx) {
    this->visitChildren(ctx);
    for (auto &pair : map) {
        if (pair.second == 0) {
            std::cerr << "La variable '" << pair.first
                      << "' est déclarée mais n'est jamais utilisée\n";
            pair.second = ++counter;
        }
    }

    return 0;
}

antlrcpp::Any
SymbolsTableVisitor::visitDeclaration(ifccParser::DeclarationContext *ctx) {
    visitChildren(ctx);
    return 0;
}

antlrcpp::Any
SymbolsTableVisitor::visitDeclaAffect(ifccParser::DeclaAffectContext *ctx) {

    std::string variable = ctx->VARIABLE()->getText();
    if (map.find(variable) == map.end()) {
        map.emplace(variable, 0);
    } else {
        std::cerr << "La variable '" << variable << "' a déjà été déclarée\n";
        exit(1);
    }

    return 0;
}

antlrcpp::Any
SymbolsTableVisitor::visitVariable(ifccParser::VariableContext *ctx) {
    std::string variable = ctx->VARIABLE()->getText();
    if (map.find(variable) == map.end()) {
        std::cerr << "La variable '" << variable << "' n'est pas déclarée\n";
        exit(1);
    } else if (map.at(variable) == 0) {
        map.at(variable) = ++counter;
    }

    return 0;
}

antlrcpp::Any SymbolsTableVisitor::visitAffect(ifccParser::AffectContext *ctx) {
    std::string variable = ctx->VARIABLE()->getText();
    visit(ctx->expression());
    if (map.find(variable) == map.end()) {
        std::cerr << "La variable '" << variable << "' n'est pas déclarée\n";
        exit(1);
    }

    return 0;
}
