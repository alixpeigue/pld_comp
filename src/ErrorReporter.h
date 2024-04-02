/**
 * @file ErrorReporter.h
 * @author H4231
 * @brief classe abstraite de redaction des messages d'erreurs
 * @date 2024-04-02
 */
#pragma once

#include "ParserRuleContext.h"

/**
 * @brief Interface de rapporteur générique permettant de signaler des erreurs
 * 
 */
class ErrorReporter {
public:

    /**
     * @brief Signaler une erreur
     * 
     * @param message le message d'erreur
     * @param ctx le contexte d'où vient l'erreur
     */
    virtual void report(const std::string &message,
                        antlr4::ParserRuleContext *ctx) const = 0;
};
