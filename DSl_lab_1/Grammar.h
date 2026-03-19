#pragma once
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include "FiniteAutomaton.h"

using namespace std;

class Grammar {
public:
	char start = 'S';

	vector<char> VN;
	vector<char> VT;

	map<char, vector<string>> productions;

	Grammar();

	string generateString();

	FiniteAutomation toFiniteAutomation();

	string classifyGrammar();
};