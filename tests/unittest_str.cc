#include "CppUnitTest.h"

#include <string>
#include <limits>
#include <algorithm>

#include "cpputils/include/cpputils.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace rc;

namespace tests {
    static void assert(bool condition, const wchar_t *msg = nullptr) {
        Assert::IsTrue(condition, msg);
    }

    static void fail(const wchar_t *msg = nullptr) {
        Assert::Fail(msg);
    }

    static void pass() { }

    TEST_CLASS(UnitTest_str) {
    public:

        TEST_METHOD(test_constructor) {
            auto s = str();
            assert(s == str());
            assert(s == "");

            str s1 = "123";
            assert(s1 == "123");
            s1 = L"≤‚ ‘111";
            assert(s1 == u8"≤‚ ‘111");
            assert(str(std::wstring(L"≤‚ ‘")) == L"≤‚ ‘");
            assert(str(std::string(u8"≤‚ ‘")) == L"≤‚ ‘");

            auto s2 = s1;
            assert(s1 == s2);
            s2 = str(s1);
            assert(s1 == s2);
        }

        class A {
        public:
            friend std::ostream &operator<<(std::ostream &os, const A &a) {
                os << "test";
                return os;
            }
        };

        TEST_METHOD(test_explicit_convert) {
            assert(str(0) == "0");
            assert(str(-1) == "-1");
            assert(str(12.34) == "12.34");
            std::stringstream ss;
            auto max_double = std::numeric_limits<double>::max();
            ss << max_double;
            assert(str(max_double) == ss.str());

            assert(str(true) == "true");
            assert(str(false) == "false");

            assert(str(nullptr) == "nullptr");

            assert(str(A()) == "test");
        }

        TEST_METHOD(test_data_and_c_str) {
            auto s = str("abc");
            assert(s.data() == s.c_str());
            assert(std::string("abc") == s.c_str());
            assert(*(s.c_str()) == 'a');
        }

        TEST_METHOD(test_to_bytes_and_to_wstring) {
            auto s = str(u8"≤‚ ‘");
            assert(s.to_bytes() == u8"≤‚ ‘");
            assert(s.to_wstring() == L"≤‚ ‘");
        }

        TEST_METHOD(test_ostream_output) {
            // the << operator will convert the UTF-8 string to ACP codepage
            auto s = str(u8"abc≤‚ ‘");
            std::stringstream ss1, ss2;
            ss1 << s;
            ss2 << "abc≤‚ ‘";
            assert(ss1.str() == ss2.str());
        }

        TEST_METHOD(test_logic_operators) {
            assert(str("abc") < str("abd"));
            assert(str("abdd") > str("abd"));
            assert(str("abc") <= str("abd"));
            assert(str("abd") <= str("abd"));
            assert(str("abdd") >= str("abd"));
            assert(str("abd") >= str("abd"));
            assert(str("abd") == str("abd"));
            assert(str("a") != str("b"));
            assert(str("") == str(""));
        }

        TEST_METHOD(test_arithmetic_operators) {
            assert(str("a") + str("b") == str("ab"));
            assert(str("≤‚") + str(" ‘") == str("≤‚ ‘"));
            assert(str("≤‚") + str() + str() == str("≤‚"));
            assert(str("") + str() + str() == str(""));

            auto s = str();
            s += u8"12345≤‚ ‘";
            s += "";
            s += L"aaa";
            s = s + L"ƒ„∫√";
            assert(s == str(u8"12345≤‚ ‘aaaƒ„∫√"));

            assert(str(L"≤‚") * 3 == str(L"≤‚≤‚≤‚"));
            assert(3 * str(L"≤‚") == str(L"≤‚≤‚≤‚"));

            s = "**";
            s *= 3;
            assert(s == str("******"));
        }

        TEST_METHOD(test_bool_casting) {
            assert(bool(str()) == false);
            assert(bool(str("")) == false);
            assert(bool(str("111")) == true);

            if (str("1")) {
                pass();
            } else {
                fail();
            }

            if (!str()) {
                pass();
            } else {
                fail();
            }
        }

        TEST_METHOD(test_int_casting) {
            auto max_int = std::numeric_limits<int>::max();

            try {
                assert(int(str("123")) == int(123));
                assert(int(str("-123")) == int(-123));
                assert(int(str(max_int)) == max_int);
            } catch (...) {
                fail();
            }

            try {
                int(str("abc"));
                fail();
            } catch (ValueError &) {
                pass();
            } catch (...) {
                fail();
            }

            try {
                int(str(max_int) + "0");
                fail();
            } catch (OutOfRangeError &) {
                pass();
            } catch (...) {
                fail();
            }
        }

        TEST_METHOD(test_long_long_casting) {
            auto max_ll = std::numeric_limits<long long>::max();

            try {
                assert(long long(str("123")) == long long(123));
                assert(long long(str("-123")) == long long(-123));
                assert(long long(str(max_ll)) == max_ll);
            } catch (...) {
                fail();
            }

            try {
                long long(str("abc"));
                fail();
            } catch (ValueError &) {
                pass();
            } catch (...) {
                fail();
            }

            try {
                long long(str(max_ll) + "0");
                fail();
            } catch (OutOfRangeError &) {
                pass();
            } catch (...) {
                fail();
            }
        }

        TEST_METHOD(test_float_casting) {
            auto max_val = std::numeric_limits<float>::max();

            try {
                assert(float(str("123")) == float(123));
                assert(float(str("-123.111")) == float(-123.111));
                assert(float(str("0.000")) == float(0));
            } catch (...) {
                fail();
            }

            try {
                float(str("abc"));
                fail();
            } catch (ValueError &) {
                pass();
            } catch (...) {
                fail();
            }

            try {
                float("1" + str(max_val));
                fail();
            } catch (OutOfRangeError &) {
                pass();
            } catch (...) {
                fail();
            }
        }

        TEST_METHOD(test_double_casting) {
            auto max_val = std::numeric_limits<double>::max();

            try {
                assert(double(str("123")) == double(123));
                assert(double(str("-123.111")) == double(-123.111));
                assert(double(str("0.000")) == double(0));
            } catch (...) {
                fail();
            }

            try {
                double(str("abc"));
                fail();
            } catch (ValueError &) {
                pass();
            } catch (...) {
                fail();
            }

            try {
                double("1" + str(max_val));
                fail();
            } catch (OutOfRangeError &) {
                pass();
            } catch (...) {
                fail();
            }
        }

        TEST_METHOD(test_iterator) {
            auto s = str(u8"a≤‚ ‘");
            auto it = s.begin();
            assert(*it == u8"a");
            ++it;
            assert(*it == u8"≤‚");
            ++it;
            assert(*it == u8" ‘");
            ++it;
            assert(it == s.end());
            ++it;
            assert(it == s.end());

            --it;
            assert(*it == u8" ‘");
            it = s.begin();
            --it;
            assert(it == s.begin());

            it = s.end();
            try {
                auto dummy = *it;
                fail();
            } catch (ValueError &) {
                pass();
            } catch (...) {
                fail();
            }

            s = str();
            assert(s.begin() == s.end());

            s = str("abcdef");
            assert(str(s.begin(), s.end()) == s);
            assert(str(++s.begin(), --s.end()) == str("bcde"));
        }

        TEST_METHOD(test_c_iterator) {
            std::string s1 = u8"abcd≤‚ ‘";
            auto it1 = s1.begin();
            str s2 = s1;
            std::for_each(s2.c_begin(), s2.c_end(), [&it1](const char &ch) {
                              assert(*it1++ == ch);
                          });
        }

        TEST_METHOD(test_length_and_size) {
            auto s = str();
            assert(s.length() == 0);
            assert(s.length() == s.size());
            assert(s.byte_size() == 0);

            s = str(u8"abc≤‚ ‘");
            assert(s.length() == 5);
            assert(s.size() == 5);
            assert(s.byte_size() == std::string(u8"abc≤‚ ‘").size());
        }

        TEST_METHOD(test_index_and_slice) {
            auto s = str();
            try {
                auto dummy = s[0];
                fail();
            } catch (IndexError &) {
                pass();
            } catch (...) {
                fail();
            }

            try {
                auto sub = s[slice(0, 10)];
                assert(sub == "");
            } catch (...) {
                fail();
            }

            s = str(u8"≤‚ ‘123");
            index_t len = s.length();

            assert(s[0] == u8"≤‚");
            assert(s[-len] == u8"≤‚");
            assert(s[-1] == u8"3");
            assert(s[len - 1] == u8"3");

            assert(s[slice()] == s); // full copy
            assert(s[slice(1)] == u8" ‘123");
            assert(s[slice(1, 3)] == u8" ‘1");
            assert(s[slice(1, -1)] == u8" ‘12");
            assert(s[slice(-4, 0)] == u8"");
            assert(s[slice(-3, 4)] == u8"12");
            assert(s[slice(-4, -1)] == u8" ‘12");
            assert(s[slice(1, -1, 2)] == u8" ‘2");

            assert(s[slice(1, 10, -1)] == u8"");
            assert(s[slice(4, 0, -1)] == u8"321 ‘");
            assert(s[slice(-1, 0, -1)] == u8"321 ‘");
            assert(s[slice(-1, -len - 1, -1)] == u8"321 ‘≤‚");

            try {
                auto dummy = s[slice(1, 2, 0)];
                fail();
            } catch (ValueError &) {
                pass();
            } catch (...) {
                fail();
            }
        }

        TEST_METHOD(test_upper_and_lower) {
            auto s = str(u8"123abcABC≤‚ ‘");
            assert(s.upper() == str(u8"123ABCABC≤‚ ‘"));
            assert(s.lower() == str(u8"123abcabc≤‚ ‘"));

            assert(str(u8"ABCD").isupper());
            assert(!str(u8"ABCD").islower());
            assert(str(u8"abcd").islower());
            assert(!str(u8"abcd").isupper());
        }

        TEST_METHOD(test_strip) {
            assert(str("111222333").strip([](auto ch) { return ch == "1" || ch == "3"; }) == "222");
            assert(str("   abc   \t\n").strip() == str("abc"));
            assert(str("abc").strip() == str("abc"));
            assert(str("aaabaabc").strip("ac") == str("baab"));

            assert(str("   a   ").lstrip() == str("a   "));
            assert(str("111a   ").lstrip("1") == str("a   "));
            assert(str("   a   ").rstrip() == str("   a"));
            assert(str("111a   ").rstrip("1") == str("111a   "));
            assert(str("111a   ").rstrip(" \t") == str("111a"));

            assert(str(u8"ƒ„ƒ„ƒ„ƒ„ƒ„ƒ„ƒ„ƒ„∫√∞°∞°∞°∞°").strip(u8"ƒ„∞°") == str(u8"∫√"));
        }
    };
}
