#pragma once

#include "ErrorReporter.h"
#include "ParserRuleContext.h"
#include <vector>
class UserErrorReporter : public ErrorReporter {
public:
    UserErrorReporter(const std::string &source);
    virtual void report(const std::string &message,
                        antlr4::ParserRuleContext *ctx) const override;

protected:
    std::vector<std::string> lines;
};
