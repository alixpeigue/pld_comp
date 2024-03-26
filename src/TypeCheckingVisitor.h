#pragma once

#include "Scope.h"
#include "ifccBaseVisitor.h"
#include "ifccParser.h"
#include "support/Any.h"
#include <unordered_map>
class TypeCheckingVisitor : public ifccBaseVisitor {

    enum VarState { DECLARED, USED };

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

    virtual antlrcpp::Any visitUnaryAdd(
        ifccParser::UnaryAddContext *ctx) override;

    virtual antlrcpp::Any visitMult(ifccParser::MultContext *ctx) override;

    virtual antlrcpp::Any visitAdd(ifccParser::AddContext *ctx) override;

    virtual antlrcpp::Any visitAffect(ifccParser::AffectContext *ctx) override;

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

protected:
    std::vector<Scope> scopes;
    std::unordered_map<std::string, Function> functions;
    VarType currentType;
};
