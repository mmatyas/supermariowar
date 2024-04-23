#pragma once

#include <cstdint>


/// Represents a solid RGB color.
struct RGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};


constexpr bool operator==(const RGB& c1, const RGB& c2) {
    return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b;
}


namespace colors {
constexpr RGB BLACK { 0, 0, 0 };
constexpr RGB MAGENTA { 255, 0, 255 };
} // colors
