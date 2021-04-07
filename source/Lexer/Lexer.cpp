/**
 * @file Lexer.h
 * @author Aaron Stanek
*/
#include "Lexer.h"

#define EMPTY_INPUT -1
#define INVALID_INPUT -2
#define REACHED_RECURSION_LIMIT -3
#define END_OF_LINE 10

/// Marks characters with LexerToken::White
/// until it hits a nonwhitespace character.
/// @param output Where the output will go, the token
/// output[i] will be the lexical value of input[i]
/// @param input Where the input comes from, there will be
/// and end-of-line character at the end
/// @param index where to begin the search for whitespace characters
/// @return the index after the last whitespace marked
long lexWhite(LexerToken* output, const char* input, long index) noexcept {
    while (input[index] == 32 || input[index] == 9) {
        output[index] = LexerToken::White;
        ++index;
    }
    return index;
}

/// Marks characters with LexerToken::UOB
/// until it hits a character that cannot be
/// a unitary operator before expression.
/// @param output Where the output will go, the token
/// output[i] will be the lexical value of input[i]
/// @param input Where the input comes from, there will be
/// and end-of-line character at the end
/// @param index where to begin the search for unitary operators before expression
/// @return the index after the last unitary operator marked
long lexUOB(LexerToken* output, const char* input, long index) noexcept {
    while (input[index] == '+' || input[index] == '-' ||
        input[index] == '~' || input[index] == '!' || input[index] == '^') {
            output[index] = LexerToken::UOB;
            ++index;
        }
    return index;
}

/// Marks characters with LexerToken::UOA
/// until it hits a character that cannot be
/// a unitary operator after expression.
/// @param output Where the output will go, the token
/// output[i] will be the lexical value of input[i]
/// @param input Where the input comes from, there will be
/// and end-of-line character at the end
/// @param index where to begin the search for unitary operators after expression
/// @return the index after the last unitary operator marked
long lexUOA(LexerToken* output, const char* input, long index) noexcept {
    while (input[index] == '!') {
        if (input[index+1] == '=') {
            // it is != or !==
            if (input[index+2] != '=') {
                // it !=
                // which is a binary operator
                break;
            }
            // it is !==
            // which is UOA BO
        }
        else if (input[index+1] == '~') {
            if (input[index+2] == '=') {
                // it is !~=
                // which is a binary operator
                break;
            }
            // it is !~
            // which is UOA UOB/BO
            // UOB wouldn't be gramatical, but we
            // don't have to check that here
        }
        // either index+1 is not =
        // which means that this is UOA
        // or index+1 is = AND index+2 is =
        // which means that this is UOA
        output[index] = LexerToken::UOA;
        ++index;
    }
    return index;
}

