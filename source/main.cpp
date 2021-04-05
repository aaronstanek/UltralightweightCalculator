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

int main() {
    std::cout << RNG_getInt(-5,5) << std::endl;
    std::cout << FTYPE_PRECISION << std::endl;
    std::cout << FLT_MIN_EXP << std::endl;
    std::cout << FLT_MAX_EXP << std::endl;
    std::cout << DBL_MIN_EXP << std::endl;
    std::cout << DBL_MAX_EXP << std::endl;
    std::cout << LDBL_MIN_EXP << std::endl;
    std::cout << LDBL_MAX_EXP << std::endl;
    return 0;
    try {

        bindEverything();

        std::string inputString;
        while (true) {
            try {
                // get input
                std::cin >> inputString;
                processingStartTime = time(NULL);
                prepareInput(inputString);
                std::vector<LexerToken> tokens;
                lexInput(tokens,inputString);
                for (long i = 0; i < tokens.size(); ++i) {
                    std::cout << (int)tokens[i];
                }
                std::cout << std::endl;
            }
            catch (UserAlert& e) {
                std::cout << e.message << std::endl;
            }
        }

    }
    catch (ManyTypeAccessError& e) {
        std::cout << std::endl << "Irrecoverable Memory Error" << std::endl;
    }
    catch (...) {
        std::cout << std::endl << "Irrecoverable Error" << std::endl;
    }
    std::cout << "Goodbye" << std::endl;
}