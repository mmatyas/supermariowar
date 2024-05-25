#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "math/Vec2.h"


TEST_CASE("Testing Vec2f default constructor") {
    Vec2f v;
    CHECK(v.x == doctest::Approx(0.0f));
    CHECK(v.y == doctest::Approx(0.0f));
}

TEST_CASE("Testing Vec2f parameterized constructor") {
    Vec2f v(1.0f, 2.0f);
    CHECK(v.x == doctest::Approx(1.0f));
    CHECK(v.y == doctest::Approx(2.0f));
}

TEST_CASE("Testing Vec2f::zero method") {
    Vec2f v = Vec2f::zero();
    CHECK(v.x == doctest::Approx(0.0f));
    CHECK(v.y == doctest::Approx(0.0f));
}

TEST_CASE("Testing Vec2f operator+=") {
    Vec2f v1(1.0f, 2.0f);
    Vec2f v2(3.0f, 4.0f);
    v1 += v2;
    CHECK(v1.x == doctest::Approx(4.0f));
    CHECK(v1.y == doctest::Approx(6.0f));
}

TEST_CASE("Testing Vec2f operator-=") {
    Vec2f v1(5.0f, 7.0f);
    Vec2f v2(2.0f, 3.0f);
    v1 -= v2;
    CHECK(v1.x == doctest::Approx(3.0f));
    CHECK(v1.y == doctest::Approx(4.0f));
}

TEST_CASE("Testing Vec2f operator*=") {
    Vec2f v(2.0f, 3.0f);
    v *= 2.0f;
    CHECK(v.x == doctest::Approx(4.0f));
    CHECK(v.y == doctest::Approx(6.0f));
}

TEST_CASE("Testing Vec2f operator/=") {
    Vec2f v(6.0f, 8.0f);
    v /= 2.0f;
    CHECK(v.x == doctest::Approx(3.0f));
    CHECK(v.y == doctest::Approx(4.0f));
}

TEST_CASE("Testing Vec2f operator+") {
    Vec2f v1(1.0f, 2.0f);
    Vec2f v2(3.0f, 4.0f);
    Vec2f result = v1 + v2;
    CHECK(result.x == doctest::Approx(4.0f));
    CHECK(result.y == doctest::Approx(6.0f));
}

TEST_CASE("Testing Vec2f operator-") {
    Vec2f v1(5.0f, 7.0f);
    Vec2f v2(2.0f, 3.0f);
    Vec2f result = v1 - v2;
    CHECK(result.x == doctest::Approx(3.0f));
    CHECK(result.y == doctest::Approx(4.0f));
}

TEST_CASE("Testing Vec2f operator* with Vec2f") {
    Vec2f v1(2.0f, 3.0f);
    Vec2f v2(4.0f, 5.0f);
    Vec2f result = v1 * v2;
    CHECK(result.x == doctest::Approx(8.0f));
    CHECK(result.y == doctest::Approx(15.0f));
}

TEST_CASE("Testing Vec2f operator/ with Vec2f") {
    Vec2f v1(6.0f, 8.0f);
    Vec2f v2(2.0f, 4.0f);
    Vec2f result = v1 / v2;
    CHECK(result.x == doctest::Approx(3.0f));
    CHECK(result.y == doctest::Approx(2.0f));
}

TEST_CASE("Testing Vec2f operator* with scalar") {
    Vec2f v(2.0f, 3.0f);
    Vec2f result = v * 2.0f;
    CHECK(result.x == doctest::Approx(4.0f));
    CHECK(result.y == doctest::Approx(6.0f));
}

TEST_CASE("Testing Vec2f operator/ with scalar") {
    Vec2f v(6.0f, 8.0f);
    Vec2f result = v / 2.0f;
    CHECK(result.x == doctest::Approx(3.0f));
    CHECK(result.y == doctest::Approx(4.0f));
}
