/**
 * @file Convert.cpp
 * @author Aaron Stanek
*/
#include "Bindings.h"
#include "../LowLevelConvert/LowLevelConvert.h"
#include "../Symbols/Symbols.h"
#include "../ManyType/ManyType.h"

void bool_implement(ManyType& ret, mtvec& arr, long recursionJuice) {
    // arr must have length 2
    convertToBool(arr[1]);
    ret = arr[1];
}

void int_implement(ManyType& ret, mtvec& arr, long recursionJuice) {
    // arr must have length 2
    convertToInt(arr[1]);
    ret = arr[1];
}

void float_implement(ManyType& ret, mtvec& arr, long recursionJuice) {
    // arr must have length 2
    convertToFtype(arr[1]);
    ret = arr[1];
}

inline void bindConvert() {
    std::string baseName;
    baseName = "bool";
    placeBuiltInSymbol(baseName,&bool_implement,1,0);
    baseName = "int";
    placeBuiltInSymbol(baseName,&int_implement,1,0);
    baseName = "float";
    placeBuiltInSymbol(baseName,&float_implement,1,0);
}