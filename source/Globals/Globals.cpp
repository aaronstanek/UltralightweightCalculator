/**
 * @file Globals.cpp
 * @author Aaron Stanek
*/
#include "Globals.h"

UserAlert::UserAlert(const UserMessage base, const char* const extra) {
    const char* memo;
    switch (base) {
        case UserMessage::MaximumRecursionDepthReached:
        memo = "Max Recursion Depth Reached";
        break;

        case UserMessage::MaximumLogicalRecursionDepthReached:
        memo = "Max Logical Recursion Depth Reached";
        break;

        case UserMessage::Timeout:
        memo = "Max Processing Time Reached";
        break;

        case UserMessage::StructureStringFormatError:
        memo = "Malformed Symbol";
        break;

        case UserMessage::InputTooLong:
        memo = "Input Too Long";
        break;

        case UserMessage::InvalidCharacter:
        memo = "Invalid Character";
        break;

        case UserMessage::SyntaxError:
        memo = "Syntax Error";
        break;

        case UserMessage::WriteToBuiltInSymbol:
        memo = "Write To Built-in Symbol";
        break;

        case UserMessage::UnknownSymbol:
        memo = "Unknown Symbol";
        break;

        case UserMessage::WrongNumberOfArguments:
        memo = "Wrong Number of Arguments";
        break;

        case UserMessage::TooManyArguments:
        memo = "Too Many Arguments";
        break;

        case UserMessage::UnexpectedType:
        memo = "Unexpected Type During";
        break;

        case UserMessage::DomainError:
        memo = "Domain Error During";
        break;

        case UserMessage::NanError:
        memo = "NaN Error During";
        break;

        case UserMessage::InfinityError:
        memo = "Infinity Error During";
        break;

        default:
        memo = "Unknown Error";
    }
    message.assign(memo);
    if (extra) {
        message.append(": ");
        message.append(extra);
    }
}

const char* UserAlert::what() const noexcept {
    return message.c_str();
}

// load initial values

/// Initial value is 500.
long maximumRecursionDepth = 500;
/// Initial value is 10000.
long maximumLogicalRecursionDepth = 10000;
/// Initial value is 120. In seconds.
double maximumProcessingTime = 120;
/// Set to the current time when processing of
/// a user input begins.
/// Initially undefined.
time_t processingStartTime;

/// Stores an updated value of maximumRecursionDepth
/// until the current user input has finished.
/// A value of -1 indicates that maximumRecursionDepth should
/// not be updated.
/// Initial value is -1.
long newMaximumRecursionDepth = -1;
/// Stores an updated value of maximumLogicalRecursionDepth
/// until the current user input has finished.
/// A value of -1 indicates that maximumLogicalRecursionDepth should
/// not be updated.
/// Initial value is -1.
long newMaximumLogicalRecursionDepth = -1;
/// Stores an updated value of maximumProcessingTime
/// until the current user input has finished.
/// A value of -1 indicates that maximumProcessingTime should
/// not be updated.
/// Initial value is -1.
double newMaximumProcessingTime = -1;

/// @throw UserAlert if time elapsed since processingStartTime
/// is greater than or equal to maximumProcessingTime
void checkProcessingTime() {
    if (difftime(time(NULL),processingStartTime) >= maximumProcessingTime) {
        throw UserAlert(UserMessage::Timeout,nullptr);
    }
}

/// Updates maximumRecursionDepth, maximumLogicalRecursionDepth,
/// and maximumProcessingTime, if indicated by newMaximumRecursionDepth,
/// newMaximumLogicalRecursionDepth, and newMaximumProcessingTime, respectively.
/// newMaximumRecursionDepth, newMaximumLogicalRecursionDepth,
/// and newMaximumProcessingTime will be set to -1 if the corresponding
/// value was updated.
void applyNewLimits() noexcept {
    if (newMaximumRecursionDepth > 0) {
        maximumRecursionDepth = newMaximumRecursionDepth;
        newMaximumRecursionDepth = -1;
    }
    if (newMaximumLogicalRecursionDepth > 0) {
        maximumLogicalRecursionDepth = newMaximumLogicalRecursionDepth;
        newMaximumLogicalRecursionDepth = -1;
    }
    if (newMaximumProcessingTime > 0) {
        maximumProcessingTime = newMaximumProcessingTime;
        newMaximumProcessingTime = -1;
    }
}