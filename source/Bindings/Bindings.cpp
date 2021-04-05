/**
 * @file Bindings.cpp
 * @author Aaron Stanek
*/
#include "Bindings.h"

inline void bindEverything() {
    bindAssign();
    bindConstants();
    bindConvert();
    bindArithmetic();
}