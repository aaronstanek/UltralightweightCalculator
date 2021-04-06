/**
 * @file Lexer.h
 * @author Aaron Stanek
*/
#include "Lexer.h"

#define EMPTY_INPUT -1
#define INVALID_INPUT -2

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
        input[index] == '~' || input[index] == '!') {
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
            // dot product
            operatorLength = 2;
        }
        else {
            // addition
            operatorLength = 1;
        }
        break;

        case '-':
        if (input[index+1] == '>' || input[index+1] == '*') {
            // arrow or cross product
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

/// Assigns lexical meaning to each character
/// of the user input. Checks for invalid characters
/// and ensures that the input conforms to the language grammar.
/// @param output Where the output will go, will have the same length
/// as input, with 1-to-1 correspondence in the indicies
/// @param input Where the input comes from, an end-of-line character will
/// be appended to the end
/// @param recursionJuice how many layers of recursion may be used by this operation
void lexer(std::vector<LexerToken>& output, std::string& input, long recursionJuice) {
    input.push_back(10);
    output.resize(input.size());
    // input has size > 0
    // the last character is 10
    // output has size > 0
    long index = 0;
    // call REX
    // index = REX(index)
    if (index < 0) {
        throw UserAlert(UserMessage::SyntaxError,nullptr);
    }
}