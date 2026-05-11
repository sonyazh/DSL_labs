# Parser and Abstract Syntax Tree

### Course: Formal Languages & Finite Automata
### Author: Sofia Zhuchkova

----

## Theory

Parsing is the stage that comes after lexical analysis. In Lab 3, the source text was split into tokens such as keywords, identifiers, numbers, operators, and delimiters. In this lab, those tokens are analyzed according to grammar rules so that the program can understand the structure of the input text.

The result of parsing is an Abstract Syntax Tree, usually called an AST. An AST keeps the important syntactic information from the source code and ignores details that are only useful for writing the text, such as semicolons or parentheses used only for grouping. For example, the expression `(x + 8) / 2` becomes a tree where `/` is the parent operation, `x + 8` is the left child, and `2` is the right child.

The parser implemented in this lab is a recursive-descent parser. This means that every grammar rule is represented by a C++ function. The parser starts from the whole program, then parses declarations, statements, blocks, and expressions by calling smaller parsing functions.

## Objectives:

* Use a `TokenType` enum to categorize tokens during lexical analysis.
* Use regular expressions to identify token types.
* Implement data structures for an Abstract Syntax Tree.
* Implement a parser that extracts syntactic information from the token stream.
* Demonstrate the parser on source text processed by the Lab 3 lexer.

## Implementation description

* The lexer already had a `TokenType` enum, so the existing token model was preserved. For Lab 6, the lexer was extended with `identifyTokenType`, which uses `std::regex_match` to classify numbers, identifiers, keywords, operators, and delimiters.

```cpp
TokenType Lexer::identifyTokenType(const string& lexeme) {
    if (regex_match(lexeme, regex("[0-9]+(\\.[0-9]+)?"))) {
        return TokenType::NUMBER;
    }
    if (regex_match(lexeme, regex("[A-Za-z_][A-Za-z0-9_]*"))) {
        return checkKeyword(lexeme);
    }
    if (regex_match(lexeme, regex("\"([^\"\\\\]|\\\\.)*\""))) {
        return TokenType::STRING;
    }
    return TokenType::UNKNOWN;
}
```

* The AST is represented by the `ASTNode` class. Each node stores its type, an optional value, and a list of child nodes. The children are stored as `unique_ptr<ASTNode>`, which gives clear ownership of the tree nodes.

```cpp
enum class ASTNodeType {
    Program,
    Block,
    FunctionDeclaration,
    VariableDeclaration,
    Assignment,
    IfStatement,
    WhileStatement,
    ReturnStatement,
    BinaryExpression,
    UnaryExpression,
    Literal,
    Identifier,
    Type
};
```

```cpp
class ASTNode {
public:
    ASTNodeType type;
    string value;
    vector<unique_ptr<ASTNode>> children;

    ASTNode(ASTNodeType type, const string& value = "");

    void addChild(unique_ptr<ASTNode> child);
    void print(int indent = 0) const;
};
```

* The parser receives the vector of tokens created by the lexer. It keeps a current position in the token list and uses helper methods such as `match`, `consume`, `currentToken`, and `peekToken` to move through the input.

```cpp
class Parser {
private:
    vector<Token> tokens;
    size_t position;
    vector<string> errors;

    Token currentToken() const;
    Token peekToken(int offset = 1) const;
    bool match(TokenType type);
    Token consume(TokenType type, const string& message);

public:
    Parser(const vector<Token>& tokens);
    unique_ptr<ASTNode> parseProgram();
    bool hasErrors() const;
    void printErrors() const;
};
```

* The parser starts with `parseProgram`. It reads declarations, functions, and statements until the end of the token stream. This creates the root `Program` node of the AST.

```cpp
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
```

* Declarations and function definitions are parsed by checking whether a type and identifier are followed by `(`. If they are, the parser creates a `FunctionDeclaration`; otherwise, it creates a `VariableDeclaration`.

```cpp
unique_ptr<ASTNode> Parser::parseDeclarationOrFunction() {
    Token typeToken = advance();
    Token nameToken = consume(TokenType::IDENTIFIER, "Expected identifier after type");

    if (match(TokenType::LPAREN)) {
        unique_ptr<ASTNode> functionNode =
            make_unique<ASTNode>(ASTNodeType::FunctionDeclaration, nameToken.value);
        functionNode->addChild(make_unique<ASTNode>(ASTNodeType::Type, typeToken.value));
        consume(TokenType::RPAREN, "Expected ')' after function parameters");
        consume(TokenType::LBRACE, "Expected '{' before function body");
        functionNode->addChild(parseBlock());
        return functionNode;
    }

    unique_ptr<ASTNode> declaration =
        make_unique<ASTNode>(ASTNodeType::VariableDeclaration, nameToken.value);
    declaration->addChild(make_unique<ASTNode>(ASTNodeType::Type, typeToken.value));
    return declaration;
}
```

* Expressions are parsed using several functions that represent operator precedence. Multiplication and division are parsed before addition and subtraction, while comparison and equality operators are parsed after arithmetic expressions.

```cpp
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
```

* The parser was integrated into the main program after the Lab 3 lexer demonstration. The same input is first tokenized, then passed to the parser, and finally the AST is printed.

```cpp
Lexer parserLexer(parserInput);
vector<Token> parserTokens = parserLexer.tokenize();

Parser parser(parserTokens);
unique_ptr<ASTNode> ast = parser.parseProgram();

if (parser.hasErrors()) {
    parser.printErrors();
}
else {
    ast->print();
}
```

## Conclusions / Screenshots / Results

The Lab 6 implementation successfully extends the Lab 3 lexer with parsing functionality. The program can now move from raw source text, to tokens, and then to a structured AST.

Example input:

```cpp
void main() {
    int x = 42;
    float result = (x + 8) / 2;
    if (result > 10) {
        x = x + 1;
    }
    while (x < 50) {
        x = x + 2;
    }
    return x;
}
```

Generated AST:

```text
Program
  FunctionDeclaration: main
    Type: void
    ParameterList
    Block
      VariableDeclaration: x
        Type: int
        Literal: 42
      VariableDeclaration: result
        Type: float
        BinaryExpression: /
          BinaryExpression: +
            Identifier: x
            Literal: 8
          Literal: 2
      IfStatement
        BinaryExpression: >
          Identifier: result
          Literal: 10
        Block
          Assignment: x
            BinaryExpression: +
              Identifier: x
              Literal: 1
      WhileStatement
        BinaryExpression: <
          Identifier: x
          Literal: 50
        Block
          Assignment: x
            BinaryExpression: +
              Identifier: x
              Literal: 2
      ReturnStatement
        Identifier: x
```

The parser recognizes the main syntactic structures needed for the language subset used in Lab 3: declarations, assignments, blocks, conditional statements, loops, return statements, and arithmetic or comparison expressions. The printed AST shows that the syntactic hierarchy is extracted correctly.

## References

* Course materials for Formal Languages & Finite Automata.
* Lab 3 lexer implementation from this project.
* C++ reference documentation for `std::regex`, `std::vector`, and `std::unique_ptr`.
