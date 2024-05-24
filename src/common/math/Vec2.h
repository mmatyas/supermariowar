#pragma once


struct Vec2f {
    float x = 0.f;
    float y = 0.f;

    static constexpr Vec2f zero() {
        return {0.f, 0.f};
    }

    constexpr Vec2f& operator+=(Vec2f& rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
    constexpr Vec2f& operator-=(Vec2f& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    constexpr Vec2f& operator*=(float val) {
        x *= val;
        y *= val;
        return *this;
    }
    constexpr Vec2f& operator/=(float val) {
        x /= val;
        y /= val;
        return *this;
    }
};


constexpr Vec2f operator+(const Vec2f& lhs, const Vec2f& rhs) {
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}
constexpr Vec2f operator-(const Vec2f& lhs, const Vec2f& rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}
constexpr Vec2f operator*(const Vec2f& lhs, const Vec2f& rhs) {
    return {lhs.x * rhs.x, lhs.y * rhs.y};
}
constexpr Vec2f operator/(const Vec2f& lhs, const Vec2f& rhs) {
    return {lhs.x / rhs.x, lhs.y / rhs.y};
}
