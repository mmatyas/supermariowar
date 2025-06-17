#pragma once

#include <cstdint>


struct Version {
    uint8_t major = 0;
    uint8_t minor = 0;
    uint8_t patch = 0;
    uint8_t build = 0;

    constexpr uint32_t asU32() const {
        return (major << 24) | (minor << 16) | (patch << 8) | build;
    }
};


constexpr bool operator==(const Version& a, const Version& b) {
    return a.asU32() == b.asU32();
}
constexpr bool operator<=(const Version& a, const Version& b) {
    return a.asU32() <= b.asU32();
}
constexpr bool operator<(const Version& a, const Version& b) {
    return a.asU32() < b.asU32();
}
constexpr bool operator!=(const Version& a, const Version& b) {
    return !(a == b);
}
constexpr bool operator>=(const Version& a, const Version& b) {
    return !(a < b);
}
constexpr bool operator>(const Version& a, const Version& b) {
    return !(a <= b);
}


/// 1.8.0.0 == Release to staff
/// 1.8.0.1 == Second release to staff
/// 1.8.0.2 == beta1
/// 1.8.0.3 == beta2
/// 1.8.0.4 == final
/// 1.9.0.0 == neagix work-in-progress, not released
/// 2.0.0.0 == fluffypillow netplay code
constexpr Version GAME_VERSION {2, 0, 0, 1};
