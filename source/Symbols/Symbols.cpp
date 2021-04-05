/**
 * @file Symbols.cpp
 * @author Aaron Stanek
*/
#include "Symbols.h"

/// A table to record all the overloads of all defined language symbols.
/// The vector<char> represents a set of overloads of a given basename.
/// Values >=0 are for user symbols,
/// the value itself gives the number of arguments.
/// -1 is for n-matched symbols.
/// Values <=-2 are for built-in symbols,
/// -(value+2) gives the number of arguments
/// for a built-in symbol.
std::unordered_map< std::string, std::vector<char> > overloadsTable;

/// Sets builtIn to false.
/// Default constructs value.mt in-place.
/// @warning Calling more than once on a given
/// object may result in a memory leak.
/// @warning Attempting to access value.mt without first
/// calling this method may result in undefined behavior.
void SymbolTableElement::setAsUserSymbol() noexcept {
    builtIn = false;
    new (&(value.mt)) ManyType();
    // The ManyType constructor is noexcept
    // and the new isn't allocating any memory
    // so this function is also noexcept
}

/// If builtIn is false, it will call value.mt.~ManyType()
SymbolTableElement::~SymbolTableElement() noexcept {
    if (!builtIn) {
        value.mt.~ManyType();
    }
}

/// Records the values of all symbols.
/// Keys have the form basename?argCount.
/// Values are either a DataExpression
/// or are a StructureVector of the form [expression,varnames...].
/// The values can never be a StructureString.
std::unordered_map< std::string, SymbolTableElement > symbolTable;

/// Creates a string of the form baseName?argCount.
/// @param exactName where the result will be placed
/// @param baseName the basename of the symbol
/// @param argCount the number of arguments in the desired overload.
/// -1 for n-matched symbols.
void createExactName(std::string& exactName, const std::string& baseName, const unsigned char argCount) {
    exactName = baseName;
    exactName.push_back('?');
    if (argCount == -1) {
        exactName.push_back('n');
    }
    else {
        exactName.append(std::to_string( (int)(argCount) ));
    }
}

/// Looks up a symbol.
/// @param exactName the symbol name to look up, should be of the form baseName?argCount
/// @return a pointer to the SymbolTableElement in the definition of the symbol, or nullptr
/// if the symbol name is not defined
SymbolTableElement* getElementFromSymbolTable(const std::string& exactName) noexcept {
    const auto it = symbolTable.find(exactName);
    return (it == symbolTable.end()) ? nullptr : &(it->second);
}

/// Loads a language built-in function into the symbol table.
/// @param baseName the name of the symbol to be created
/// @param func a pointer to the implementation of the language built-in function
/// @param argCount the number of arguments accepted by the built-in function
/// @param delayMask the delayMask of the language built-in function
/// @see SymbolTableElement
/// @warning does not check for name conflicts,
/// may result in undefined behavior if the specific overload already exists
void placeBuiltInSymbol(const std::string& baseName, const boundFunction func, const char argCount, const unsigned char delayMask) {
    // this happens at the start of the program
    // there shouldn't be any name conflicts
    overloadsTable[baseName].push_back( (argCount == -1) ? -1 : -argCount-2 );
    // overloadsTable[baseName] was created if it didn't already exist
    std::string exactName;
    createExactName(exactName,baseName,argCount);
    SymbolTableElement& elem = symbolTable[exactName];
    // symbolTable[exactName] will be created at this line
    elem.value.func = func;
    // ok because SymbolTableElement expects
    // to hold a function pointer by default
    elem.delayMask = delayMask;
}

