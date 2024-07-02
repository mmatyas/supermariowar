#pragma once


template<typename T>
struct Vec2 {
    T x = 0;
    T y = 0;

    constexpr explicit Vec2() = default;
    constexpr Vec2(T inX, T inY) : x(inX), y(inY) {}

    static constexpr Vec2 zero() {
        return Vec2();
    }

    constexpr Vec2& operator+=(const Vec2& rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
    constexpr Vec2& operator-=(const Vec2& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    template<typename Scalar> constexpr Vec2& operator*=(Scalar val) {
        x *= val;
        y *= val;
        return *this;
    }
    template<typename Scalar> constexpr Vec2& operator/=(Scalar val) {
        x /= val;
        y /= val;
        return *this;
    }
};


template<typename T> constexpr Vec2<T> operator+(const Vec2<T>& lhs, const Vec2<T>& rhs) {
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}
template<typename T> constexpr Vec2<T> operator-(const Vec2<T>& lhs, const Vec2<T>& rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}
template<typename T> constexpr Vec2<T> operator*(const Vec2<T>& lhs, const Vec2<T>& rhs) {
    return {lhs.x * rhs.x, lhs.y * rhs.y};
}
template<typename T> constexpr Vec2<T> operator/(const Vec2<T>& lhs, const Vec2<T>& rhs) {
    return {lhs.x / rhs.x, lhs.y / rhs.y};
}

template<typename T, typename Scalar> constexpr Vec2<T> operator*(const Vec2<T>& lhs, Scalar scalar) {
    return {lhs.x * scalar, lhs.y * scalar};
}
template<typename T, typename Scalar> constexpr Vec2<T> operator/(const Vec2<T>& lhs, Scalar scalar) {
    return {lhs.x / scalar, lhs.y / scalar};
}


using Vec2f = Vec2<float>;
using Vec2s = Vec2<short>;
