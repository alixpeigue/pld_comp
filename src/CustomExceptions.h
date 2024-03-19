#pragma once
#include <exception>

class ReturnException : public std::exception {};
class ContinueException : public std::exception {};
class BreakException : public std::exception {};
