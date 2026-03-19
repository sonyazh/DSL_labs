# DSL Lab 3: Lexical Analysis and Tokenization

## Lab Name
**Lexer Implementation: Converting Source Code to Tokens**

## Creator
**Sofia Zhuchkova**

---

## Theory

### Lexical Analysis
Lexical analysis is the first phase of compilation and interpretation. It involves breaking down source code into meaningful units called **tokens**. The lexer (lexical analyzer) reads the input character-by-character and groups them into tokens according to language rules.

### Tokens
A token is a basic unit of code containing:
- **Type**: Classification of the token (keyword, identifier, operator, literal, etc.)
- **Value**: The actual text/content of the token
- **Line and Column**: Position information for error reporting

Example: `int x = 42;` produces tokens:
```
[KEYWORD_INT: "int"] [IDENTIFIER: "x"] [ASSIGN: "="] [NUMBER: "42"] [SEMICOLON: ";"]
```

### Token Types
Tokens are classified into several categories:

1. **Literals**
   - NUMBER: `42`, `3.14`
   - IDENTIFIER: `x`, `myVariable`, `_count`
   - STRING: `"hello"`, `"world"`

2. **Keywords**
   - Control flow: `if`, `else`, `while`, `for`
   - Type declarations: `int`, `float`, `void`
   - Other: `return`

3. **Operators**
   - Arithmetic: `+`, `-`, `*`, `/`
   - Assignment: `=`
   - Comparison: `==`, `!=`, `<`, `>`, `<=`, `>=`

4. **Delimiters**
   - Parentheses: `(`, `)`
   - Braces: `{`, `}`
   - Brackets: `[`, `]`
   - Punctuation: `;`, `,`, `.`

### Lexer Algorithm
The lexer operates by:
1. Maintaining a current position in the input string
2. Reading characters and grouping them based on language rules
3. Recognizing patterns (numbers, keywords, identifiers, operators)
4. Creating tokens with type, value, line, and column information
5. Handling whitespace and comments
6. Returning an END_OF_FILE token when input is exhausted

---

## Objectives

1. Implement a comprehensive Token class with type classification and position tracking
2. Create a Lexer class that converts source code into a stream of tokens
3. Recognize and classify different token types (keywords, identifiers, operators, delimiters, literals)
4. Handle multi-character operators (`==`, `!=`, `<=`, `>=`)
5. Support decimal numbers and string literals with escape sequences
6. Track line and column numbers for accurate error reporting
7. Demonstrate the lexer with various code examples

---

## Implementation Description

### 1. Token Class

The `Token` class represents a single lexeme with metadata:

```cpp
class Token {
public:
    TokenType type;      // Token classification
    string value;        // The actual text
    int line;           // Source line number
    int column;         // Source column number

    Token(TokenType type, const string& value, int line = 0, int column = 0);
    string getTypeName() const;
    void print() const;
};
```

### 2. TokenType Enumeration

Comprehensive enumeration covering all language constructs:

```cpp
enum class TokenType {
    // Literals
    NUMBER, IDENTIFIER, STRING,
    
    // Keywords
    KEYWORD_IF, KEYWORD_ELSE, KEYWORD_WHILE, KEYWORD_FOR,
    KEYWORD_INT, KEYWORD_FLOAT, KEYWORD_RETURN, KEYWORD_VOID,
    
    // Operators
    PLUS, MINUS, MULTIPLY, DIVIDE, ASSIGN,
    EQUAL, NOT_EQUAL, LESS, GREATER, LESS_EQUAL, GREATER_EQUAL,
    
    // Delimiters
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET,
    SEMICOLON, COMMA, DOT,
    
    // Special
    END_OF_FILE, UNKNOWN
};
```

### 3. Lexer Class Structure

```cpp
class Lexer {
private:
    string input;
    size_t position;
    int line;
    int column;

    // Character navigation
    char currentChar();
    char peekChar(int offset = 1);
    void advance();
    
    // Token recognition methods
    Token readNumber();
    Token readIdentifierOrKeyword();
    Token readString();
    
    // Utility methods
    TokenType checkKeyword(const string& word);
    bool isDigit(char c) const;
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;

public:
    Lexer(const string& input);
    vector<Token> tokenize();
    Token nextToken();
};
```

### 4. Key Methods

