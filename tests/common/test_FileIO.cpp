#include "doctest.h"

#include "FileIO.h"

#include <array>
#include <cstdio>
#include <filesystem>
#include <random>
#include <string>

namespace fs = std::filesystem;


struct TempFile {
    TempFile()
        : m_path(fs::temp_directory_path() / randomName())
    {}

    ~TempFile() {
        fs::remove(m_path);
    }

    const fs::path& path() const { return m_path; }

    TempFile(const TempFile&) = delete;
    TempFile& operator=(const TempFile&) = delete;

private:
    static std::string randomName() {
        static std::mt19937_64 rng(std::random_device{}());
        return "test_fileio_" + std::to_string(rng());
    }

    fs::path m_path;
};


TEST_CASE("FileIO plain data") {
    TempFile tmp;
    BinaryFile f(tmp.path(), "w+b");
    REQUIRE(f.is_open());

    f.write_i8(-42);
    CHECK(f.pos() == 1);
    f.write_u8(200);
    CHECK(f.pos() == 2);
    f.write_i16(-12345);
    CHECK(f.pos() == 4);
    f.write_i32(123456789);
    CHECK(f.pos() == 8);
    f.write_float(3.1415926f);
    CHECK(f.pos() == 12);
    f.write_bool(true); // stored on 1 byte
    CHECK(f.pos() == 13);

    f.rewind();

    CHECK(f.read_i8() == -42);
    CHECK(f.pos() == 1);
    CHECK(f.read_u8() == 200);
    CHECK(f.pos() == 2);
    CHECK(f.read_i16() == -12345);
    CHECK(f.pos() == 4);
    CHECK(f.read_i32() == 123456789);
    CHECK(f.pos() == 8);
    CHECK(f.read_float() == 3.1415926f);
    CHECK(f.pos() == 12);
    CHECK(f.read_bool() == true);
    CHECK(f.pos() == 13);
}


TEST_CASE("FileIO array") {
    TempFile tmp;
    BinaryFile f(tmp.path(), "w+b");
    REQUIRE(f.is_open());

    const std::array<int16_t, 3> expected_i16 { 1, 0, -42 };
    const std::array<int32_t, 3> expected_i32 { 123456789, 0, -42 };
    for (int16_t value : expected_i16) {
        f.write_i16(value);
    }
    for (int32_t value : expected_i32) {
        f.write_i32(value);
    }
    CHECK(f.pos() == expected_i16.size() * 2 + expected_i32.size() * 4);

    f.rewind();

    std::array<int16_t, 3> actual_i16 = {};
    std::array<int32_t, 3> actual_i32 = {};
    f.read_i16_array(actual_i16.data(), actual_i16.size());
    f.read_i32_array(actual_i32.data(), actual_i32.size());
    CHECK(actual_i16 == expected_i16);
    CHECK(actual_i32 == expected_i32);
    CHECK(f.pos() == expected_i16.size() * 2 + expected_i32.size() * 4);
}


TEST_CASE("FileIO strings") {
    TempFile tmp;

    const std::vector<std::string> tests { "", "Hello World" };
    for (const std::string& expected : tests) {
        CAPTURE(expected);

        BinaryFile f(tmp.path(), "w+b");
        REQUIRE(f.is_open());

        const long expected_bytelen = 4 + expected.size() + 1;
        f.write_string_long(expected);
        CHECK(f.pos() == expected_bytelen);

        f.rewind();
        CHECK(f.read_string_long(256) == expected);
        CHECK(f.pos() == expected_bytelen);
    }
    {
        BinaryFile f(tmp.path(), "w+b");
        REQUIRE(f.is_open());

        // Check read error
        CHECK_THROWS(f.read_string_long(256));

        // Check zero stored length
        f.write_i32(0);
        f.rewind();
        CHECK(f.read_string_long(256) == std::string());
    }
    {
        { BinaryFile create(tmp.path(), "wb"); }
        BinaryFile f(tmp.path(), "rb");
        REQUIRE(f.is_open());

        // Test write error
        CHECK_THROWS(f.write_string_long("test"));
    }
}
