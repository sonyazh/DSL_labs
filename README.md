# Intro to formal languages. Regular grammars. Finite Automata.

### Course: Formal Languages & Finite Automata
### Author: Zhuchkova Sofia

----

## Theory
Formal languages are mathematical models used to describe sets of strings over a given alphabet. The grammar in such languages is defined by the formula:
G=(VN​,VT​,P,S), in which VN represents the non-terminal symbols, VT - terminal symbols, P stands for the production rules that are used in our grammar, and S is the starting symbol.

A right-linear grammar is a special type of grammar where production rules have the form
A→aB or 
A→a, where 
A and B are non-terminal symbols and a is a terminal symbol. Right-linear grammars are important because they can be directly converted into finite automata.

A finite automaton is a mathematical model of computation used to recognize formal languages. It consists of a set of states, an alphabet, a transition function, a start state, and one or more final states. When a string is processed by the automaton, it moves between states according to the input symbols. If the automaton finishes in a final state after reading the entire input, the string is accepted; otherwise, it is rejected.

## Objectives:

1. Understand the structure of a formal grammar.
2. Implement a Grammar class representing non-terminals, terminals, and production rules.
3. Generate valid strings based on the defined grammar.
4. Convert the grammar into an equivalent finite automaton.
5. Implement a method to verify whether a given word belongs to the language.

## Implementation description

* A Grammar class was implemented to represent the formal grammar. It stores the start symbol, the set of non-terminal symbols, the set of terminal symbols, and the production rules using appropriate C++ data structures.
* A method called generateString() was implemented to generate valid words from the grammar. The algorithm starts from the start symbol and repeatedly replaces non-terminals with one of their production rules until only terminal symbols remain.
* A FiniteAutomaton class was implemented to represent the equivalent automaton. It stores the states, alphabet, transition function, start state, and final states.
* A method toFiniteAutomaton() converts the grammar into an equivalent automaton by transforming each production rule into a corresponding transition.
* The accepts() method checks whether a given string belongs to the language by simulating transitions of the finite automaton for each character of the input word.


### Grammar class

```
class Grammar {
public:
	char start = 'S';

	vector<char> VN;
	vector<char> VT;

	map<char, vector<string>> productions;

	Grammar();

	string generateString();

	FiniteAutomation toFiniteAutomation();
};
```

### Grammar rules

```
Grammar::Grammar() {
	VN = { 'S', 'A', 'B', 'C' };
	VT = { 'a', 'b', 'c', 'd' };

	productions['S'] = { "dA" };
	productions['A'] = { "aB", "b" };
	productions['B'] = { "bC", "d" };
	productions['C'] = { "cB", "aA" };
}
```

### String generation

```
string Grammar::generateString() {
	string result = "S";

	bool hasNoTerminal = true;

	while (hasNoTerminal) {
		hasNoTerminal = false;

		for (char c : result) {
			if (isupper(c)) {
				hasNoTerminal = true;
				break;
			}
		}

		if (!hasNoTerminal) {
			break;
		}

		for (size_t i = 0; i < result.size(); i++) {
			char symbol = result[i];
			if (isupper(symbol)) {
				auto rules = productions[symbol];
				int index = rand() % rules.size();
				string replacement = rules[index];

				result.replace(i, 1, replacement);

				break;
			}
		}
	}

	return result;
}
```

### Finite Automaton acceptance check

```
bool FiniteAutomation::accepts(string word) {
	char current = startState;
	for (char c : word) {
		auto it = transitions.find({ current, c });
		if (it == transitions.end())
			return false;
		current = it->second;
	}

	for (char f : finalStates)
		if (current == f)
			return true;

	return false;
}
```

### main

```
int main()
{
    Grammar g;

    FiniteAutomation fa = g.toFiniteAutomation();

    string word;
    cout << "Enter word: ";
    cin >> word;

    if (fa.accepts(word))
        cout << "Exists in language!\n";
    else
        cout << "Does not exist in language :(\n";

}
```

## Conclusions / Screenshots / Results
<img width="446" height="118" alt="image" src="https://github.com/user-attachments/assets/6bcbe62f-444d-44f5-b348-d3a225a76906" />
<img width="451" height="119" alt="image" src="https://github.com/user-attachments/assets/ab0cb8e2-3009-4f8b-9579-0876fa86da9a" />
