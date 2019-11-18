#include <utility>
#include <vector>
#include <set>
#include <regex>
#include <unordered_map>
#include <iostream>

#include "formula.hpp"


using literal = std::string;
using clause = std::set<literal>;

void print_clause(std::string name, const clause& a) {
    std::cout << name << ": ";
    for (const auto& i : a) {
        std::cout << i << ",";
    } std::cout << "\n";
}

class ClausalForm {
    private:
        std::vector<clause> m_clauses;

    public:
        ClausalForm(std::string formula);
        ClausalForm(std::vector<clause>& clauses) : m_clauses(clauses) {};

        ~ClausalForm() = default;

        bool evaluate(std::unordered_map<std::string, bool> i){
            bool val = true;

            // Conjunction of clauses
            for (const auto& clause_i : m_clauses) {
                bool tmp = false;

                // Disjunction of literals
                for (const auto& literal_i : clause_i) {
                    if (i.find(literal_i) != i.end()) {
                        tmp |= i[literal_i];
                    } else if (i.find("~" + literal_i) != i.end()) {
                        tmp |= !i[literal_i];
                    }
                }

                val &= tmp;
            }

            return val;
        }

        bool is_satisfiable() {
            if (m_clauses.empty()) { return true; } // Vacuously true

            for (const auto& clause_i : m_clauses) {
                if (clause_i.empty()) { return false; } // Empty clause is unsatisfiable
            }

            std::vector<clause> clauses(m_clauses);
            std::vector<clause> tmp_vec;
            for (size_t a = 0; a < clauses.size() - 1; ++a) {
                for (size_t b = a + 1; b < clauses.size(); ++b) {
                    literal conflict;

                    if (determine_conflicts(clauses[a], clauses[b], conflict)) {
                        clause tmp_clause = resolve(clauses[a], clauses[b], conflict);

                        print_clause("C", tmp_clause);

                        if (tmp_clause.empty()) { return false; }

                        tmp_vec.push_back(tmp_clause);
                    }
                }

                clauses.insert(clauses.begin(), tmp_vec.begin(), tmp_vec.end());
                tmp_vec.clear();
            }

            return true;
        }


        bool determine_conflicts(const clause& a, const clause& b, literal& res) {
            bool already_found = false;

            for (const auto& literal_i : a) {
                literal complement = (literal_i[0] != '~' ? "~" + literal_i : literal_i.substr(1));
                
                if (b.find(complement) != b.end()) {
                    if (already_found) { 
                        res = "";
                        return false; 
                    }

                    already_found = true;
                    res = complement;
                }
            }

            return already_found;
        }


        clause resolve(const clause& a, const clause& b, const literal& trouble) {
            literal complement = (trouble[0] != '~' ? "~" + trouble : trouble.substr(1));

            clause res(a);
            res.insert(b.begin(), b.end());

            res.erase(trouble);
            res.erase(complement);

            return res;
        }
};
