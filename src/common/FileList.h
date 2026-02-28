#pragma once

#include <array>
#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#define MAXMUSICCATEGORY        11
#define MAXWORLDMUSICCATEGORY   9
#define MAXCATEGORYTRACKS       64


//it was kinda a bad idea to have skinlist and announcer list based on this, because both are accessed in different ways (skinlist like an vector and announcer list like a list). grrrr
class SimpleFileList {
public:
    SimpleFileList() = default;
    SimpleFileList(const std::string& dirpath, const std::string& extension, bool fAlphabetize = false);
    virtual ~SimpleFileList() = default;

    std::string at(size_t index) const;
    size_t count() const { return m_filelist.size(); }
    size_t currentIndex() const { return m_index; }
    std::string currentPath() const { return at(m_index); }

    void setCurrentIndex(size_t index);
    void setCurrentPath(const std::string& path);

    void next();
    void prev();
    void random();

    void add(std::string path);
    bool find(const std::string& name);

protected:
    std::vector<std::string> m_filelist;
    size_t m_index = -1;
};


class SimpleDirectoryList : public SimpleFileList {
public:
    SimpleDirectoryList(const std::string& dirpath);
    virtual ~SimpleDirectoryList() = default;
};


class AnnouncerList : public SimpleFileList {
public:
    AnnouncerList();
};

class GraphicsList : public SimpleDirectoryList {
public:
    GraphicsList();
};

class SoundsList : public SimpleDirectoryList {
public:
    SoundsList();
};

class TourList : public SimpleFileList {
public:
    TourList();
};

class WorldList : public SimpleFileList {
public:
    WorldList();
};

class BackgroundList : public SimpleFileList {
public:
    BackgroundList();
};

class FiltersList : public SimpleFileList {
public:
    FiltersList();
};


struct SkinListNode {
    std::string name;
    std::string path;
};

class SkinList {
public:
    SkinList();

    size_t count() const { return m_skins.size(); }
    const SkinListNode& at(size_t idx) const { return m_skins.at(idx); }

private:
    std::vector<SkinListNode> m_skins;
};


enum class MusicCategory : unsigned char {
    Land,
    Underground,
    Underwater,
    Castle,
    Platforms,
    Ghost,
    Bonus,
    Battle,
    Desert,
    Clouds,
    Snow,
    COUNT,
};
constexpr std::string_view to_string(MusicCategory category) noexcept {
    switch (category) {
        case MusicCategory::Land: return "Land";
        case MusicCategory::Underground: return "Underground";
        case MusicCategory::Underwater: return "Underwater";
        case MusicCategory::Castle: return "Castle";
        case MusicCategory::Platforms: return "Platforms";
        case MusicCategory::Ghost: return "Ghost";
        case MusicCategory::Bonus: return "Bonus";
        case MusicCategory::Battle: return "Battle";
        case MusicCategory::Desert: return "Desert";
        case MusicCategory::Clouds: return "Clouds";
        case MusicCategory::Snow: return "Snow";
        case MusicCategory::COUNT: return "COUNT";
    }
}


struct MapMusicOverride {
    std::string mapname;
    std::vector<std::filesystem::path> songs;
};


class MusicPack {
public:
    explicit MusicPack(std::string name);

    static std::optional<MusicPack> load(const std::filesystem::path& musicDirectory);

    void updateWithOverrides(const std::vector<MapMusicOverride>& overrides);

    const std::string& name() const { return m_name; }
    const std::filesystem::path& music(size_t musicID) const;
    const std::filesystem::path& randomMusic(MusicCategory category, const std::string& mapName, const std::string& background);
    const std::filesystem::path& nextMusic(MusicCategory category, const std::string& mapName, const std::string& background);

private:
    std::string m_name;
    size_t m_currentMusic = 0;

    std::vector<std::filesystem::path> m_all_songs;

    std::unordered_map<MusicCategory, std::vector<size_t>> m_category_songs;
    std::unordered_map<std::string, std::vector<size_t>> m_map_overrides;
    std::unordered_map<std::string, std::vector<size_t>> m_background_overrides;
};


class MusicList
{
public:
    MusicList();

    const std::filesystem::path& music(size_t musicID) const {
        return m_entries.at(m_currentIndex).music(musicID);
    }
    const std::filesystem::path& currentMusic() const {
        return m_currentMusic;
    }
    void setRandomMusic(MusicCategory category, const std::string& mapName, const std::string& background);
    void setNextMusic(MusicCategory category, const std::string& mapName, const std::string& background);

    size_t currentIndex() const {
        return m_currentIndex;
    }
    void setCurrent(size_t index) {
        m_currentIndex = index < m_entries.size() ? index : 0;
    }
    const std::string& currentName() const {
        return m_entries.at(m_currentIndex).name();
    }

    void next();
    void prev();
    void random();

    void updateEntriesWithOverrides(const std::vector<MapMusicOverride>& overrides);

private:
    std::filesystem::path m_currentMusic;
    std::vector<MusicPack> m_entries;
    size_t m_currentIndex = 0;
};


struct WorldMusicOverride {
    std::string worldname;
    std::filesystem::path song;
};


class WorldMusicEntry {
public:
    explicit WorldMusicEntry(std::string name);

    static std::unique_ptr<WorldMusicEntry> load(const std::string& musicDirectory);

    const std::string& music(size_t musicID, const std::string& worldName) const;
    const std::string& name() const { return m_name; }

    void updateWithOverrides(const std::vector<WorldMusicOverride>& overrides);

private:
    std::string m_name;
    std::map<std::string, std::string> m_worldOverrides;
    std::array<std::string, MAXWORLDMUSICCATEGORY + 2> m_songFileNames;
};


class WorldMusicList {
public:
    WorldMusicList();

    size_t currentIndex() const {
        return m_currentIndex;
    }
    void setCurrent(size_t index) {
        m_currentIndex = index < m_entries.size() ? index : 0;
    }
    const std::string& currentName() const {
        return m_entries[m_currentIndex]->name();
    }
    const std::string& currentMusic(int musicID, const std::string& worldName) const {
        return m_entries[m_currentIndex]->music(musicID, worldName);
    }
    size_t count() const {
        return m_entries.size();
    }

    void next();
    void prev();
    void random();

    void updateEntriesWithOverrides(const std::vector<WorldMusicOverride>& overrides);

private:
    std::vector<std::unique_ptr<WorldMusicEntry>> m_entries;
    size_t m_currentIndex = 0;
};

/// Adds music overrides to the music lists
void UpdateMusicWithOverrides(MusicList& musiclist, WorldMusicList& worldmusiclist);
