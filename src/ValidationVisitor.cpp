/**
 * @file ValidationVisitor.cpp
 * @author H4231
 * @brief Verifie des éléments du programme non rejeté par la grammaire
 *          -nombre de paramètre dans les fonction
 *          -initialisation des variables
 *          -type des retours
 * @date 2024-04-02
 * 
 */

#include "Scope.h"
#include "ValidationVisitor.h"
#include "ifccParser.h"
#include "ifccVisitor.h"
#include "support/Any.h"
#include <optional>
#include <ostream>
#include <sstream>
#include <stdexcept>

/**
 * @brief Visite un axiome dans le cadre de la validation de l'arbre de syntaxe.
 * 
 * @param ctx contexte antlr4 du noeud
 * @return antlrcpp::Any 
 */

antlrcpp::Any ValidationVisitor::visitAxiom(ifccParser::AxiomContext *ctx) {
    this->visitChildren(ctx);
    return 0;
}

/**
 * @brief  Visite un contexte représentant une constante entière.
 * 
 * @param ctx contexte antlr4 du noeud
 * @return antlrcpp::Any 
 */

antlrcpp::Any ValidationVisitor::visitIntConst(
    ifccParser::IntConstContext *ctx) {
    return VarType(VarType::INT);
}

/**
 * @brief  Visite un contexte représentant une constante
 * 
 * @param ctx contexte antlr4 du noeud
 * @return antlrcpp::Any 
 */

antlrcpp::Any ValidationVisitor::visitCharConst(
    ifccParser::CharConstContext *ctx) {
    return VarType(VarType::INT);
}

/**
 * @brief Visite le contexte et mentionne si la variable est utilisé mais pas initialisée
 * 
 * @param ctx contexte antlr4 du noeud
 * @return antlrcpp::Any 
 */

antlrcpp::Any ValidationVisitor::visitVariable(
    ifccParser::VariableContext *ctx) {

    std::string name = ctx->VARIABLE()->getText();

    if (auto var = this->getVariable(name)) {
        var->second = VarState::USED;
        return var->first;
    } else {
        std::ostringstream message;
        message << "Variable '" << ctx->VARIABLE()->getText()
                << "' is used without being initialized";
        reporter.report(message.str(), ctx);
        exit(1);
    }
}

/**
 * @brief Visite le parent 
 * 
 * @param ctx contexte antlr4 du noeud
 * @return antlrcpp::Any 
 */

antlrcpp::Any ValidationVisitor::visitParen(ifccParser::ParenContext *ctx) {
    return this->visit(ctx->expression());
}

/**
 * @brief Visite le contexte d'une pre incrementation et mentionne si la variable est utilisé mais pas initialisée
 * 
 * @param ctx contexte antlr4 du noeud
 * @return antlrcpp::Any 
 */

antlrcpp::Any ValidationVisitor::visitPreInc(ifccParser::PreIncContext *ctx) {
    std::string name = ctx->VARIABLE()->getText();

    if (auto var = this->getVariable(name)) {
        return var->first;
    } else {
        std::ostringstream message;
        message << "Variable '" << ctx->VARIABLE()->getText()
                << "' is used withoud being initialized";
        reporter.report(message.str(), ctx);
        exit(1);
    }
}

/**
 * @brief Visite le contexte d'une pre decrementation et mentionne si la variable est utilisé mais pas initialisée
 * 
 * @param ctx contexte antlr4 du noeud
 * @return antlrcpp::Any 
 */

antlrcpp::Any ValidationVisitor::visitPreDec(ifccParser::PreDecContext *ctx) {
    std::string name = ctx->VARIABLE()->getText();

    if (auto var = this->getVariable(name)) {
        return var->first;
    } else {
        std::ostringstream message;
        message << "Variable '" << ctx->VARIABLE()->getText()
                << "' is used withoud being initialized";
        reporter.report(message.str(), ctx);
        exit(1);
    }
}

/**
 * @brief Visite le contexe des opérateurs unaires (-a ou +a) 
 * 
 * @param ctx contexte antlr4 du noeud
 * @return antlrcpp::Any 
 */
antlrcpp::Any ValidationVisitor::visitUnaryAdd(
    ifccParser::UnaryAddContext *ctx) {
    return this->visit(ctx->expression());
}

/**
 * @brief Visite le contexte d'une multiplication, division ou modulo et
 *        verifie les operandes
 * 
 * @param ctx contexte antlr4 du noeud 
 * @return antlrcpp::Any 
 */
antlrcpp::Any ValidationVisitor::visitMult(ifccParser::MultContext *ctx) {
    VarType t1 = this->visit(ctx->expression(0));
    VarType t2 = this->visit(ctx->expression(1));
    return std::max(t1, t2);
}

/**
 * @brief Visite le contexte d'une addition ou  soustraction et
 *        verifie les operandes
 * 
 * @param ctx contexte antlr4 du noeud
 * @return antlrcpp::Any 
 */
antlrcpp::Any ValidationVisitor::visitAdd(ifccParser::AddContext *ctx) {
    VarType t1 = this->visit(ctx->expression(0));
    VarType t2 = this->visit(ctx->expression(1));
    return std::max(t1, t2);
}

