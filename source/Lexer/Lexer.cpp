/**
 * @file Lexer.h
 * @author Aaron Stanek
*/
#include "Lexer.h"

/// Assigns lexical meaning to each character
/// of the user input. Checks for invalid characters
/// and ensures that the input conforms to the language grammar.
/// @param output Where the output will go, will have the same length
/// as input, with 1-to-1 correspondence in the indicies
/// @param input Where the input comes from, an end of line character will
/// be appended to the end
/// @param recursionJuice how many layers of recursion may be used by this operation
void lexer(std::vector<LexerToken>& output, std::string& input, long recursionJuice) {

}