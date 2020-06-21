#include "cpputils/binpack.hpp"
#include "cpputils/endian.hpp"
#include "cpputils/string.hpp"

int main() {
    using namespace rc::test;
    test_endian_module();
    test_binpack_module();
    test_string_module();
}