/// Marks characters with LexerToken::BO
/// until it reaches the end of a binary operator.
/// @param output Where the output will go, the token
/// output[i] will be the lexical value of input[i]
/// @param input Where the input comes from, there will be
/// and end-of-line character at the end
/// @param index the suspected first index of the binary operator
/// @return the index after the binary operator,
/// or INVALID_INPUT if the input at index could not be interpreted as
/// a valid binary operator. If INVALID_INPUT is returned, then
/// output will not have been changed.
long lexBO(LexerToken* output, const char* input, long index) noexcept {
    int operatorLength = INVALID_INPUT;
    switch (input[index]) {
        case ',':
        case '%':
        case '&':
        case '|':
        case '^':
        case ';':
        // if any of these is the first character of
        // the operator, then we know that there is only
        // one character in the operator
        operatorLength = 1;
        break;

        case '+':
        if (input[index+1] == '*') {
            // cross product
            operatorLength = 2;
        }
        else {
            // addition
            operatorLength = 1;
        }
        break;

        case '-':
        if (input[index+1] == '>' || input[index+1] == '*') {
            // arrow or dot product
            operatorLength = 2;
        }
        else {
            // subtraction
            operatorLength = 1;
        }
        break;

        case '*':
        if (input[index+1] == '*') {
            // exponentiation
            operatorLength = 2;
        }
        else {
            // multiplication
            operatorLength = 1;
        }
        break;

        case '/':
        if (input[index+1] == '/' || input[index+1] == '*') {
            // floor division or matrix multiplication
            operatorLength = 2;
        }
        else {
            // float division
            operatorLength = 1;
        }
        break;

        case '=':
        case '>':
        case '<':
        if (input[index+1] == '=') {
            // == or >= or <=
            operatorLength = 2;
        }
        else {
            // = or > or <
            operatorLength = 1;
        }
        break;

        case '~':
        if (input[index+1] == '=') {
            // approximate equality comparison
            operatorLength = 2;
        }
        // else there is no binary operator with this form
        break;

        case '!':
        if (input[index+1] == '=') {
            // nonequality operator
            operatorLength = 2;
        }
        else if (input[index+1] == '~') {
            if (input[index+2] == '=') {
                operatorLength = 3;
            }
        }
        // else there is no binary operator with this form
        break;

        default:
        break;
    }
    if (operatorLength == INVALID_INPUT) {
        return INVALID_INPUT;
    }
    while (operatorLength) {
        output[index] = LexerToken::BO;
        ++index;
    }
    return index;
}

/// Marks characters with LexerToken::Number
/// until it reaches the end of a string of digits.
/// @param output Where the output will go, the token
/// output[i] will be the lexical value of input[i]
/// @param input Where the input comes from, there will be
/// and end-of-line character at the end
/// @param index the suspected first index of the string of digits
/// @return the index after the string of digits
/// or EMPTY_INPUT if the first character is not a numeral
/// or INVALID_INPUT if the last character is an underscore.
/// If EMPTY_INPUT is returned, output will not have been changed.
/// If INVALID_INPUT is returned, output will have been changed
/// up to the index of the invalid character.
long lexNumberDigits(LexerToken* output, const char* input, long index) noexcept {
    if (input[index] < '0' || input[index] > '9') {
        // we need to start with a numeral
        return EMPTY_INPUT;
    }
    while ((input[index] >= '0' && input[index] <= '9') || input[index] == '_') {
        // iterate over the digit string
        output[index] = LexerToken::Number;
        ++index;
    }
    // the digit string contains at least one character
    // we need to ensure that the last character
    // is not an underscore
    if (input[index-1] == '_') {
        return INVALID_INPUT;
    }
    else {
        return index;
    }
}

/// Marks characters with LexerToken::Number
/// until it reaches the end of a number.
/// @param output Where the output will go, the token
/// output[i] will be the lexical value of input[i]
/// @param input Where the input comes from, there will be
/// and end-of-line character at the end
/// @param index the suspected first index of the number
/// @return the index after the number,
/// or INVALID_INPUT if the input at index could not be interpreted
/// as a number. If INVALID_INPUT is returned, output may still
/// have been changed.
long lexNumber(LexerToken* output, const char* input, long index) noexcept {
    // we expect to start with a digit string
    long t = lexNumberDigits(output,input,index);
    bool hasValidDigits = false;
    if (t < 0) {
        if (t == INVALID_INPUT) {
            return INVALID_INPUT;
        }
        // EMPTY_INPUT is ok
        // we just don't update index
        // or hasValidDigits
    }
    else {
        index = t;
        hasValidDigits = true;
    }
    // we might have a decimal point here
    if (input[index] == '.') {
        output[index] = LexerToken::Number;
        ++index;
        t = lexNumberDigits(output,input,index);
        if (t < 0) {
            if (t == INVALID_INPUT) {
                return INVALID_INPUT;
            }
            // EMPTY_INPUT may or may not be ok
            // don't update index
            // or hasValidDigits
        }
        else {
            index = t;
            hasValidDigits = true;
        }
    }
    // if we don't have any digits yet
    // then this is not a valid number
    if (!hasValidDigits) {
        return INVALID_INPUT;
    }
    // we might have an exponent
    if (input[index] == 'e' || input[index] == 'E') {
        output[index] = LexerToken::Number;
        ++index;
        // the exponent might have a sign
        if (input[index] == '+' || input[index] == '-') {
            output[index] = LexerToken::Number;
            ++index;
        }
        // we must have digits here
        if (lexNumberDigits(output,input,index) < 0) {
            return INVALID_INPUT;
        }
    }
    // we have reached the end of the number
    // we should make sure that we are not in the middle
    // of another number
    // we can't have 0-9 or _ here
    // but we could have . e E
    // e and E could be the start of a symbol
    // . can't be anything valid
    if (input[index] == '.') {
        return INVALID_INPUT;
    }
    else {
        return index;
    }
}

