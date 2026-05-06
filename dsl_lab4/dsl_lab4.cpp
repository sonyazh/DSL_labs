#include <cctype>
#include <iostream>
#include <memory>
#include <random>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

const int UNBOUNDED_REPEAT_LIMIT = 5;

enum class TokenType {
    Literal,
    LeftParen,
    RightParen,
    Or,
    Star,
    Plus,
    Question,
    Power
};

struct Token {
    TokenType type;
    string text;
    int number = 0;
};

string tokenTypeName(TokenType type) {
    switch (type) {
    case TokenType::Literal: return "Literal";
    case TokenType::LeftParen: return "LeftParen";
    case TokenType::RightParen: return "RightParen";
    case TokenType::Or: return "Or";
    case TokenType::Star: return "Star";
    case TokenType::Plus: return "Plus";
    case TokenType::Question: return "Question";
    case TokenType::Power: return "Power";
    }

    return "Unknown";
}

vector<Token> tokenize(const string& expression) {
    vector<Token> tokens;

    for (size_t i = 0; i < expression.size(); ++i) {
        char ch = expression[i];

        if (isspace(static_cast<unsigned char>(ch))) {
            continue;
        }

        if (ch == '(') {
            tokens.push_back({ TokenType::LeftParen, "(" });
        }
        else if (ch == ')') {
            tokens.push_back({ TokenType::RightParen, ")" });
        }
        else if (ch == '|') {
            tokens.push_back({ TokenType::Or, "|" });
        }
        else if (ch == '*') {
            tokens.push_back({ TokenType::Star, "*" });
        }
        else if (ch == '+') {
            tokens.push_back({ TokenType::Plus, "+" });
        }
        else if (ch == '?') {
            tokens.push_back({ TokenType::Question, "?" });
        }
        else if (ch == '^') {
            int number = 0;
            string text = "^";
            ++i;

            if (i >= expression.size() || !isdigit(static_cast<unsigned char>(expression[i]))) {
                throw runtime_error("Expected number after ^");
            }

            while (i < expression.size() && isdigit(static_cast<unsigned char>(expression[i]))) {
                number = number * 10 + (expression[i] - '0');
                text += expression[i];
                ++i;
            }

            --i;
            tokens.push_back({ TokenType::Power, text, number });
        }
        else {
            tokens.push_back({ TokenType::Literal, string(1, ch) });
        }
    }

    return tokens;
}

struct Node {
    virtual ~Node() = default;
    virtual string generate(mt19937& rng, vector<string>& steps) const = 0;
    virtual set<size_t> match(const string& word, size_t position) const = 0;
    virtual string describe() const = 0;
};

using NodePtr = shared_ptr<Node>;

struct Literal : Node {
    char value;

    explicit Literal(char value) : value(value) {}

    string generate(mt19937&, vector<string>& steps) const override {
        steps.push_back("Append literal '" + string(1, value) + "'");
        return string(1, value);
    }

    set<size_t> match(const string& word, size_t position) const override {
        if (position < word.size() && word[position] == value) {
            return { position + 1 };
        }

        return {};
    }

    string describe() const override {
        return string(1, value);
    }
};

struct Choice : Node {
    vector<NodePtr> options;

    explicit Choice(vector<NodePtr> options) : options(move(options)) {}

    string generate(mt19937& rng, vector<string>& steps) const override {
        uniform_int_distribution<int> distribution(0, static_cast<int>(options.size()) - 1);
        int index = distribution(rng);

        steps.push_back("Encounter choice " + describe());
        steps.push_back("Randomly select: " + options[index]->describe());

        return options[index]->generate(rng, steps);
    }

    set<size_t> match(const string& word, size_t position) const override {
        set<size_t> result;

        for (const NodePtr& option : options) {
            set<size_t> matches = option->match(word, position);
            result.insert(matches.begin(), matches.end());
        }

        return result;
    }

    string describe() const override {
        string text = "(";
        for (size_t i = 0; i < options.size(); ++i) {
            if (i > 0) {
                text += "|";
            }
            text += options[i]->describe();
        }
        text += ")";
        return text;
    }
};

struct Repeat : Node {
    NodePtr node;
    int minimum;
    int maximum;
    string operation;

