#pragma once
#include <string>
#include <iostream>

using namespace std;

enum class TokenType {
    // Literals
    NUMBER,
    IDENTIFIER,
    STRING,

    // Keywords
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_WHILE,
    KEYWORD_FOR,
    KEYWORD_INT,
    KEYWORD_FLOAT,
    KEYWORD_RETURN,
    KEYWORD_VOID,

    // Operators
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    ASSIGN,
    EQUAL,
    NOT_EQUAL,
    LESS,
    GREATER,
    LESS_EQUAL,
    GREATER_EQUAL,

    // Delimiters
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    SEMICOLON,
    COMMA,
    DOT,

    // Special
    END_OF_FILE,
    UNKNOWN
};

class Token {
public:
    TokenType type;
    string value;
    int line;
    int column;

    Token();
    Token(TokenType type, const string& value, int line = 0, int column = 0);

    string getTypeName() const;
    void print() const;
};
