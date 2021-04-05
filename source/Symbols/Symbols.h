/**
 * @file Symbols.h
 * @author Aaron Stanek
 * @brief Structs and functions
 * for mapping language symbol names
 * to their values, including built-in
 * function lookup
*/
#pragma once
#include "../Globals/Globals.h"
#include "../ManyType/ManyType.h"

#include <unordered_map>

typedef void (*boundFunction)(ManyType&,mtvec&,long);
// the first argument is the return value
// the second is a vector of [function_name,args...]

union SymbolTableElementUnion {
    boundFunction func;
    ManyType mt;
    inline SymbolTableElementUnion() noexcept {};
    inline ~SymbolTableElementUnion() noexcept {};
};

struct SymbolTableElement {
    SymbolTableElementUnion value;
    unsigned char delayMask;
    // the nth bit (from LSB) marks if the
    // nth argument should be left unevaulated when
    // placed into the function
    // 0 for all user functions
    // note that the function name DOES NOT COUNT as the first argument
    bool builtIn;
    // builtIn means that value holds a function pointer
    // not builtIn means that value holds a ManyType
    inline SymbolTableElement() noexcept {
        builtIn = true;
        // this means that value is a function pointer
        // with only trivial constructor and destructor
    };
    void setAsUserSymbol() noexcept;
    ~SymbolTableElement() noexcept;
};

void placeBuiltInSymbol(const std::string&, const boundFunction, const char, const unsigned char);

void placeUserSymbol(const std::string&, const ManyType&, const char, const unsigned char);

bool removeUserSymbol(const std::string&, const char);

void removeUserSymbolList(const std::string&, std::vector<char>&);

const SymbolTableElement& readSymbol(const std::string&, const char);