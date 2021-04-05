#pragma once
#include "../Globals/Globals.h"

typedef uint_fast8_t ManyTypeLabelInt;

enum class ManyTypeLabel : ManyTypeLabelInt {
    None = 0x01,
    Bool = 0x02,
    Int = 0x04,
    Ftype = 0x08,
    DataString = 0x10,
    DataVector = 0x20,
    StructureString = 0x40,
    StructureVector = 0x80,
    // bitmasks
    DataExpression = None | Bool | Int | Ftype | DataString | DataVector,
    StructureExpression = StructureString | StructureVector,
    String = DataString | StructureString,
    Vector = DataVector | StructureVector,
    Pointer = String | Vector
};

class ManyType;
typedef std::vector<ManyType> mtvec;

union ManyTypeUnion {
    bool Bool;
    long Int;
    ftype Ftype;
    std::string* String;
    mtvec* Vector;
};

class ManyType {
    private:
    ManyTypeLabel label;
    ManyTypeUnion value;
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
    void wrapStructureString();
};

inline ManyType::ManyType() noexcept {
    label = ManyTypeLabel::None;
}

inline ManyTypeLabel ManyType::type() const noexcept {
    return label;
}

struct ManyTypeAccessError : public std::exception {
    const char * what() const noexcept;
};