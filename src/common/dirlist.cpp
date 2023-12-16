#include "dirlist.h"

#include "linfunc.h"

#include <algorithm>


GenericDirIterator::GenericDirIterator(const fs::path& dirpath)
{
    // It's OK if the directory doesn't exist
    std::error_code err;
    m_dir_it = fs::directory_iterator(dirpath, err);
}

bool GenericDirIterator::next(fs::path& out)
{
    bool found = false;
    while (entry_valid() && !found) {
        found = passesFilter(*m_dir_it);
        if (found) {
            out = m_dir_it->path();
        }
        m_dir_it++;
    }
    return found;
}


FilesIterator::FilesIterator(const fs::path& dirpath)
    : GenericDirIterator(dirpath)
{}

FilesIterator::FilesIterator(const fs::path& dirpath, std::string extension)
    : GenericDirIterator(dirpath)
{
    addExtension(std::move(extension));
}

FilesIterator& FilesIterator::addExtension(std::string&& ext)
{
    inPlaceLowerCase(ext);
    m_extensions.emplace_back(std::move(ext));
    return *this;
}

bool FilesIterator::passesFilter(const fs::directory_entry& entry) const
{
    std::string lowercase_ext = entry.path().extension().string();
    inPlaceLowerCase(lowercase_ext);
    return std::find(m_extensions.cbegin(), m_extensions.cend(), lowercase_ext) != m_extensions.cend();
}


SubdirsIterator::SubdirsIterator(const fs::path& dirpath)
    : GenericDirIterator(dirpath)
{}

bool SubdirsIterator::passesFilter(const fs::directory_entry& entry) const
{
    return entry.is_directory();
}
