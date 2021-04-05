/**
 * @file ManyType.cpp
 * @author Aaron Stanek
*/
#include "ManyType.h"

ManyType::ManyType(const ManyType& other) noexcept {
    // breaks const
    label = other.label;
    value = other.value;
    ((ManyType*)(&other))->label = ManyTypeLabel::None;
    // equivalent to default constructing this one
    // then swapping their labels and values
}

ManyType::~ManyType() noexcept {
    // ensures that the value field
    // is writable without causing
    // a memory leak
    if (static_cast<ManyTypeLabelInt>(label) & static_cast<ManyTypeLabelInt>(ManyTypeLabel::Pointer)) {
        if (static_cast<ManyTypeLabelInt>(label) & static_cast<ManyTypeLabelInt>(ManyTypeLabel::String)) {
            delete value.String;
        }
        else {
            delete value.Vector;
        }
    }
    // this function does not change label
    // either the object is going out of scope
    // and label does not matter anymore
    // or the calling function will change it
}

void ManyType::operator=(const ManyType& other) noexcept {
    // breaks const
    // swaps labels and values
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

void ManyType::putNone() noexcept {
    if (label != ManyTypeLabel::None) {
        this->~ManyType();
        label = ManyTypeLabel::None;
    }
}

void ManyType::putBool(const bool x) noexcept {
    if (label != ManyTypeLabel::Bool) {
        this->~ManyType();
        label = ManyTypeLabel::Bool;
    }
    value.Bool = x;
}

bool ManyType::getBool() const {
    if (label != ManyTypeLabel::Bool) {
        throw ManyTypeAccessError();
    }
    else {
        return value.Bool;
    }
}

void ManyType::putInt(const long x) noexcept {
    if (label != ManyTypeLabel::Int) {
        this->~ManyType();
        label = ManyTypeLabel::Int;
    }
    value.Int = x;
}

long ManyType::getInt() const {
    if (label != ManyTypeLabel::Int) {
        throw ManyTypeAccessError();
    }
    else {
        return value.Int;
    }
}

void ManyType::putFtype(const ftype x) noexcept {
    if (label != ManyTypeLabel::Ftype) {
        this->~ManyType();
        label = ManyTypeLabel::Ftype;
    }
    value.Ftype = x;
}

ftype ManyType::getFtype() const {
    if (label != ManyTypeLabel::Ftype) {
        throw ManyTypeAccessError();
    }
    else {
        return value.Ftype;
    }
}

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

std::string& ManyType::getDataString() const {
    if (label != ManyTypeLabel::DataString) {
        throw ManyTypeAccessError();
    }
    else {
        return *(value.String);
    }
}

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

mtvec& ManyType::getDataVector() const {
    if (label != ManyTypeLabel::DataVector) {
        throw ManyTypeAccessError();
    }
    else {
        return *(value.Vector);
    }
}

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

std::string& ManyType::getStructureString() const {
    if (label != ManyTypeLabel::StructureString) {
        throw ManyTypeAccessError();
    }
    else {
        return *(value.String);
    }
}

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

mtvec& ManyType::getStructureVector() const {
    if (label != ManyTypeLabel::StructureVector) {
        throw ManyTypeAccessError();
    }
    else {
        return *(value.Vector);
    }
}

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
            if (static_cast<ManyTypeLabelInt>(label) & ~static_cast<ManyTypeLabelInt>(ManyTypeLabel::String)) {
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
            if (static_cast<ManyTypeLabelInt>(label) & ~static_cast<ManyTypeLabelInt>(ManyTypeLabel::Vector)) {
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

void ManyType::wrapStructureString() {
    // only ever called if this holds a StructureString
    // but we expect to have a function-like StructureVector
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

const char * ManyTypeAccessError::what() const noexcept {
    return "ManyTypeAccessError";
}