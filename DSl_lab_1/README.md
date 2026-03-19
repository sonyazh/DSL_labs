# DSL Lab 1: Finite Automaton to Regular Grammar Conversion

## Lab Name
**Converting Finite Automata to Regular Grammars**

## Creator
[Your Name Here]

---

## Theory

### Finite Automata (FA)
A Finite Automaton is a mathematical model of computation consisting of:
- **Q**: A finite set of states
- **Σ**: A finite input alphabet
- **δ**: A transition function (δ: Q × Σ → Q)
- **q₀**: An initial/start state
- **F**: A set of final/accepting states

A deterministic finite automaton (DFA) processes input strings one symbol at a time, transitioning between states according to the transition function. A string is accepted if, after processing all symbols, the automaton is in a final state.

### Regular Grammar
A regular grammar (Type 3 in Chomsky hierarchy) is a formal grammar with productions of the form:
- **A → aB** (right-linear: non-terminal on the right)
- **A → a** (terminal only)
- **A → ε** (epsilon/empty production)

Where **A** and **B** are non-terminals, and **a** is a terminal symbol.

### Conversion Algorithm: FA → Regular Grammar
The conversion from a finite automaton to an equivalent regular grammar follows these rules:

1. **Non-terminals (VN)** ← States of the automaton (Q)
2. **Terminals (VT)** ← Alphabet symbols (Σ)
3. **Start symbol** ← Initial state (q₀)
4. **Production rules**:
   - For each transition δ(q_i, a) = q_j, create rule: **q_i → a q_j**
   - If q_j is a final state, also create rule: **q_i → a**
5. The resulting grammar generates exactly the same language as the automaton

---

## Objectives

1. Implement a data structure to represent finite automata with states, alphabet, transitions, and final states
2. Implement the automaton acceptance algorithm to test whether input strings belong to the language
3. Implement conversion from finite automata to equivalent regular grammars
4. Verify that the automaton is deterministic
5. Display the converted grammar in a readable format

---

## Implementation Description

### 1. FiniteAutomaton Class Structure

```cpp
class FiniteAutomation {
public:
    vector<char> states;                        // Q = {q0, q1, q2, ...}
    vector<char> alphabet;                      // Σ = {a, b, c, ...}
    map<pair<char, char>, char> transitions;    // δ: (state, symbol) → state
    char startState;                            // q0
    vector<char> finalStates;                   // F = {qf1, qf2, ...}
    
    bool accepts(string word);
    Grammar toRegularGrammar();
    bool isDeterministic();
};
```

### 2. String Acceptance Algorithm

The `accepts()` method simulates the automaton on an input string:

```cpp
bool FiniteAutomation::accepts(string word) {
    char current = startState;
    for (char c : word) {
        auto it = transitions.find({ current, c });
        if (it == transitions.end())
            return false;  // No transition exists
        current = it->second;
    }
    
    // Check if we ended in a final state
    for (char f : finalStates)
        if (current == f)
            return true;
    return false;
}
```

### 3. FA to Grammar Conversion

The `toRegularGrammar()` method converts the automaton to an equivalent grammar:

```cpp
Grammar FiniteAutomation::toRegularGrammar() {
    Grammar g;
    g.VN = states;           // States become non-terminals
    g.VT = alphabet;         // Alphabet becomes terminals
    g.start = startState;    // Start state becomes start symbol
    g.productions.clear();
    
    // Convert transitions to productions
    for (auto& t : transitions) {
        char from = t.first.first;
        char symbol = t.first.second;
        char to = t.second;
        
        // ALWAYS add A → aB
        string rule;
        rule += symbol;
        rule += to;
        g.productions[from].push_back(rule);
        
        // IF destination is final → also add A → a
        if (find(finalStates.begin(), finalStates.end(), to) != finalStates.end()) {
            string finalRule;
            finalRule += symbol;
            g.productions[from].push_back(finalRule);
        }
    }
    return g;
}
```

### 4. Determinism Check

```cpp
bool FiniteAutomation::isDeterministic() {
    // The map structure guarantees at most one transition per (state, symbol) pair
    return true;
}
```

### 5. Test Automaton

The automaton is initialized with:
- **Q** = {0, 1, 2, 3}
- **Σ** = {a, b, c}
- **q₀** = 0
- **F** = {3}
- **Transitions**:
  - δ(0, a) = 1
  - δ(1, a) = 2
  - δ(1, b) = 1
  - δ(2, c) = 3
  - δ(3, c) = 3

Accepted strings: "abac", "abbac", "abbbac", etc. (pattern: a(b)*ac*)

---

## Conclusions

The implementation successfully demonstrates the theoretical equivalence between finite automata and regular grammars. The conversion algorithm transforms state transitions into grammatical productions while preserving the accepted language. The deterministic nature of the automaton (guaranteed by the map-based transition function) ensures a clean one-to-one translation to a regular grammar. Both representations recognize the same regular language, validating the correctness of the conversion process.

---

## How to Compile and Run

```bash
cd c:\_Projects\DSL\DSl_labs
msbuild DSl_labs.sln /p:Configuration=Debug /p:Platform=x64
.\x64\Debug\DSl_lab.exe
```

Or open `DSl_labs.sln` in Visual Studio and press **Ctrl+F5** to run.

---
