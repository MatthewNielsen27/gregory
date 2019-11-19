#include <iostream>

#include "dpll.hpp"

using namespace::std;

int main(const int argc, const char** argv) {

    formula f = {{"p", "q"}, {"q", "r"}, {"~p", "~s", "t"}, {"~p", "s", "u"}, {"~p", "~t", "u"}, {"~p", "s", "~u"}, {"~p", "~s", "~u"}};
    interpretation i;

    if (evaluation::satisfied == dpll(f, i)) {
        cout << "Equation is satisfied by the following partial interpretation: " << endl;

        for (auto it = i.begin(); it != i.end(); ++it) {
            cout << it->first << " -> " << it->second << endl;
        }
    } else {
        cout << "Equation cannot be satisfied." << endl;
    }

    return 0;
}