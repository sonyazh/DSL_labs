#pragma once
#include "Token.h"
#include <vector>
#include <string>

using namespace std;

class Lexer {
private:
    string input;
    size_t position;
    int line;
    int column;

    char currentChar();
    char peekChar(int offset = 1);
    void advance();
    void skipWhitespace();
    void skipComments();

    Token readNumber();
    Token readIdentifierOrKeyword();
    Token readString();

    TokenType checkKeyword(const string& word);
    TokenType identifyTokenType(const string& lexeme);
    bool isDigit(char c) const;
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;

public:
    Lexer(const string& input);
    vector<Token> tokenize();
    Token nextToken();
};
