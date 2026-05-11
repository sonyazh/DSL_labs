#include "Lexer.h"
#include <cctype>
#include <regex>

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
    if (regex_match(word, regex("if")))       return TokenType::KEYWORD_IF;
    if (regex_match(word, regex("else")))     return TokenType::KEYWORD_ELSE;
    if (regex_match(word, regex("while")))    return TokenType::KEYWORD_WHILE;
    if (regex_match(word, regex("for")))      return TokenType::KEYWORD_FOR;
    if (regex_match(word, regex("int")))      return TokenType::KEYWORD_INT;
    if (regex_match(word, regex("float")))    return TokenType::KEYWORD_FLOAT;
    if (regex_match(word, regex("return")))   return TokenType::KEYWORD_RETURN;
    if (regex_match(word, regex("void")))     return TokenType::KEYWORD_VOID;
    return TokenType::IDENTIFIER;
}

TokenType Lexer::identifyTokenType(const string& lexeme) {
    if (regex_match(lexeme, regex("[0-9]+(\\.[0-9]+)?"))) {
        return TokenType::NUMBER;
    }
    if (regex_match(lexeme, regex("[A-Za-z_][A-Za-z0-9_]*"))) {
        return checkKeyword(lexeme);
    }
    if (regex_match(lexeme, regex("\"([^\"\\\\]|\\\\.)*\""))) {
        return TokenType::STRING;
    }
    if (regex_match(lexeme, regex("\\+"))) return TokenType::PLUS;
    if (regex_match(lexeme, regex("-"))) return TokenType::MINUS;
    if (regex_match(lexeme, regex("\\*"))) return TokenType::MULTIPLY;
    if (regex_match(lexeme, regex("/"))) return TokenType::DIVIDE;
    if (regex_match(lexeme, regex("="))) return TokenType::ASSIGN;
    if (regex_match(lexeme, regex("=="))) return TokenType::EQUAL;
    if (regex_match(lexeme, regex("!="))) return TokenType::NOT_EQUAL;
    if (regex_match(lexeme, regex("<"))) return TokenType::LESS;
    if (regex_match(lexeme, regex(">"))) return TokenType::GREATER;
    if (regex_match(lexeme, regex("<="))) return TokenType::LESS_EQUAL;
    if (regex_match(lexeme, regex(">="))) return TokenType::GREATER_EQUAL;
    if (regex_match(lexeme, regex("\\("))) return TokenType::LPAREN;
    if (regex_match(lexeme, regex("\\)"))) return TokenType::RPAREN;
    if (regex_match(lexeme, regex("\\{"))) return TokenType::LBRACE;
    if (regex_match(lexeme, regex("\\}"))) return TokenType::RBRACE;
    if (regex_match(lexeme, regex("\\["))) return TokenType::LBRACKET;
    if (regex_match(lexeme, regex("\\]"))) return TokenType::RBRACKET;
    if (regex_match(lexeme, regex(";"))) return TokenType::SEMICOLON;
    if (regex_match(lexeme, regex(","))) return TokenType::COMMA;
    if (regex_match(lexeme, regex("\\."))) return TokenType::DOT;
    return TokenType::UNKNOWN;
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

    return Token(identifyTokenType(number), number, line, startCol);
}

Token Lexer::readIdentifierOrKeyword() {
    int startCol = column;
    string identifier;

    while (isAlphaNumeric(currentChar())) {
        identifier += currentChar();
        advance();
    }

    TokenType type = identifyTokenType(identifier);
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
        return Token(identifyTokenType("=="), "==", line, startCol);
    }
    if (ch == '!' && currentChar() == '=') {
        advance();
        return Token(identifyTokenType("!="), "!=", line, startCol);
    }
    if (ch == '<' && currentChar() == '=') {
        advance();
        return Token(identifyTokenType("<="), "<=", line, startCol);
    }
    if (ch == '>' && currentChar() == '=') {
        advance();
        return Token(identifyTokenType(">="), ">=", line, startCol);
    }

    // Single-character tokens
    switch (ch) {
        case '+':  return Token(identifyTokenType("+"), "+", line, startCol);
        case '-':  return Token(identifyTokenType("-"), "-", line, startCol);
        case '*':  return Token(identifyTokenType("*"), "*", line, startCol);
        case '/':  return Token(identifyTokenType("/"), "/", line, startCol);
        case '=':  return Token(identifyTokenType("="), "=", line, startCol);
        case '<':  return Token(identifyTokenType("<"), "<", line, startCol);
        case '>':  return Token(identifyTokenType(">"), ">", line, startCol);
        case '(':  return Token(identifyTokenType("("), "(", line, startCol);
        case ')':  return Token(identifyTokenType(")"), ")", line, startCol);
        case '{':  return Token(identifyTokenType("{"), "{", line, startCol);
        case '}':  return Token(identifyTokenType("}"), "}", line, startCol);
        case '[':  return Token(identifyTokenType("["), "[", line, startCol);
        case ']':  return Token(identifyTokenType("]"), "]", line, startCol);
        case ';':  return Token(identifyTokenType(";"), ";", line, startCol);
        case ',':  return Token(identifyTokenType(","), ",", line, startCol);
        case '.':  return Token(identifyTokenType("."), ".", line, startCol);
        default:   return Token(identifyTokenType(string(1, ch)), string(1, ch), line, startCol);
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
