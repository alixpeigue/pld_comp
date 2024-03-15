#include "SymbolsTableVisitor.h"

antlrcpp::Any SymbolsTableVisitor::visitAxiom(ifccParser::AxiomContext *ctx) {

    this->visitChildren(ctx);

    // Une fois tous les enfants visités, on émet un warning si une variable a été déclarée
    // mais pas utilisée (elle n'est pas utilisée si sa position dans la pile est toujours à 0)
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

    // Si la variable déclarée n'existe pas dans la table des symboles
    // on l'ajoute et on initialise sa position dans la pile à 0
    // Sinon, la variable a déjà été déclarée et on arrête le programme avec une erreur
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

    // Si la variable visitée n'existe pas dans la table des symboles
    // C'est qu'elle n'a pas été déclarée, on arrête le programme avec une erreur
    // Sinon, on définit sa véritable position dans la pile
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

    // Si la variable visitée n'existe pas dans la table des symboles
    // C'est qu'elle n'a pas été déclarée, on arrête le programme avec une erreur
    if (map.find(variable) == map.end()) {
        std::cerr << "La variable '" << variable << "' n'est pas déclarée\n";
        exit(1);
    }

    return 0;
}
