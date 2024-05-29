#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "Version.h"


TEST_CASE("Version operator==") {
    constexpr Version v1 {1, 0, 0, 0};
    constexpr Version v2 {1, 0, 1, 0};

    CHECK(v1 == v1);
    CHECK_FALSE(v1 == v2);
}

TEST_CASE("Version operator!=") {
    constexpr Version v1 {1, 0, 0, 0};
    constexpr Version v2 {1, 0, 1, 0};

    CHECK_FALSE(v1 != v1);
    CHECK(v1 != v2);
}

TEST_CASE("Version operator<") {
    constexpr Version v1 {1, 0, 0, 0};
    constexpr Version v3 {1, 0, 1, 0};
    constexpr Version v4 {1, 1, 0, 0};
    constexpr Version v5 {2, 0, 0, 0};

    CHECK_FALSE(v1 < v1);
    CHECK(v1 < v3);
    CHECK(v1 < v4);
    CHECK(v1 < v5);
    CHECK_FALSE(v5 < v1);
}

TEST_CASE("Version operator<=") {
    constexpr Version v1 {1, 0, 0, 0};
    constexpr Version v2 {1, 0, 1, 0};
    constexpr Version v3 {1, 1, 0, 0};
    constexpr Version v4 {2, 0, 0, 0};

    CHECK(v1 <= v1);
    CHECK(v1 <= v2);
    CHECK(v1 <= v3);
    CHECK(v1 <= v4);
    CHECK_FALSE(v4 <= v1);
}

TEST_CASE("Version operator>") {
    constexpr Version v1 {1, 0, 0, 0};
    constexpr Version v2 {1, 0, 1, 0};
    constexpr Version v3 {1, 1, 0, 0};
    constexpr Version v4 {2, 0, 0, 0};

    CHECK_FALSE(v1 > v1);
    CHECK(v2 > v1);
    CHECK(v3 > v1);
    CHECK(v4 > v1);
    CHECK_FALSE(v1 > v4);
}

TEST_CASE("Version operator>=") {
    constexpr Version v1 {1, 0, 0, 0};
    constexpr Version v2 {1, 0, 1, 0};
    constexpr Version v3 {1, 1, 0, 0};
    constexpr Version v4 {2, 0, 0, 0};

    CHECK(v1 >= v1);
    CHECK(v2 >= v1);
    CHECK(v3 >= v1);
    CHECK(v4 >= v1);
    CHECK_FALSE(v1 >= v4);
}
