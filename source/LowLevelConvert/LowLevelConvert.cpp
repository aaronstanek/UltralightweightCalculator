/**
 * @file LowLevelConvert.cpp
 * @author Aaron Stanek
*/
#include "LowLevelConvert.h"

unsigned long convertSignedToUnsigned(const long x) noexcept {
    // platform-independent safe convert
    if (x >= 0) {
        return (unsigned long)(x) + (unsigned long)(MAX_INTEGER_VALUE);
    }
    else {
        return (unsigned long)(x + (long)(MAX_INTEGER_VALUE));
    }
}

long convertUnsignedToSigned(const unsigned long x) noexcept {
    // platform-independent safe convert
    if (x >= (unsigned long)(MAX_INTEGER_VALUE)) {
        return (long)(x - (unsigned long)(MAX_INTEGER_VALUE));
    }
    else {
        return (long)(x) - (long)(MAX_INTEGER_VALUE);
    }
}

void convertToBool(ManyType& x) {
    // in-place
    switch (x.type()) {
        case ManyTypeLabel::None:
        x.putBool(false);
        return;

        case ManyTypeLabel::Bool:
        return;

        case ManyTypeLabel::Int:
        x.putBool(x.getInt());
        // implicit conversion
        return;

        case ManyTypeLabel::Ftype:
        {
            const ftype value = x.getFtype();
            x.putBool( (value >= 0.5) || (value <= -0.5) );
        }
        return;

        default:
        throw UserAlert(UserMessage::UnexpectedType,"Conversion to Bool");
    }
}

void convertToInt(ManyType& x) {
    // in-place
    switch (x.type()) {
        case ManyTypeLabel::None:
        x.putInt(0);
        return;

        case ManyTypeLabel::Bool:
        x.putInt(x.getBool());
        // implicit conversion
        return;

        case ManyTypeLabel::Int:
        return;

        case ManyTypeLabel::Ftype:
        {
            const ftype value = x.getFtype();
            if (value > ((ftype)(MAX_INTEGER_VALUE)) || value < ((ftype)(MIN_INTEGER_VALUE))) {
                // if the floating point value is
                // outside the range that can be held by long
                throw UserAlert(UserMessage::DomainError,"Conversion to Int");
            }
            // we can hold x in a long
            // round towards zero
            x.putInt( (value >= 0.0) ? floor(value) : ceil(value) );
        }
        return;

        default:
        throw UserAlert(UserMessage::UnexpectedType,"Conversion to Int");
    }
}

void convertToFtype(ManyType& x) {
    // in-place
    switch (x.type()) {
        case ManyTypeLabel::None:
        x.putFtype(0.0);
        return;

        case ManyTypeLabel::Bool:
        x.putFtype(x.getBool());
        // implicit conversion
        return;

        case ManyTypeLabel::Int:
        x.putFtype(x.getInt());
        // implicit conversion
        return;

        case ManyTypeLabel::Ftype:
        return;

        default:
        throw UserAlert(UserMessage::UnexpectedType,"Conversion to Float");
    }
}

void convertToStructureString(ManyType& x) {
    if (static_cast<ManyTypeLabelInt>(x.type()) & ~static_cast<ManyTypeLabelInt>(ManyTypeLabel::String)) {
        throw UserAlert(UserMessage::StructureStringFormatError,"Not a String");
    }
    if (x.type() == ManyTypeLabel::StructureString) {
        return;
    }
    // DataString
    const std::string& s = x.getDataString();
    if (s.size() == 0) {
        throw UserAlert(UserMessage::StructureStringFormatError,"Empty String");
    }
    // we will only allow valid characters
    // % may only be used at index 0
    for (int_fast32_t i = 0; i < s.size(); ++i) {
        const char c = s[i];
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_') {
            continue;
        }
        if (c == '%') {
            if (i == 0) {
                continue;
            }
        }
        throw UserAlert(UserMessage::StructureStringFormatError,s.c_str());
    }
}