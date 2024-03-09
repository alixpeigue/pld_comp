#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"


class  CodeGenVisitor : public ifccBaseVisitor {
	public:
        CodeGenVisitor(std::unordered_map<std::string, int> &symbolsTable): symbolsTable(symbolsTable) {};
        virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override ;
        virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;
        virtual antlrcpp::Any visitDeclaration(ifccParser::DeclarationContext *ctx) override;
        virtual antlrcpp::Any visitDeclaAffect(ifccParser::DeclaAffectContext *ctx) override;
        virtual antlrcpp::Any visitVariable(ifccParser::VariableContext *ctx) override;
        virtual antlrcpp::Any visitAffect(ifccParser::AffectContext *ctx) override;
        virtual antlrcpp::Any visitIntConst(ifccParser::IntConstContext *ctx) override;
        virtual antlrcpp::Any visitCharConst(ifccParser::CharConstContext *ctx) override;
        virtual antlrcpp::Any visitParen(ifccParser::ParenContext *ctx) override;
        virtual antlrcpp::Any visitMult(ifccParser::MultContext *ctx) override;
        virtual antlrcpp::Any visitAdd(ifccParser::AddContext *ctx) override;
        virtual antlrcpp::Any visitUnaryAdd(ifccParser::UnaryAddContext *ctx) override;
    private:
        std::unordered_map<std::string, int> &symbolsTable;
        int counterTempVariables = 0;
};