#include "CppUnitTest.h"

#include "cpputils/include/cpputils.h"
#include "helpers.h"
#include "cpputils/include/win32_bytes.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace rc;
using namespace win32;

namespace tests {

    TEST_CLASS(UnitTest_bytes) {
    public:

        TEST_METHOD(test_bytes_1) {
            auto s = decode(encode(decode("ÄãºÃ", Encoding::ANSI), Encoding::UTF8), Encoding::UTF8);
            assert(s == u8"ÄãºÃ");
        }

        TEST_METHOD(test_bytes_2) {
            auto s = decode(encode(str(L"ëŠÄX"), Encoding::BIG5), Encoding::BIG5);
            assert(s == u8"ëŠÄX");
        }
    };
}
