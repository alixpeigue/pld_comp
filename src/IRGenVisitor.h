#pragma once

#include "antlr4-runtime.h"
#include "ifccBaseVisitor.h"
#include "ifccParser.h"
#include "ir.h"
#include "support/Any.h"

class IRGenVisitor : public ifccBaseVisitor {
public:
    IRGenVisitor(std::vector<std::unique_ptr<ir::CFG>> &ir) : ir(ir){};

    virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override;

    virtual antlrcpp::Any visitScope(ifccParser::ScopeContext *ctx) override;

    virtual antlrcpp::Any visitIf_stmt(
        ifccParser::If_stmtContext *ctx) override;

    virtual antlrcpp::Any visitWhile_stmt(
        ifccParser::While_stmtContext *ctx) override;

    virtual antlrcpp::Any visitDo_while_stmt(
        ifccParser::Do_while_stmtContext *ctx) override;

    virtual antlrcpp::Any visitReturn_stmt(
        ifccParser::Return_stmtContext *ctx) override;

    virtual antlrcpp::Any visitDeclaAffect(
        ifccParser::DeclaAffectContext *ctx) override;

    virtual antlrcpp::Any visitVariable(
        ifccParser::VariableContext *ctx) override;

    virtual antlrcpp::Any visitAffect(ifccParser::AffectContext *ctx) override;

    virtual antlrcpp::Any visitIntConst(
        ifccParser::IntConstContext *ctx) override;

    virtual antlrcpp::Any visitCharConst(
        ifccParser::CharConstContext *ctx) override;

    virtual antlrcpp::Any visitParen(ifccParser::ParenContext *ctx) override;

    virtual antlrcpp::Any visitMult(ifccParser::MultContext *ctx) override;

    virtual antlrcpp::Any visitAdd(ifccParser::AddContext *ctx) override;

    virtual antlrcpp::Any visitUnaryAdd(
        ifccParser::UnaryAddContext *ctx) override;

private:
    std::vector<std::unique_ptr<ir::CFG>> &ir;
    std::unique_ptr<ir::CFG> currentFunction;
    std::unique_ptr<ir::BasicBlock> currentBlock;
    int counterTempVariables = 0;
    int counterBlocks = 0;
};
