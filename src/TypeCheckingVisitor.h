#pragma once

#include "Scope.h"
#include "ifccBaseVisitor.h"
#include "ifccParser.h"
#include "support/Any.h"
#include <unordered_map>
class TypeCheckingVisitor : public ifccBaseVisitor {
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

protected:
    using Block = std::unordered_map<std::string, Variable>;
    using Function = std::unordered_map<std::string, std::vector<VarType>>;
    std::vector<Block> blocks;
    std::vector<Function> functions;
};
