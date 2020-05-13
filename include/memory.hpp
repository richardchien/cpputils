#pragma once

#include <memory>

namespace rc {
    using std::make_shared;

    template <typename T>
    std::shared_ptr<T> make_shared_array(const size_t size) {
        return std::shared_ptr<T>(new T[size], [](T *p) { delete[] p; });
    }
} // namespace rc
