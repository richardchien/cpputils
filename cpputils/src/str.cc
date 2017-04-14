#include "str.h"

#include <memory>
#include <codecvt>
#include <algorithm>
#include <cwctype>
#include <regex>

#include "utf8/utf8.h"
#include "exceptions.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

static auto multibyte_to_widechar(int code_page, const char *multibyte_str) {
    auto len = MultiByteToWideChar(code_page, 0, multibyte_str, -1, nullptr, 0);
    auto c_wstr_sptr = std::shared_ptr<wchar_t>(new wchar_t[len + 1]);
    MultiByteToWideChar(code_page, 0, multibyte_str, -1, c_wstr_sptr.get(), len);
    return c_wstr_sptr;
}

static auto widechar_to_multibyte(int code_page, const wchar_t *widechar_str) {
    auto len = WideCharToMultiByte(code_page, 0, widechar_str, -1, nullptr, 0, nullptr, nullptr);
    auto c_str_sptr = std::shared_ptr<char>(new char[len + 1]);
    WideCharToMultiByte(code_page, 0, widechar_str, -1, c_str_sptr.get(), len, nullptr, nullptr);
    return c_str_sptr;
}
#endif

rc::str::str() {}

rc::str::str(const str &str) : str() {
    this->inner_str_ = str.inner_str_;
}

rc::str::str(const std::string &str) : str() {
    this->inner_str_ = str;
}

rc::str::str(const char *c_str) : str(std::string(c_str)) {}

rc::str::str(const std::wstring &wstr) : str() {
    this->inner_str_ = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(wstr);
}

rc::str::str(const wchar_t *c_wstr) : str(std::wstring(c_wstr)) {}

rc::str::str(nullptr_t) {
    this->inner_str_ = "nullptr";
}

const char *rc::str::data() const {
    return this->inner_str_.data();
}

const char *rc::str::c_str() const {
    return this->inner_str_.c_str();
}

std::string rc::str::to_bytes() const {
    return this->inner_str_;
}

std::wstring rc::str::to_wstring() const {
    return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(this->inner_str_);
}

rc::str &rc::str::operator=(const str &other) {
    if (&other != this) {
        this->inner_str_ = other.inner_str_;
    }
    return *this;
}

std::ostream &rc::operator<<(std::ostream &os, const str &obj) {
#ifdef _WIN32
    os << widechar_to_multibyte(CP_ACP, multibyte_to_widechar(CP_UTF8, obj.inner_str_.c_str()).get());
#else
    os << obj.inner_str_;
#endif
    return os;
}

rc::str rc::operator+(const str &lhs, const str &rhs) {
    return str(lhs.inner_str_ + rhs.inner_str_);
}

rc::str &rc::operator+=(str &lhs, const str &rhs) {
    lhs = lhs + rhs;
    return lhs;
}

rc::str rc::operator*(const str &obj, int count) {
    auto result = str();
    for (auto i = 0; i < count; i++) {
        result += obj;
    }
    return result;
}

rc::str rc::operator*(int count, const str &obj) {
    return obj * count;
}

rc::str &rc::operator*=(str &obj, int count) {
    obj = obj * count;
    return obj;
}

rc::str::operator bool() const {
    return this->inner_str_.length() > 0;
}

rc::str::operator int() const {
    try {
        return stoi(this->inner_str_);
    } catch (std::invalid_argument &) {
        throw ValueError("could not convert string to int: \"" + *this + "\"");
    } catch (std::out_of_range &) {
        throw OutOfRangeError("converted value out of range of int");
    }
}

rc::str::operator long long() const {
    try {
        return stoll(this->inner_str_);
    } catch (std::invalid_argument &) {
        throw ValueError("could not convert string to long long: \"" + *this + "\"");
    } catch (std::out_of_range &) {
        throw OutOfRangeError("converted value out of range of long long");
    }
}

rc::str::operator float() const {
    try {
        return stof(this->inner_str_);
    } catch (std::invalid_argument &) {
        throw ValueError("could not convert string to float: \"" + *this + "\"");
    } catch (std::out_of_range &) {
        throw OutOfRangeError("converted value out of range of float");
    }
}

rc::str::operator double() const {
    try {
        return stod(this->inner_str_);
    } catch (std::invalid_argument &) {
        throw ValueError("could not convert string to double: \"" + *this + "\"");
    } catch (std::out_of_range &) {
        throw OutOfRangeError("converted value out of range of double");
    }
}

rc::str::iterator::iterator(std::string::const_iterator begin,
                            std::string::const_iterator end,
                            std::string::const_iterator it) :
    begin_(begin), end_(end), it_(it) {}

rc::str::iterator &rc::str::iterator::operator++() {
    if (this->it_ != this->end_) {
        try {
            utf8::next(this->it_, this->end_);
        } catch (utf8::not_enough_room &) {
            this->it_ = this->end_; // move to end
        }
    }
    return *this;
}

rc::str::iterator rc::str::iterator::operator++(int) {
    auto retval = *this;
    ++(*this);
    return retval;
}

