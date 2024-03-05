#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

#include "antlr4-runtime.h"
#include "ifccBaseVisitor.h"
#include "ifccLexer.h"
#include "ifccParser.h"

#include "CodeGenVisitor.h"

using namespace antlr4;
using namespace std;

int main(int argn, const char **argv) {
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

  ifccLexer lexer(&input);
  CommonTokenStream tokens(&lexer);

  tokens.fill();

  ifccParser parser(&tokens);
  tree::ParseTree *tree = parser.axiom();

  if (parser.getNumberOfSyntaxErrors() != 0) {
    cerr << "error: syntax error during parsing" << endl;
    exit(1);
  }

  CodeGenVisitor v;
  v.visit(tree);

  return 0;
}
