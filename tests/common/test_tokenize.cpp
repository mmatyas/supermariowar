#include "doctest.h"

#include "linfunc.h"


TEST_CASE("Empty string") {
    const std::string text = "";
    const std::list<std::string_view> result = tokenize(text, ',');
    CHECK(result.size() == 1);
    CHECK(result.front() == text);
}

TEST_CASE("No delimiter") {
    const std::string text = "textwithoutdelimiter";
    const std::list<std::string_view> result = tokenize(text, ',');
    CHECK(result.size() == 1);
    CHECK(result.front() == text);
}

TEST_CASE("One or more delimiters") {
    const std::string text = "apple,banana,cherry";

    SUBCASE("Zero") {
        const std::list<std::string_view> result = tokenize(text, ',', 0);
        const std::list<std::string_view> expected {"apple,banana,cherry"};
        CHECK(result.size() == expected.size());
        CHECK(result == expected);
    }

    SUBCASE("One split") {
        const std::list<std::string_view> result = tokenize(text, ',', 1);
        const std::list<std::string_view> expected {"apple", "banana,cherry"};
        CHECK(result.size() == expected.size());
        CHECK(result == expected);
    }

    SUBCASE("Two splits") {
        const std::list<std::string_view> result = tokenize(text, ',', 2);
        const std::list<std::string_view> expected {"apple", "banana", "cherry"};
        CHECK(result.size() == expected.size());
        CHECK(result == expected);
    }

    SUBCASE("Unlimited") {
        const std::list<std::string_view> result = tokenize(text, ',');
        const std::list<std::string_view> expected {"apple", "banana", "cherry"};
        CHECK(result.size() == expected.size());
        CHECK(result == expected);
    }
}
