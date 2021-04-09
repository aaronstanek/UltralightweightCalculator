/**
 * @file ManyType.cpp
 * @author Aaron Stanek
*/
#include "ManyType.h"

/// Raw copies the bytes from other.
/// Then default constructs other.
/// @param other the ManyType object to construct from
/// @warning breaks const
ManyType::ManyType(const ManyType& other) noexcept {
    label = other.label;
    value = other.value;
    ((ManyType*)(&other))->label = ManyTypeLabel::None;
}

/// Checks the type stored by this object.
/// Calls nontrivial destructors, if any.
/// Can be called directly to ensure that
/// the value field is writable without causing
/// a memory leak.
/// @warning
/// This function does not change label.
/// If being deleted, this does not matter.
/// If called directly, the caller is responsible
/// for updating label correctly.
ManyType::~ManyType() noexcept {
    if ((ManyTypeLabelInt)(label) & (ManyTypeLabelInt)(ManyTypeLabel::Pointer)) {
        if ((ManyTypeLabelInt)(label) & (ManyTypeLabelInt)(ManyTypeLabel::String)) {
            delete value.String;
        }
        else {
            delete value.Vector;
        }
    }
}

/// Raw swaps the bytes in this object and other.
/// @param other the ManyType object to swap contents with
/// @warning breaks const
void ManyType::operator=(const ManyType& other) noexcept {
    {
        ManyTypeLabel label_holder = label;
        label = other.label;
        ((ManyType*)(&other))->label = label_holder;
    }
    {
        ManyTypeUnion value_holder = value;
        value = other.value;
        ((ManyType*)(&other))->value = value_holder;
    }
}

/// Stores none in this object.
/// Sets label to None.
void ManyType::putNone() noexcept {
    if (label != ManyTypeLabel::None) {
        this->~ManyType();
        label = ManyTypeLabel::None;
    }
}

/// Sets label to Bool. Stores a boolean value.
/// @param x the boolean value to be stored in this object.
void ManyType::putBool(const bool x) noexcept {
    if (label != ManyTypeLabel::Bool) {
        this->~ManyType();
        label = ManyTypeLabel::Bool;
    }
    value.Bool = x;
}

/// @return the boolean value stored in this object.
/// @throw ManyTypeAccessError if value is not boolean.
bool ManyType::getBool() const {
    if (label != ManyTypeLabel::Bool) {
        throw ManyTypeAccessError();
    }
    else {
        return value.Bool;
    }
}

/// Sets label to Int. Stores an integer value.
/// @param x the integer value to be stored in this object.
void ManyType::putInt(const long x) noexcept {
    if (label != ManyTypeLabel::Int) {
        this->~ManyType();
        label = ManyTypeLabel::Int;
    }
    value.Int = x;
}

/// @return the integer value stored in this object.
/// @throw ManyTypeAccessError if value is not an integer.
long ManyType::getInt() const {
    if (label != ManyTypeLabel::Int) {
        throw ManyTypeAccessError();
    }
    else {
        return value.Int;
    }
}

/// Sets label to Float. Stores a floating-point value.
/// @param x the floating-point value to be stored in this object.
void ManyType::putFtype(const ftype x) noexcept {
    if (label != ManyTypeLabel::Ftype) {
        this->~ManyType();
        label = ManyTypeLabel::Ftype;
    }
    value.Ftype = x;
}

/// @return the floating-point value stored in this object.
/// @throw ManyTypeAccessError if value is not floating-point.
ftype ManyType::getFtype() const {
    if (label != ManyTypeLabel::Ftype) {
        throw ManyTypeAccessError();
    }
    else {
        return value.Ftype;
    }
}

/// Creates std::string in this object, if not present.
/// If the current value is DataString or StructureString, the value will be unchanged.
/// Sets label to DataString.
/// @return reference to the std::string stored by this object.
std::string& ManyType::putDataString() {
    if (label != ManyTypeLabel::DataString) {
        if (label != ManyTypeLabel::StructureString) {
            // this is not a string at all
            // we need to clear the value
            this->~ManyType();
            // and then set up a new string
            value.String = new std::string;
        }
        label = ManyTypeLabel::DataString;
    }
    return *(value.String);
}

/// @return reference to the std::string stored by this object.
/// @throw ManyTypeAccessError if value is not DataString.
std::string& ManyType::getDataString() const {
    if (label != ManyTypeLabel::DataString) {
        throw ManyTypeAccessError();
    }
    else {
        return *(value.String);
    }
}

/// Creates mtvec in this object, if not present.
/// If the current value is DataVector or StructureVector, the value will be unchanged.
/// Sets label to DataVector.
/// @return reference to the mtvec stored by this object.
mtvec& ManyType::putDataVector() {
    if (label != ManyTypeLabel::DataVector) {
        if (label != ManyTypeLabel::StructureVector) {
            // this is not a vector at all
            // we need to clear the value
            this->~ManyType();
            // and then set up a new vector
            value.Vector = new mtvec;
        }
        label = ManyTypeLabel::DataVector;
    }
    return *(value.Vector);
}

