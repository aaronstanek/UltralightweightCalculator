#include "Lexer.h"
#include "LexerSubFunctions.h"

LexerSubFunc getLexerSubFunction(const LexerState state, const char c) {
    // we know that c is a valid character
    // (or just that it's in the range 32 to 126 if it's in a string-literal)
    // E e _ may be numeric or alphabetic depending on the context
    // 0-9 may be numeric or alphabetic depending on the context
    // % may be operator or alphabetic depending on the context
    // anything may be a quote depending on the context
    if (static_cast<LexerStateInt>(state) & static_cast<LexerStateInt>(LexerState::Mask_InQuote)) {
        return &lexInputQuote;
    }
    switch (c) {
        case 'E':
        case 'e':
        case '_':
        // these are ususally alphabetic
        // but they might appear in a number
        return (state == LexerState::Number) ? &lexInputNumeric : &lexInputAlpha;

        case '"':
        return &lexInputQuote;

        case '(':
        case '[':
        return &lexInputOpenContainer;

        case ')':
        case ']':
        return &lexInputCloseContainer;

        case '.':
        return &lexInputNumeric;

        case '%':
        // might be mod operator
        // or the start of a delay symbol
        return static_cast<LexerStateInt>(state) &
                static_cast<LexerStateInt>(LexerState::Mask_StartOfExpression)
                ? &lexInputAlpha : &lexInputOperator;

        default:
        break;
    }
    if (c >= '0' && c <= '9') {
        // numerals might appear in a symbol name
        return (state == LexerState::SymbolName) ? &lexInputAlpha : &lexInputNumeric;
    }
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
        // it must be alphabetic
        return &lexInputAlpha;
    }
    // it wasn't any of the other categories
    return &lexInputOperator;
}

void lexInput(std::vector<LexerToken>& tokens, const std::string& s) {
    tokens.resize(s.size());
    LexerState state = LexerState::StartOfInput;
    for (long i = 0; i < s.size(); ++i) {
        if (i & 0x0F == 0) {
            checkProcessingTime();
            // check to see if we are running
            // overtime every 16 characters
        }
        // select a subfunction depending on
        // the character class of s[i]
        // then immediately call the subfunction
        getLexerSubFunction(state,s[i])(tokens,s,state,i);
    }
}