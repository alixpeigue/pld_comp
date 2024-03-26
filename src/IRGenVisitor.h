#pragma once

#include "antlr4-runtime.h"
#include "ifccBaseVisitor.h"
#include "ifccParser.h"
#include "ir.h"
#include "support/Any.h"

// Linearise le code vers notre IR
class IRGenVisitor : public ifccBaseVisitor {
public:
    IRGenVisitor(std::vector<std::unique_ptr<ir::CFG>> &ir) : ir(ir){};


    virtual antlrcpp::Any visitAxiom(ifccParser::AxiomContext *ctx) override;

    virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override;

    virtual antlrcpp::Any visitScope(ifccParser::ScopeContext *ctx) override;

    virtual antlrcpp::Any visitIf_stmt(
        ifccParser::If_stmtContext *ctx) override;

    virtual antlrcpp::Any visitWhile_stmt(
        ifccParser::While_stmtContext *ctx) override;

    virtual antlrcpp::Any visitDo_while_stmt(
        ifccParser::Do_while_stmtContext *ctx) override;

    virtual antlrcpp::Any visitContinue_stmt(ifccParser::Continue_stmtContext *ctx) override;

    virtual antlrcpp::Any visitBreak_stmt(ifccParser::Break_stmtContext *ctx) override;

    virtual antlrcpp::Any visitFunc_call(
        ifccParser::Func_callContext *ctx) override;

    virtual antlrcpp::Any visitFunction(
        ifccParser::FunctionContext *ctx) override;

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

    virtual antlrcpp::Any visitCompare(
        ifccParser::CompareContext *ctx) override;

    virtual antlrcpp::Any visitCompareEq(
        ifccParser::CompareEqContext *ctx) override;

    virtual antlrcpp::Any visitXorBin(ifccParser::XorBinContext *ctx) override;

    virtual antlrcpp::Any visitOrBin(ifccParser::OrBinContext *ctx) override;

    virtual antlrcpp::Any visitAndBin(ifccParser::AndBinContext *ctx) override;

    virtual antlrcpp::Any visitAnd(ifccParser::AndContext *ctx) override;

    virtual antlrcpp::Any visitOr(ifccParser::OrContext *ctx) override;

    virtual antlrcpp::Any visitPostDec(
        ifccParser::PostDecContext *ctx) override;

    virtual antlrcpp::Any visitPreInc(ifccParser::PreIncContext *ctx) override;

    virtual antlrcpp::Any visitPostInc(
        ifccParser::PostIncContext *ctx) override;

    virtual antlrcpp::Any visitPreDec(ifccParser::PreDecContext *ctx) override;

private:
    std::vector<std::unique_ptr<ir::CFG>> &ir;
    std::unique_ptr<ir::CFG> currentFunction;
    ir::BasicBlock *currentBlock;
    int counterTempVariables = 0;
    int counterBlocks = 0;
};
