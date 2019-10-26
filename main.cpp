#include <iostream>

#include "formula.hpp"

using namespace::std;

int main(const int argc, const char** argv) {

    Formula p("p");
    Formula q("q");

    std::unordered_map<std::string, Formula*> formula_lookup;

    Formula* f = make_formula("p q & !", formula_lookup);

    Interpretation i;

    i[formula_lookup["p"]] = true;
    i[formula_lookup["q"]] = true;

    cout << *f << endl;
    cout << f->evaluate(i) << endl;

    delete f;
    

    return 0;
}