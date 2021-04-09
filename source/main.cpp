/**
 * @file main.cpp
 * @author Aaron Stanek
*/
#include "Globals/StaticAssert.h"
// also includes Globals.h

#include <iostream>

#include "ManyType/ManyType.h"
#include "Symbols/Symbols.h"
#include "LowLevelConvert/LowLevelConvert.h"
#include "Bindings/Bindings.h"
#include "Compute/EvaluateExpression.h"
#include "Lexer/Lexer.h"

#include "Globals/RNG.h"

void print_lexer_tokens(std::vector<LexerToken>& v) {
    for (long i = 0; i < v.size(); ++i) {
        LexerToken t = v[i];
        if (t == LexerToken::EOI) {
            std::cout << "e";
        }
        else if (t == LexerToken::White) {
            std::cout << "w";
        }
        else if (t == LexerToken::Number) {
            std::cout << "n";
        }
        else if (t == LexerToken::NumberMultiply) {
            std::cout << "N";
        }
        else if (t == LexerToken::Symbol) {
            std::cout << "s";
        }
        else if (t == LexerToken::SymbolMultiply) {
            std::cout << "S";
        }
        else if (t == LexerToken::Function) {
            std::cout << "f";
        }
        else if (t == LexerToken::FunctionMultiply) {
            std::cout << "F";
        }
        else if (t == LexerToken::Quote) {
            std::cout << "q";
        }
        else if (t == LexerToken::QuoteMultiply) {
            std::cout << "Q";
        }
        else if (t == LexerToken::Open) {
            std::cout << "(";
        }
        else if (t == LexerToken::OpenMultiply) {
            std::cout << "[";
        }
        else if (t == LexerToken::UOB) {
            std::cout << "-";
        }
        else if (t == LexerToken::UOBMultiply) {
            std::cout << "+";
        }
        else if (t == LexerToken::Close) {
            std::cout << ")";
        }
        else if (t == LexerToken::UOA) {
            std::cout << "!";
        }
        else if (t == LexerToken::BO) {
            std::cout << "*";
        }
        else {
            std::cout << "?";
        }
    }
    std::cout << std::endl << std::endl;
}

void test_lexer(const char* z) {
    std::string s = z;
    try {
        std::vector<LexerToken> lex;
        lexer(lex,s,maximumRecursionDepth);
        std::cout << z << std::endl;
        print_lexer_tokens(lex);
    }
    catch (UserAlert& e) {
        std::cout << e.message << std::endl;
    }
}

int main() {
    try {

        bindEverything();

        // test_lexer("hi");
        // test_lexer("");
        test_lexer("p+");

    }
    catch (ManyTypeAccessError& e) {
        std::cout << std::endl << "Irrecoverable Memory Error" << std::endl;
    }
    catch (...) {
        std::cout << std::endl << "Irrecoverable Error" << std::endl;
    }
    std::cout << "Goodbye" << std::endl;
}