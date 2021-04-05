#include "Lexer.h"

bool isInvalidCharacter(const char c) {
    // we will allow A-Z a-z 0-9 _ .
    // +-*/&|^=~%!;<>
    // ()[]"" backslash
    // this is most of the ASCII
    // characters in the range 32 to 126
    if (c < 32 || c > 126) {
        // outside the allowed range
        // this is a non-ASCII character
        // or maybe a tab?
        std::string message = "Code ";
        message += std::to_string((unsigned char)c);
        throw UserAlert(UserMessage::InvalidCharacter,message.c_str());
    }
    // in the allowed range, we will not allow
    // # $ ' : ? @ ` { }
    // outside of string-literals
    switch (c) {
        case '#':
        case '$':
        case '\'':
        case ':':
        case '?':
        case '@':
        case '\\':
        case '`':
        case '{':
        case '}':
        return true;

        default:
        return false;
    }
}

void prepareInput(std::string& s) {
    // we remove all the whitespace
    // except that which is in quotes
    // we also make sure that all ()[]""
    // are paired
    // we scan for invalid characters
    // and make sure that the input is a good length
    if (s.size() > MAX_INPUT_SIZE) {
        throw UserAlert(UserMessage::InputTooLong,nullptr);
    }
    // we can fit it in 32-bits
    long writeIndex = 0;
    long parenLevel = 0;
    long squareLevel = 0;
    bool inQuote = false;
    bool escapeQuote = false;
    for (long readIndex = 0; readIndex < s.size(); ++readIndex) {
        if (readIndex & 0xFF == 0) {
            checkProcessingTime();
            // check to see if we are running
            // overtime every 256 characters
        }
        // check for invalid characters
        const char c = s[readIndex];
        if (isInvalidCharacter(c)) {
            if (!inQuote) {
                std::string message;
                message.push_back(c);
                throw UserAlert(UserMessage::InvalidCharacter,message.c_str());
            }
        }
        // we have a valid character
        if (c != 32 || inQuote) {
            // we only copy it
            // to the output, if it is
            // a nonwhitespace, or
            // in a quote
            s[writeIndex++] = c;
        }
        // now check ()[]"" pairs
        if (inQuote) {
            // we are in a quote
            // we only care about the
            // end of the quote
            if (escapeQuote) {
                // we ignore the character
                // following a backslash
                escapeQuote = false;
            }
            else if (c == '"') {
                // end of quote
                inQuote = false;
            }
            else if (c == '\\') {
                // start of escape
                escapeQuote = true;
            }
        }
        else {
            // we are not in a quote
            switch (c) {
                case '"':
                // start of quote
                inQuote = true;
                break;

                case '(':
                ++parenLevel;
                break;

                case ')':
                if (parenLevel) {
                    --parenLevel;
                    break;
                }
                else {
                    // we have unmatched ()
                    throw UserAlert(UserMessage::SyntaxError,"Unmatched ()");
                }

                case '[':
                ++squareLevel;
                break;

                case ']':
                if (squareLevel) {
                    --squareLevel;
                    break;
                }
                else {
                    // we have unmatched []
                    throw UserAlert(UserMessage::SyntaxError,"Unmatched []");
                }

                default:
                // we don't care
                // what character this is
                break;
            }
        }
    }
    // need to check for valid end of input conditions
    if (parenLevel || squareLevel || inQuote) {
        throw UserAlert(UserMessage::SyntaxError,"Invalid End of Input");
    }
    // now need to remove extra characters
    // at the end of s
    // writeIndex is incremented for every character
    // written, it is the new length of s
    s.resize(writeIndex);
}