#pragma once

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;


/// Base class for filtered directory iteration.
class GenericDirIterator {
public:
    GenericDirIterator(const fs::path& dirpath);

    bool next(fs::path& out);

protected:
    virtual bool passesFilter(const fs::directory_entry& entry) const = 0;

private:
    fs::directory_iterator m_dir_it;

    bool entry_valid() const { return m_dir_it != fs::end(m_dir_it); };
};


/// Iterator for finding files with the specific extensions.
class FilesIterator : public GenericDirIterator {
public:
    FilesIterator(const fs::path& dirpath, std::string extension);
    FilesIterator(const fs::path& dirpath);

    FilesIterator& addExtension(std::string&& ext);

protected:
    virtual bool passesFilter(const fs::directory_entry& entry) const override;

private:
    std::vector<std::string> m_extensions;
};


/// Iterator for finding subdirectories.
class SubdirsIterator : public GenericDirIterator {
public:
    SubdirsIterator(const fs::path& dirpath);

protected:
    virtual bool passesFilter(const fs::directory_entry& entry) const override;
};
