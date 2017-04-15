#include <iostream>

#include "cpputils/include/cpputils.h"

using namespace rc;

using std::cout;
using std::endl;

int main() {
    auto sp = str(u8"1123╡Б йт123123...123").rsplit(u8"123", 2);
    for (auto seg : sp) {
        cout << seg << "|" << endl;
    }

    system("pause");
    return 0;
}
