#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>
#include <sstream>

#include "ConstantPropagationVisitor.h"
#include "IRGenVisitor.h"
#include "IRx86Visitor.h"
#include "JumpBlocksVisitor.h"
#include "RV64Visitor.h"
#include "RemoveBlocksVisitor.h"
#include "UnusedTempVarRemoverVisitor.h"
#include "UserErrorReporter.h"
#include "ValidationVisitor.h"
#include "ifccLexer.h"
#include "ifccParser.h"

#include "IRBaseVisitor.h"
#include "Options.h"
#include "ir.h"

#include <unistd.h>

using namespace antlr4;
using namespace std;

int main(int argn, char **argv) {

    Options opt = Options::fromArgs(argn, argv);

    stringstream in;
    ifstream lecture(opt.in);
    if (!lecture.good()) {
        cerr << "error: cannot read file: " << opt.in << endl;
        exit(1);
    }
    in << lecture.rdbuf();

    ostream *os;
    unique_ptr<ofstream> ofs;
    if (opt.out == "stdout") {
        os = &std::cout;
    } else {
        ofs = make_unique<ofstream>(opt.out, ofstream::out);
        os = ofs.get();
    }

    std::unique_ptr<IRBaseVisitor> codeGenVisitor;
    if (opt.target == "rv64")
        codeGenVisitor = std::make_unique<RV64Visitor>(*os);
    else if (opt.target == "x86-64")
        codeGenVisitor = std::make_unique<IRx86Visitor>(*os);
    else {
        std::cerr << "Unknown architecture '" << opt.target << "'";
        exit(0);
    }

    ANTLRInputStream input(in.str());

    // Initialisation du lexer et récupération des tokens
    ifccLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    tokens.fill();

    // Initialisation du parser et création de l'arbre syntaxique
    ifccParser parser(&tokens);
    tree::ParseTree *tree = parser.axiom();

    // Gestion des erreurs de syntaxe
    if (parser.getNumberOfSyntaxErrors() != 0) {
        cerr << "error: syntax error during parsing" << endl;
        exit(1);
    }

    // liste de graphe (un graphe = une fonction)
    vector<std::unique_ptr<ir::CFG>> ir;
    UserErrorReporter reporter(in.str());
    ValidationVisitor v0(reporter);
    v0.visit(tree);
    IRGenVisitor v1(ir);
    v1.visit(tree); // creer l'IR

    ConstantPropagationVisitor cstpropv;
    for (const auto &i : ir) {
        i->visitBlocks(cstpropv);
    }

    UnusedTempVarRemoverVisitor tmprmv;
    for (const auto &i : ir) {
        i->visitBlocks(tmprmv);
    }

    JumpBlocksVisitor jbv;
    for (const auto &i : ir) {
        i->visitBlocks(jbv);
    }

    RemoveBlocksVisitor rmbv;
    for (const auto &i : ir) {
        i->visitBlocks(rmbv);
    }

    for (const auto &i : ir) {
        i->visitBlocks(*codeGenVisitor);
    }
    return 0;
}
