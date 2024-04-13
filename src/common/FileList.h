#ifndef FILELIST_H
#define FILELIST_H

#include "path.h"
#include "RandomNumberGenerator.h"

#include <map>
#include <string>
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
    AnnouncerList(): SimpleFileList(convertPath("sfx/announcer/"), ".txt") {}
};

class GraphicsList : public SimpleDirectoryList {
public:
    GraphicsList(): SimpleDirectoryList(convertPath("gfx/packs/")) {}
};

class SoundsList : public SimpleDirectoryList {
public:
    SoundsList(): SimpleDirectoryList(convertPath("sfx/packs/")) {}
};

class TourList : public SimpleFileList {
public:
    TourList() : SimpleFileList(convertPath("tours/"), ".txt") {}
};

class WorldList : public SimpleFileList {
public:
    WorldList() : SimpleFileList(convertPath("worlds/"), ".txt", true) {}
};

class BackgroundList : public SimpleFileList {
public:
    BackgroundList() : SimpleFileList(convertPath("gfx/packs/Classic/backgrounds/"), ".png") {}
};

class FiltersList : public SimpleFileList {
public:
    FiltersList() : SimpleFileList(convertPath("filters/"), ".txt") {}
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


class MusicOverride
{
public:
    std::vector<int> songs;
};

class MusicEntry
{
public:
    MusicEntry(const std::string & musicdirectory);
    ~MusicEntry() {}

    void UpdateWithOverrides();

    std::string GetMusic(unsigned int musicID);
    std::string GetRandomMusic(int iCategoryID, const char * szMapName, const char * szBackground);
    std::string GetNextMusic(int iCategoryID, const char * szMapName, const char * szBackground);

    int numsongsforcategory[MAXMUSICCATEGORY];
    int songsforcategory[MAXMUSICCATEGORY][MAXCATEGORYTRACKS];
    std::vector<std::string> songFileNames;

    std::map<std::string, MusicOverride*> mapoverride;
    std::map<std::string, MusicOverride*> backgroundoverride;

    std::string name;
    unsigned short iCurrentMusic;

    bool fError;

    bool fUsesMapOverrides;
    bool fUsesBackgroundOverrides;
};

class MusicList
{
public:
    MusicList();
    ~MusicList();

    std::string GetMusic(int musicID);
    void SetRandomMusic(int iCategoryID, const char * szMapName, const char * szBackground);
    void SetNextMusic(int iCategoryID, const char * szMapName, const char * szBackground);
    std::string GetCurrentMusic();

    int GetCurrentIndex() {
        return currentIndex;
    };
    void SetCurrent(unsigned int index) {
        if (index < entries.size())
            currentIndex = index;
        else
            currentIndex = 0;
    };

    const char * current_name() {
        return entries[currentIndex]->name.c_str();
    };
    void next();
    void prev();
    void random() {
        currentIndex = RANDOM_INT(entries.size());
    };

    void UpdateEntriesWithOverrides();

private:
    std::string CurrentMusic;
    std::vector<MusicEntry*> entries;
    int currentIndex;
};

class WorldMusicEntry
{
public:
    WorldMusicEntry(const std::string & musicdirectory);
    ~WorldMusicEntry() {}

    void UpdateWithOverrides();

    std::string GetMusic(unsigned int musicID, const char * szWorldName);

    std::string songFileNames[MAXWORLDMUSICCATEGORY + 2];
    std::string name;

    std::map<std::string, std::string> worldoverride;

    bool fError;

    bool fUsesWorldOverrides;
};

class WorldMusicList
{
public:
    WorldMusicList();
    ~WorldMusicList();

    std::string GetMusic(int musicID, const char * szWorldName);
    std::string GetCurrentMusic();

    int GetCurrentIndex() {
        return currentIndex;
    }
    void SetCurrent(unsigned int index) {
        if (index < entries.size())
            currentIndex = index;
        else
            currentIndex = 0;
    };

    const char * current_name() {
        return entries[currentIndex]->name.c_str();
    }
    void next();
    void prev();
    void random() {
        currentIndex = RANDOM_INT(entries.size());
    }

    int GetCount() {
        return entries.size();
    }

    void UpdateEntriesWithOverrides();

private:
    std::string CurrentMusic;
    std::vector<WorldMusicEntry*> entries;
    int currentIndex;
};

#endif // FILELIST_H
