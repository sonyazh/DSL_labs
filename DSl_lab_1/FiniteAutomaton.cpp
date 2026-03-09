#include "FiniteAutomaton.h"

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