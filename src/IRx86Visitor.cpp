#include "IRx86Visitor.h"


class IRx86Visitor : public IRBaseVisitor {
    void visitAffect(ir::Affect &affect) {
        std::cout << "    mov # A compléter !";
    }

    void visitAffectConst(ir::AffectConst &affectConst) {
        std::cout << "    mov # A compléter !";


    }

};
