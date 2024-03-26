#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

#include "IRGenVisitor.h"
#include "IRx86Visitor.h"
#include "antlr4-runtime.h"
#include "ifccBaseVisitor.h"
#include "ifccLexer.h"
#include "ifccParser.h"

#include "CodeGenVisitor.h"
#include "SymbolsTableVisitor.h"

#include "IRBaseVisitor.h"
#include "Scope.h"
#include "ir.h"

using namespace antlr4;
using namespace std;

int main(int argn, const char **argv) {

    // Gestion de l'input
    stringstream in;
    if (argn == 2) {
        ifstream lecture(argv[1]);
        if (!lecture.good()) {
            cerr << "error: cannot read file: " << argv[1] << endl;
            exit(1);
        }
        in << lecture.rdbuf();
    } else {
        cerr << "usage: ifcc path/to/file.c" << endl;
        exit(1);
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

    // SymbolsTableVisitor symbolsTable;
    // symbolsTable.visit(tree);
    // CodeGenVisitor v(symbolsTable.getMap());
    // v.visit(tree);
    vector<std::unique_ptr<ir::CFG>> ir;
    IRGenVisitor v1(ir);
    v1.visit(tree);

    std::cout << ".intel_syntax noprefix\n";
    IRx86Visitor v2;
    for (const auto & i : ir) {
        i->visitBlocks(v2);
    }
    return 0;
}
