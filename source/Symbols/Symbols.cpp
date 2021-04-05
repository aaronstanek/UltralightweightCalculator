/**
 * @file Symbols.cpp
 * @author Aaron Stanek
*/
#include "Symbols.h"

std::unordered_map< std::string, std::vector<char> > overloadsTable;
// can only be accessed from within this file

// the char for each entry are the overloads,
// values >=0 are for user symbols
// -1 is for n match symbols
// values <=-2 are for built-in symbols
// -(value+2) for the number of arguments

void SymbolTableElement::setAsUserSymbol() noexcept {
    builtIn = false;
    new (&(value.mt)) ManyType();
    // we call this before we store a ManyType
    // object in value
    // failure to call will cause problems
    // calling multiple times may cause problems
    // so don't do that
    // The ManyType constructor is noexcept
    // and the new isn't allocating any memory
    // so this function is also noexcept
}

SymbolTableElement::~SymbolTableElement() noexcept {
    if (!builtIn) {
        value.mt.~ManyType();
    }
    // if we stored a ManyType, we will have to
    // destruct it
    // if we stored a function pointer, we do not
    // need to destruct it
}

std::unordered_map< std::string, SymbolTableElement > symbolTable;
// can only be accessed from within this file
// keys have the form basename?argCount
// values are either DataExpression
// or are a StructureVector of the form [expression,varnames...]
// the values can never be a StructureString

void createExactName(std::string& exactName, const std::string& baseName, const unsigned char argCount) {
    // create a string of the form baseName?argCount
    exactName = baseName;
    exactName.push_back('?');
    if (argCount == -1) {
        exactName.push_back('n');
    }
    else {
        exactName.append(std::to_string( (int)(argCount) ));
    }
}

SymbolTableElement* getElementFromSymbolTable(const std::string& exactName) noexcept {
    const auto it = symbolTable.find(exactName);
    return (it == symbolTable.end()) ? nullptr : &(it->second);
}

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

bool removeUserSymbol(const std::string& baseName, const char argCount) {
    // return true if successful delete
    // return false otherwise
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

void removeUserSymbolList(const std::string& baseName, std::vector<char>& vec) {
    // gets a list of all overloads of a given baseName that can be removed
    // and places it in vec
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

const SymbolTableElement& readSymbol(const std::string& baseName, const char argCount) {
    // returns the SymbolTableElement corresponding to the given baseName
    // and argCount
    // may return n match if an exact match is not present
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