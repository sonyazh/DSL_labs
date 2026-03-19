#include "Token.h"

Token::Token() : type(TokenType::UNKNOWN), value(""), line(0), column(0) {}

Token::Token(TokenType type, const string& value, int line, int column)
    : type(type), value(value), line(line), column(column) {}

string Token::getTypeName() const {
    switch (type) {
        case TokenType::NUMBER:             return "NUMBER";
        case TokenType::IDENTIFIER:         return "IDENTIFIER";
        case TokenType::STRING:             return "STRING";
        case TokenType::KEYWORD_IF:         return "KEYWORD_IF";
        case TokenType::KEYWORD_ELSE:       return "KEYWORD_ELSE";
        case TokenType::KEYWORD_WHILE:      return "KEYWORD_WHILE";
        case TokenType::KEYWORD_FOR:        return "KEYWORD_FOR";
        case TokenType::KEYWORD_INT:        return "KEYWORD_INT";
        case TokenType::KEYWORD_FLOAT:      return "KEYWORD_FLOAT";
        case TokenType::KEYWORD_RETURN:     return "KEYWORD_RETURN";
        case TokenType::KEYWORD_VOID:       return "KEYWORD_VOID";
        case TokenType::PLUS:               return "PLUS";
        case TokenType::MINUS:              return "MINUS";
        case TokenType::MULTIPLY:           return "MULTIPLY";
        case TokenType::DIVIDE:             return "DIVIDE";
        case TokenType::ASSIGN:             return "ASSIGN";
        case TokenType::EQUAL:              return "EQUAL";
        case TokenType::NOT_EQUAL:          return "NOT_EQUAL";
        case TokenType::LESS:               return "LESS";
        case TokenType::GREATER:            return "GREATER";
        case TokenType::LESS_EQUAL:         return "LESS_EQUAL";
        case TokenType::GREATER_EQUAL:      return "GREATER_EQUAL";
        case TokenType::LPAREN:             return "LPAREN";
        case TokenType::RPAREN:             return "RPAREN";
        case TokenType::LBRACE:             return "LBRACE";
        case TokenType::RBRACE:             return "RBRACE";
        case TokenType::LBRACKET:           return "LBRACKET";
        case TokenType::RBRACKET:           return "RBRACKET";
        case TokenType::SEMICOLON:          return "SEMICOLON";
        case TokenType::COMMA:              return "COMMA";
        case TokenType::DOT:                return "DOT";
        case TokenType::END_OF_FILE:        return "END_OF_FILE";
        default:                            return "UNKNOWN";
    }
}

void Token::print() const {
    cout << "[" << getTypeName() << ": \"" << value << "\"] ";
}
