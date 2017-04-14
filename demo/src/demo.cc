#include <iostream>

#include "cpputils/include/cpputils.h"

using namespace rc;

using std::cout;
using std::endl;

int main() {
    auto sp = str(u8" 123 3 3 3  ≤‚ \n ‘   \t    ").rsplit(" \t\n", 1);
    for (auto sub : sp) {
        cout << sub << "|" << endl;
    }

    system("pause");
    return 0;
}
