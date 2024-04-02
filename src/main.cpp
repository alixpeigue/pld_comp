#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>

#include "IRGenVisitor.h"
#include "IRx86Visitor.h"
#include "RV64Visitor.h"
#include "UserErrorReporter.h"
#include "ValidationVisitor.h"
#include "antlr4-runtime.h"
#include "ifccBaseVisitor.h"
#include "ifccLexer.h"
#include "ifccParser.h"

#include "IRBaseVisitor.h"
#include "Scope.h"
#include "ir.h"

#include <unistd.h>

using namespace antlr4;
using namespace std;

int main(int argn, char **argv) {

    // Gestion de l'input
    // if (argn == 2) {
    //     ifstream lecture(argv[1]);
    //     if (!lecture.good()) {
    //         cerr << "error: cannot read file: " << argv[1] << endl;
    //         exit(1);
    //     }
    //     in << lecture.rdbuf();
    // } else {
    //     cerr << "usage: ifcc path/to/file.c" << endl;
    //     exit(1);
    // }
    int opt;
    std::string arch = "x86-64";
    while ((opt = getopt(argn, argv, "t::")) != -1) {
        switch (opt) {
        case 't':
            arch = optarg;
            break;
        default:
            std::cerr << "usage :" << argv[0] << "[-t target] file\n";
            exit(1);
        }
    }

    // Handling mandatory argument
    if (optind >= argn) {
        std::cerr << "usage :" << argv[0] << "[-t target] file\n";
        exit(1);
    }

    std::string filename = argv[optind];

    stringstream in;
    ifstream lecture(filename);
    if (!lecture.good()) {
        cerr << "error: cannot read file: " << filename << endl;
        exit(1);
    }
    in << lecture.rdbuf();

    std::unique_ptr<IRBaseVisitor> codeGenVisitor;
    if (arch == "rv64")
        codeGenVisitor = std::make_unique<RV64Visitor>();
    else if (arch == "x86-64")
        codeGenVisitor = std::make_unique<IRx86Visitor>();
    else {
        std::cerr << "Unknown architecture '" << arch << "'";
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

    // std::cout << ".intel_syntax noprefix\n";
    for (const auto &i : ir) {
        i->visitBlocks(*codeGenVisitor);
    }
    return 0;
}