rc::str::iterator &rc::str::iterator::operator--() {
    if (this->it_ != this->begin_) {
        try {
            utf8::prior(this->it_, this->begin_);
        } catch (utf8::not_enough_room &) {
            this->it_ = this->begin_; // move to begin
        }
    }
    return *this;
}

rc::str::iterator rc::str::iterator::operator--(int) {
    auto retval = *this;
    --(*this);
    return retval;
}

rc::str rc::str::iterator::operator*() const {
    auto no_value_available_error = ValueError("no valid value available");
    if (this->it_ == this->end_) {
        throw no_value_available_error;
    }

    uint32_t codepoint;
    try {
        codepoint = utf8::peek_next(this->it_, this->end_);
    } catch (utf8::not_enough_room &) {
        throw no_value_available_error;
    }
    unsigned char u[5] = {0};
    utf8::append(codepoint, u);
    return str(u);
}

rc::str::str(iterator begin, iterator end) {
    str result;
    for (auto it = begin; it != end; ++it) {
        result += *it;
    }
    *this = result;
}

rc::str::str(std::string::const_iterator begin, std::string::const_iterator end) {
    this->inner_str_ = std::string(begin, end);
}

size_t rc::str::length() const {
    size_t len = 0;
    for (auto it = this->begin(); it != this->end(); ++it) {
        len++;
    }
    return len;
}

size_t rc::str::size() const {
    return this->length();
}

size_t rc::str::byte_size() const {
    return this->inner_str_.size();
}

rc::str rc::str::operator[](index_t idx) const {
    auto index_error = IndexError("index out of range");

    auto begin = this->begin();
    auto end = this->end();

    if (begin == end) {
        // empty string has no elements
        throw index_error;
    }

    iterator it;
    if (idx >= 0) {
        // positive index
        it = begin;
        for (auto i = index_t(0); i < idx; i++) {
            // move "idx" times, from index "0" to index "idx"
            if (it == end) {
                // reach the end of the string before moving to the right index
                throw index_error;
            }
            ++it;
        }
    } else {
        // negative index
        it = end;
        auto reached_begin = false;
        for (auto i = index_t(0); i < -idx; i++) {
            // move "-idx" times, from index "len" (does not exist) to index "len + idx"
            --it;
            if (it == begin) {
                // reach the begin of the string, should just reach once
                if (reached_begin) {
                    throw index_error;
                }
                reached_begin = true;
            }
        }
    }

    try {
        return *it;
    } catch (ValueError &) {
        // caught no valid value error
        throw index_error;
    }
}

rc::str rc::str::operator[](slice slc) const {
    if (slc.stride == 0) {
        throw ValueError("slice stride (step) cannot be zero");
    }

    index_t len = this->length();

    if (len == 0) {
        // the string is empty
        return "";
    }

    if (slc.use_default_stop) {
        slc.stop = len; // convert [x::y] to [x:len:y]
    }

    // convert negative indices to positive ones
    if (slc.start < 0) {
        slc.start = len + slc.start;
    }
    if (slc.stop < 0) {
        slc.stop = len + slc.stop;
    }

    if (slc.start == slc.stop
        || slc.start > slc.stop && slc.stride > 0
        || slc.start < slc.stop && slc.stride < 0) {
        // the given range is empty
        return "";
    }

    str result;

    if (slc.stride > 0) {
        // index increasing

        if (slc.stop <= 0 || slc.start >= len) {
            // the given range has no intersection with the string
            return "";
        }

        // limit indices of the two ends
        slc.start = max(slc.start, 0);
        slc.stop = min(slc.stop, len);

        auto it = this->begin();

        // move to index "slc.start"
        for (auto i = index_t(0); i < slc.start; i++) {
            ++it;
        }

        for (auto i = slc.start; i < slc.stop; i += slc.stride) {
            try {
                result += *it;
                for (auto j = index_t(0); j < slc.stride; j++) {
                    // move forward
                    ++it;
                }
            } catch (ValueError &) {
                // caught no valid value error
                throw ValueError("invalid unicode substring");
            }
        }
    } else {
        // index decreasing

        if (slc.start <= -1 || slc.stop >= len - 1) {
            // the given range has no intersection with the string
            return "";
        }

        // limit indices of the two ends
        slc.start = min(slc.start, len - 1);
        slc.stop = max(slc.stop, -1);

        auto it = this->end();

        // move to index "slc.start"
        for (auto i = index_t(len); i > slc.start; i--) {
            --it;
        }

        for (auto i = slc.start; i > slc.stop; i += slc.stride) {
            try {
                result += *it;
                for (auto j = index_t(0); j < -slc.stride; j++) {
                    // move backward
                    --it;
                }
            } catch (ValueError &) {
                // caught no valid value error
                throw ValueError("invalid unicode substring");
            }
        }
    }

    return result;
}

rc::str rc::str::upper() const {
    auto wstr = this->to_wstring();
    transform(wstr.begin(), wstr.end(), wstr.begin(), towupper);
    return str(wstr);
}

rc::str rc::str::lower() const {
    auto wstr = this->to_wstring();
    transform(wstr.begin(), wstr.end(), wstr.begin(), towlower);
    return str(wstr);
}

