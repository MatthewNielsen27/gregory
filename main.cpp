#include <iostream>

#include "dpll.hpp"

int main(const int argc, const char** argv) {

    formula f = {{"~p", "q", "r"}, {"p"}};
    interpretation i;

    cout << (evaluation::satisfied == dpll(f, i)) << endl;

    return 0;
}