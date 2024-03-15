#pragma once

#include "antlr4-runtime.h"
#include "ifccBaseVisitor.h"
#include "ifccParser.h"
#include "ir.h"
#include "support/Any.h"

//Linearise le code vers notre IR
class IRGenVisitor : public ifccBaseVisitor {
public:
    IRGenVisitor(std::vector<std::unique_ptr<ir::CFG>> &ir) : ir(ir){};

    virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override;

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

    virtual antlrcpp::Any visitShift(ifccParser::ShiftContext *ctx) override;

private:
    std::vector<std::unique_ptr<ir::CFG>> &ir;
    std::unique_ptr<ir::CFG> currentFunction;
    std::unique_ptr<ir::BasicBlock> currentBlock;
    int counterTempVariables = 0;
};
