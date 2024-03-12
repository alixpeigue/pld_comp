#pragma once

#include "IRBaseVisitor.h"
#include "ir.h"
#include <iostream>

class IRx86Visitor : public IRBaseVisitor {
    void visitAffect(ir::Affect &affect) {
        std::cout << "    mov # A compléter !";
    }
};
