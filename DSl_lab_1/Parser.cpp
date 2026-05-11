#include "Parser.h"
#include <iostream>

Parser::Parser(const vector<Token>& tokens)
    : tokens(tokens), position(0) {}

Token Parser::currentToken() const {
    if (position >= tokens.size()) {
        return Token(TokenType::END_OF_FILE, "");
    }
    return tokens[position];
}

Token Parser::peekToken(int offset) const {
    if (position + offset >= tokens.size()) {
        return Token(TokenType::END_OF_FILE, "");
    }
    return tokens[position + offset];
}

bool Parser::isAtEnd() const {
    return currentToken().type == TokenType::END_OF_FILE;
}

bool Parser::check(TokenType type) const {
    return currentToken().type == type;
}

bool Parser::match(TokenType type) {
    if (!check(type)) {
        return false;
    }
    advance();
    return true;
}

Token Parser::advance() {
    if (!isAtEnd()) {
        position++;
    }
    return tokens[position - 1];
}

Token Parser::consume(TokenType type, const string& message) {
    if (check(type)) {
        return advance();
    }

    Token token = currentToken();
    errors.push_back(message + " at line " + to_string(token.line) +
        ", column " + to_string(token.column) + ". Found '" + token.value + "'.");
    return Token(type, "", token.line, token.column);
}

bool Parser::isTypeToken(TokenType type) const {
    return type == TokenType::KEYWORD_INT ||
           type == TokenType::KEYWORD_FLOAT ||
           type == TokenType::KEYWORD_VOID;
}

bool Parser::isComparisonOperator(TokenType type) const {
    return type == TokenType::LESS ||
           type == TokenType::GREATER ||
           type == TokenType::LESS_EQUAL ||
           type == TokenType::GREATER_EQUAL;
}

bool Parser::isAddOperator(TokenType type) const {
    return type == TokenType::PLUS || type == TokenType::MINUS;
}

bool Parser::isMultiplyOperator(TokenType type) const {
    return type == TokenType::MULTIPLY || type == TokenType::DIVIDE;
}

unique_ptr<ASTNode> Parser::parseProgram() {
    unique_ptr<ASTNode> program = make_unique<ASTNode>(ASTNodeType::Program);

    while (!isAtEnd()) {
        if (isTypeToken(currentToken().type)) {
            program->addChild(parseDeclarationOrFunction());
        }
        else {
            program->addChild(parseStatement());
        }
    }

    return program;
}

unique_ptr<ASTNode> Parser::parseDeclarationOrFunction() {
    Token typeToken = advance();
    Token nameToken = consume(TokenType::IDENTIFIER, "Expected identifier after type");

    if (match(TokenType::LPAREN)) {
        unique_ptr<ASTNode> functionNode =
            make_unique<ASTNode>(ASTNodeType::FunctionDeclaration, nameToken.value);
        functionNode->addChild(make_unique<ASTNode>(ASTNodeType::Type, typeToken.value));

        unique_ptr<ASTNode> parameters = make_unique<ASTNode>(ASTNodeType::ParameterList);
        if (!check(TokenType::RPAREN)) {
            do {
                Token paramType = consume(currentToken().type, "Expected parameter type");
                if (!isTypeToken(paramType.type)) {
                    errors.push_back("Expected parameter type at line " + to_string(paramType.line) +
                        ", column " + to_string(paramType.column) + ".");
                }
                Token paramName = consume(TokenType::IDENTIFIER, "Expected parameter name");
                unique_ptr<ASTNode> parameter =
                    make_unique<ASTNode>(ASTNodeType::Parameter, paramName.value);
                parameter->addChild(make_unique<ASTNode>(ASTNodeType::Type, paramType.value));
                parameters->addChild(move(parameter));
            } while (match(TokenType::COMMA));
        }

        consume(TokenType::RPAREN, "Expected ')' after function parameters");
        functionNode->addChild(move(parameters));
        consume(TokenType::LBRACE, "Expected '{' before function body");
        functionNode->addChild(parseBlock());
        return functionNode;
    }

    unique_ptr<ASTNode> declaration =
        make_unique<ASTNode>(ASTNodeType::VariableDeclaration, nameToken.value);
    declaration->addChild(make_unique<ASTNode>(ASTNodeType::Type, typeToken.value));

    if (match(TokenType::ASSIGN)) {
        declaration->addChild(parseExpression());
    }

    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
    return declaration;
}

unique_ptr<ASTNode> Parser::parseStatement() {
    if (match(TokenType::LBRACE)) {
        return parseBlock();
    }
    if (match(TokenType::KEYWORD_IF)) {
        return parseIfStatement();
    }
    if (match(TokenType::KEYWORD_WHILE)) {
        return parseWhileStatement();
    }
    if (match(TokenType::KEYWORD_RETURN)) {
        return parseReturnStatement();
    }
    if (isTypeToken(currentToken().type)) {
        return parseDeclarationOrFunction();
    }
    if (match(TokenType::SEMICOLON)) {
        return make_unique<ASTNode>(ASTNodeType::Empty);
    }

    return parseAssignmentOrExpressionStatement();
}

unique_ptr<ASTNode> Parser::parseBlock() {
    unique_ptr<ASTNode> block = make_unique<ASTNode>(ASTNodeType::Block);

    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        block->addChild(parseStatement());
    }

    consume(TokenType::RBRACE, "Expected '}' after block");
    return block;
}

