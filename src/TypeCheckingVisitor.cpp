#include "Scope.h"
#include "TypeCheckingVisitor.h"
#include "ifccParser.h"
#include "ifccVisitor.h"
#include "support/Any.h"
#include <optional>
#include <ranges>
#include <stdexcept>

antlrcpp::Any TypeCheckingVisitor::visitAxiom(ifccParser::AxiomContext *ctx) {
    this->visitChildren(ctx);
    for (auto &scope : this->scopes) {
        for (auto &var : scope) {
            if (var.second.second == DECLARED) {
                std::cout << "Waring : variable '" << var.first
                          << "' is declared but never used\n";
            }
        }
    }
    return 0;
}

antlrcpp::Any TypeCheckingVisitor::visitIntConst(
    ifccParser::IntConstContext *ctx) {
    return VarType(VarType::INT);
}

antlrcpp::Any TypeCheckingVisitor::visitCharConst(
    ifccParser::CharConstContext *ctx) {
    return VarType(VarType::INT);
}

antlrcpp::Any TypeCheckingVisitor::visitVariable(
    ifccParser::VariableContext *ctx) {

    std::string name = ctx->VARIABLE()->getText();

    if (auto var = this->getVariable(name)) {
        var->second = USED;
        return var->first;
    } else {
        std::cerr << "Error at " << ctx->VARIABLE()->getSymbol()->getLine()
                  << ":"
                  << ctx->VARIABLE()->getSymbol()->getCharPositionInLine()
                  << ", variable '" << name << "' has not been declared\n";
        exit(1);
    }
}

antlrcpp::Any TypeCheckingVisitor::visitParen(ifccParser::ParenContext *ctx) {
    return this->visit(ctx->expression());
}

antlrcpp::Any TypeCheckingVisitor::visitUnaryAdd(
    ifccParser::UnaryAddContext *ctx) {
    return this->visit(ctx->expression());
}

antlrcpp::Any TypeCheckingVisitor::visitMult(ifccParser::MultContext *ctx) {
    VarType t1 = this->visit(ctx->expression(0));
    VarType t2 = this->visit(ctx->expression(1));
    return std::max(t1, t2);
}

antlrcpp::Any TypeCheckingVisitor::visitAdd(ifccParser::AddContext *ctx) {
    VarType t1 = this->visit(ctx->expression(0));
    VarType t2 = this->visit(ctx->expression(1));
    return std::max(t1, t2);
}

antlrcpp::Any TypeCheckingVisitor::visitAffect(ifccParser::AffectContext *ctx) {

    VarType rtype = this->visit(ctx->expression());

    if (rtype == VarType::VOID) {
        std::cerr << "Error at " << ctx->VARIABLE()->getSymbol()->getLine()
                  << ":"
                  << ctx->VARIABLE()->getSymbol()->getCharPositionInLine()
                  << " assign void value to variable '"
                  << ctx->VARIABLE()->getText() << "'\n";
        exit(1);
    }

    if (auto ltype = this->getVariable(ctx->VARIABLE()->getText())) {
        return ltype->first;
    } else {
        std::cerr << "Error at " << ctx->VARIABLE()->getSymbol()->getLine()
                  << ":"
                  << ctx->VARIABLE()->getSymbol()->getCharPositionInLine()
                  << " assign to undeclared variable '"
                  << ctx->VARIABLE()->getText() << "'\n";
        exit(1);
    }
}

antlrcpp::Any TypeCheckingVisitor::visitFunc_call(
    ifccParser::Func_callContext *ctx) {
    std::string funcName = ctx->VARIABLE()->getText();
    Function f;
    try {
        f = this->functions.at(funcName);
    } catch (std::out_of_range) {
        VarType ret = VarType::INT;
        return ret;
    }
    if (f.size() - 1 > ctx->expression().size()) {
        std::cout << f.size() << " " << ctx->expression().size();
        std::cerr << "Error at " << ctx->VARIABLE()->getSymbol()->getLine()
                  << ":"
                  << ctx->VARIABLE()->getSymbol()->getCharPositionInLine()
                  << " too few arguments for function '"
                  << ctx->VARIABLE()->getText() << "'\n";
        exit(1);
    }
    return f.at(0);
}

antlrcpp::Any TypeCheckingVisitor::visitFunction(
    ifccParser::FunctionContext *ctx) {

    this->scopes.push_back(Scope());
    std::string funcName = ctx->VARIABLE(0)->getText();

    Function function;
    function.reserve(ctx->type().size());

    for (auto &type : ctx->type()) {
        function.push_back(type->getText());
    }

    for (int i = 1; i < ctx->type().size(); ++i) {
        this->scopes.back()[ctx->VARIABLE(i)->getText()] =
            std::make_pair(ctx->type(i)->getText(), DECLARED);
    }

    this->functions[funcName] = function;
    this->visitChildren(ctx);
    this->scopes.pop_back();
    return 0;
}

antlrcpp::Any TypeCheckingVisitor::visitScope(ifccParser::ScopeContext *ctx) {
    this->scopes.push_back(Scope());
    this->visitChildren(ctx);
    this->scopes.pop_back();
    return 0;
}

antlrcpp::Any TypeCheckingVisitor::visitDeclaration(
    ifccParser::DeclarationContext *ctx) {

    this->currentType = ctx->type()->getText();
    this->visitChildren(ctx);
    return 0;
}

antlrcpp::Any TypeCheckingVisitor::visitDeclaAffect(
    ifccParser::DeclaAffectContext *ctx) {

    std::string varName = ctx->VARIABLE()->getText();
    this->scopes.back()[varName] = std::make_pair(this->currentType, DECLARED);
    return 0;
}

TypeCheckingVisitor::Var *TypeCheckingVisitor::getVariable(
    const std::string &name) {

    for (auto &scope : scopes | std::ranges::views::reverse) {
        try {
            return &scope.at(name);
        } catch (std::out_of_range) {
        }
    }
    return nullptr;
}
