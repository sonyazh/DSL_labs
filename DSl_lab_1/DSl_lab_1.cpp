#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using Symbol = std::string;
using ProductionBody = std::vector<Symbol>;

struct Grammar {
    std::set<Symbol> nonTerminals;
    std::set<Symbol> terminals;
    std::map<Symbol, std::set<ProductionBody>> productions;
    Symbol start;
};

class CnfNormalizer {
public:
    static Grammar normalize(const Grammar& input) {
        Grammar grammar = input;

        addFreshStart(grammar);
        eliminateEpsilonProductions(grammar);
        eliminateRenaming(grammar);
        eliminateNonProductiveSymbols(grammar);
        eliminateInaccessibleSymbols(grammar);
        replaceTerminalsInLongProductions(grammar);
        breakLongProductions(grammar);
        eliminateNonProductiveSymbols(grammar);
        eliminateInaccessibleSymbols(grammar);

        return grammar;
    }

private:
    static bool isNonTerminal(const Grammar& grammar, const Symbol& symbol) {
        return grammar.nonTerminals.count(symbol) > 0;
    }

    static bool isTerminal(const Grammar& grammar, const Symbol& symbol) {
        return grammar.terminals.count(symbol) > 0;
    }

    static Symbol freshNonTerminal(const Grammar& grammar, const std::string& prefix) {
        int index = 1;
        Symbol candidate;

        do {
            candidate = prefix + std::to_string(index++);
        } while (grammar.nonTerminals.count(candidate) > 0 || grammar.terminals.count(candidate) > 0);

        return candidate;
    }

    static void addFreshStart(Grammar& grammar) {
        const Symbol oldStart = grammar.start;
        const Symbol newStart = freshNonTerminal(grammar, "S0_");

        grammar.nonTerminals.insert(newStart);
        grammar.productions[newStart].insert({ oldStart });
        grammar.start = newStart;
    }

    static void eliminateEpsilonProductions(Grammar& grammar) {
        std::set<Symbol> nullable;
        bool changed = true;

        while (changed) {
            changed = false;

            for (const auto& production : grammar.productions) {
                const Symbol& left = production.first;
                const std::set<ProductionBody>& bodies = production.second;

                for (const auto& body : bodies) {
                    const bool bodyIsNullable = body.empty()
                        || std::all_of(body.begin(), body.end(), [&](const Symbol& symbol) {
                            return nullable.count(symbol) > 0;
                        });

                    if (bodyIsNullable && nullable.insert(left).second) {
                        changed = true;
                    }
                }
            }
        }

        std::map<Symbol, std::set<ProductionBody>> normalized;

        for (const auto& production : grammar.productions) {
            const Symbol& left = production.first;
            const std::set<ProductionBody>& bodies = production.second;

            for (const auto& body : bodies) {
                if (body.empty()) {
                    if (left == grammar.start) {
                        normalized[left].insert({});
                    }
                    continue;
                }

                std::vector<int> nullablePositions;
                for (int i = 0; i < static_cast<int>(body.size()); ++i) {
                    if (nullable.count(body[i]) > 0) {
                        nullablePositions.push_back(i);
                    }
                }

                const int variants = 1 << nullablePositions.size();
                for (int mask = 0; mask < variants; ++mask) {
                    ProductionBody candidate;

                    for (int i = 0; i < static_cast<int>(body.size()); ++i) {
                        const auto position = std::find(nullablePositions.begin(), nullablePositions.end(), i);
                        const bool removeSymbol = position != nullablePositions.end()
                            && ((mask >> static_cast<int>(position - nullablePositions.begin())) & 1) == 1;

                        if (!removeSymbol) {
                            candidate.push_back(body[i]);
                        }
                    }

                    if (!candidate.empty() || left == grammar.start) {
                        normalized[left].insert(candidate);
                    }
                }
            }
        }

        grammar.productions = normalized;
    }

