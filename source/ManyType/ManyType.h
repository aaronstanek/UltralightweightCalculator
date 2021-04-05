/**
 * @file ManyType.h
 * @author Aaron Stanek
 * @brief Classes for holding numeric
 * and std types in a union
*/
#pragma once
#include "../Globals/Globals.h"

typedef uint_fast8_t ManyTypeLabelInt;

/// Identifies a datatype using a numeric value.
/// The values are bitmask-compatible.
enum class ManyTypeLabel : ManyTypeLabelInt {
    None = 0x01, ///< language type none
    Bool = 0x02, ///< language type bool
    Int = 0x04, ///< language type int
    Ftype = 0x08, ///< language type float
    DataString = 0x10, ///< language type string
    DataVector = 0x20, ///< language types rowvec, colvec, matrix
    StructureString = 0x40, ///< symbol name
    StructureVector = 0x80, ///< function call
    // bitmasks
    /// bitmask: none, bool, int, float, string, rowvec, colvec, matrix
    /// \n types accessible to the user
    DataExpression = None | Bool | Int | Ftype | DataString | DataVector,
    /// bitmask: symbol name, function call
    /// \n types not accessible to the user
    StructureExpression = StructureString | StructureVector,
    /// bitmask: string, symbol name
    /// \n types implemented with std::string
    String = DataString | StructureString,
    /// bitmask: rowvec, colvec, matrix, function call
    /// \n types implemented with std::vector
    Vector = DataVector | StructureVector,
    /// bitmask: string, rowvec, colvec, matrix, symbol name, function call
    /// \n types implemented with std::string or std::vector
    /// \n indicates that a nontrivial destructor needs to be called upon deletion
    Pointer = String | Vector
};

class ManyType;
typedef std::vector<ManyType> mtvec;

/// stores one of: bool, long, ftype, string*, vector<ManyType>*
union ManyTypeUnion {
    bool Bool;
    long Int;
    ftype Ftype;
    std::string* String;
    mtvec* Vector;
};

/// Uses ManyTypeUnion to store a value.
/// Uses ManyTypeLabel to identify the
/// type of the stored value.
class ManyType {
    private:
    ManyTypeLabel label; ///< The type of the stored value
    ManyTypeUnion value; ///< The value stored by the object
    public:
    inline ManyType() noexcept;
    ManyType(const ManyType&) noexcept;
    ~ManyType() noexcept;
    void operator=(const ManyType&) noexcept;
    inline ManyTypeLabel type() const noexcept;
    void putNone() noexcept;
    void putBool(const bool) noexcept;
    bool getBool() const;
    void putInt(const long) noexcept;
    long getInt() const;
    void putFtype(const ftype) noexcept;
    ftype getFtype() const;
    std::string& putDataString();
    std::string& getDataString() const;
    mtvec& putDataVector();
    mtvec& getDataVector() const;
    std::string& putStructureString();
    std::string& getStructureString() const;
    mtvec& putStructureVector();
    mtvec& getStructureVector() const;
    void makeCopyFrom(const ManyType&,long);
    void wrapInVector();
};

/// Default constructor.
/// label is set to None.
inline ManyType::ManyType() noexcept {
    label = ManyTypeLabel::None;
}

/// @return the type of data stored by this object
inline ManyTypeLabel ManyType::type() const noexcept {
    return label;
}

/// Thrown when the attempting to read from the
/// wrong side of a ManyTypeUnion.
struct ManyTypeAccessError : public std::exception {
    const char * what() const noexcept;
};