/**
 * @brief Visite le contexte d'un decalage (operation bits) et verifie le type de l'operande droite 
 * 
 * @param ctx contexte antlr4 du noeud
 * @return antlrcpp::Any 
 */
antlrcpp::Any ValidationVisitor::visitShift(ifccParser::ShiftContext *ctx) {
    VarType t1 = this->visit(ctx->expression(0));
    VarType t2 = this->visit(ctx->expression(1));
    if (t2 != VarType::INT) {
        std::ostringstream message;
        message << "'" << ctx->expression(1)->getText() << "' has invalid type";
        reporter.report(message.str(), ctx);
    }
    return t1;
}

/**
 * @brief Visite le contexte d'une comparaison
 * 
 * @param ctx contexte antlr4 du noeud
 * @return antlrcpp::Any 
 */

antlrcpp::Any ValidationVisitor::visitCompare(ifccParser::CompareContext *ctx) {
    return VarType(VarType::INT);
}

/**
 * @brief Visite le contexte d'un test d'egalite
 * 
 * @param ctx contexte antlr4 du noeud
 * @return antlrcpp::Any 
 */

antlrcpp::Any ValidationVisitor::visitCompareEq(
    ifccParser::CompareEqContext *ctx) {
    return VarType(VarType::INT);
}

/**
 * @brief Visite le contexte de l'operateur logique AND (bits)
 * 
 * @param ctx contexte antlr4 du noeud
 * @return antlrcpp::Any 
 */
antlrcpp::Any ValidationVisitor::visitAndBin(ifccParser::AndBinContext *ctx) {
    VarType t1 = this->visit(ctx->expression(0));
    VarType t2 = this->visit(ctx->expression(1));
    return std::max(t1, t2);
}

/**
 * @brief Visite le contexte de l'operateur logique XOR (bits)
 * 
 * @param ctx contexte antlr4 du noeud
 * @return antlrcpp::Any 
 */
antlrcpp::Any ValidationVisitor::visitXorBin(ifccParser::XorBinContext *ctx) {
    VarType t1 = this->visit(ctx->expression(0));
    VarType t2 = this->visit(ctx->expression(1));
    return std::max(t1, t2);
}

/** 
 *  @brief Visite le contexte de l'operateur logique OR (bits)
 * 
 * @param ctx contexte antlr4 du noeud
 * @return antlrcpp::Any 
 */

antlrcpp::Any ValidationVisitor::visitOrBin(ifccParser::OrBinContext *ctx) {
    VarType t1 = this->visit(ctx->expression(0));
    VarType t2 = this->visit(ctx->expression(1));
    return std::max(t1, t2);
}


/**
 * @brief Visite le contexte du and &&
 * 
 * @param ctx contexte antlr4 du noeud
 * @return antlrcpp::Any 
 */
antlrcpp::Any ValidationVisitor::visitAnd(ifccParser::AndContext *ctx) {
    this->visitChildren(ctx);
    return VarType(VarType::INT);
}

/**
 * @brief Visite le contexte du or || 
 * 
 * @param ctx contexte antlr4 du noeud
 * @return antlrcpp::Any 
 */
antlrcpp::Any ValidationVisitor::visitOr(ifccParser::OrContext *ctx) {
    this->visitChildren(ctx);
    return VarType(VarType::INT);
}

/**
 * @brief Visite le contexte d'une affectation et verifie si la variable a ete declare au prealable
 * 
 * @param ctx contexte antlr4 du noeud
 * @return antlrcpp::Any 
 */
antlrcpp::Any ValidationVisitor::visitAffect(ifccParser::AffectContext *ctx) {

    Var *ltype;

    if (!(ltype = this->getVariable(ctx->VARIABLE()->getText()))) {
        std::ostringstream message;
        message << "Variable '" << ctx->VARIABLE()->getText()
                << "' has not been declared";
        this->reporter.report(message.str(), ctx);
        exit(1);
    }

    this->visit(ctx->expression());

    return ltype->first;
}

/**
 * @brief Visite le contexte d'une post incrementation et mentionner si la variable est utilisé mais pas initialisée
 * 
 * @param ctx contexte antlr4 du noeud
 * @return antlrcpp::Any 
 */
antlrcpp::Any ValidationVisitor::visitPostInc(ifccParser::PostIncContext *ctx) {
    std::string name = ctx->VARIABLE()->getText();

    if (auto var = this->getVariable(name)) {
        return var->first;
    } else {
        std::ostringstream message;
        message << "Variable '" << ctx->VARIABLE()->getText()
                << "' is used withoud being initialized";
        reporter.report(message.str(), ctx);
        exit(1);
    }
}


/**
 * @brief Visite le contexte d'une post decrementation et mentionner si la variable est utilisé mais pas initialisée
 * 
 * @param ctx contexte antlr4 du noeud
 * @return antlrcpp::Any 
 */