    Repeat(NodePtr node, int minimum, int maximum, string operation)
        : node(move(node)), minimum(minimum), maximum(maximum), operation(move(operation)) {}

    string generate(mt19937& rng, vector<string>& steps) const override {
        uniform_int_distribution<int> distribution(minimum, maximum);
        int count = distribution(rng);
        string result;

        steps.push_back("Encounter repetition " + describe());
        steps.push_back("Randomly repeat " + node->describe() + " " + to_string(count) + " time(s)");

        for (int i = 0; i < count; ++i) {
            result += node->generate(rng, steps);
        }

        return result;
    }

    set<size_t> match(const string& word, size_t position) const override {
        set<size_t> result;
        set<size_t> current = { position };

        for (int count = 0; count <= maximum; ++count) {
            if (count >= minimum) {
                result.insert(current.begin(), current.end());
            }

            if (count == maximum) {
                break;
            }

            set<size_t> next;
            for (size_t currentPosition : current) {
                set<size_t> matches = node->match(word, currentPosition);
                next.insert(matches.begin(), matches.end());
            }

            current = next;
            if (current.empty()) {
                break;
            }
        }

        return result;
    }

    string describe() const override {
        return node->describe() + operation;
    }
};

struct Concat : Node {
    vector<NodePtr> children;

    explicit Concat(vector<NodePtr> children) : children(move(children)) {}

    string generate(mt19937& rng, vector<string>& steps) const override {
        string result;

        steps.push_back("Process concatenation " + describe());
        for (const NodePtr& child : children) {
            result += child->generate(rng, steps);
        }

        return result;
    }

    set<size_t> match(const string& word, size_t position) const override {
        set<size_t> positions = { position };

        for (const NodePtr& child : children) {
            set<size_t> nextPositions;

            for (size_t currentPosition : positions) {
                set<size_t> matches = child->match(word, currentPosition);
                nextPositions.insert(matches.begin(), matches.end());
            }

            positions = nextPositions;
            if (positions.empty()) {
                break;
            }
        }

        return positions;
    }

    string describe() const override {
        string text;
        for (const NodePtr& child : children) {
            text += child->describe();
        }
        return text;
    }
};

class Parser {
public:
    explicit Parser(vector<Token> tokens) : tokens(move(tokens)) {}

    NodePtr parse() {
        NodePtr ast = parseChoice();

        if (!isAtEnd()) {
            throw runtime_error("Unexpected token '" + peek().text + "'");
        }

        return ast;
    }

private:
    vector<Token> tokens;
    size_t position = 0;

    NodePtr parseChoice() {
        vector<NodePtr> options;
        options.push_back(parseConcat());

        while (match(TokenType::Or)) {
            options.push_back(parseConcat());
        }

        if (options.size() == 1) {
            return options[0];
        }

        return make_shared<Choice>(move(options));
    }

    NodePtr parseConcat() {
        vector<NodePtr> children;

        while (!isAtEnd() && peek().type != TokenType::RightParen && peek().type != TokenType::Or) {
            children.push_back(parseRepeat());
        }

        if (children.empty()) {
            return make_shared<Concat>(vector<NodePtr>{});
        }

        if (children.size() == 1) {
            return children[0];
        }

        return make_shared<Concat>(move(children));
    }

    NodePtr parseRepeat() {
        NodePtr node = parsePrimary();

        while (!isAtEnd()) {
            if (match(TokenType::Star)) {
                node = make_shared<Repeat>(node, 0, UNBOUNDED_REPEAT_LIMIT, "*");
            }
            else if (match(TokenType::Plus)) {
                node = make_shared<Repeat>(node, 1, UNBOUNDED_REPEAT_LIMIT, "+");
            }
            else if (match(TokenType::Question)) {
                node = make_shared<Repeat>(node, 0, 1, "?");
            }
            else if (!isAtEnd() && peek().type == TokenType::Power) {
                int power = advance().number;
                node = make_shared<Repeat>(node, power, power, "^" + to_string(power));
            }
            else {
                break;
            }
        }

        return node;
    }

