#include "Grammar.h"

Grammar::Grammar() {
	VN = { 'S', 'A', 'B', 'C' };
	VT = { 'a', 'b', 'c', 'd' };

	productions['S'] = { "dA" };
	productions['A'] = { "aB", "b" };
	productions['B'] = { "bC", "d" };
	productions['C'] = { "cB", "aA" };
}

FiniteAutomation Grammar::toFiniteAutomation() {
	FiniteAutomation fa;

	fa.states = VN;
	fa.alphabet = VT;
	fa.startState = start;

	char finalState = 'F';
	fa.states.push_back(finalState);
	fa.finalStates.push_back(finalState);

	for (auto& p : productions) {
		char left = p.first;

		for (auto& rule : p.second) {
			if (rule.size() == 2) {
				char terminal = rule[0];
				char nextState = rule[1];

				fa.transitions[{left, terminal}] = nextState;
			}
			else if (rule.size() == 1) {
				char terminal = rule[0];
				fa.transitions[{left, terminal}] = finalState;
			}
		}
	}
	return fa;
}