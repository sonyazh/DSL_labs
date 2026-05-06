# Regular expressions

### Course: Formal Languages & Finite Automata
### Zhuchkova Sofia

----

## Theory

Regular expressions are a formal way to describe patterns in strings. They are built from simple symbols and operators that explain how those symbols may appear. For example, a regular expression can say that a word must start with a certain letter, that one of several symbols may be chosen, or that a symbol may be repeated several times.

In formal language theory, regular expressions are used to define regular languages. A regular language is a set of strings that can be recognized by a finite automaton. This makes regular expressions closely connected with deterministic and nondeterministic finite automata, because both describe the same class of languages.

Regular expressions are also widely used in practical programming. They are used for searching text, validating input, checking formats, extracting data, lexical analysis in compilers, and filtering strings. Examples include checking whether an email address has a valid structure, finding all words that match a pattern in a file, or splitting source code into tokens.

The main operations used in this laboratory work are concatenation, choice, and repetition. Concatenation means that expressions are placed one after another. Choice, written with `|`, means that one option from a group may be selected. Repetition operators such as `*`, `+`, `?`, and `^n` describe how many times a symbol or group can appear.

In this implementation, regular expressions are not hardcoded. The program reads an expression, converts it into tokens, builds an abstract syntax tree, and then uses that structure to generate valid words and check whether an input word belongs to one of the given expressions.

## Objectives:

* Understand how regular expressions describe formal languages.
* Convert a regular expression string into tokens.
* Build a parser that creates an abstract syntax tree from the tokens.
* Generate valid words dynamically from the abstract syntax tree.
* Check whether an input word belongs to any of the given regular expressions.
* Show the sequence of processing steps used while generating a word.

## Implementation description

The program starts by tokenizing the regular expression. Spaces are ignored, while literals and operators are converted into token objects. Parenthesized groups such as `(P|Q|R)` are displayed as grouped tokens for readability, and power repetitions such as `^3` are stored together with their numeric value.

```cpp
struct Token {
    TokenType type;
    string text;
    int number = 0;
};
```

After tokenization, the parser builds an abstract syntax tree. The tree contains nodes for literals, choices, repetitions, and concatenations. This structure allows the program to understand the expression dynamically instead of generating results with hardcoded rules.

```cpp
struct Node {
    virtual ~Node() = default;
    virtual string generate(mt19937& rng, vector<string>& steps) const = 0;
    virtual set<size_t> match(const string& word, size_t position) const = 0;
    virtual string describe() const = 0;
};
```

The `Literal` node represents one symbol. During generation it appends that symbol to the result. During matching it checks whether the current input character is equal to the stored symbol.

```cpp
struct Literal : Node {
    char value;

    explicit Literal(char value) : value(value) {}
};
```

The `Choice` node represents alternatives written with `|`. During generation the program randomly selects one option and records this action in the processing sequence. This fixes the bonus part, because the program now shows the selected option instead of listing all possible options.

```cpp
steps.push_back("Encounter choice " + describe());
steps.push_back("Randomly select: " + options[index]->describe());
```

The `Repeat` node represents `*`, `+`, `?`, and `^n`. Unlimited repetitions are limited to 5 times, as required by the laboratory task. The generator randomly chooses a number of repetitions between the minimum and maximum allowed values.

```cpp
const int UNBOUNDED_REPEAT_LIMIT = 5;
```

The `Concat` node represents expressions placed one after another. It generates a word by generating each child node in order and joining the results.

```cpp
for (const NodePtr& child : children) {
    result += child->generate(rng, steps);
}
```

The program also includes an input checking system. After generating examples, it asks the user to enter a word. The word is checked against all stored regular expressions, and the program prints whether it belongs to one of them.

```cpp
bool belongsToRegex(const NodePtr& ast, const string& word) {
    set<size_t> positions = ast->match(word, 0);
    return positions.find(word.size()) != positions.end();
}
```

## Conclusions / Screenshots / Results

The program successfully interprets the given regular expressions dynamically. It can tokenize expressions, build an abstract syntax tree, generate valid random words, show the generation sequence, and check whether user input belongs to any of the defined expressions.

Example expressions used in the program:

```text
O(P|Q|R)+ 2(3|4)
A*B(C|D|E) F(G|H|i)^2
(J+)K (L|M|N)* O? (P|Q)^3
```

Example generated output:

```text
Regular expression: O(P|Q|R)+ 2(3|4)
Tokens: O, (P|Q|R), +, 2, (3|4)
AST: O(P|Q|R)+2(3|4)
Generated word: OQ24
Processing sequence:
  1. Process concatenation O(P|Q|R)+2(3|4)
  2. Append literal 'O'
  3. Encounter repetition (P|Q|R)+
  4. Randomly repeat (P|Q|R) 1 time(s)
  5. Encounter choice (P|Q|R)
  6. Randomly select: Q
```

Example input checking:

```text
'OPP23' belongs to:
  - O(P|Q|R)+ 2(3|4)

'XYZ' does not belong to any regex.
```

## References

* Course materials for Formal Languages & Finite Automata.
* Regular expressions and finite automata theory.
* C++ documentation for strings, vectors, sets, smart pointers, and random number generation.
