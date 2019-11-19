#include <set>
#include <string>
#include <list>
#include <unordered_map>


// Custom type specification
using literal = std::string;
using clause = std::set<literal>;
using formula = std::list<clause>;
using interpretation = std::unordered_map<literal, bool>;
enum class evaluation {satisfied, unsatisfied, unsure};


// Function to return the complement of a literal
literal complement_of(const literal& l){
    return (l[0] != '~' ? "~" + l : l.substr(1));
}

// Delete any instances of l from f
int prune(formula& f, const literal& l) {
    for (auto& c : f) { c.erase(l); }
}

bool is_trivially_unsatisfiable(formula& f) {
    std::set<literal> seen;

    for (const auto& c : f) {
        if (c.empty()) {
            return true;
        } else if (c.size() == 1) {
            if (seen.find(complement_of(*c.begin())) != seen.end()) {
                return true;
            } else {
                seen.insert(*c.begin());
            }
        }
    }

    return false;
}

bool trivial_clauses_exausted(const formula& f) {
    for (auto& c : f) {
        if (c.size() == 1) {
            return false;
        }
    }

    return true;
}


evaluation evaluate(const clause& c, const interpretation& i) {
    bool running_total = false;

    for (auto& l : c) {
        if (i.find(l) == i.end()) {
            return evaluation::unsure;
        }

        running_total |= i.at(l);
    }

    return running_total ? evaluation::satisfied : evaluation::unsatisfied;
}


evaluation evaluate(const formula& f, const interpretation& i) {
    bool running_total = false;

    for (auto& c : f) {
        if (c.empty()) {
            return evaluation::unsatisfied;
        }

        auto res = evaluate(c, i);

        if (res == evaluation::unsure) {
            return res;
        }

        running_total |= (res == evaluation::satisfied ? true : false);
    }

    return running_total | f.empty() ? evaluation::satisfied : evaluation::unsatisfied;
}



bool unit_resolution(formula& f, interpretation& i) {
    while (!trivial_clauses_exausted(f)) {
        for (auto it = f.begin(); it != f.end();) {
            if ((*it).size() == 1) {
                literal l = *(*it).begin();
                literal l_prime = complement_of(l);

                prune(f, l_prime);                  // Remove l' from clauses in f

                i[l] = true;
                i[l_prime] = false;

                it = f.erase(it);                        // Remove {l} from f
            } else if ((*it).empty()) {
                return false;                       // Empty clause is unsatisfiable
            } else {
                ++it;
            }
        }
    }

    return true;
}


void simplify(formula& f, const literal& l, const interpretation& i) {
    literal l_prime = complement_of(l);

    literal satisfying_literal = i.at(l) ? l : l_prime;
    literal unsatisfying_literal = !i.at(l) ? l : l_prime;

    for (auto it = f.begin(); it != f.end();) {
        if ((*it).find(satisfying_literal) != (*it).end()) {
            it = f.erase(it);
        } else {
            (*it).erase(unsatisfying_literal);
            ++it;
        }
    }
}

evaluation dpll(const formula& b, interpretation& i) {
    if (b.empty()) {
        return evaluation::satisfied;
    }

    formula b_prime(b);
    interpretation i_prime(i);

    if (unit_resolution(b_prime, i_prime) == false) {
        return evaluation::unsatisfied;
    }

    // Check satisfaction under partial interpretation i_prime
    auto res = evaluate(b_prime, i_prime);
    if (res != evaluation::unsure) {
        if (res == evaluation::satisfied) {
            i = i_prime;
        }

        return res;
    }

    literal p = *(*b_prime.begin()).begin();
    literal p_prime = complement_of(p);

    const bool val = true;

    // Attempt initial truth value for p
    formula b_1(b_prime);
    i_prime[p] = val;
    i_prime[p_prime] = !val;
    {
        simplify(b_1, p, i_prime);
        auto res = dpll(b_1, i_prime);

        if (res == evaluation::satisfied) {
            i = i_prime;
            return res;
        }
    }

    // Backtrack for the opposite truth value for p
    formula b_2(b_prime);
    i_prime[p] = !val;
    i_prime[p_prime] = val;
    {  
        simplify(b_2, p, i_prime);
        auto res = dpll(b_2, i_prime);
        i = i_prime;
        return res;
    }
}
