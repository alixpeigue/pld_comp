/**
 * @file UserErrorReporter.cpp
 * @author H4231
 * @brief Gestion d'erreur
 * @date 2024-04-02
 * 
 */


#include "ParserRuleContext.h"
#include "Token.h"
#include "UserErrorReporter.h"
#include <iostream>
#include <sstream>
#include <string>

/**
 * @brief Construct a new User Error Reporter:: User Error Reporter object
 * 
 * @param source 
 */

UserErrorReporter::UserErrorReporter(const std::string &source) {
    std::istringstream iss(source);
    std::string line;
    while (std::getline(iss, line)) {
        this->lines.push_back(line);
    }
}

/**
 * @brief redaction du message d'erreur 
 * 
 * @param message message levee lors de l'erreur 
 * @param ctx contexte antlr4 du noeud
 */

void UserErrorReporter::report(const std::string &message,
                               antlr4::ParserRuleContext *ctx) const {
    std::cerr << "Error at " << ctx->start->getLine() << ":"
              << ctx->start->getCharPositionInLine() << " : " << message
              << "\n";
    std::cerr << this->lines[ctx->start->getLine() - 1] << "\n";
    size_t endOfSymbol =
        ctx->stop->getCharPositionInLine() + ctx->stop->getText().size();
    for (uint32_t i = 0; i < endOfSymbol; ++i) {
        if (i < ctx->start->getCharPositionInLine()) {
            std::cerr << " ";
        } else {
            std::cerr << "^";
        }
    }
    std::cerr << "\n";
}
