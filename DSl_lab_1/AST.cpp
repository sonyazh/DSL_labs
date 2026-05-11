#include "AST.h"
#include <utility>

ASTNode::ASTNode(ASTNodeType type, const string& value)
    : type(type), value(value) {}

void ASTNode::addChild(unique_ptr<ASTNode> child) {
    if (child) {
        children.push_back(move(child));
    }
}

string ASTNode::getTypeName() const {
    switch (type) {
        case ASTNodeType::Program:              return "Program";
        case ASTNodeType::Block:                return "Block";
        case ASTNodeType::FunctionDeclaration:  return "FunctionDeclaration";
        case ASTNodeType::ParameterList:        return "ParameterList";
        case ASTNodeType::Parameter:            return "Parameter";
        case ASTNodeType::VariableDeclaration:  return "VariableDeclaration";
        case ASTNodeType::Assignment:           return "Assignment";
        case ASTNodeType::IfStatement:          return "IfStatement";
        case ASTNodeType::WhileStatement:       return "WhileStatement";
        case ASTNodeType::ReturnStatement:      return "ReturnStatement";
        case ASTNodeType::ExpressionStatement:  return "ExpressionStatement";
        case ASTNodeType::BinaryExpression:     return "BinaryExpression";
        case ASTNodeType::UnaryExpression:      return "UnaryExpression";
        case ASTNodeType::Literal:              return "Literal";
        case ASTNodeType::Identifier:           return "Identifier";
        case ASTNodeType::Type:                 return "Type";
        case ASTNodeType::Empty:                return "Empty";
        default:                                return "Error";
    }
}

void ASTNode::print(int indent) const {
    for (int i = 0; i < indent; i++) {
        cout << "  ";
    }

    cout << getTypeName();
    if (!value.empty()) {
        cout << ": " << value;
    }
    cout << endl;

    for (const auto& child : children) {
        child->print(indent + 1);
    }
}
