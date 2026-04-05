#pragma once

#include <filesystem>
#include <optional>
#include <vector>


class DirIterator {
protected:
    DirIterator(const std::filesystem::path& path);
    virtual ~DirIterator() = default;

    virtual bool isAccepted(const std::filesystem::directory_entry& entry) const = 0;

public:
    std::optional<std::filesystem::path> next();

protected:
    std::filesystem::directory_iterator m_iter;
};


/// Iterates over the files inside a directory.
class FilesIterator : public DirIterator {
public:
    FilesIterator(const std::filesystem::path& path, std::vector<std::string> extensions = {});

private:
    std::vector<std::string> m_extensions;

    bool isAccepted(const std::filesystem::directory_entry& entry) const override;
};


/// Iterates over the subdirectories inside a directory.
class SubdirsIterator : public DirIterator  {
public:
    SubdirsIterator(const std::filesystem::path& path);

private:
    bool isAccepted(const std::filesystem::directory_entry& entry) const override;
};
