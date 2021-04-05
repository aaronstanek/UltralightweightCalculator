/**
 * @file Arithmetic.cpp
 * @author Aaron Stanek
*/
#include "Bindings.h"
#include "../LowLevelConvert/LowLevelConvert.h"
#include "../Symbols/Symbols.h"
#include "../ManyType/ManyType.h"

void add_implement(ManyType& ret, mtvec& arr, long recursionJuice) {
    // arr must have length 3
    convertToFtype(arr[1]);
    convertToFtype(arr[2]);
    ftype output = arr[1].getFtype() + arr[2].getFtype();
    if (std::isnan(output)) {
        throw UserAlert(UserMessage::NanError,"add");
    }
    if (std::isinf(output)) {
        throw UserAlert(UserMessage::InfinityError,"add");
    }
    ret.putFtype(output);
}

void sub_implement(ManyType& ret, mtvec& arr, long recursionJuice) {
    // arr must have length 3
    convertToFtype(arr[1]);
    convertToFtype(arr[2]);
    ftype output = arr[1].getFtype() - arr[2].getFtype();
    if (std::isnan(output)) {
        throw UserAlert(UserMessage::NanError,"sub");
    }
    if (std::isinf(output)) {
        throw UserAlert(UserMessage::InfinityError,"sub");
    }
    ret.putFtype(output);
}

inline void bindArithmetic() {
    std::string baseName;
    baseName = "add";
    placeBuiltInSymbol(baseName,&add_implement,2,0);
    baseName = "sub";
    placeBuiltInSymbol(baseName,&sub_implement,2,0);
}