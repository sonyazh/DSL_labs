// Lexer Demonstration Program

#include <iostream>
#include "Lexer.h"

using namespace std;

int main() {
    cout << "=== LEXER DEMONSTRATION ===" << endl << endl;

    // Example 1: Simple variable declaration
    string code1 = "int x = 42;";
    cout << "Input: " << code1 << endl;
    cout << "Tokens: ";

    Lexer lexer1(code1);
    vector<Token> tokens1 = lexer1.tokenize();

    for (const auto& token : tokens1) {
        if (token.type != TokenType::END_OF_FILE)
            token.print();
    }
    cout << endl << endl;

    // Example 2: If statement
    string code2 = "if (x > 10) { y = x * 2; }";
    cout << "Input: " << code2 << endl;
    cout << "Tokens: ";

    Lexer lexer2(code2);
    vector<Token> tokens2 = lexer2.tokenize();

    for (const auto& token : tokens2) {
        if (token.type != TokenType::END_OF_FILE)
            token.print();
    }
    cout << endl << endl;

    // Example 3: Complex expression
    string code3 = "float result = (a + b) / (c - d);";
    cout << "Input: " << code3 << endl;
    cout << "Tokens: ";

    Lexer lexer3(code3);
    vector<Token> tokens3 = lexer3.tokenize();

    for (const auto& token : tokens3) {
        if (token.type != TokenType::END_OF_FILE)
            token.print();
    }
    cout << endl << endl;

    // Example 4: While loop with string
    string code4 = R"(while (i < 100) i = i + 1;)";
    cout << "Input: " << code4 << endl;
    cout << "Tokens: ";

    Lexer lexer4(code4);
    vector<Token> tokens4 = lexer4.tokenize();

    for (const auto& token : tokens4) {
        if (token.type != TokenType::END_OF_FILE)
            token.print();
    }
    cout << endl << endl;

    // Example 5: Detailed output with line/column info
    cout << "=== DETAILED TOKEN INFORMATION ===" << endl << endl;

    string code5 = R"(void main() {
    int x = 42;
    return x;
})";

    cout << "Input:" << endl << code5 << endl << endl;
    cout << "Detailed Tokens:" << endl;

    Lexer lexer5(code5);
    vector<Token> tokens5 = lexer5.tokenize();

    for (const auto& token : tokens5) {
        if (token.type != TokenType::END_OF_FILE) {
            cout << "Type: " << token.getTypeName()
                 << " | Value: \"" << token.value
                 << "\" | Line: " << token.line
                 << " | Column: " << token.column << endl;
        }
    }

    return 0;
}
