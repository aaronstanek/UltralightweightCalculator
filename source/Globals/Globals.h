/**
 * @file Globals.h
 * @author Aaron Stanek
 * @brief Imports common library modules,
 * defines UserAlert exception,
 * manages global variables,
 * defines numeric limit macros
*/
#pragma once

// include all the common library stuff
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdint.h>
#include <time.h>
#include <exception>
#include <string>
#include <vector>

// include a definition of ftype
// also includes float.h
#include "Ftype.h"

enum class UserMessage: uint_fast8_t {
    UnknownError,
    // meta
    MaximumRecursionDepthReached,
    MaximumLogicalRecursionDepthReached,
    Timeout,
    StructureStringFormatError,
    // text processing
    InputTooLong,
    InvalidCharacter,
    SyntaxError,
    // evaluation
    WriteToBuiltInSymbol,
    UnknownSymbol,
    WrongNumberOfArguments,
    TooManyArguments,
    UnexpectedType,
    DomainError,
    NanError,
    InfinityError
};

struct UserAlert : public std::exception {
    std::string message;
    UserAlert(const UserMessage, const char* const);
    const char* what() const noexcept;
};

extern long maximumRecursionDepth;
extern long maximumLogicalRecursionDepth;
extern double maximumProcessingTime;
extern time_t processingStartTime;

// add places to hold updated values

extern long newMaximumRecursionDepth;
extern long newMaximumLogicalRecursionDepth;
extern double newMaximumProcessingTime;

void checkProcessingTime();

void applyNewLimits() noexcept;

#define MAX_maximumRecursionDepth 1000000
#define MIN_maximumRecursionDepth 10
#define MIN_maximumLogicalRecursionDepth 10
/// In seconds.
#define MAX_maximumProcessingTime 86401
/// In seconds.
#define MIN_maximumProcessingTime 3

/// Maximum number of elements in a StructureVector
/// representing a function definition
#define MAX_ARGS_DEF 128
/// Maximum number of elements in a StructureVector
/// representing a function call
#define MAX_ARGS_USED 65536
/// Maximum number of elements in a DataVector.
/// Maximum number of bytes in the user input.
#define MAX_INPUT_SIZE 1048576

// 32 bit integer limits
#define MAX_INTEGER_VALUE 2147483647
#define MIN_INTEGER_VALUE -2147483647
#define MAX_UNSIGNED_INTEGER_VALUE 4294967295
// INT32_MIN and INT32_MAX are only defined
// if that exact sized integer is supported
// INT32_MIN will be one less than MIN_INTEGER_VALUE on
// all 2's complement systems, but to maintain
// compatibility with 1's complement and
// sign-magnitude systems, we will only use the range that
// is strictly required by the standard