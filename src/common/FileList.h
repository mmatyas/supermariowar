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
class SimpleFileList
{
public:
    SimpleFileList();
    SimpleFileList(const std::string &path, const std::string &extension, bool fAlphabetize = false);
    virtual ~SimpleFileList();
    const char * GetIndex(unsigned int index);
    int GetCount() {
        return filelist.size();
    }

    int GetCurrentIndex() {
        return currentIndex;
    };
    void SetCurrent(unsigned int index);

    const char * current_name();
    void SetCurrentName(const std::string &name);

    void next();
    void prev();
    void random();

    void add(const char * name);
    bool find(const char * name);

protected:
    std::vector<std::string> filelist;
    int currentIndex;
};

class SimpleDirectoryList : public SimpleFileList
{
public:
    SimpleDirectoryList(const std::string &path);
    virtual ~SimpleDirectoryList() {
        ;
    };
};

class SkinListNode
{
public:
    SkinListNode(std::string skinName, std::string skinPath);
    ~SkinListNode() {}

    std::string sSkinName;
    std::string sSkinPath;
};

class SkinList
{
public:
    SkinList();
    int GetCount() {
        return skins.size();
    }
    const char * GetIndex(unsigned int index);
    const char * GetSkinName(unsigned int index);

private:
    std::vector<SkinListNode*> skins;
};

class AnnouncerList : public SimpleFileList
{
public:
    AnnouncerList() : SimpleFileList(convertPath("sfx/announcer/"), ".txt") {
        ;
    };
};

class GraphicsList : public SimpleDirectoryList
{
public:
    GraphicsList() : SimpleDirectoryList(convertPath("gfx/packs/")) {
        ;
    };
};

class SoundsList : public SimpleDirectoryList
{
public:
    SoundsList() : SimpleDirectoryList(convertPath("sfx/packs/")) {
        ;
    };
};

class TourList : public SimpleFileList
{
public:
    TourList() : SimpleFileList(convertPath("tours/"), ".txt") {
        ;
    };
};

class WorldList : public SimpleFileList
{
public:
    WorldList() : SimpleFileList(convertPath("worlds/"), ".txt", true) {
        ;
    };
};

class BackgroundList : public SimpleFileList
{
public:
    BackgroundList() : SimpleFileList(convertPath("gfx/packs/Classic/backgrounds/"), ".png") {
        ;
    };
};

class FiltersList : public SimpleFileList
{
public:
    FiltersList() : SimpleFileList(convertPath("filters/"), ".txt") {
        ;
    };
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
