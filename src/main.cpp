#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

#include "antlr4-runtime.h"
#include "ifccBaseVisitor.h"
#include "ifccLexer.h"
#include "ifccParser.h"

#include "CodeGenVisitor.h"
#include "SymbolsTableVisitor.h"

#include "IRBaseVisitor.h"
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

<<<<<<< HEAD

    // Visite de l'arbre pour créer la table des symboles
    SymbolsTableVisitor symbolsTableVisitor;
    symbolsTableVisitor.visit(tree);

    // Visite de l'arbre en passant la table des symboles pour générer le code assembleur
    CodeGenVisitor v(symbolsTableVisitor.getMap());
=======
    SymbolsTableVisitor symbolsTable;
    symbolsTable.visit(tree);
    IRGenVisitor v(symbolsTable.getMap());
>>>>>>> bf6933f (added Affect instruction)
    v.visit(tree);

    return 0;
}
