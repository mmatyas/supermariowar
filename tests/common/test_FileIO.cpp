#include "doctest.h"

#include "FileIO.h"

#include <cstdio>
#include <filesystem>
#include <random>
#include <stdexcept>
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


TEST_CASE("FileIO strings") {
    const std::vector<std::string> tests { "", "Hello World" };
    for (const std::string& expected : tests) {
        CAPTURE(expected);

        TempFile tmp;
        BinaryFile f(tmp.path(), "w+b");
        REQUIRE(f.is_open());

        const long expected_bytelen = 4 + expected.size() + 1;
        f.write_string_long(expected);
        CHECK(f.pos() == expected_bytelen);

        f.rewind();
        CHECK(f.read_string_long(256) == expected);
        CHECK(f.pos() == expected_bytelen);
    }
}
