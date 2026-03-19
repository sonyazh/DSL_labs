#pragma once
#include <map>
#include <vector>
#include <string>

using namespace std;

class Grammar;

class FiniteAutomation {
public:
	vector<char> states;
	vector<char> alphabet;

	map<pair<char, char>, char> transitions;

	char startState;

	vector<char> finalStates;

	bool accepts(string word);

	Grammar toRegularGrammar();

	bool isDeterministic();
};