#pragma once

#include <cassert>
#include <cstdint>
#include <string>
#include <type_traits>
#include <vector>

#include "endian.hpp"

namespace rc {
    template <typename _StringSizeType = uint32_t>
    class BinPack {
    public:
        using string_size_type = _StringSizeType;

        BinPack() : BinPack({}, 0) {
        }

        BinPack(byte_vec bytes) : _bytes(std::move(bytes)) {
        }

        template <typename Iter>
        BinPack(Iter begin, Iter end) : _bytes(std::move(begin), std::move(end)) {
        }

        Endian endian() const {
            return _endian;
        }

        void set_endian(Endian endian) {
            _endian = endian;
        }

        size_t size() const {
            return _bytes.size();
        }

        bool empty() const {
            return _bytes.empty();
        }

        void clear() {
            _bytes.clear();
        }

        byte *data() {
            return _bytes.data();
        }

        const byte *data() const {
            return _bytes.data();
        }

#define __trivial_only(T) std::enable_if_t<std::is_trivial_v<T>> * = nullptr
#define __string_only(S) std::enable_if_t<std::is_same_v<S, std::string>> * = nullptr

        template <typename T, __trivial_only(T)>
        bool has_next() const {
            return size() >= sizeof(T);
        }

        template <typename S, __string_only(S)>
        bool has_next() const {
            if (!has_next<string_size_type>()) return false;
            const auto str_size = peek<string_size_type>();
            return size() - sizeof(string_size_type) >= str_size;
        }

        template <typename T, __trivial_only(T)>
        size_t put(const T &val) {
            const auto b = val_to_bytes<T>(val, _endian);
            std::copy(b.cbegin(), b.cend(), std::back_inserter(_bytes));
            return b.size();
        }

        template <typename S, __string_only(S)>
        size_t put(const S &s) {
            const string_size_type size = s.size(); // with possible truncation
            const auto size_size = put<string_size_type>(size);
            std::copy(s.cbegin(), s.cbegin() + size, std::back_inserter(_bytes));
            return size_size + size;
        }

        template <typename T, __trivial_only(T)>
        T peek() const {
            assert(has_next<T>());
            byte_vec b;
            std::copy(_bytes.cbegin(), _bytes.cbegin() + sizeof(T), std::back_inserter(b));
            return bytes_to_val<T>(b, _endian);
        }

        template <typename S, __string_only(S)>
        S peek() const {
            assert(has_next<S>());
            S s;
            const auto str_size = peek<string_size_type>();
            const auto begin = _bytes.cbegin() + sizeof(string_size_type), end = begin + str_size;
            std::copy(begin, end, std::back_inserter(s));
            return s;
        }

        template <typename T, __trivial_only(T)>
        T get() {
            auto res = peek<T>();
            _bytes.erase(_bytes.cbegin(), _bytes.cbegin() + sizeof(T));
            return res;
        }

        template <typename S, __string_only(S)>
        S get() {
            assert(has_next<S>());
            S s;
            const auto str_size = get<string_size_type>();
            const auto begin = _bytes.cbegin(), end = begin + str_size;
            std::copy(begin, end, std::back_inserter(s));
            _bytes.erase(begin, end);
            return s;
        }

#undef __trivial_only
#undef __string_only

    private:
        byte_vec _bytes;
        Endian _endian = little_endian;
    };

    inline void test_binpack_module() {
        BinPack binpack;

        assert(binpack.put<int32_t>(0x01020304) == 4);
        assert(binpack.size() == 4);
        assert(binpack.has_next<int32_t>());
        assert(binpack.get<int32_t>() == 0x01020304);
        assert(!binpack.has_next<int16_t>());
        assert(binpack.put<int16_t>(0x0102) == 2);
        assert(!binpack.has_next<int32_t>());
        assert(binpack.get<uint8_t>() == 0x02); // little endian by default
        assert(binpack.get<uint8_t>() == 0x01);
        assert(binpack.empty());

        binpack.set_endian(big_endian);
        binpack.put<int32_t>(0x01020304);
        assert(binpack.get<int16_t>() == 0x0102);
        binpack.set_endian(little_endian);
        assert(binpack.get<int16_t>() == 0x0403);

        assert(binpack.put<std::string>("foo") == 4 + 3);
        assert(binpack.has_next<std::string>());
        assert(binpack.peek<std::string>() == "foo");
        assert(binpack.get<std::string>() == "foo");
        assert(!binpack.has_next<std::string>());

        binpack.clear();
        assert(binpack.empty());

        binpack.put<int>(0x01020304);
        BinPack bp2{binpack.data(), binpack.data() + binpack.size()};
        assert(bp2.get<int>() == 0x01020304);
    }
} // namespace rc
