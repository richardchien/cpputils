#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <vector>

namespace rc {
    using byte = uint8_t;
    using byte_vec = std::vector<byte>;

    inline bool _is_big_endian() {
        union {
            uint32_t val;
            byte bytes[sizeof(uint32_t)];
        } u{0x01020304};
        return u.bytes[0] == 0x01;
    }

    enum Endian {
        little_endian = -1,
        big_endian = 1,
    };

    inline Endian system_endian() {
        return _is_big_endian() ? big_endian : little_endian;
    }

#define __trivial_only(T) std::enable_if_t<std::is_trivial_v<T>> * = nullptr

    template <typename T, __trivial_only(T)>
    byte_vec val_to_bytes(const T val, const Endian endian = system_endian()) {
        union {
            T val;
            byte bytes[sizeof(T)];
        } u{val};
        if (endian != system_endian()) {
            std::reverse(std::begin(u.bytes), std::end(u.bytes));
        }
        return {std::begin(u.bytes), std::end(u.bytes)};
    }

    template <typename T, __trivial_only(T)>
    T bytes_to_val(const byte_vec &bytes, const Endian endian = system_endian()) {
        assert(bytes.size() == sizeof(T));
        union {
            T val;
            byte bytes[sizeof(T)];
        } u;
        if (endian == system_endian()) {
            std::copy(bytes.cbegin(), bytes.cend(), std::begin(u.bytes));
        } else {
            std::reverse_copy(bytes.cbegin(), bytes.cend(), std::begin(u.bytes));
        }
        return u.val;
    }

    template <typename T, __trivial_only(T)>
    T swap_endian(const T val) {
        const auto endian = system_endian();
        return bytes_to_val<T>(val_to_bytes<T>(val, endian), static_cast<Endian>(-endian));
    }

    template <typename T, __trivial_only(T)>
    T ensure_endian(const T val, const Endian endian) {
        return endian == system_endian() ? val : swap_endian<T>(val);
    }

#undef __trivial_only

    inline void test_endian_module() {
        static_assert(-little_endian == big_endian);
        static_assert(sizeof(byte) == 1);
        assert(system_endian() == little_endian || system_endian() == big_endian);
        if (system_endian() == little_endian) {
            const auto b = val_to_bytes<int32_t>(0x01020304);
            assert(b[0] == 0x04 && b[1] == 0x03 && b[2] == 0x02 && b[3] == 0x01);
            assert(bytes_to_val<int32_t>(b) == 0x01020304);
        }
        assert(swap_endian<int32_t>(0x01020304) == 0x04030201);
        assert(val_to_bytes<int32_t>(ensure_endian<int32_t>(0x01020304, little_endian))[0] == 0x04);
        assert(val_to_bytes<int32_t>(ensure_endian<int32_t>(0x01020304, big_endian))[0] == 0x01);
    }
} // namespace rc
