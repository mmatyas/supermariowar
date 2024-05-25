#pragma once


struct Vec2f {
    float x = 0.f;
    float y = 0.f;

    constexpr explicit Vec2f() = default;
    constexpr explicit Vec2f(float inX, float inY) : x(inX), y(inY) {}

    static constexpr Vec2f zero() {
        return Vec2f();
    }

    constexpr Vec2f& operator+=(const Vec2f& rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
    constexpr Vec2f& operator-=(const Vec2f& rhs) {
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
    return Vec2f(lhs.x + rhs.x, lhs.y + rhs.y);
}
constexpr Vec2f operator-(const Vec2f& lhs, const Vec2f& rhs) {
    return Vec2f(lhs.x - rhs.x, lhs.y - rhs.y);
}
constexpr Vec2f operator*(const Vec2f& lhs, const Vec2f& rhs) {
    return Vec2f(lhs.x * rhs.x, lhs.y * rhs.y);
}
constexpr Vec2f operator/(const Vec2f& lhs, const Vec2f& rhs) {
    return Vec2f(lhs.x / rhs.x, lhs.y / rhs.y);
}

constexpr Vec2f operator*(const Vec2f& lhs, float scalar) {
    return Vec2f(lhs.x * scalar, lhs.y * scalar);
}
constexpr Vec2f operator/(const Vec2f& lhs, float scalar) {
    return Vec2f(lhs.x / scalar, lhs.y / scalar);
}
