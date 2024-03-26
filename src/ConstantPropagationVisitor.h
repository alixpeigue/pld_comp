#pragma once

#include "IRBaseVisitor.h"
#include "ir.h"
#include <unordered_map>
class ConstantPropagationVisitor : public IRBaseVisitor {
    virtual void visitAffectConst(ir::AffectConst &affect) override;
    virtual void visitAffect(ir::Affect &affect) override;

protected:
    std::unordered_map<std::string, int> constants;
};