/// Marks characters with LexerToken::Symbol
/// until it reaches the end of a symbol.
/// @param output Where the output will go, the token
/// output[i] will be the lexical value of input[i]
/// @param input Where the input comes from, there will be
/// and end-of-line character at the end
/// @param index the suspected first index of the symbol
/// @return the index after the symbol,
/// or INVALID_INPUT if the input at index could not be interpreted
/// as a symbol. If INVALID_INPUT is returned,
/// output will not have been changed.
long lexSymbol(LexerToken* output, const char* input, long index) noexcept {
    // check for initial character
    if ((input[index] >= 'a' && input[index] <= 'z') || (input[index] >= 'A' && input[index] <= 'Z')
        || (input[index] == '_') || (input[index] == '%')) {
            // it's a valid start to a symbol
            output[index] = LexerToken::Symbol;
            ++index;
        }
    else {
        return INVALID_INPUT;
    }
    // check for subsequent characters
    while ((input[index] >= 'a' && input[index] <= 'z') || (input[index] >= 'A' && input[index] <= 'Z')
        || (input[index] >= '0' && input[index] <= '9') || (input[index] == '_')) {
            output[index] = LexerToken::Symbol;
            ++index;
        }
    return index;
}

/// Marks characters with LexerToken::Quote
/// until it reaches the end of a string-literal.
/// @param output Where the output will go, the token
/// output[i] will be the lexical value of input[i]
/// @param input Where the input comes from, there will be
/// and end-of-line character at the end
/// @param index the suspected first index of the string-literal
/// @return the index after the string-literal,
/// or INVALID_INPUT if the input at index could not be interpreted
/// as a string-literal. If INVALID_INPUT is returned,
/// output may still have been changed.
long lexQuote(LexerToken* output, const char* input, long index) noexcept {
    const char firstChar = input[index];
    if (firstChar != '\"' && firstChar != '\'') {
        return INVALID_INPUT;
    }
    output[index] = LexerToken::Quote;
    ++index;
    bool escaped = false;
    while (true) {
        if (input[index] == END_OF_LINE) {
            // never valid
            return INVALID_INPUT;
        }
        output[index] = LexerToken::Quote;
        if (escaped) {
            escaped = false;
        }
        else if (input[index] == '\\') {
            escaped = true;
        }
        else if (input[index] == firstChar) {
            return index + 1;
        }
        else if ((input[index] < 32 || input[index] > '~') && input[index] != 9) {
            // invalid character
            return INVALID_INPUT;
        }
        ++index;
    }
}

