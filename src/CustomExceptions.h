/**
 * @file CustomExceptions.h
 * @author H4231
 * @brief Initilisation des classes d'erreur
 * @date 2024-04-02
 */

#pragma once
#include <exception>

class ReturnException : public std::exception {};
class ContinueException : public std::exception {};
class BreakException : public std::exception {};
