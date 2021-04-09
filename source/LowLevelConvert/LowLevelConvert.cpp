/**
 * @file LowLevelConvert.cpp
 * @author Aaron Stanek
*/
#include "LowLevelConvert.h"

/// provides platform-independent conversion from long to unsigned long
/// @param x the number to convert
/// @return x + ( 2^31 - 1 )
/// @warning x cannot be equal to or greater than (2^31)
/// @warning x cannot be equal to or less than -(2^31)
unsigned long convertSignedToUnsigned(const long x) noexcept {
    // platform-independent safe convert
    if (x >= 0) {
        return (unsigned long)(x) + (unsigned long)(MAX_INTEGER_VALUE);
    }
    else {
        return (unsigned long)(x + (long)(MAX_INTEGER_VALUE));
    }
}

/// provides platform-independent conversion from unsigned long to long
/// @param x the number to convert
/// @return x - ( 2^31 - 1 )
/// @warning x cannot be equal to or greater than ( 2^32 - 1 )
long convertUnsignedToSigned(const unsigned long x) noexcept {
    // platform-independent safe convert
    if (x >= (unsigned long)(MAX_INTEGER_VALUE)) {
        return (long)(x - (unsigned long)(MAX_INTEGER_VALUE));
    }
    else {
        return (long)(x) - (long)(MAX_INTEGER_VALUE);
    }
}

/// Converts a ManyType object in-place.
/// The type of the ManyType object after
/// this operation will be Bool.
/// @param x the object to convert
/// @throw UserAlert if the value of x cannot be converted to bool
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

/// Converts a ManyType object in-place.
/// The type of the ManyType object after
/// this operation will be Int.
/// @param x the object to convert
/// @throw UserAlert if the value of x cannot be converted to long
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

/// Converts a ManyType object in-place.
/// The type of the ManyType object after
/// this operation will be Float.
/// @param x the object to convert
/// @throw UserAlert if the value of x cannot be converted to ftype
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

/// Converts a ManyType object in-place.
/// The type of the ManyType object after
/// this operation will be StructureString.
/// @param x the object to convert
/// @throw UserAlert if the value of x cannot be converted
/// to a syntactically correct StructureString
void convertToStructureString(ManyType& x) {
    if ((ManyTypeLabelInt)(x.type()) & ~(ManyTypeLabelInt)(ManyTypeLabel::String)) {
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