#include "FiniteAutomaton.h"
#include "Grammar.h"
#include <algorithm>

using namespace std;

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

Grammar FiniteAutomation::toRegularGrammar() {
	Grammar g;

	// Non-terminals are the states of the automaton
	g.VN = states;

	// Terminals are the symbols in the alphabet
	g.VT = alphabet;

	// Start symbol is the start state
	g.start = startState;

	// Clear default productions
	g.productions.clear();

	// Build productions from transitions
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

bool FiniteAutomation::isDeterministic() {
	// Since the transition map only allows ONE transition per (state, symbol) pair,
	// the automaton is inherently deterministic by design.
	// There is exactly one or zero transitions for each state-symbol combination.
	return false;
}