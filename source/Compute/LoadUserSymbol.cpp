/**
 * @file LoadUserSymbol.cpp
 * @author Aaron Stanek
*/
#include "LoadUserSymbol.h"
#include "../LowLevelConvert/LowLevelConvert.h"
#include <unordered_map>

void resolveLocalVaraibleNames(ManyType& x, std::unordered_map<std::string,const ManyType*>& localVars, long recursionJuice) {
    if (recursionJuice <= 0) {
        throw UserAlert(UserMessage::MaximumRecursionDepthReached,nullptr);
    }
    else {
        --recursionJuice;
    }
    // resolve all the local variable names in x, and elements pointed to by x
    long logicalRecursionJuice = maximumLogicalRecursionDepth;
    // the loop runs exactly once
    // unless we have a local variable
    // we can replace local variables with other local variables
    // until we hit maximumLogicalRecursionDepth
    while (true) {
        if (x.type() == ManyTypeLabel::StructureString) {
            // it's a symbol name
            // check if it is a local variable name
            const auto it = localVars.find(x.getStructureString());
            if (it != localVars.end()) {
                // it's a local variable
                // check recursionJuice before continuing
                if (logicalRecursionJuice <= 0) {
                    throw UserAlert(UserMessage::MaximumLogicalRecursionDepthReached,nullptr);
                }
                else {
                    --logicalRecursionJuice;
                }
                // make sure that we are not running overtime
                checkProcessingTime();
                // now do the replacement
                x.makeCopyFrom(*(it->second),recursionJuice);
                // the value will replace the (localVariable) string
                // but the value might contain other
                // local variables
                // if so, we might need to replace it again
                continue;
            }
        }
        else if (static_cast<ManyTypeLabelInt>(x.type()) & static_cast<ManyTypeLabelInt>(ManyTypeLabel::Vector)) {
            checkProcessingTime();
            // it's a Vector
            // forward the processing to all children resursively
            // the first element is the function name or data type, ignore it
            mtvec& vec = (x.type() == ManyTypeLabel::StructureVector) ? x.getStructureVector() : x.getDataVector();
            // check to see if the first argument
            // is qualified for replacement
            if (vec[0].getStructureString()[0] == '%') {
                // we should consider replacing it
                resolveLocalVaraibleNames(vec[0],localVars,recursionJuice);
                // we require that the first element of a StructureVector
                // be a StructureString
                // if the user tried to do something else, we have a problem
                convertToStructureString(vec[0]);
                // we have a valid StructureString as the first
                // element of our StructureVector
            }
            for (int_fast32_t i = 1; i < vec.size(); ++i) {
                resolveLocalVaraibleNames(vec[i],localVars,recursionJuice);
            }
        }
        break;
    }
}

void loadUserSymbol(ManyType& ret, const SymbolTableElement& source, const mtvec& callVec, long recursionJuice) {
    if (recursionJuice <= 0) {
        throw UserAlert(UserMessage::MaximumRecursionDepthReached,nullptr);
    }
    else {
        --recursionJuice;
    }
    // copies the source to ret
    // resolves any local variable names
    // we know that source is not built-in
    // and that it has a valid value.mt field
    if (source.value.mt.type() != ManyTypeLabel::StructureVector) {
        // it's not function-like
        // we can just copy it and be done
        ret.makeCopyFrom(source.value.mt,recursionJuice);
        // done!
    }
    else {
        // it's function-like, we have to copy only part
        // and do local variable name lookup
        const mtvec& sourceVec = source.value.mt.getStructureVector();
        // sourceVec[0] is the expression that needs to copied,
        // sourceVec[>0] are the local variable names
        ret.makeCopyFrom(sourceVec[0],recursionJuice);
        // we have copied the expression into ret
        // now resolve varnames
        if (sourceVec.size() == 1) {
            // there are no local variables
            // only an expression
            return;
        }
        std::unordered_map<std::string,const ManyType*> localVars;
        localVars.reserve(sourceVec.size() - 1);
        // the sourceVec[n] is the name of the value
        // stored at callVec[n] for n > 0
        // all the values in callVec will have been resolved already
        // sourceVec and callVec must have the same length
        // because we got to sourceVec using the size of callVec
        // and we know that all the elements of sourceVec[>0]
        // are strings because we checked them on the way in
        for (int_fast32_t i = sourceVec.size() - 1; i >= 1; --i) {
            localVars[sourceVec[i].getStructureString()] = &callVec[i];
        }
        // we now have the mapping from local variable names
        // to their values
        resolveLocalVaraibleNames(ret,localVars,recursionJuice);
        // all the local variable names have been resolved
        // we are done now
    }
}