#include <cstdlib>
#include <ctime>
#include "Grammar.h"

using namespace std;

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