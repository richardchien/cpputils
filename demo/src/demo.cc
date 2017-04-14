#include <iostream>

#include "cpputils/include/cpputils.h"

using namespace std;
using namespace rc;

class A {
    friend ostream &operator<<(ostream &os, const A &obj) {
        os << 1;
        return os;
    }
};

int main() {
    auto sp = str(" abc asdddd    sad    ").split([](auto ch) { return ch == " "; });
    for (auto sub : sp) {
        cout << sub << endl;
    }

    system("pause");
    return 0;
}
