#include "Scope.h"
#include "TypeCheckingVisitor.h"
#include "ifccParser.h"
#include "support/Any.h"
#include <ranges>
#include <stdexcept>

antlrcpp::Any TypeCheckingVisitor::visitIntConst(
    ifccParser::IntConstContext *ctx) {
    return VarType::INT;
}

antlrcpp::Any TypeCheckingVisitor::visitCharConst(
    ifccParser::CharConstContext *ctx) {
    return VarType::INT;
}

antlrcpp::Any TypeCheckingVisitor::visitVariable(
    ifccParser::VariableContext *ctx) {

    std::string name = ctx->VARIABLE()->getText();

    for (auto &block : blocks | std::ranges::views::reverse) {
        try {
            return block.at(name).first;
        } catch (std::out_of_range) {
            std::cerr << "at " << ctx->VARIABLE()->getSymbol()->getLine() << ":"
                      << ctx->VARIABLE()->getSymbol()->getCharPositionInLine()
                      << ", variable " << name << "has not been declared";
        }
    }
    return VarType::INT;
}

antlrcpp::Any TypeCheckingVisitor::visitParen(ifccParser::ParenContext *ctx) {
    return
}
