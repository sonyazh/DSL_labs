// DSl_lab_1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
using namespace std;
#include "Grammar.h"

int main()
{
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
    
    cout << "=== Testing Finite Automaton ===" << endl;
    
    // Test words
    vector<string> testWords = { "abac", "abbac", "abbbac", "ababc" };
    for (auto word : testWords) {
        cout << "Word '" << word << "': " << (fa.accepts(word) ? "ACCEPTED" : "REJECTED") << endl;
    }
    
    cout << "\n=== Checking if Automaton is Deterministic ===" << endl;
    cout << "Is Deterministic: " << (fa.isDeterministic() ? "YES" : "NO") << endl;
    
    cout << "\n=== Converting Finite Automaton to Regular Grammar ===" << endl;
    
    // Convert FA to Grammar
    Grammar grammar = fa.toRegularGrammar();
    
    cout << "\nNon-terminals (VN): ";
    for (char nt : grammar.VN) {
        cout << nt << " ";
    }
    cout << "\nTerminals (VT): ";
    for (char t : grammar.VT) {
        cout << t << " ";
    }
    cout << "\nStart symbol: " << grammar.start << endl;
    
    cout << "\nProductions:" << endl;
    for (auto& p : grammar.productions) {
        cout << p.first << " -> ";
        for (int i = 0; i < p.second.size(); i++) {
            if (i > 0) cout << " | ";
            if (p.second[i].empty())
                cout << "ε";
            else
                cout << p.second[i];
        }
        cout << endl;
    }
    
    return 0;
}