    NodePtr parsePrimary() {
        if (match(TokenType::LeftParen)) {
            NodePtr node = parseChoice();
            expect(TokenType::RightParen);
            return node;
        }

        if (isAtEnd()) {
            throw runtime_error("Unexpected end of expression");
        }

        Token token = advance();
        if (token.type != TokenType::Literal) {
            throw runtime_error("Expected literal or group, got '" + token.text + "'");
        }

        return make_shared<Literal>(token.text[0]);
    }

    void expect(TokenType type) {
        if (!match(type)) {
            throw runtime_error("Expected token " + tokenTypeName(type));
        }
    }

    bool match(TokenType type) {
        if (isAtEnd() || peek().type != type) {
            return false;
        }

        ++position;
        return true;
    }

    Token advance() {
        return tokens[position++];
    }

    const Token& peek() const {
        return tokens[position];
    }

    bool isAtEnd() const {
        return position >= tokens.size();
    }
};

struct RegexProgram {
    string expression;
    vector<Token> tokens;
    NodePtr ast;
};

RegexProgram buildRegexProgram(const string& expression) {
    vector<Token> tokens = tokenize(expression);
    Parser parser(tokens);
    NodePtr ast = parser.parse();
    return { expression, tokens, ast };
}

bool belongsToRegex(const NodePtr& ast, const string& word) {
    set<size_t> positions = ast->match(word, 0);
    return positions.find(word.size()) != positions.end();
}

void printTokens(const vector<Token>& tokens) {
    cout << "Tokens: ";
    bool first = true;

    for (size_t i = 0; i < tokens.size(); ++i) {
        string text;

        if (tokens[i].type == TokenType::LeftParen) {
            int depth = 0;

            do {
                if (tokens[i].type == TokenType::LeftParen) {
                    ++depth;
                }
                else if (tokens[i].type == TokenType::RightParen) {
                    --depth;
                }

                text += tokens[i].text;
                ++i;
            } while (i < tokens.size() && depth > 0);

            --i;
        }
        else {
            text = tokens[i].text;
        }

        if (!first) {
            cout << ", ";
        }

        cout << text;
        first = false;
    }
    cout << endl;
}

void printGeneratedWord(const RegexProgram& program, mt19937& rng) {
    vector<string> steps;
    string word = program.ast->generate(rng, steps);

    cout << "Regular expression: " << program.expression << endl;
    printTokens(program.tokens);
    cout << "AST: " << program.ast->describe() << endl;
    cout << "Generated word: " << word << endl;
    cout << "Processing sequence:" << endl;

    for (size_t i = 0; i < steps.size(); ++i) {
        cout << "  " << i + 1 << ". " << steps[i] << endl;
    }

    cout << endl;
}

void checkWord(const string& word, const vector<RegexProgram>& programs) {
    vector<string> matches;

    for (const RegexProgram& program : programs) {
        if (belongsToRegex(program.ast, word)) {
            matches.push_back(program.expression);
        }
    }

    if (matches.empty()) {
        cout << "'" << word << "' does not belong to any regex." << endl;
        return;
    }

    cout << "'" << word << "' belongs to:" << endl;
    for (const string& expression : matches) {
        cout << "  - " << expression << endl;
    }
}

int main() {
    vector<string> expressions = {
        "O(P|Q|R)+ 2(3|4)",
        "A*B(C|D|E) F(G|H|i)^2",
        "(J+)K (L|M|N)* O? (P|Q)^3"
    };

    vector<RegexProgram> programs;
    for (const string& expression : expressions) {
        programs.push_back(buildRegexProgram(expression));
    }

    random_device randomDevice;
    mt19937 rng(randomDevice());

    cout << "Regex lab program" << endl;
    cout << "Unbounded repetitions (* and +) are limited to " << UNBOUNDED_REPEAT_LIMIT << " times." << endl;
    cout << endl;

    cout << "Generated examples:" << endl << endl;
    for (const RegexProgram& program : programs) {
        printGeneratedWord(program, rng);
    }

    cout << "Enter a word to check. Empty line exits." << endl;
    string word;
    while (getline(cin, word) && !word.empty()) {
        checkWord(word, programs);
        cout << endl << "Enter a word to check. Empty line exits." << endl;
    }

    return 0;
}
