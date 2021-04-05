#pragma once
#include "../Globals/Globals.h"

typedef unsigned char LexerTokenInt;

enum class LexerToken: LexerTokenInt {
    Number,
    SymbolName,
    Quote,
    OpenContainer, // ([
    CloseContainer, // )]
    UnitaryOperatorBefore, // can only be +-!
    UnitaryOperatorAfter, // can only be !
    BinaryOperator1of1, // single character operator
    BinaryOperator1of2, // multi-character operator
    BinaryOperator2of2 // multi-character operator
};

typedef uint_fast16_t LexerStateInt;

enum class LexerState: LexerStateInt {
    StartOfInput = 0x0001,
    Number = 0x0002,
    SymbolName = 0x0004,
    Quote = 0x0008,
    QuoteEscape = 0x0010,
    QuoteEnd = 0x0020,
    OpenContainer = 0x0040,
    CloseContainer = 0x0080,
    UnitaryOperator = 0x0100,
    BinaryOperator1 = 0x0200,
    BinaryOperator2 = 0x0400,
    // bitmasks
    Mask_InQuote = Quote | QuoteEscape,
    Mask_BinaryOperator = BinaryOperator1 | BinaryOperator2,
    Mask_Operator = Mask_BinaryOperator | UnitaryOperator,
    Mask_StartOfExpression = StartOfInput | OpenContainer | Mask_Operator,
    Mask_AllowUnitaryAfter = Number | SymbolName | CloseContainer,
    Prohibit_Numeric = SymbolName | QuoteEnd | CloseContainer,
    Prohibit_AlphaOpen = Number | QuoteEnd | CloseContainer,
    Prohibit_Close = StartOfInput | Mask_BinaryOperator,
    Prohibit_Quote = Number | SymbolName | QuoteEnd | CloseContainer | UnitaryOperator,
    Prohibit_BinaryOperator = StartOfInput | OpenContainer | BinaryOperator2
};

void prepareInput(std::string&);

void lexInput(std::vector<LexerToken>&,const std::string&);