#include "doctest.h"

#include "util/ContainerHelpers.h"
#include "util/DirIterator.h"

#include <filesystem>
#include <fstream>
#include <random>

namespace fs = std::filesystem;

namespace {
class TempDir {
public:
    TempDir() {
        m_path = fs::temp_directory_path() / randomName();
        fs::create_directory(m_path);
    }

    ~TempDir() {
        std::error_code ec;
        fs::remove_all(m_path, ec); // ignore cleanup failure
    }

    const fs::path& path() const { return m_path; }

private:
    static std::string randomName() {
        static std::mt19937_64 rng(std::random_device{}());
        return "diriter_test_" + std::to_string(rng());
    }

    fs::path m_path;
};

void touch(const fs::path& p) {
    std::ofstream f(p.string());
}

void mkdir(const fs::path& p) {
    fs::create_directory(p);
}
} // namespace


TEST_CASE("FilesIterator filters by extension") {
    TempDir tmp;

    touch(tmp.path() / "a.txt");
    touch(tmp.path() / "b.md");
    touch(tmp.path() / "c.cpp");
    mkdir(tmp.path() / "dir");

    FilesIterator it(tmp.path(), {".txt", ".md"});

    std::vector<std::string> found;
    while (auto p = it.next()) {
        found.emplace_back(p->filename().string());
    }

    CHECK(found.size() == 2);
    CHECK(utils::contains(found, "a.txt"));
    CHECK(utils::contains(found, "b.md"));
}


TEST_CASE("SubdirsIterator lists only subdirectories") {
    TempDir tmp;

    touch(tmp.path() / "file1.txt");
    touch(tmp.path() / "file2.md");
    mkdir(tmp.path() / "sub1");
    mkdir(tmp.path() / "sub2");

    SubdirsIterator it(tmp.path());

    std::vector<std::string> found;
    while (auto p = it.next()) {
        found.emplace_back(p->filename().string());
    }

    CHECK(found.size() == 2);
    CHECK(utils::contains(found, "sub1"));
    CHECK(utils::contains(found, "sub2"));
}


TEST_CASE("Iterator handles empty directory") {
    TempDir tmp;

    FilesIterator it(tmp.path());

    CHECK(!it.next().has_value());
}


TEST_CASE("Iterator handles missing directory") {
    FilesIterator it("this_should_not_exist_12345");

    CHECK(!it.next().has_value());
}


TEST_CASE("Returned paths remain valid") {
    TempDir tmp;
    touch(tmp.path() / "x.txt");

    FilesIterator it(tmp.path());
    auto p = it.next();

    REQUIRE(p.has_value());
    CHECK(fs::exists(*p));
}
