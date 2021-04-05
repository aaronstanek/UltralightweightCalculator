#include "Bindings.h"
#include "../Symbols/Symbols.h"
#include "../ManyType/ManyType.h"

void none_implement(ManyType& ret, mtvec& arr, long recursionJuice) noexcept {
    ret.putNone();
}

void true_implement(ManyType& ret, mtvec& arr, long recursionJuice) noexcept {
    ret.putBool(true);
}

void false_implement(ManyType& ret, mtvec& arr, long recursionJuice) noexcept {
    ret.putBool(false);
}

void pi_implement(ManyType& ret, mtvec& arr, long recursionJuice) noexcept {
    ret.putFtype(M_PI);
}

void e_implement(ManyType& ret, mtvec& arr, long recursionJuice) noexcept {
    ret.putFtype(M_E);
}

constexpr long computeNumberOfBits(const unsigned long x, const long output) noexcept {
    // returns the minimum number of bits needed to represent x
    return (x == 0) ? output : computeNumberOfBits(x/2,output+1);
}

void floatexponent_implement(ManyType& ret, mtvec& arr, long recursionJuice) noexcept {
    ret.putInt(
        computeNumberOfBits(
            ((unsigned long)(-FTYPE_EXPONENT_MIN)) +
            ((unsigned long)(FTYPE_EXPONENT_MAX)) + 1, 0
        )
    );
    // at runtime, this will just be ret.putInt(const)
    // (-FTYPE_EXPONENT_MIN) + FTYPE_EXPONENT_MAX + 1
    // is the number of exponent values that are supported
    // on the target platform
    // as long as fewer than half of the exponent range is
    // reserved for special values, this will return the
    // number of bits used for the exponent
}

void floatprecision_implement(ManyType& ret, mtvec& arr, long recursionJuice) noexcept {
    ret.putInt(FTYPE_PRECISION);
}

inline void bindConstants() {
    std::string baseName;
    baseName = "none";
    placeBuiltInSymbol(baseName,&none_implement,0,0);
    baseName = "true";
    placeBuiltInSymbol(baseName,&true_implement,0,0);
    baseName = "false";
    placeBuiltInSymbol(baseName,&false_implement,0,0);
    baseName = "pi";
    placeBuiltInSymbol(baseName,&pi_implement,0,0);
    baseName = "e";
    placeBuiltInSymbol(baseName,&e_implement,0,0);
    baseName = "floatexponent";
    placeBuiltInSymbol(baseName,&floatexponent_implement,0,0);
    baseName = "floatprecision";
    placeBuiltInSymbol(baseName,&floatprecision_implement,0,0);
}