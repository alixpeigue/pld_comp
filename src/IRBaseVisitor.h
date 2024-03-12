#pragma once

#include "ir.h"
class IRBaseVisitor {
public:
    virtual void visitAffect(ir::Affect &affect) = 0;
    virtual void visitAdd(ir::Add & add) = 0;
};
