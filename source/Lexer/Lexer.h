/**
 * @file Lexer.h
 * @author Aaron Stanek
 * @brief It lexes user input
*/
#pragma once
#include "../Globals/Globals.h"

typedef unsigned char LexerTokenInt;

/// Identifies the lexical classification
/// of an input character.
/// The values are bitmask-compatible.
enum class LexerToken : LexerTokenInt {
    // do primative bitmasks first
    /// bitmask: Checks if token can have Multiply suffix.
    CanMultiply = 0x80,
    /// bitmask: Checks if token has multiply suffix.
    IsMultiply = 0x40,
    // now the actual values
    /// End Of Input
    EOI = 0x01,
    /// Whitespace
    White = 0x02,
    /// Number
    Number = CanMultiply | 0x01,
    /// Number preceded by implicit multiplication
    NumberMultiply = Number | IsMultiply,
    /// Symbol
    Symbol = CanMultiply | 0x02,
    /// Symbol preceded by implicit multiplication
    SymbolMultiply = Symbol | IsMultiply,
    /// Symbol followed by (
    Function = CanMultiply | 0x04,
    /// Function preceded by implicit multiplication
    FunctionMultiply = Function | IsMultiply,
    /// Quote
    Quote = CanMultiply | 0x08,
    /// Quote preceded by implicit multiplication
    QuoteMultiply = Quote | IsMultiply,
    /// ( or [ or .[
    Open = CanMultiply | 0x10,
    /// Open preceded by implicit multiplication
    OpenMultiply = Open | IsMultiply,
    /// ) or ]
    Close = 0x04,
    /// Unitary Operator Before expression
    UOB = 0x08,
    /// Unitary Operator After expression
    UOA = 0x10,
    /// Binary Operator
    BO = 0x20,
    /// bitmask: can be used compare values without considering
    /// the IsMultiply bit.\n
    /// e.g. x & IgnoreMultiply == Number\n
    /// This will check if x is a Number or NumberMultiply.
    /// This mask is only valid if the CanMultiply bit is 1.
    IgnoreMultiply = 0x1F,
    /// bitmask: Symbol, SymbolMultiply, Function, FunctionMultiply.
    /// This mask is only valid if the CanMultiply bit is 1.
    IsSymbolLike = 0x02 | 0x04,
    /// bitmask: EOI or Close.
    /// This mask is only valid if the CanMultiply bit is 0.
    Terminal = EOI | Close,
    /// bitmask: UOB or UOA.
    /// This mask is only valid if the CanMultiply bit is 0.
    Unitary = UOB | UOA,
    /// bitmask UOB, UOA, BO.
    /// This mask is only valid if the CanMultiply bit is 0.
    Operator = Unitary | BO
};

void lexer(std::vector<LexerToken>&, std::string&, long);