bool rc::str::isupper() const {
    return *this == this->upper();
}

bool rc::str::islower() const {
    return *this == this->lower();
}

rc::str rc::str::strip_(std::function<bool(str)> predication, bool left, bool right) const {
    auto begin = this->begin();
    auto end = this->end();
    if (left) {
        while (begin != end) {
            if (predication(*begin)) {
                ++begin;
            } else {
                break;
            }
        }
    }
    if (right) {
        while (begin != end) {
            auto cur = *(--end)++;
            if (predication(cur)) {
                --end;
            } else {
                break;
            }
        }
    }
    return str(begin, end);
}

rc::str rc::str::strip_(const char *c_chars, bool left, bool right) const {
    return this->strip_(str(c_chars), left, right);
}

rc::str rc::str::strip_(str chars, bool left, bool right) const {
    return this->strip_([&chars](auto ch) {
                            for (auto strip_ch : chars) {
                                if (ch == strip_ch) {
                                    return true;
                                }
                            }
                            return false;
                        }, left, right);
}

rc::str rc::str::strip_(bool left, bool right) const {
    return this->strip_([](auto ch) {
                            auto wstr = ch.to_wstring();
                            auto wch = *wstr.c_str();
                            return iswspace(wch);
                        }, left, right);
}

std::vector<rc::str> rc::str::split(std::function<bool(str)> predication, int maxsplit) const {
    std::vector<str> results;
    auto begin = this->begin();
    auto end = this->end();
    auto part_begin = begin;
    for (auto it = begin; it != end;) {
        if (predication(*it)) {
            // meet a delimiter here
            if (it != part_begin) {
                // should split
                results.push_back(str(part_begin, it));
            }
            while (it != end && predication(*it)) {
                // move to the next non-delimiter character
                ++it;
            }
            part_begin = it;

            // check split times
            if (maxsplit > 0) {
                maxsplit--;
            } else if (maxsplit == 0) {
                if (it != end) {
                    results.push_back(str(it, end));
                }
                break;
            }
        } else {
            ++it;
        }
    }

    return results;
}

std::vector<rc::str> rc::str::split(str sep, int maxsplit) const {
    return this->split([&sep](auto ch) {
                           for (auto sep_ch : sep) {
                               if (ch == sep_ch) {
                                   return true;
                               }
                           }
                           return false;
                       }, maxsplit);
}

std::vector<rc::str> rc::str::split(const char *c_sep, int maxsplit) const {
    return this->split(str(c_sep), maxsplit);
}

std::vector<rc::str> rc::str::split(int maxsplit) const {
    return this->split([](auto ch) {
                           auto wstr = ch.to_wstring();
                           auto wch = *wstr.c_str();
                           return iswspace(wch);
                       }, maxsplit);
}

std::vector<rc::str> rc::str::rsplit(std::function<bool(str)> predication, int maxsplit) const {
    std::vector<str> results_reversed;
    auto begin = this->begin();
    auto end = this->end();
    auto part_end = end;
    for (auto it = end; it != begin;) {
        if (predication(*(--it)++)) {
            // meet a delimiter here
            if (it != part_end) {
                // should split
                results_reversed.push_back(str(it, part_end));
            }
            while (it != begin && predication(*(--it)++)) {
                // move to the next (from right to left) non-delimiter character
                --it;
            }
            part_end = it;

            // check split times
            if (maxsplit > 0) {
                maxsplit--;
            } else if (maxsplit == 0) {
                if (it != begin) {
                    results_reversed.push_back(str(begin, it));
                }
                break;
            }
        } else {
            --it;
        }
    }

    std::vector<str> results;
    while (!results_reversed.empty()) {
        results.push_back(results_reversed.back());
        results_reversed.pop_back();
    }
    return results;
}

std::vector<rc::str> rc::str::rsplit(str sep, int maxsplit) const {
    return this->rsplit([&sep](auto ch) {
                            for (auto sep_ch : sep) {
                                if (ch == sep_ch) {
                                    return true;
                                }
                            }
                            return false;
                        }, maxsplit);
}

std::vector<rc::str> rc::str::rsplit(const char *c_sep, int maxsplit) const {
    return this->rsplit(str(c_sep), maxsplit);
}

std::vector<rc::str> rc::str::rsplit(int maxsplit) const {
    return this->rsplit([](auto ch) {
                            auto wstr = ch.to_wstring();
                            auto wch = *wstr.c_str();
                            return iswspace(wch);
                        }, maxsplit);
}

std::vector<rc::str> rc::str::splitlines(bool keepends) const {
    std::vector<str> results;
    auto it = this->c_begin();
    auto end = this->c_end();
    std::regex exp("\\r\\n|\\r|\\n");
    std::smatch sm;
    while (regex_search(it, end, sm, exp)) {
        results.push_back(str(it, it + sm.position()) + (keepends ? sm.str() : ""));
        it += sm.position() + sm.length();
    }
    if (it != end) {
        results.push_back(str(it, end));
    }
    return results;
}
