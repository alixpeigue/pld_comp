#pragma once

#include "ParserRuleContext.h"

class ErrorReporter {
public:
    virtual void report(const std::string &message,
                        antlr4::ParserRuleContext *ctx) const = 0;
};