/// Loads a user-defined symbol into the symbol table.
/// Overwrites if the specific user-defined overload already exists.
/// @param baseName the name of the symbol to be created
/// @param mt the definition of the symbol, must conform to symbolTable structure
/// @param argCount the number of arguments accepted by the user-defined symbol
/// @param delayMask the delayMask of the user-defined symbol
/// @see symbolTable
/// @see SymbolTableElement
/// @throw UserAlert if the specific overload is a built-in symbol
void placeUserSymbol(const std::string& baseName, const ManyType& mt, const char argCount, const unsigned char delayMask) {
    // there might be a name conflict
    std::string exactName;
    createExactName(exactName,baseName,argCount);
    // check if a symbol with this name exists
    SymbolTableElement* elem = getElementFromSymbolTable(exactName);
    if (elem) {
        // a symbol with this exact name already exists
        if (elem->builtIn) {
            throw UserAlert(UserMessage::WriteToBuiltInSymbol,baseName.c_str());
        }
        // at this point in the function,
        // this symbol can be overwritten
        // no need to update overloads
        // it should already be set as a user symbol
    }
    else {
        // no symbol with this exact name exists
        overloadsTable[baseName].push_back(argCount);
        // overloadsTable[baseName] was created if it didn't already exist
        elem = &(symbolTable[exactName]);
        // symbolTable[exactName] will be created at this line
        // and elem will not be nullptr
        elem->setAsUserSymbol();
        // mark it so that we can place a ManyType object
        // into the SymbolTableElement value
        elem->delayMask = delayMask;
    }
    // now we actually do the copying
    elem->value.mt = mt;
}

/// Removes a user-defined overload from the symbol table.
/// @param baseName the name of the symbol to be removed
/// @param argCount the number of arguments accepted by the user-defined symbol
/// @return true if the overload was deleted, false otherwise
bool removeUserSymbol(const std::string& baseName, const char argCount) {
    std::string exactName;
    createExactName(exactName,baseName,argCount);
    // check if it exists and if we can delete it
    const SymbolTableElement* elem = getElementFromSymbolTable(exactName);
    if (elem) {
        // a symbol with this exact name exists
        if (elem->builtIn) {
            // can't delete a built-in symbol
            return false;
        }
        // it exists and we can delete it
        symbolTable.erase(exactName);
        std::vector<char>& vec = overloadsTable.at(baseName);
        // this will not fail because every entry in symbolTable
        // must have a corresponding entry in overloadsTable
        for (int_fast16_t i = 0; i < vec.size(); ++i) {
            if (vec[i] == argCount) {
                // when we find the entry in overloadsTable
                // for this exact overload, move the last entry
                // for this baseName into its position
                // then exit loop
                vec[i] = vec.back();
                vec.pop_back();
                break;
            }
        }
        if (vec.size() == 0) {
            // if there are no overloads
            // for this baseName
            overloadsTable.erase(baseName);
        }
        return true;
    }
    else {
        // no symbol with extactName
        return false;
    }
}

/// Generates a list of overloads that can be removed for a given symbol name.
/// If no user-defined overloads exist, then the list will be empty.
/// Appends the list to vec.
/// @param baseName the baseName to look up
/// @param vec a vector onto which the result list will be appended in-place
/// the values will have the same form as in overloadsTable
/// @see overloadsTable
void removeUserSymbolList(const std::string& baseName, std::vector<char>& vec) {
    const auto it = overloadsTable.find(baseName);
    if (it != overloadsTable.end()) {
        // baseName is a key in overloadsTable
        const std::vector<char>& overloads = it->second;
        for (int_fast16_t i = 0; i < overloads.size(); ++i) {
            if (overloads[i] >= 0) {
                // built-in symbols have negative overload values
                // so they will be ignored
                vec.push_back(overloads[i]);
            }
        }
    }
}

/// Returns a reference to the SymbolTableElement corresponding
/// to the given baseName and argCount.
/// May return n-match if an exact match is not present.
/// @param baseName the baseName to look up
/// @param argCount the number of arguments of the desired function / variable
/// @return A reference to a value in symbolTable.
/// @throw UserAlert if there is no match
const SymbolTableElement& readSymbol(const std::string& baseName, const char argCount) {
    std::string exactName;
    SymbolTableElement* elem;
    if (argCount >= 0) {
        createExactName(exactName,baseName,argCount);
        elem = getElementFromSymbolTable(exactName);
        if (elem) {
            return *elem;
        }
    }
    // there is not an exact match
    // try using n matched symbol
    createExactName(exactName,baseName,-1);
    elem = getElementFromSymbolTable(exactName);
    if (elem) {
        return *elem;
    }
    // there is no match
    if (overloadsTable.count(baseName)) {
        // there is at least one overload for the baseName,
        // just not any that match here
        throw UserAlert(UserMessage::WrongNumberOfArguments,baseName.c_str());
    }
    else {
        // the baseName is completely unknown
        throw UserAlert(UserMessage::UnknownSymbol,baseName.c_str());
    }
}