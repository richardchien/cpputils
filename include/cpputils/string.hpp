#pragma once

#include <algorithm>
#include <functional>
#include <limits>
#include <string>
#include <string_view>

namespace rc {
    inline bool string_startswith(const std::string_view &sv, const std::string_view &prefix) {
        return sv.substr(0, prefix.length()) == prefix;
    }

    inline bool string_endswith(const std::string_view &sv, const std::string_view &suffix) {
        if (sv.length() < suffix.length()) return false;
        return sv.substr(sv.length() - suffix.length()) == suffix;
    }

    inline bool string_contains(const std::string_view &sv, const std::string_view &sub) {
        return sv.find(sub) != std::string_view ::npos;
    }

    inline void string_replace(std::string &s, const std::string_view &old_sub, const std::string_view &new_sub) {
        std::string res;
        res.reserve(s.length());

        std::string::size_type last_pos = 0;
        std::string::size_type pos;
        while ((pos = s.find(old_sub, last_pos)) != std::string::npos) {
            res.append(s, last_pos, pos - last_pos);
            res += new_sub;
            last_pos = pos + old_sub.length();
        }

        res += s.substr(last_pos);
        s.swap(res);
    }

    inline auto _isspace_s(int ch) {
        return ch >= 0 && ch <= std::numeric_limits<unsigned char>::max() ? std::isspace(ch) : 0;
    }

    inline void string_ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not_fn(_isspace_s)));
    }

    inline void string_rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not_fn(_isspace_s)).base(), s.end());
    }

    inline void string_trim(std::string &s) {
        string_ltrim(s);
        string_rtrim(s);
    }

    inline void string_tolower(std::string &s) {
        std::transform(s.cbegin(), s.cend(), s.begin(), [](auto ch) { return std::tolower(ch); });
    }

    inline void string_toupper(std::string &s) {
        std::transform(s.cbegin(), s.cend(), s.begin(), [](auto ch) { return std::toupper(ch); });
    }

    template <typename Func, typename S, typename... Args>
    auto string_blah_copy(Func &&func, S &&s, Args &&... args) {
        std::decay_t<S> s_copy = std::forward<S>(s);
        func(s_copy, args...);
        return s_copy;
    }

    using std::to_string;

    template <typename T, std::enable_if_t<std::is_convertible_v<T &&, std::string>> * = nullptr>
    std::string to_string(T &&val) {
        return static_cast<std::string>(std::forward<T>(val));
    }

    template <typename T, std::enable_if_t<std::is_same_v<std::decay_t<T>, bool>> * = nullptr>
    std::string to_string(T val) {
        return val ? "true" : "false";
    }
} // namespace rc

#include <cassert>

namespace rc::test {
    inline void test_string_module() {
        assert(string_startswith("foobar", "foo"));
        assert(!string_startswith("foobar", "bar"));
        assert(string_endswith("foobar", "bar"));
        assert(!string_endswith("foobar", "foo"));
        assert(string_contains("foobar", "oo"));
        assert(!string_contains("foobar", "hello"));

        std::string s = "hello, world";
        string_replace(s, "world", "SJTU");
        assert(s == "hello, SJTU");

        s = "\n   hello   \n";
        string_trim(s);
        assert(s == "hello");

        s = "heLLo你好";
        string_tolower(s);
        assert(s == "hello你好");
        string_toupper(s);
        assert(s == "HELLO你好");

        s = " hello ";
        assert(string_blah_copy(string_trim, s) == "hello");
        assert(s == " hello ");
        assert(string_blah_copy(string_replace, s, " ", "a") == "ahelloa");
        assert(s == " hello ");

        using rc::to_string;
        assert(to_string(123u) == "123");
        assert(to_string(-123) == "-123");
        assert(to_string("foo") == "foo");
        assert(to_string(true) == "true");
        struct {
            operator std::string() const {
                return "foo";
            }
        } a;
        assert(to_string(a) == "foo");
    }
} // namespace rc::test