unique_ptr<ASTNode> Parser::parseIfStatement() {
    unique_ptr<ASTNode> ifNode = make_unique<ASTNode>(ASTNodeType::IfStatement);

    consume(TokenType::LPAREN, "Expected '(' after if");
    ifNode->addChild(parseExpression());
    consume(TokenType::RPAREN, "Expected ')' after if condition");
    ifNode->addChild(parseStatement());

    if (match(TokenType::KEYWORD_ELSE)) {
        ifNode->addChild(parseStatement());
    }

    return ifNode;
}

unique_ptr<ASTNode> Parser::parseWhileStatement() {
    unique_ptr<ASTNode> whileNode = make_unique<ASTNode>(ASTNodeType::WhileStatement);

    consume(TokenType::LPAREN, "Expected '(' after while");
    whileNode->addChild(parseExpression());
    consume(TokenType::RPAREN, "Expected ')' after while condition");
    whileNode->addChild(parseStatement());

    return whileNode;
}

unique_ptr<ASTNode> Parser::parseReturnStatement() {
    unique_ptr<ASTNode> returnNode = make_unique<ASTNode>(ASTNodeType::ReturnStatement);

    if (!check(TokenType::SEMICOLON)) {
        returnNode->addChild(parseExpression());
    }

    consume(TokenType::SEMICOLON, "Expected ';' after return statement");
    return returnNode;
}

unique_ptr<ASTNode> Parser::parseAssignmentOrExpressionStatement() {
    if (check(TokenType::IDENTIFIER) && peekToken().type == TokenType::ASSIGN) {
        Token nameToken = advance();
        advance();

        unique_ptr<ASTNode> assignment =
            make_unique<ASTNode>(ASTNodeType::Assignment, nameToken.value);
        assignment->addChild(parseExpression());
        consume(TokenType::SEMICOLON, "Expected ';' after assignment");
        return assignment;
    }

    unique_ptr<ASTNode> expressionStatement =
        make_unique<ASTNode>(ASTNodeType::ExpressionStatement);
    expressionStatement->addChild(parseExpression());
    consume(TokenType::SEMICOLON, "Expected ';' after expression");
    return expressionStatement;
}

unique_ptr<ASTNode> Parser::parseExpression() {
    return parseEquality();
}

unique_ptr<ASTNode> Parser::parseEquality() {
    unique_ptr<ASTNode> expression = parseComparison();

    while (check(TokenType::EQUAL) || check(TokenType::NOT_EQUAL)) {
        Token operatorToken = advance();
        unique_ptr<ASTNode> binary =
            make_unique<ASTNode>(ASTNodeType::BinaryExpression, operatorToken.value);
        binary->addChild(move(expression));
        binary->addChild(parseComparison());
        expression = move(binary);
    }

    return expression;
}

unique_ptr<ASTNode> Parser::parseComparison() {
    unique_ptr<ASTNode> expression = parseTerm();

    while (isComparisonOperator(currentToken().type)) {
        Token operatorToken = advance();
        unique_ptr<ASTNode> binary =
            make_unique<ASTNode>(ASTNodeType::BinaryExpression, operatorToken.value);
        binary->addChild(move(expression));
        binary->addChild(parseTerm());
        expression = move(binary);
    }

    return expression;
}

unique_ptr<ASTNode> Parser::parseTerm() {
    unique_ptr<ASTNode> expression = parseFactor();

    while (isAddOperator(currentToken().type)) {
        Token operatorToken = advance();
        unique_ptr<ASTNode> binary =
            make_unique<ASTNode>(ASTNodeType::BinaryExpression, operatorToken.value);
        binary->addChild(move(expression));
        binary->addChild(parseFactor());
        expression = move(binary);
    }

    return expression;
}

unique_ptr<ASTNode> Parser::parseFactor() {
    unique_ptr<ASTNode> expression = parseUnary();

    while (isMultiplyOperator(currentToken().type)) {
        Token operatorToken = advance();
        unique_ptr<ASTNode> binary =
            make_unique<ASTNode>(ASTNodeType::BinaryExpression, operatorToken.value);
        binary->addChild(move(expression));
        binary->addChild(parseUnary());
        expression = move(binary);
    }

    return expression;
}

unique_ptr<ASTNode> Parser::parseUnary() {
    if (check(TokenType::MINUS)) {
        Token operatorToken = advance();
        unique_ptr<ASTNode> unary =
            make_unique<ASTNode>(ASTNodeType::UnaryExpression, operatorToken.value);
        unary->addChild(parseUnary());
        return unary;
    }

    return parsePrimary();
}

unique_ptr<ASTNode> Parser::parsePrimary() {
    if (check(TokenType::NUMBER) || check(TokenType::STRING)) {
        Token literal = advance();
        return make_unique<ASTNode>(ASTNodeType::Literal, literal.value);
    }

    if (check(TokenType::IDENTIFIER)) {
        Token identifier = advance();
        return make_unique<ASTNode>(ASTNodeType::Identifier, identifier.value);
    }

    if (match(TokenType::LPAREN)) {
        unique_ptr<ASTNode> expression = parseExpression();
        consume(TokenType::RPAREN, "Expected ')' after expression");
        return expression;
    }

    Token token = currentToken();
    errors.push_back("Expected expression at line " + to_string(token.line) +
        ", column " + to_string(token.column) + ". Found '" + token.value + "'.");
    advance();
    return make_unique<ASTNode>(ASTNodeType::Error, token.value);
}

bool Parser::hasErrors() const {
    return !errors.empty();
}

void Parser::printErrors() const {
    for (const string& error : errors) {
        cout << "Parser error: " << error << endl;
    }
}