/// Lexes input until it reaches ) or ] or end-of-line.
/// @param output Where the output will go, the token
/// output[i] will be the lexical value of input[i]
/// @param input Where the input comes from, there will be
/// and end-of-line character at the end
/// @param recursionJuice how many layers of recursion may be used by this operation
/// @param index the first index of the expression sequence
/// @return the index after the expression seqeunce or INVALID_INPUT
/// if there is a syntax error or EMPTY_INPUT if a termination character is reached immediately
/// or REACHED_RECURSION_LIMIT if the recurion limit is reached. If an error state
/// is returned, output may still have been changed.
long lexExpressionSequence(LexerToken* output, const char* input, long recursionJuice, long index) noexcept {
    if (recursionJuice <= 0) {
        return REACHED_RECURSION_LIMIT;
    }
    else {
        --recursionJuice;
    }
    long t;
    char checkChar;
    bool firstRound = true;
    bool implicitMultiply = false;
    while (true) {
        if (!firstRound) {
            // check for binary operator
            t = lexBO(output,input,index);
            if (t < 0) {
                implicitMultiply = true;
            }
            else {
                implicitMultiply = false;
                index = t;
            }
        }
        // then whitespace
        index = lexWhite(output,input,index);
        // then UOB
        index = lexUOB(output,input,index);
        // then simple expression
        if (input[index] == '(' || input[index] == '[') {
            checkChar = (input[index] == '(') ? ')' : ']';
            output[index] = (LexerToken)( (LexerTokenInt)(LexerToken::Open) |
                ((LexerTokenInt)(LexerToken::IsMultiply) * implicitMultiply) );
            ++index;
            t = lexExpressionSequence(output,input,recursionJuice,index);
            if (t == INVALID_INPUT || t == REACHED_RECURSION_LIMIT) {
                return t;
            }
            if (t != EMPTY_INPUT) {
                // if it is empty, that's ok,
                // just don't update index
                index = t;
            }
            if (input[index] != checkChar) {
                // we must close with the correct character
                return INVALID_INPUT;
            }
            output[index] = LexerToken::Close;
            ++index;
        }
        else {
            // it's not ( or [
            t = lexQuote(output,input,index);
            if (t != INVALID_INPUT) {
                // we have a valid string-literal
                if (implicitMultiply) {
                    output[index] = LexerToken::QuoteMultiply;
                }
                index = t;
            }
            else {
                t = lexSymbol(output,input,index);
                if (t != INVALID_INPUT) {
                    // we have a valid symbol
                    if (implicitMultiply) {
                        output[index] = LexerToken::SymbolMultiply;
                    }
                    index = t;
                }
                else {
                    t = lexNumber(output,input,index);
                    if (t != INVALID_INPUT) {
                        // we have a valid number
                        if (implicitMultiply) {
                            output[index] = LexerToken::NumberMultiply;
                        }
                        index = t;
                    }
                    else {
                        // we couldn't match it against any of the
                        // permitted patterns
                        if (firstRound) {
                            if (input[index] == ')' || input[index] == ']'
                                || input[index] == END_OF_LINE) {
                                    return EMPTY_INPUT;
                                }
                        }
                        // this isn't an empty input
                        // that's a problem
                        return INVALID_INPUT;
                    }
                }   
            }
        }
        // check for more unitary operators
        index = lexUOA(output,input,index);
        // and trailing whitespace
        index = lexWhite(output,input,index);
        // check for the end of the sequence
        if (input[index] == ')' || input[index] == ']' || input[index] == END_OF_LINE) {
            return index;
        }
        // this isn't the end
        firstRound = false;
    }
}

/// Assigns lexical meaning to each character
/// of the user input. Checks for invalid characters
/// and ensures that the input conforms to the language grammar.
/// @param output Where the output will go, will have the same length
/// as input, with 1-to-1 correspondence in the indicies
/// @param input Where the input comes from, an end-of-line character will
/// be appended to the end
/// @param recursionJuice how many layers of recursion may be used by this operation
void lexer(std::vector<LexerToken>& output, std::string& input, long recursionJuice) {
    input.push_back(END_OF_LINE);
    output.resize(input.size());
    // input has size > 0
    // the last character is END_OF_LINE
    // output has size > 0
    long index = 0;
    // call REX
    // index = REX(index)
    if (index < 0) {
        throw UserAlert(UserMessage::SyntaxError,nullptr);
    }
}