/// @return reference to the mtvec stored by this object.
/// @throw ManyTypeAccessError if value is not DataVector.
mtvec& ManyType::getDataVector() const {
    if (label != ManyTypeLabel::DataVector) {
        throw ManyTypeAccessError();
    }
    else {
        return *(value.Vector);
    }
}

/// Creates std::string in this object, if not present.
/// If the current value is DataString or StructureString, the value will be unchanged.
/// Sets label to StructureString.
/// @return reference to the std::string stored by this object.
std::string& ManyType::putStructureString() {
    if (label != ManyTypeLabel::StructureString) {
        if (label != ManyTypeLabel::DataString) {
            // this is not a string at all
            // we need to clear the value
            this->~ManyType();
            // and then set up a new string
            value.String = new std::string;
        }
        label = ManyTypeLabel::StructureString;
    }
    return *(value.String);
}

/// @return reference to the std::string stored by this object.
/// @throw ManyTypeAccessError if value is not StructureString.
std::string& ManyType::getStructureString() const {
    if (label != ManyTypeLabel::StructureString) {
        throw ManyTypeAccessError();
    }
    else {
        return *(value.String);
    }
}

/// Creates mtvec in this object, if not present.
/// If the current value is DataVector or StructureVector, the value will be unchanged.
/// Sets label to StructureVector.
/// @return reference to the mtvec stored by this object.
mtvec& ManyType::putStructureVector() {
    if (label != ManyTypeLabel::StructureVector) {
        if (label != ManyTypeLabel::DataVector) {
            // this is not a vector at all
            // we need to clear the value
            this->~ManyType();
            // and then set up a new vector
            value.Vector = new mtvec;
        }
        label = ManyTypeLabel::StructureVector;
    }
    return *(value.Vector);
}

/// @return reference to the mtvec stored by this object.
/// @throw ManyTypeAccessError if value is not StructureVector.
mtvec& ManyType::getStructureVector() const {
    if (label != ManyTypeLabel::StructureVector) {
        throw ManyTypeAccessError();
    }
    else {
        return *(value.Vector);
    }
}

/// The current contents of this object are deleted
/// and replaced with a deep copy of another ManyType object.
/// @param other the object to copy
/// @param recursionJuice how many layers of recursion may be used by this operation
/// @warning may result in undefined behavior if other is a reference
/// to this object
void ManyType::makeCopyFrom(const ManyType& other, long recursionJuice) {
    switch (other.label) {
        case ManyTypeLabel::Bool:
            putBool(other.value.Bool);
            break;
        case ManyTypeLabel::Int:
            putInt(other.value.Int);
            break;
        case ManyTypeLabel::Ftype:
            putFtype(other.value.Ftype);
            break;
        case ManyTypeLabel::DataString:
        case ManyTypeLabel::StructureString:
            if ((ManyTypeLabelInt)(label) & ~(ManyTypeLabelInt)(ManyTypeLabel::String)) {
                // value is not string-compatible
                this->~ManyType();
                value.String = new std::string;
            }
            // at this point, value will be string-compatible
            label = other.label;
            *(value.String) = *(other.value.String);
            break;
        case ManyTypeLabel::DataVector:
        case ManyTypeLabel::StructureVector: {
            if (recursionJuice <= 0) {
                throw UserAlert(UserMessage::MaximumRecursionDepthReached,nullptr);
            }
            else {
                --recursionJuice;
            }
            if ((ManyTypeLabelInt)(label) & ~(ManyTypeLabelInt)(ManyTypeLabel::Vector)) {
                // value is not vector-compatible
                this->~ManyType();
                value.Vector = new mtvec;
            }
            // at this point, value will be vector-compatible
            label = other.label;
            mtvec& destination = *(value.Vector);
            const mtvec& source = *(other.value.Vector);
            destination.resize(source.size());
            for (int_fast32_t i = 0; i < destination.size(); ++i) {
                destination[i].makeCopyFrom(source[i],recursionJuice);
            }
            break;
        }
        default:
            // ManyTypeLabel::None
            putNone();
    }
}

/// Creates a StructureVector.
/// Shallow copies this object into
/// the first and only element of that StructureVector.
/// Updates the value of this object to be that StructureVector.
/// Sets label to StructureVector.
void ManyType::wrapInVector() {
    ManyType t;
    mtvec& newVec = t.putStructureVector();
    newVec.resize(1);
    newVec[0] = *this;
    // newVec[0] holds the value we care about
    // *this holds none
    *this = t;
    // *this holds a StructureVector containing
    // the value that we care about
    // t holds none
}

/// @return "ManyTypeAccessError"
const char * ManyTypeAccessError::what() const noexcept {
    return "ManyTypeAccessError";
}