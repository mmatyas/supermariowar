#include "DirIterator.h"

#include "linfunc.h"

#include <algorithm>

namespace fs = std::filesystem;


namespace {
std::string lowercaseExt(const fs::directory_entry& entry) {
    return lowercase(entry.path().extension().string());
}

bool contains(const std::vector<std::string>& list, const std::string& item) {
    return std::find(list.cbegin(), list.cend(), item) != list.cend();
}
} // namespace


DirIterator::DirIterator(const fs::path& path)
{
    std::error_code err;  // It's OK if the directory doesn't exist
    m_iter = fs::directory_iterator(path, err);
}


std::optional<fs::path> DirIterator::next()
{
    std::optional<fs::path> result = std::nullopt;

    while (m_iter != fs::end(m_iter) && !result) {
        if (isAccepted(*m_iter)) {
            result = std::make_optional(m_iter->path());
        }
        m_iter++;
    }

    return result;
}


FilesIterator::FilesIterator(const fs::path& path, std::vector<std::string> extensions)
    : DirIterator(path)
    , m_extensions(std::move(extensions))
{}

bool FilesIterator::isAccepted(const fs::directory_entry& entry) const {
    return m_extensions.empty() || contains(m_extensions, lowercaseExt(entry));
}


SubdirsIterator::SubdirsIterator(const fs::path& path)
    : DirIterator(path)
{}

bool SubdirsIterator::isAccepted(const fs::directory_entry& entry) const {
    return entry.is_directory();
}
