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

long maximumRecursionDepth = 500;
long maximumLogicalRecursionDepth = 10000;
double maximumProcessingTime = 120; // in seconds
time_t processingStartTime; // initially undefined

long newMaximumRecursionDepth = -1;
long newMaximumLogicalRecursionDepth = -1;
double newMaximumProcessingTime = -1;

void checkProcessingTime() {
    if (difftime(time(NULL),processingStartTime) >= maximumProcessingTime) {
        throw UserAlert(UserMessage::Timeout,nullptr);
    }
}

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