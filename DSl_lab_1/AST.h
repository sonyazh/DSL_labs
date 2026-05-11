#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;

enum class ASTNodeType {
    Program,
    Block,
    FunctionDeclaration,
    ParameterList,
    Parameter,
    VariableDeclaration,
    Assignment,
    IfStatement,
    WhileStatement,
    ReturnStatement,
    ExpressionStatement,
    BinaryExpression,
    UnaryExpression,
    Literal,
    Identifier,
    Type,
    Empty,
    Error
};

class ASTNode {
private:
    string getTypeName() const;

public:
    ASTNodeType type;
    string value;
    vector<unique_ptr<ASTNode>> children;

    ASTNode(ASTNodeType type, const string& value = "");

    void addChild(unique_ptr<ASTNode> child);
    void print(int indent = 0) const;
};
