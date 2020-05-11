#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <vector>

namespace rc {
    inline bool _is_big_endian() {
        union {
            uint32_t val;
            uint8_t bytes[sizeof(uint32_t)];
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

    using byte = uint8_t;
    using byte_vec = std::vector<byte>;

    template <typename T>
    byte_vec val_to_bytes(T val, Endian endian = system_endian()) {
        union {
            T val;
            uint8_t bytes[sizeof(T)];
        } u{val};
        if (endian != system_endian()) {
            std::reverse(std::begin(u.bytes), std::end(u.bytes));
        }
        return {std::begin(u.bytes), std::end(u.bytes)};
    }

    template <typename T>
    T bytes_to_val(const byte_vec &bytes, Endian endian = system_endian()) {
        assert(bytes.size() == sizeof(T));
        union {
            T val;
            uint8_t bytes[sizeof(T)];
        } u;
        if (endian == system_endian()) {
            std::copy(bytes.cbegin(), bytes.cend(), std::begin(u.bytes));
        } else {
            std::reverse_copy(bytes.cbegin(), bytes.cend(), std::begin(u.bytes));
        }
        return u.val;
    }

    template <typename T>
    T swap_endian(T val) {
        const auto endian = system_endian();
        return bytes_to_val<T>(val_to_bytes<T>(val, endian), static_cast<Endian>(-endian));
    }

    template <typename T>
    T ensure_endian(T val, Endian endian) {
        return endian == system_endian() ? val : swap_endian<T>(val);
    }

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
