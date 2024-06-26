#pragma once

#include "ErrorReporter.h"
#include "Scope.h"
#include "ifccBaseVisitor.h"
#include "ifccParser.h"
#include "support/Any.h"
#include <unordered_map>

/**
 * @brief Classe qui vérifie que l'arbre de syntaxe est valide
 * @details
 - variables non déclarées
 - validation des types
 - variables déclarées plusieurs fois
 - variables déclarées mais pas utilisées
 - function déclarées plusieurs fois
 - respect des signatures des fonctions
 */
class ValidationVisitor : public ifccBaseVisitor {
public:
    /**
     * @brief Construit un visiteur de validation d'AST
     * 
     * @param reporter La classe à qui reporter les erreurs
     */
    ValidationVisitor(const ErrorReporter &reporter) : reporter(reporter) {}

protected:
    enum class VarState { DECLARED, USED };

    using Var = std::pair<VarType, VarState>;
    using Scope = std::unordered_map<std::string, Var>;
    using Function = std::vector<VarType>;

    virtual antlrcpp::Any visitAxiom(ifccParser::AxiomContext *ctx) override;
    virtual antlrcpp::Any visitIntConst(
        ifccParser::IntConstContext *ctx) override;

    virtual antlrcpp::Any visitCharConst(
        ifccParser::CharConstContext *ctx) override;

    virtual antlrcpp::Any visitVariable(
        ifccParser::VariableContext *ctx) override;

    virtual antlrcpp::Any visitParen(ifccParser::ParenContext *ctx) override;

    virtual antlrcpp::Any visitPreInc(ifccParser::PreIncContext *ctx) override;

    virtual antlrcpp::Any visitPreDec(ifccParser::PreDecContext *ctx) override;

    virtual antlrcpp::Any visitUnaryAdd(
        ifccParser::UnaryAddContext *ctx) override;

    virtual antlrcpp::Any visitMult(ifccParser::MultContext *ctx) override;

    virtual antlrcpp::Any visitAdd(ifccParser::AddContext *ctx) override;

    virtual antlrcpp::Any visitShift(ifccParser::ShiftContext *ctx) override;

    virtual antlrcpp::Any visitCompare(
        ifccParser::CompareContext *ctx) override;

    virtual antlrcpp::Any visitCompareEq(
        ifccParser::CompareEqContext *ctx) override;

    virtual antlrcpp::Any visitAndBin(ifccParser::AndBinContext *ctx) override;

    virtual antlrcpp::Any visitXorBin(ifccParser::XorBinContext *ctx) override;

    virtual antlrcpp::Any visitOrBin(ifccParser::OrBinContext *ctx) override;

    virtual antlrcpp::Any visitAnd(ifccParser::AndContext *ctx) override;

    virtual antlrcpp::Any visitOr(ifccParser::OrContext *ctx) override;

    virtual antlrcpp::Any visitAffect(ifccParser::AffectContext *ctx) override;

    virtual antlrcpp::Any visitPostInc(
        ifccParser::PostIncContext *ctx) override;

    virtual antlrcpp::Any visitPostDec(
        ifccParser::PostDecContext *ctx) override;

    virtual antlrcpp::Any visitFunc_call(
        ifccParser::Func_callContext *ctx) override;

    virtual antlrcpp::Any visitFunction(
        ifccParser::FunctionContext *ctx) override;

    virtual antlrcpp::Any visitScope(ifccParser::ScopeContext *ctx) override;

    virtual antlrcpp::Any visitDeclaration(
        ifccParser::DeclarationContext *ctx) override;

    virtual antlrcpp::Any visitDeclaAffect(
        ifccParser::DeclaAffectContext *ctx) override;

    Var *getVariable(const std::string &name);

    std::vector<Scope> scopes;
    std::unordered_map<std::string, Function> functions;
    VarType currentType;
    const ErrorReporter &reporter;
};
