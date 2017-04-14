#include <iostream>

#include "cpputils/include/cpputils.h"

using namespace rc;

using std::cout;
using std::endl;

int main() {
    auto sp = str(u8"   123\r\r\n\n abc \n\n\r").splitlines();
    for (auto sub : sp) {
        cout << sub << "|" << endl;
    }

    system("pause");
    return 0;
}