antlrcpp::Any ValidationVisitor::visitPostDec(ifccParser::PostDecContext *ctx) {
    std::string name = ctx->VARIABLE()->getText();

    if (auto var = this->getVariable(name)) {
        return var->first;
    } else {
        std::ostringstream message;
        message << "Variable '" << ctx->VARIABLE()->getText()
                << "' is used without being initialized";
        reporter.report(message.str(), ctx);
        exit(1);
    }
}


/**
 * @brief Visite le contexte et mentionner si la fonction a le bon nombre d'argument sinon léve une erreur 
 * 
 * @param ctx contexte antlr4 du noeud
 * @return antlrcpp::Any 
 */
antlrcpp::Any ValidationVisitor::visitFunc_call(
    ifccParser::Func_callContext *ctx) {
    std::string funcName = ctx->VARIABLE()->getText();
    Function f;
    try {
        f = this->functions.at(funcName);
    } catch (std::out_of_range &) {
        VarType ret = VarType::INT;
        return ret;
    }
    if (f.size() - 1 > ctx->expression().size()) {
        std::ostringstream message;
        message << "Too few arguments : function '"
                << ctx->VARIABLE()->getText() << "' takes " << f.size() - 1
                << " arguments but " << ctx->expression().size()
                << " were given";
        this->reporter.report(message.str(), ctx);
        exit(1);
    }
    if (f.at(0) == VarType::VOID &&
        !dynamic_cast<ifccParser::StatementContext *>(ctx->parent)) {

        this->reporter.report("void value not ignored as it should be", ctx);
        exit(1);
    }
    return f.at(0);
}

/**
 * @brief Visite le contexte d'une fonction et en verifie la validite 
 * 
 * @param ctx contexte antlr4 du noeud
 * @return antlrcpp::Any 
 */
antlrcpp::Any ValidationVisitor::visitFunction(
    ifccParser::FunctionContext *ctx) {

    this->scopes.push_back(Scope());
    std::string funcName = ctx->VARIABLE(0)->getText();

    Function function;
    function.reserve(ctx->type().size());

    for (auto &type : ctx->type()) {
        function.push_back(type->getText());
    }

    for (uint32_t i = 1; i < ctx->type().size(); ++i) {
        this->scopes.back()[ctx->VARIABLE(i)->getText()] =
            std::make_pair(ctx->type(i)->getText(), VarState::DECLARED);
    }

    this->functions[funcName] = function;
    this->visitChildren(ctx);

    for (auto &var : this->scopes.back()) {
        if (var.second.second == VarState::DECLARED) {
            std::cerr << "Waring : variable '" << var.first
                      << "' is declared but never used\n";
        }
    }

    this->scopes.pop_back();
    return 0;
}

/**
 * @brief Visite le contexte et mentionner que la variable est declaree mais pas utilisee
 * 
 * @param ctx contexte antlr4 du noeud
 * @return antlrcpp::Any 
 */
antlrcpp::Any ValidationVisitor::visitScope(ifccParser::ScopeContext *ctx) {
    this->scopes.push_back(Scope());
    this->visitChildren(ctx);
    for (auto &var : this->scopes.back()) {
        if (var.second.second == VarState::DECLARED) {
            std::cerr << "Warning : variable '" << var.first
                      << "' is declared but never used\n";
        }
    }
    this->scopes.pop_back();
    return 0;
}

/**
 * @brief Visite le contexte et vérifie que la variable est un type correcte
 * 
 * @param ctx contexte antlr4 du noeud
 * @return antlrcpp::Any 
 */

antlrcpp::Any ValidationVisitor::visitDeclaration(
    ifccParser::DeclarationContext *ctx) {

    this->currentType = ctx->type()->getText();

    if (this->currentType == VarType::VOID) {
        this->reporter.report("Illegal type 'void' for variable", ctx->type());
        exit(1);
    }

    this->visitChildren(ctx);
    return 0;
}


/**
 * @brief Visite le contexte de déclaration avec affectation et met à jour les scopes en ajoutant la variable déclarée.
 * 
 * @param ctx contexte antlr4 du noeud
 * @return antlrcpp::Any 
 */
antlrcpp::Any ValidationVisitor::visitDeclaAffect(
    ifccParser::DeclaAffectContext *ctx) {

    // if (ctx->expression()) {

    //     VarType rtype = this->visit(ctx->expression());

    //     if (rtype == VarType::VOID) {
    //         std::ostringstream message;
    //         message << "Assign void value '" << ctx->expression()->getText()
    //                 << "' to variable '" << ctx->VARIABLE()->getText() <<
    //                 "'";
    //         this->reporter.report(message.str(), ctx);
    //         exit(1);
    //     }
    // }
    this->visitChildren(ctx);

    std::string varName = ctx->VARIABLE()->getText();
    this->scopes.back()[varName] =
        std::make_pair(this->currentType, VarState::DECLARED);
    return 0;
}


/**
 * @brief Récupère une variable par son nom dans les scopes.
 * 
 * @param name 
 * @return ValidationVisitor::Var* 
 */
ValidationVisitor::Var *ValidationVisitor::getVariable(
    const std::string &name) {

    for (auto scope = scopes.rbegin(); scope != scopes.rend(); ++scope) {
        try {
            return &scope->at(name);
        } catch (std::out_of_range) {
        }
    }
    return nullptr;
}
