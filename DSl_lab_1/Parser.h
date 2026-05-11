#pragma once

#include "AST.h"
#include "Token.h"
#include <memory>
#include <string>
#include <vector>

using namespace std;

class Parser {
private:
    vector<Token> tokens;
    size_t position;
    vector<string> errors;

    Token currentToken() const;
    Token peekToken(int offset = 1) const;
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(TokenType type);
    Token advance();
    Token consume(TokenType type, const string& message);

    bool isTypeToken(TokenType type) const;
    bool isComparisonOperator(TokenType type) const;
    bool isAddOperator(TokenType type) const;
    bool isMultiplyOperator(TokenType type) const;

    unique_ptr<ASTNode> parseDeclarationOrFunction();
    unique_ptr<ASTNode> parseStatement();
    unique_ptr<ASTNode> parseBlock();
    unique_ptr<ASTNode> parseIfStatement();
    unique_ptr<ASTNode> parseWhileStatement();
    unique_ptr<ASTNode> parseReturnStatement();
    unique_ptr<ASTNode> parseAssignmentOrExpressionStatement();
    unique_ptr<ASTNode> parseExpression();
    unique_ptr<ASTNode> parseEquality();
    unique_ptr<ASTNode> parseComparison();
    unique_ptr<ASTNode> parseTerm();
    unique_ptr<ASTNode> parseFactor();
    unique_ptr<ASTNode> parseUnary();
    unique_ptr<ASTNode> parsePrimary();

public:
    Parser(const vector<Token>& tokens);

    unique_ptr<ASTNode> parseProgram();
    bool hasErrors() const;
    void printErrors() const;
};
