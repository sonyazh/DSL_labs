#include "Lexer.h"
#include <cctype>

Lexer::Lexer(const string& input)
    : input(input), position(0), line(1), column(1) {}

char Lexer::currentChar() {
    if (position >= input.length())
        return '\0';
    return input[position];
}

char Lexer::peekChar(int offset) {
    if (position + offset >= input.length())
        return '\0';
    return input[position + offset];
}

void Lexer::advance() {
    if (position < input.length()) {
        if (input[position] == '\n') {
            line++;
            column = 1;
        }
        else {
            column++;
        }
        position++;
    }
}

void Lexer::skipWhitespace() {
    while (currentChar() != '\0' && isspace(currentChar())) {
        advance();
    }
}

void Lexer::skipComments() {
    if (currentChar() == '/' && peekChar() == '/') {
        while (currentChar() != '\0' && currentChar() != '\n') {
            advance();
        }
        if (currentChar() == '\n')
            advance();
    }
}

bool Lexer::isDigit(char c) const {
    return c >= '0' && c <= '9';
}

bool Lexer::isAlpha(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

TokenType Lexer::checkKeyword(const string& word) {
    if (word == "if")       return TokenType::KEYWORD_IF;
    if (word == "else")     return TokenType::KEYWORD_ELSE;
    if (word == "while")    return TokenType::KEYWORD_WHILE;
    if (word == "for")      return TokenType::KEYWORD_FOR;
    if (word == "int")      return TokenType::KEYWORD_INT;
    if (word == "float")    return TokenType::KEYWORD_FLOAT;
    if (word == "return")   return TokenType::KEYWORD_RETURN;
    if (word == "void")     return TokenType::KEYWORD_VOID;
    return TokenType::IDENTIFIER;
}

Token Lexer::readNumber() {
    int startCol = column;
    string number;

    while (isDigit(currentChar())) {
        number += currentChar();
        advance();
    }

    // Handle decimal numbers
    if (currentChar() == '.' && isDigit(peekChar())) {
        number += currentChar();
        advance();
        while (isDigit(currentChar())) {
            number += currentChar();
            advance();
        }
    }

    return Token(TokenType::NUMBER, number, line, startCol);
}

Token Lexer::readIdentifierOrKeyword() {
    int startCol = column;
    string identifier;

    while (isAlphaNumeric(currentChar())) {
        identifier += currentChar();
        advance();
    }

    TokenType type = checkKeyword(identifier);
    return Token(type, identifier, line, startCol);
}

Token Lexer::readString() {
    int startCol = column;
    string str;

    advance(); // skip opening quote
    while (currentChar() != '\0' && currentChar() != '"') {
        if (currentChar() == '\\') {
            advance();
            switch (currentChar()) {
                case 'n':  str += '\n'; break;
                case 't':  str += '\t'; break;
                case '\\': str += '\\'; break;
                case '"':  str += '"';  break;
                default:   str += currentChar();
            }
            advance();
        }
        else {
            str += currentChar();
            advance();
        }
    }

    if (currentChar() == '"')
        advance(); // skip closing quote

    return Token(TokenType::STRING, str, line, startCol);
}

Token Lexer::nextToken() {
    skipWhitespace();
    skipComments();

    int startCol = column;

    if (currentChar() == '\0')
        return Token(TokenType::END_OF_FILE, "", line, startCol);

    // Numbers
    if (isDigit(currentChar()))
        return readNumber();

    // Identifiers and Keywords
    if (isAlpha(currentChar()))
        return readIdentifierOrKeyword();

    // Strings
    if (currentChar() == '"')
        return readString();

    // Operators and Delimiters
    char ch = currentChar();
    advance();

    // Two-character operators
    if (ch == '=' && currentChar() == '=') {
        advance();
        return Token(TokenType::EQUAL, "==", line, startCol);
    }
    if (ch == '!' && currentChar() == '=') {
        advance();
        return Token(TokenType::NOT_EQUAL, "!=", line, startCol);
    }
    if (ch == '<' && currentChar() == '=') {
        advance();
        return Token(TokenType::LESS_EQUAL, "<=", line, startCol);
    }
    if (ch == '>' && currentChar() == '=') {
        advance();
        return Token(TokenType::GREATER_EQUAL, ">=", line, startCol);
    }

    // Single-character tokens
    switch (ch) {
        case '+':  return Token(TokenType::PLUS, "+", line, startCol);
        case '-':  return Token(TokenType::MINUS, "-", line, startCol);
        case '*':  return Token(TokenType::MULTIPLY, "*", line, startCol);
        case '/':  return Token(TokenType::DIVIDE, "/", line, startCol);
        case '=':  return Token(TokenType::ASSIGN, "=", line, startCol);
        case '<':  return Token(TokenType::LESS, "<", line, startCol);
        case '>':  return Token(TokenType::GREATER, ">", line, startCol);
        case '(':  return Token(TokenType::LPAREN, "(", line, startCol);
        case ')':  return Token(TokenType::RPAREN, ")", line, startCol);
        case '{':  return Token(TokenType::LBRACE, "{", line, startCol);
        case '}':  return Token(TokenType::RBRACE, "}", line, startCol);
        case '[':  return Token(TokenType::LBRACKET, "[", line, startCol);
        case ']':  return Token(TokenType::RBRACKET, "]", line, startCol);
        case ';':  return Token(TokenType::SEMICOLON, ";", line, startCol);
        case ',':  return Token(TokenType::COMMA, ",", line, startCol);
        case '.':  return Token(TokenType::DOT, ".", line, startCol);
        default:   return Token(TokenType::UNKNOWN, string(1, ch), line, startCol);
    }
}

vector<Token> Lexer::tokenize() {
    vector<Token> tokens;

    while (true) {
        Token token = nextToken();
        tokens.push_back(token);
        if (token.type == TokenType::END_OF_FILE)
            break;
    }

    return tokens;
}
