#pragma once

#include <cstdint>
#include <functional>


/// Represents a solid RGB color.
struct RGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;

    constexpr bool operator==(const RGB&) const = default;
};


namespace colors {
constexpr RGB BLACK { 0, 0, 0 };
constexpr RGB MAGENTA { 255, 0, 255 };
} // colors


template<>
struct std::hash<RGB> {
    size_t operator()(const RGB& c) const noexcept {
        return (static_cast<size_t>(c.r) << 16) | (static_cast<size_t>(c.g) << 8) | static_cast<size_t>(c.b);
    }
};
