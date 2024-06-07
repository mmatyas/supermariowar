#pragma once

#include <array>
#include <map>
#include <memory>
#include <string>
#include <vector>

#define MAXMUSICCATEGORY        11
#define MAXWORLDMUSICCATEGORY   9
#define MAXCATEGORYTRACKS       64


// Adds music overrides to the music lists
void UpdateMusicWithOverrides();


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

    std::string getPath(size_t index) const;
    std::string getName(size_t index) const;

private:
    std::vector<SkinListNode> m_skins;
};


class MusicOverride {
public:
    std::vector<size_t> songs;
};


class MusicEntry {
public:
    explicit MusicEntry(std::string name);

    static std::unique_ptr<MusicEntry> load(const std::string& musicDirectory);

    void updateWithOverrides();

    const std::string& name() const { return m_name; }
    const std::string& music(size_t musicID) const;
    const std::string& randomMusic(size_t categoryID, const std::string& mapName, const std::string& background);
    const std::string& nextMusic(size_t categoryID, const std::string& mapName, const std::string& background);

private:
    std::string m_name;
    size_t m_currentMusic = 0;

    size_t numsongsforcategory[MAXMUSICCATEGORY];
    size_t songsforcategory[MAXMUSICCATEGORY][MAXCATEGORYTRACKS];
    std::vector<std::string> songFileNames;

    std::map<std::string, MusicOverride*> mapoverride;
    std::map<std::string, MusicOverride*> backgroundoverride;
};


class MusicList
{
public:
    MusicList();

    const std::string& music(size_t musicID) const {
        return m_entries[m_currentIndex]->music(musicID);
    }
    const std::string& currentMusic() const {
        return m_currentMusic;
    }
    void setRandomMusic(size_t iCategoryID, const std::string& szMapName, const std::string& szBackground);
    void setNextMusic(size_t iCategoryID, const std::string& szMapName, const std::string& szBackground);

    size_t currentIndex() const {
        return m_currentIndex;
    }
    void setCurrent(size_t index) {
        m_currentIndex = index < m_entries.size() ? index : 0;
    }
    const std::string& currentName() const {
        return m_entries[m_currentIndex]->name();
    }

    void next();
    void prev();
    void random();

    void updateEntriesWithOverrides();

private:
    std::string m_currentMusic;
    std::vector<std::unique_ptr<MusicEntry>> m_entries;
    size_t m_currentIndex = 0;
};


class WorldMusicEntry {
public:
    explicit WorldMusicEntry(std::string name);

    static std::unique_ptr<WorldMusicEntry> load(const std::string& musicDirectory);

    const std::string& music(size_t musicID, const std::string& worldName) const;
    const std::string& name() const { return m_name; }

    void updateWithOverrides();

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

    void updateEntriesWithOverrides();

private:
    std::vector<std::unique_ptr<WorldMusicEntry>> m_entries;
    size_t m_currentIndex = 0;
};