    static void eliminateRenaming(Grammar& grammar) {
        std::map<Symbol, std::set<ProductionBody>> normalized;

        for (const Symbol& source : grammar.nonTerminals) {
            std::set<Symbol> reachableByUnit{ source };
            std::queue<Symbol> queue;
            queue.push(source);

            while (!queue.empty()) {
                const Symbol current = queue.front();
                queue.pop();

                for (const ProductionBody& body : grammar.productions[current]) {
                    if (body.size() == 1 && isNonTerminal(grammar, body[0])) {
                        if (reachableByUnit.insert(body[0]).second) {
                            queue.push(body[0]);
                        }
                    }
                }
            }

            for (const Symbol& target : reachableByUnit) {
                for (const ProductionBody& body : grammar.productions[target]) {
                    const bool isUnitProduction = body.size() == 1 && isNonTerminal(grammar, body[0]);
                    if (!isUnitProduction) {
                        normalized[source].insert(body);
                    }
                }
            }
        }

        grammar.productions = normalized;
    }

    static void eliminateInaccessibleSymbols(Grammar& grammar) {
        std::set<Symbol> accessible{ grammar.start };
        std::queue<Symbol> queue;
        queue.push(grammar.start);

        while (!queue.empty()) {
            const Symbol current = queue.front();
            queue.pop();

            for (const ProductionBody& body : grammar.productions[current]) {
                for (const Symbol& symbol : body) {
                    if (isNonTerminal(grammar, symbol) && accessible.insert(symbol).second) {
                        queue.push(symbol);
                    }
                }
            }
        }

        keepOnlyNonTerminals(grammar, accessible);
    }

    static void eliminateNonProductiveSymbols(Grammar& grammar) {
        std::set<Symbol> productive;
        bool changed = true;

        while (changed) {
            changed = false;

            for (const auto& production : grammar.productions) {
                const Symbol& left = production.first;
                const std::set<ProductionBody>& bodies = production.second;

                for (const ProductionBody& body : bodies) {
                    const bool bodyIsProductive = std::all_of(body.begin(), body.end(), [&](const Symbol& symbol) {
                        return isTerminal(grammar, symbol) || productive.count(symbol) > 0;
                    });

                    if (bodyIsProductive && productive.insert(left).second) {
                        changed = true;
                    }
                }
            }
        }

        keepOnlyNonTerminals(grammar, productive);
    }

    static void keepOnlyNonTerminals(Grammar& grammar, const std::set<Symbol>& kept) {
        std::map<Symbol, std::set<ProductionBody>> filteredProductions;

        for (const Symbol& left : kept) {
            for (const ProductionBody& body : grammar.productions[left]) {
                const bool referencesOnlyKeptSymbols = std::all_of(body.begin(), body.end(), [&](const Symbol& symbol) {
                    return isTerminal(grammar, symbol) || kept.count(symbol) > 0;
                });

                if (referencesOnlyKeptSymbols) {
                    filteredProductions[left].insert(body);
                }
            }
        }

        grammar.nonTerminals = kept;
        grammar.productions = filteredProductions;
    }

    static void replaceTerminalsInLongProductions(Grammar& grammar) {
        std::map<Symbol, Symbol> terminalAliases;

        for (const Symbol& terminal : grammar.terminals) {
            const Symbol alias = freshNonTerminal(grammar, "T_" + terminal + "_");
            grammar.nonTerminals.insert(alias);
            grammar.productions[alias].insert({ terminal });
            terminalAliases[terminal] = alias;
        }

        std::map<Symbol, std::set<ProductionBody>> normalized = grammar.productions;

        for (auto& production : normalized) {
            std::set<ProductionBody>& bodies = production.second;
            std::set<ProductionBody> rewrittenBodies;

            for (ProductionBody body : bodies) {
                if (body.size() >= 2) {
                    for (Symbol& symbol : body) {
                        if (isTerminal(grammar, symbol)) {
                            symbol = terminalAliases[symbol];
                        }
                    }
                }

                rewrittenBodies.insert(body);
            }

            bodies = rewrittenBodies;
        }

        grammar.productions = normalized;
    }

