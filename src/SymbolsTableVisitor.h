#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"


class  SymbolsTableVisitor : public ifccBaseVisitor {
	public:
        SymbolsTableVisitor(): counter(0) {};
        virtual antlrcpp::Any visitDeclaration(ifccParser::DeclarationContext *ctx) override;
        virtual antlrcpp::Any visitDeclaAffect(ifccParser::DeclaAffectContext *ctx) override;
        virtual antlrcpp::Any visitVariable(ifccParser::VariableContext *ctx) override;
        virtual antlrcpp::Any visitAffect(ifccParser::AffectContext *ctx) override;
        virtual antlrcpp::Any visitAxiom(ifccParser::AxiomContext *ctx) override;
        std::unordered_map<std::string, int> &getMap() { return map;};

    private:
        std::unordered_map<std::string, int> map;
        int counter;
};

