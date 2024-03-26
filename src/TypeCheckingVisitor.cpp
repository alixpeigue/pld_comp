#include "Scope.h"
#include "TypeCheckingVisitor.h"
#include "ifccParser.h"
#include "support/Any.h"

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
    return VarType::INT;
}
