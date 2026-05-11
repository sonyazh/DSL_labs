// DSl_lab_1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
using namespace std;
#include "Grammar.h"
#include "Lexer.h"
#include "Parser.h"

int main()
{
    cout << "=== DSL LABs: Finite Automaton & Lexer ===" << endl << endl;

    // ===== PART 1: FINITE AUTOMATON TO REGULAR GRAMMAR =====
    cout << "PART 1: Finite Automaton to Regular Grammar Conversion" << endl;
    cout << "======================================================" << endl << endl;

    // Create a Finite Automaton based on the assignment
    FiniteAutomation fa;
    
    // Q = {q0, q1, q2, q3}
    fa.states = { '0', '1', '2', '3' };
    
    // Alphabet = {a, b, c}
    fa.alphabet = { 'a', 'b', 'c' };
    
    // Start state = q0
    fa.startState = '0';
    
    // Final states = {q3}
    fa.finalStates = { '3' };
    
    // Transitions:
    fa.transitions[{'0', 'a'}] = '1';  // δ(q0,a) = q1
    fa.transitions[{'1', 'b'}] = '1';  // δ(q1,b) = q1
    fa.transitions[{'1', 'a'}] = '2';  // δ(q1,a) = q2
    fa.transitions[{'2', 'c'}] = '3';  // δ(q2,c) = q3
    fa.transitions[{'3', 'c'}] = '3';  // δ(q3,c) = q3
    
    cout << "Testing Finite Automaton:" << endl;
    
    // Test words
    vector<string> testWords = { "abac", "abbac", "abbbac", "ababc" };
    for (auto word : testWords) {
        cout << "  Word '" << word << "': " << (fa.accepts(word) ? "ACCEPTED" : "REJECTED") << endl;
    }
    
    cout << "\nChecking if Automaton is Deterministic:" << endl;
    cout << "  Is Deterministic: " << (fa.isDeterministic() ? "YES" : "NO") << endl;
    
    cout << "\nConverting Finite Automaton to Regular Grammar:" << endl;
    
    // Convert FA to Grammar
    Grammar grammar = fa.toRegularGrammar();
    
    cout << "  Non-terminals (VN): ";
    for (char nt : grammar.VN) {
        cout << nt << " ";
    }
    cout << "\n  Terminals (VT): ";
    for (char t : grammar.VT) {
        cout << t << " ";
    }
    cout << "\n  Start symbol: " << grammar.start << endl;
    
    cout << "\n  Productions:" << endl;
    for (auto& p : grammar.productions) {
        cout << "    " << p.first << " -> ";
        for (int i = 0; i < p.second.size(); i++) {
            if (i > 0) cout << " | ";
            if (p.second[i].empty())
                cout << "ε";
            else
                cout << p.second[i];
        }
        cout << endl;
    }

    // ===== PART 2: LEXER DEMONSTRATION =====
    cout << "\n\nPART 2: Lexer Demonstration" << endl;
    cout << "============================" << endl << endl;

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

    // Example 4: While loop
    string code4 = "while (i < 100) i = i + 1;";
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
    cout << "Detailed Token Information:" << endl;
    string code5 = "void main() { int x = 42; }";
    cout << "Input: " << code5 << endl << endl;

    Lexer lexer5(code5);
    vector<Token> tokens5 = lexer5.tokenize();

    for (const auto& token : tokens5) {
        if (token.type != TokenType::END_OF_FILE) {
            cout << "  Type: " << token.getTypeName()
                 << " | Value: \"" << token.value
                 << "\" | Line: " << token.line
                 << " | Column: " << token.column << endl;
        }
    }

    // ===== PART 3: PARSER AND AST DEMONSTRATION =====
    cout << "\n\nPART 3: Parser and AST Demonstration" << endl;
    cout << "====================================" << endl << endl;

    string parserInput = R"(void main() {
    int x = 42;
    float result = (x + 8) / 2;
    if (result > 10) {
        x = x + 1;
    }
    while (x < 50) {
        x = x + 2;
    }
    return x;
})";

    cout << "Input:" << endl << parserInput << endl << endl;

    Lexer parserLexer(parserInput);
    vector<Token> parserTokens = parserLexer.tokenize();

    Parser parser(parserTokens);
    unique_ptr<ASTNode> ast = parser.parseProgram();

    if (parser.hasErrors()) {
        parser.printErrors();
    }
    else {
        cout << "Abstract Syntax Tree:" << endl;
        ast->print();
    }

    return 0;
}

