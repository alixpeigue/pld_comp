/**
 * @file ErrorReporter.h
 * @author H4231
 * @brief classe abstraite de redaction des messages d'erreurs
 * @date 2024-04-02
 */
#pragma once

#include "ParserRuleContext.h"

class ErrorReporter {
public:
    virtual void report(const std::string &message,
                        antlr4::ParserRuleContext *ctx) const = 0;
};
