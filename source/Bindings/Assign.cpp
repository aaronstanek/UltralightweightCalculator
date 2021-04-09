/**
 * @file Assign.cpp
 * @author Aaron Stanek
*/
#include "Bindings.h"
#include "../ManyType/ManyType.h"
#include "../Symbols/Symbols.h"
#include "../LowLevelConvert/LowLevelConvert.h"

void assign_implement(ManyType& ret, mtvec& arr, long recursionJuice) {
    // arr has length 3
    // arr[1] has type string and is the name of the
    // symbol being assigned (delayMask)
    // arr[2] is the value being assigned to it
    if (arr[1].type() != ManyTypeLabel::StructureString) {
        throw UserAlert(UserMessage::UnexpectedType,"assign");
    }
    // arr[2] will be a DataExpression
    // it's a constant, so we know that argCount
    // must be zero
    // similarly, delayMask must be 0
    ret.makeCopyFrom(arr[2],recursionJuice);
    placeUserSymbol(arr[1].getStructureString(),arr[2],0,0);
}

void arrow_implement(ManyType& ret, mtvec& arr, long recursionJuice) {
    // arr has length 3
    // arr[1] is some kind of function call (or symbol name) (delayMask)
    // arr[2] has some kind of expression (delayMask)
    // if arr[1] is a string, convert it to a function call
    if (arr[1].type() == ManyTypeLabel::StructureString) {
        // wrap it into a function-like
        arr[1].wrapInVector();
    }
    ManyType name;
    ManyType callObject;
    unsigned char delayMask = 0;
    if (arr[1].type() == ManyTypeLabel::StructureVector) {
        // it's a function call
        mtvec& avec = arr[1].getStructureVector();
        // check number of arguments
        if (avec.size() > MAX_ARGS_DEF) {
            throw UserAlert(UserMessage::TooManyArguments,"In Definition");
        }
        // make sure that they're all strings
        // we expect a function name, and local variable names
        for (long i = 0; i < avec.size(); ++i) {
            if (avec[i].type() != ManyTypeLabel::StructureString) {
                throw UserAlert(UserMessage::UnexpectedType,"arrow");
            }
            // safe to assume that StructureString
            // has length > 0
            if (avec[i].getStructureString()[0] == '%') {
                // this is a delayArgument
                if (i < 1 || i > 8) {
                    // this is not one of the qualified arguments
                    throw UserAlert(UserMessage::SyntaxError,"% Used Outside First 8 Arguments");
                }
                else {
                    // this is a qualified argument
                    delayMask |= 1 << (i-1);
                }
            }
        }
        // all the elements of avec are StructureStrings
        // now place the first element of avec
        // into the name variable
        // and place the expression (arr[2])
        // in its place
        name = avec[0];
        avec[0] = arr[2];
        // then move the [expression,varnames...]
        // to the callObject
        callObject = arr[1];
        // arr[1] and arr[2] are both None
        // name and callObject are set
    }
    else {
        throw UserAlert(UserMessage::UnexpectedType,"arrow");
    }
    // now place it
    // callObject.getVector().size()-1 is the argCount for this symbol
    placeUserSymbol(name.getStructureString(),callObject,callObject.getStructureVector().size()-1,delayMask);
    ret.putNone();
}

void remove1_implement(ManyType& ret, mtvec& arr, long recursionJuice) {
    // arr has length 2
    // arr[1] is the symbol baseName to be deleted (delayMask)
    // returns the number of items deleted
    if (arr[1].type() != ManyTypeLabel::StructureString) {
        throw UserAlert(UserMessage::UnexpectedType,"remove");
    }
    // now get the list of symbols with that baseName
    // that can be deleted
    const std::string& baseName = arr[1].getStructureString();
    std::vector<char> deleteList;
    removeUserSymbolList(baseName, deleteList);
    // the list is now populated with all those things that
    // can be deleted
    long deleteCount = 0;
    // deleteCount measures how many things were actually deleted
    // in theory this should be the same as the size of
    // deleteList, but removeUserSymbol will have the
    // authoritative answer
    for (long i = 0; i < deleteList.size(); ++i) {
        if (removeUserSymbol(baseName,deleteList[i])) {
            ++deleteCount;
        }
    }
    // now return the number of elements
    // that were actually deleted
    ret.putInt(deleteCount);
}

void remove2_implement(ManyType& ret, mtvec& arr, long recursionJuice) {
    // arr has length 3
    // arr[1] is the symbol baseName to be deleted (delayMask)
    // arr[2] is the overload to be deleted (int)
    // returns true if symbol was deleted
    // returns false otherwise
    if (arr[1].type() != ManyTypeLabel::StructureString) {
        throw UserAlert(UserMessage::UnexpectedType,"remove");
    }
    convertToInt(arr[2]);
    // test to make sure that this
    // int can fit inside 8 bits
    long argCount = arr[2].getInt();
    if (argCount < 0 || argCount >= MAX_ARGS_DEF) {
        // one less than MAX_ARGS_DEF because we
        // don't consider the name
        throw UserAlert(UserMessage::DomainError,"remove");
    }
    // it can fit
    bool output = removeUserSymbol(arr[1].getStructureString(),argCount);
    // and now return if the deletion was successful
    ret.putBool(output);
}

void bindAssign() {
    std::string baseName;
    baseName = "assign";
    placeBuiltInSymbol(baseName,&assign_implement,2,0x01);
    baseName = "arrow";
    placeBuiltInSymbol(baseName,&arrow_implement,2,0x03);
    baseName = "remove";
    placeBuiltInSymbol(baseName,&remove1_implement,1,0x01);
    placeBuiltInSymbol(baseName,&remove2_implement,2,0x01);
}