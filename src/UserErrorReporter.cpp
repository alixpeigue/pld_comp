#include "ParserRuleContext.h"
#include "Token.h"
#include "UserErrorReporter.h"
#include <iostream>
#include <sstream>
#include <string>

UserErrorReporter::UserErrorReporter(const std::string &source) {
    std::istringstream iss(source);
    std::string line;
    while (std::getline(iss, line)) {
        this->lines.push_back(line);
    }
}

void UserErrorReporter::report(const std::string &message,
                               antlr4::ParserRuleContext *ctx) const {
    std::cerr << "Error at " << ctx->start->getLine() << ":"
              << ctx->start->getCharPositionInLine() << " : " << message
              << "\n";
    std::cerr << this->lines[ctx->start->getLine() - 1] << "\n";
    size_t endOfSymbol =
        ctx->stop->getCharPositionInLine() + ctx->stop->getText().size();
    for (int i = 0; i < endOfSymbol; ++i) {
        if (i < ctx->start->getCharPositionInLine()) {
            std::cerr << " ";
        } else {
            std::cerr << "^";
        }
    }
    std::cerr << "\n";
}
