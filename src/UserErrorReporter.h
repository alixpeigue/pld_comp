/**
 * @file UserErrorReporter.h
 * @author H4231
 * @brief Gestion d'erreur
 * @date 2024-04-02
 * 
 */

#pragma once

#include "ErrorReporter.h"
#include "ParserRuleContext.h"
#include <vector>

/**
 * @brief Rapporteur d'erreurs à l'utilisateur : fournit un massage d'erreur adapté à l'affichage pour les utilisateurs
 * 
 */
class UserErrorReporter : public ErrorReporter {
public:

    /**
     * @brief Construit le rapporteur
     * 
     * @param source le code source sur lequel le parsing est effectué
     */
    UserErrorReporter(const std::string &source);
    virtual void report(const std::string &message,
                        antlr4::ParserRuleContext *ctx) const override;

protected:
    std::vector<std::string> lines;
};
