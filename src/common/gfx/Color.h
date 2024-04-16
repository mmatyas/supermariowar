#pragma once

#include <cstdint>


/// Represents a solid RGB color.
struct RGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};


namespace colors {
constexpr RGB BLACK { 0, 0, 0 };
constexpr RGB MAGENTA { 255, 0, 255 };
} // colors
