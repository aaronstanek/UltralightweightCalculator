/**
 * @file LoadUserSymbol.h
 * @author Aaron Stanek
 * @brief Function for replacing
 * a user-defined language symbol
 * with its definition
*/
#pragma once
#include "../Globals/Globals.h"
#include "../ManyType/ManyType.h"
#include "../Symbols/Symbols.h"

void loadUserSymbol(ManyType&, const SymbolTableElement&, const mtvec&, long);