#### Reading Numbers
```cpp
Token Lexer::readNumber() {
    string number;
    while (isDigit(currentChar())) {
        number += currentChar();
        advance();
    }
    // Handle decimal numbers
    if (currentChar() == '.' && isDigit(peekChar())) {
        number += currentChar();
        advance();
        while (isDigit(currentChar())) {
            number += currentChar();
            advance();
        }
    }
    return Token(TokenType::NUMBER, number, line, column);
}
```

#### Keyword Recognition
```cpp
TokenType Lexer::checkKeyword(const string& word) {
    if (word == "if")       return TokenType::KEYWORD_IF;
    if (word == "else")     return TokenType::KEYWORD_ELSE;
    if (word == "while")    return TokenType::KEYWORD_WHILE;
    if (word == "for")      return TokenType::KEYWORD_FOR;
    if (word == "int")      return TokenType::KEYWORD_INT;
    if (word == "float")    return TokenType::KEYWORD_FLOAT;
    if (word == "return")   return TokenType::KEYWORD_RETURN;
    if (word == "void")     return TokenType::KEYWORD_VOID;
    return TokenType::IDENTIFIER;
}
```

#### String Handling
```cpp
Token Lexer::readString() {
    advance(); // skip opening quote
    string str;
    while (currentChar() != '\0' && currentChar() != '"') {
        if (currentChar() == '\\') {
            advance();
            switch (currentChar()) {
                case 'n':  str += '\n'; break;
                case 't':  str += '\t'; break;
                case '\\': str += '\\'; break;
                case '"':  str += '"';  break;
                default:   str += currentChar();
            }
            advance();
        }
        else {
            str += currentChar();
            advance();
        }
    }
    if (currentChar() == '"')
        advance(); // skip closing quote
    return Token(TokenType::STRING, str, line, column);
}
```

#### Main Tokenization Method
```cpp
vector<Token> Lexer::tokenize() {
    vector<Token> tokens;
    while (true) {
        Token token = nextToken();
        tokens.push_back(token);
        if (token.type == TokenType::END_OF_FILE)
            break;
    }
    return tokens;
}
```

### 5. Usage Examples

```cpp
// Example 1: Simple declaration
string code = "int x = 42;";
Lexer lexer(code);
vector<Token> tokens = lexer.tokenize();

for (const auto& token : tokens) {
    if (token.type != TokenType::END_OF_FILE)
        token.print();  // Output: [KEYWORD_INT: "int"] [IDENTIFIER: "x"] ...
}

// Example 2: Complex expression
string code2 = "float result = (a + b) / (c - d);";
Lexer lexer2(code2);
vector<Token> tokens2 = lexer2.tokenize();
```

---

## Features

✓ **8 Keywords**: if, else, while, for, int, float, return, void  
✓ **Multi-character Operators**: ==, !=, <=, >=  
✓ **Decimal Numbers**: Supports floating-point literals  
✓ **String Literals**: With escape sequence support (\n, \t, \\, \")  
✓ **Position Tracking**: Line and column information for error reporting  
✓ **Comment Skipping**: Handles C-style // comments  
✓ **Whitespace Handling**: Automatically skips spaces, tabs, newlines  
✓ **Comprehensive Token Set**: 26+ distinct token types  

---

## Conclusions

This lexer implementation demonstrates the fundamental concepts of lexical analysis. By converting raw source code into structured tokens, the lexer forms the bridge between the textual input and higher-level parsing stages. The design supports accurate error reporting through position tracking and efficiently recognizes all major language constructs using character-at-a-time processing with lookahead capabilities. The modular approach with separate recognition methods for different token categories makes the code maintainable and extensible for additional language features.

---

## How to Compile and Run

The lexer is integrated into the main DSL Lab project:

```bash
cd c:\_Projects\DSL\DSl_labs
msbuild DSl_labs.sln /p:Configuration=Debug /p:Platform=x64
.\x64\Debug\DSl_lab_1.exe
```

Or open `DSl_labs.sln` in Visual Studio and press **Ctrl+F5** to run.

The program will demonstrate the lexer with 5 different code examples, showing tokenization results and detailed token information.

---

## Files

- **Token.h / Token.cpp**: Token class and type enumeration
- **Lexer.h / Lexer.cpp**: Lexer implementation
- **DSl_lab_1.cpp**: Integration and demonstration

