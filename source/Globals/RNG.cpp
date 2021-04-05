#include "RNG.h"
#include "../LowLevelConvert/LowLevelConvert.h"

struct RNG_State_Struct {
    unsigned long A, B;
    RNG_State_Struct() noexcept {
        const long long t = difftime(time(NULL),0);
        // value of t may be implementation-defined
        // t is the current time to whatever precision
        // we are able to get
        A = t & 0xFFFFFFFF;
        B = (t >> 32) & 0xFFFFFFFF;
    }
};

RNG_State_Struct RNG_State;

unsigned long RNG_getRaw() noexcept {
    if (RNG_State.B >= MAX_UNSIGNED_INTEGER_VALUE) {
        RNG_State.B = 0;
        if (RNG_State.A >= MAX_UNSIGNED_INTEGER_VALUE) {
            RNG_State.A = 0;
        }
        else {
            ++RNG_State.A;
        }
    }
    else {
        ++RNG_State.B;
    }
    unsigned long A = RNG_State.A;
    unsigned long B = RNG_State.B;
    for (int_fast8_t i = 0; i < 10; ++i) {
        A = (A << 3) | ((A >> 29) & 0x07);
        B += 314159265; // pi = 3.14159265
        A += B << 7;
        B ^= A;
        A ^= (B >> 16) & 0xFFFF;
        A ^= (B & 0xFFFF) << 16;
    }
    return A;
}

long RNG_getInt(const long minValue, const long maxValue) noexcept {
    // safe to assume that minValue <= maxValue
    const unsigned long uMinValue = convertSignedToUnsigned(minValue);
    const unsigned long largest = convertSignedToUnsigned(maxValue) - uMinValue;
    // largest is the largest value from getRaw that we will accept
    unsigned long mask = 1;
    while (mask < largest) {
        mask = (mask << 1) | 0x01;
    }
    // mask is >= largest
    // we can now efficiently generate
    // values in a reasonable range
    unsigned long output;
    while (true) {
        output = RNG_getRaw();
        output &= mask;
        // output is now is a close enough range
        // to what we need to generate
        // max 50% chance that we have to repeat the loop
        if (output <= largest) {
            break;
        }
    }
    return convertUnsignedToSigned(output + uMinValue);
}