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

/// A type suitable to reference all of the language
/// built-in functions.
/// The first argument is the return value.
/// The second, a vector of [function_name,args...].
/// The last is how many layer of recursion to allow
/// in the computation.
typedef void (*boundFunction)(ManyType&,mtvec&,long);

/// A type to hold either a pointer to a language
/// built-in function, or a user-defined ManyType object.
union SymbolTableElementUnion {
    /// A pointer to a language built-in function.
    boundFunction func;
    /// A user-defined ManyType object.
    /// @warning must be constructed before use.
    /// @warning must be destructed before deletion.
    ManyType mt;
    /// Trivial constructor.
    /// @warning Does not construct a ManyType
    /// object, even if we expect mt to be defined.
    inline SymbolTableElementUnion() noexcept {};
    /// Trivial destructor.
    /// @warning Does not call ManyType destructor,
    /// even if mt is defined.
    inline ~SymbolTableElementUnion() noexcept {};
};

/// The value type of the table of symbols.
/// Uses SymbolTableElementUnion to hold
/// either a pointer to a built-in function,
/// or a user-defined ManyType object.
struct SymbolTableElement {
    /// The value held by this element.
    SymbolTableElementUnion value;
    /// The nth bit (from LSB) marks if the
    /// nth argument should be left unevaulated when
    /// passed to the relevant function.
    /// Note that the function name DOES NOT COUNT as the first argument.
    unsigned char delayMask;
    /// Indicates if this object holds a pointer to a language built-in symbol.
    /// If true, value.func is defined.
    /// If false, value.mt is defined.
    bool builtIn;
    /// Default constructor.
    /// Sets builtIn to true, but does not define value.func
    /// @warning the caller must either define value.func
    /// or call setAsUserSymbol()
    inline SymbolTableElement() noexcept {
        builtIn = true;
    };
    void setAsUserSymbol() noexcept;
    ~SymbolTableElement() noexcept;
};

void placeBuiltInSymbol(const std::string&, const boundFunction, const char, const unsigned char);

void placeUserSymbol(const std::string&, const ManyType&, const char, const unsigned char);

bool removeUserSymbol(const std::string&, const char);

void removeUserSymbolList(const std::string&, std::vector<char>&);

const SymbolTableElement& readSymbol(const std::string&, const char);