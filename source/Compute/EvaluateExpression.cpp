/**
 * @file EvaluateExpression.cpp
 * @author Aaron Stanek
*/
#include "EvaluateExpression.h"
#include "LoadUserSymbol.h"
#include "../Symbols/Symbols.h"
#include "../Bindings/Bindings.h"

void evaluateExpression(ManyType& x, long recursionJuice) {
    if (recursionJuice <= 0) {
        throw UserAlert(UserMessage::MaximumRecursionDepthReached,nullptr);
    }
    else {
        --recursionJuice;
    }
    long logicalRecursionJuice = maximumLogicalRecursionDepth;
    // we will do everything we can
    // to convert x to a DataExpression
    while (true) {
        // cycle this loop until we have
        // a valid DataExpression
        if (static_cast<ManyTypeLabelInt>(x.type()) & static_cast<ManyTypeLabelInt>(ManyTypeLabel::DataExpression)) {
            // we are done
            return;
        }
        // check recursionJuice before continuing
        if (logicalRecursionJuice <= 0) {
            throw UserAlert(UserMessage::MaximumLogicalRecursionDepthReached,nullptr);
        }
        else {
            --logicalRecursionJuice;
        }
        // make sure that we are not running overtime
        checkProcessingTime();
        // we have some kind of function
        // or variable to deal with
        if (x.type() == ManyTypeLabel::StructureString) {
            // wrap this in a function call
            x.wrapInVector();
        }
        // x must now be a vector / function call at this point
        mtvec& callVec = x.getStructureVector();
        if (callVec.size() > MAX_ARGS_USED) {
            // we need to be able to fit the
            // number of arguments in a 32 bit integer
            throw UserAlert(UserMessage::TooManyArguments,"In Call");
        }
        const SymbolTableElement& symbol = readSymbol(
            callVec[0].getStructureString(),
            (callVec.size() > MAX_ARGS_DEF) ? (char)(-1) : (char)(callVec.size()-1)
            );
        // we need to look at the delayMask before evaluating its arguments
        for (int_fast32_t i = 1; i < callVec.size(); ++i) {
            if (i <= 8) {
                if ( (symbol.delayMask >> (i-1) ) & 0x01 ) {
                    // take the i-1 th bit of symbol.delayMask (from LSB)
                    // if it is 1, then that means that we should delay
                    // evaluation of this argument
                    continue;
                }
            }
            // don't delay evaluation of this argument
            evaluateExpression(callVec[i],recursionJuice);
        }
        // now we can call the function
        ManyType ret;
        if (symbol.builtIn) {
            // built-in symbol
            symbol.value.func(ret,callVec,recursionJuice);
        }
        else {
            // user-defined symbol
            loadUserSymbol(ret,symbol,callVec,recursionJuice);
        }
        x = ret;
        // after this, we go back to the top of the loop
    }
}