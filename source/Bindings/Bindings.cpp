/**
 * @file Bindings.cpp
 * @author Aaron Stanek
*/
#include "Bindings.h"

void bindEverything() {
    bindAssign();
    bindConstants();
    bindConvert();
    bindArithmetic();
}