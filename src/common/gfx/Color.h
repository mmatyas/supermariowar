#pragma once

#include <cstdint>


/// Represents an RGBA color.
struct RGBA {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};


/// Represents a solid RGB color.
struct RGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;

    constexpr RGBA withAlpha(uint8_t a) const {
        return RGBA { r, g, b, a };
    }
};
