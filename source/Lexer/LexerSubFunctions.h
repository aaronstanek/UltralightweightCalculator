#pragma once
#include "../Globals/Globals.h"
#include "Lexer.h"

typedef void (*LexerSubFunc)(std::vector<LexerToken>&, const std::string&, LexerState&, const long);

void lexInputNumeric(std::vector<LexerToken>&, const std::string&, LexerState&, const long);

void lexInputAlpha(std::vector<LexerToken>&, const std::string&, LexerState&, const long);

void lexInputQuote(std::vector<LexerToken>&, const std::string&, LexerState&, const long);

void lexInputOpenContainer(std::vector<LexerToken>&, const std::string&, LexerState&, const long);

void lexInputCloseContainer(std::vector<LexerToken>&, const std::string&, LexerState&, const long);

void lexInputOperator(std::vector<LexerToken>&, const std::string&, LexerState&, const long);