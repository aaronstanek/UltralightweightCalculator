#include "Lexer.h"
#include "LexerSubFunctions.h"

void lexInputNumeric(std::vector<LexerToken>& tokens, const std::string& s, LexerState& state, const long i) {
    if (static_cast<LexerStateInt>(state) & static_cast<LexerStateInt>(LexerState::Prohibit_Numeric)) {
        throw UserAlert(UserMessage::SyntaxError,"Invalid Token Before Number");
    }
    tokens[i] = LexerToken::Number;
    state = LexerState::Number;
}

void lexInputAlpha(std::vector<LexerToken>& tokens, const std::string& s, LexerState& state, const long i) {
    if (static_cast<LexerStateInt>(state) & static_cast<LexerStateInt>(LexerState::Prohibit_AlphaOpen)) {
        throw UserAlert(UserMessage::SyntaxError,"Invalid Token Before Symbol");
    }
    tokens[i] = LexerToken::SymbolName;
    state = LexerState::SymbolName;
}

void lexInputQuote(std::vector<LexerToken>& tokens, const std::string& s, LexerState& state, const long i) {
    if (state == LexerState::Quote) {
        tokens[i] = LexerToken::Quote;
        if (s[i] == '"') {
            state = LexerState::QuoteEnd;
        }
        else if (s[i] == '\\') {
            state = LexerState::QuoteEscape;
        }
        // if both are false, then
        // we do not change the state
    }
    else if (static_cast<LexerStateInt>(state) & static_cast<LexerStateInt>(LexerState::Prohibit_Quote)) {
        throw UserAlert(UserMessage::SyntaxError,"Invalid Token Before String Literal");
    }
    else {
        // state is not Quote
        // this is the start of the
        // string-literal
        tokens[i] = LexerToken::Quote;
        state = LexerState::Quote;
    }
}

void lexInputOpenContainer(std::vector<LexerToken>& tokens, const std::string& s, LexerState& state, const long i) {
    if (static_cast<LexerStateInt>(state) & static_cast<LexerStateInt>(LexerState::Prohibit_AlphaOpen)) {
        throw UserAlert(UserMessage::SyntaxError,"Invalid Token Before ( or [");
    }
    tokens[i] = LexerToken::OpenContainer;
    state = LexerState::OpenContainer;
}

void lexInputCloseContainer(std::vector<LexerToken>& tokens, const std::string& s, LexerState& state, const long i) {
    if (static_cast<LexerStateInt>(state) & static_cast<LexerStateInt>(LexerState::Prohibit_Close)) {
        throw UserAlert(UserMessage::SyntaxError,"Invalid Token Before ) or ]");
    }
    tokens[i] = LexerToken::CloseContainer;
    state = LexerState::CloseContainer;
}

void lexInputOperator(std::vector<LexerToken>& tokens, const std::string& s, LexerState& state, const long i) {
    // WARNING
    // there are two goto statements used to avoid code duplication
    // and repeat condition checking
    // each has extactly one entry point, and one exit point
    // explanations are provided to assist with code review
    const char c = s[i];
    if (c == '!') {
        // this is a special case
        // we need to check for unitary operator after expression
        if (state == LexerState::QuoteEnd) {
            // we can't place ! after a quote
            throw UserAlert(UserMessage::SyntaxError,"! Cannot Follow String-Literal");
        }
        if (static_cast<LexerStateInt>(state) & static_cast<LexerStateInt>(LexerState::Mask_AllowUnitaryAfter)) {
            // we can place ! as a unitary operator after expression
            tokens[i] = LexerToken::UnitaryOperatorAfter;
            state = LexerState::UnitaryOperator;
            return;
        }
        // ! can be placed before or after an expression
        // we just determined that this ! cannot be placed after
        // the previous expression, but we don't know yet if
        // it can be attached to the following expression
        // there is a block below for checking that, so we will
        // use that block instead of duplicating it,
        // or duplicating the c == '!' condition
        goto checkUnitaryOperatorPlacementBefore;
    }
    if (c == '+' || c == '-') {
        // check for unitary operator placement before expression
        checkUnitaryOperatorPlacementBefore:
        // we have only c = + - !
        // if c == !, then the ! cannot be attached to the previous expression
        if (state == LexerState::UnitaryOperator) {
            // can't have two unitary operators in a row
            throw UserAlert(UserMessage::SyntaxError,"Unitary Operator Cannot Follow Unitary Operator");
        }
        if (static_cast<LexerStateInt>(state) & static_cast<LexerStateInt>(LexerState::Mask_StartOfExpression)) {
            // we can place a unitary operator here
            tokens[i] = LexerToken::UnitaryOperatorBefore;
            state = LexerState::UnitaryOperator;
            return;
        }
        // we can't place a unitary operator here
    }
    // we do not have a unitary operator
    // we also will not have c = '!'
    // because we have exhausted all the LexerState options for it
    if (static_cast<LexerStateInt>(state) & static_cast<LexerStateInt>(LexerState::Prohibit_BinaryOperator)) {
        throw UserAlert(UserMessage::SyntaxError,"Invalid Token Before Binary Operator");
    }
    if (state == LexerState::UnitaryOperator) {
        // we know that s[i-1] and tokens[i-1]
        // will exist and will be defined
        if (c == '=' && s[i-1] == '!') {
            // this is !=
            if (tokens[i-1] == LexerToken::UnitaryOperatorBefore) {
                throw UserAlert(UserMessage::SyntaxError,"Invalid Token Before !=");
                // before the != is StartOfInput, OpenContainer or a binary operator
            }
            // we have a valid binary operator !=
            // we can use the code block below for
            // dealing with the case of a 2-character
            // binary operator
            goto placeBinaryOperator;
        }
    }
    if (state != LexerState::BinaryOperator1) {
        tokens[i] = LexerToken::BinaryOperator1of1;
        state = LexerState::BinaryOperator1;
    }
    else {
        // state = BinaryOperator1
        placeBinaryOperator:
        // we have a 2-character binary operator
        // token[i-1] will exist and will be defined
        // either because state == BinaryOperator1
        // or because state == UnitaryOperator
        tokens[i-1] = LexerToken::BinaryOperator1of2;
        tokens[i] = LexerToken::BinaryOperator2of2;
        state = LexerState::BinaryOperator2;
    }
}