    static void breakLongProductions(Grammar& grammar) {
        std::map<Symbol, std::set<ProductionBody>> normalized;

        for (const auto& production : grammar.productions) {
            const Symbol& left = production.first;
            const std::set<ProductionBody>& bodies = production.second;

            for (const ProductionBody& body : bodies) {
                if (body.size() <= 2) {
                    normalized[left].insert(body);
                    continue;
                }

                Symbol currentLeft = left;
                for (size_t i = 0; i < body.size() - 2; ++i) {
                    const Symbol helper = freshNonTerminal(grammar, "X_");
                    grammar.nonTerminals.insert(helper);
                    normalized[currentLeft].insert({ body[i], helper });
                    currentLeft = helper;
                }

                normalized[currentLeft].insert({ body[body.size() - 2], body[body.size() - 1] });
            }
        }

        grammar.productions = normalized;
    }
};

std::string formatBody(const ProductionBody& body) {
    if (body.empty()) {
        return "epsilon";
    }

    std::ostringstream output;
    for (const Symbol& symbol : body) {
        output << symbol;
    }

    return output.str();
}

void printGrammar(const Grammar& grammar, const std::string& title) {
    std::cout << title << '\n';
    std::cout << "Vn = { ";
    for (const Symbol& nonTerminal : grammar.nonTerminals) {
        std::cout << nonTerminal << ' ';
    }
    std::cout << "}\nVt = { ";
    for (const Symbol& terminal : grammar.terminals) {
        std::cout << terminal << ' ';
    }
    std::cout << "}\nP:\n";

    for (const auto& production : grammar.productions) {
        const Symbol& left = production.first;
        const std::set<ProductionBody>& bodies = production.second;

        for (const ProductionBody& body : bodies) {
            std::cout << "  " << left << " -> " << formatBody(body) << '\n';
        }
    }

    std::cout << "S = " << grammar.start << "\n\n";
}

bool isCnf(const Grammar& grammar) {
    for (const auto& production : grammar.productions) {
        const Symbol& left = production.first;
        const std::set<ProductionBody>& bodies = production.second;

        if (grammar.nonTerminals.count(left) == 0) {
            return false;
        }

        for (const ProductionBody& body : bodies) {
            if (body.empty()) {
                if (left != grammar.start) {
                    return false;
                }
            } else if (body.size() == 1) {
                if (grammar.terminals.count(body[0]) == 0) {
                    return false;
                }
            } else if (body.size() == 2) {
                if (grammar.nonTerminals.count(body[0]) == 0 || grammar.nonTerminals.count(body[1]) == 0) {
                    return false;
                }
            } else {
                return false;
            }
        }
    }

    return true;
}

Grammar buildVariantGrammar() {
    return Grammar{
        { "S", "A", "B", "C", "D" },
        { "a", "b" },
        {
            { "S", { { "b", "A" }, { "A", "C" } } },
            { "A", { { "b", "S" }, { "B", "C" }, { "A", "b", "C" } } },
            { "B", { { "C", "b", "a", "C" }, { "a" }, { "b", "S", "a" } } },
            { "C", { {} } },
            { "D", { { "A", "B" } } }
        },
        "S"
    };
}

int main() {
    const Grammar grammar = buildVariantGrammar();
    const Grammar cnfGrammar = CnfNormalizer::normalize(grammar);

    printGrammar(grammar, "Initial grammar");
    printGrammar(cnfGrammar, "Grammar in Chomsky Normal Form");

    assert(isCnf(cnfGrammar));
    assert(cnfGrammar.nonTerminals.count("D") == 0);
    assert(cnfGrammar.nonTerminals.count("C") == 0);

    std::cout << "Tests passed: epsilon productions, renaming, inaccessible symbols, "
        << "non-productive symbols, and CNF shape were verified.\n";

    return 0;
}
