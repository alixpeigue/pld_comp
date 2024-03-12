#include "IRx86Visitor.h"


class IRx86Visitor : public IRBaseVisitor {
    void visitAffect(ir::Affect &affect) {}

    void visitAffectConst(ir::AffectConst &affectConst) {
        std::cout << "    mov # A compléter !";


    }

};
