#pragma once

#include "ir.h"
class IRBaseVisitor {
public:
    virtual void visitAffect(ir::Affect &affect) = 0;
};
