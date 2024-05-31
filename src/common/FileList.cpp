#include "FileList.h"

#include "dirlist.h"
#include "GlobalConstants.h"
#include "linfunc.h"

#include <algorithm>
#include <fstream>
#include <list>
#include <cstring>

extern const char * g_szMusicCategoryNames[MAXMUSICCATEGORY];

extern MusicList* musiclist;
extern WorldMusicList* worldmusiclist;

namespace {
const short g_iDefaultMusicCategory[MAXMUSICCATEGORY] {0, 0, 0, 0, 0, 1, 2, 3, 0, 0, 0};
}// namespace


struct MapMusicOverride {
    std::string mapname;
    std::vector<std::string> songs;
};

struct WorldMusicOverride {
    std::string worldname;
    std::string song;
};

std::vector<MapMusicOverride> mapmusicoverrides;
std::vector<WorldMusicOverride> worldmusicoverrides;

void UpdateMusicWithOverrides()
{
    enum class Category : unsigned char {
        None,
        Maps,
        Worlds,
    };

    std::ifstream file(convertPath("music/Overrides.txt"));
    if (!file)
        return;

    Category currentCategory = Category::None;
    std::string line;
    while (std::getline(file, line)) {
        //Ignore comment lines
        if (line.empty() || line[0] == '#' || line[0] == '\n' || line[0] == '\r' || line[0] == ' ' || line[0] == '\t')
            continue;

        //Chop off line ending
        line = line.substr(0, line.find('\r'));
        line = line.substr(0, line.find('\n'));

        //If we found a category header
        if (line[0] == '[') {
            std::transform(line.begin(), line.end(), line.begin(), ::tolower);

            if (line == "[maps]")
                currentCategory = Category::Maps;
            else if (line == "[worlds]")
                currentCategory = Category::Worlds;

            continue;
        }

        //If we're not in a category, ignore this line
        if (currentCategory == Category::None)
            continue;

        std::list<std::string> tokens = tokenize(line, ',');
        if (tokens.empty())
            continue;

        if (currentCategory == Category::Maps) {
            MapMusicOverride override;

            override.mapname = std::move(tokens.front());
            tokens.pop_front();

            for (const std::string& token : tokens) {
                std::string path = convertPath(token);
                if (FileExists(path))
                    override.songs.emplace_back(std::move(path));
            }
            //Don't add overrides that have no songs
            if (!override.songs.empty())
                mapmusicoverrides.emplace_back(std::move(override));
        }
        else if (currentCategory == Category::Worlds) {
            WorldMusicOverride override;

            override.worldname = std::move(tokens.front());
            tokens.pop_front();

            std::string path = convertPath(tokens.front());
            if (FileExists(path)) {
                override.song = std::move(path);
                worldmusicoverrides.emplace_back(std::move(override));
            }
        }
    }

    musiclist->updateEntriesWithOverrides();
    worldmusiclist->updateEntriesWithOverrides();
}

///////////// SimpleFileList ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SimpleFileList::SimpleFileList(const std::string& dirpath, const std::string& extension, bool fAlphabetize)
{
    DirectoryListing dir(dirpath, extension);
    std::string curname;
    while (dir(curname)) {
        m_filelist.emplace_back(dir.fullName(curname));
    }

    if (m_filelist.empty()) {
        printf("WARNING: The directory `%s` is empty\n", dirpath.c_str());
        m_index = -1;
        return;
    }

    m_index = 0;

    //Alphabetize the list, ignoring author
    if (fAlphabetize) {
        std::vector<std::string> names;
        names.reserve(m_filelist.size());

        //Get only the names of the files, no author information
        for (const std::string& filepath : m_filelist) {
            std::string name = stripPathAndExtension(filepath);
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            names.emplace_back(std::move(name));
        }

        //Now bubblesort them
        bool fDone = false;
        while (!fDone) {
            fDone = true;
            for (size_t i = 0; i < m_filelist.size() - 1; i++) {
                if (names[i].compare(names[i + 1]) > 0) {
                    fDone = false;
                    std::swap(names[i], names[i + 1]);
                    std::swap(m_filelist[i], m_filelist[i + 1]);
                }
            }
        }
    }
}

void SimpleFileList::setCurrentIndex(size_t index)
{
    if (index < m_filelist.size())
        m_index = index;
}

void SimpleFileList::setCurrentPath(const std::string& name)
{
    auto it = std::find(m_filelist.cbegin(), m_filelist.cend(), name);
    if (it != m_filelist.cend())
        m_index = it - m_filelist.cbegin();
}

void SimpleFileList::next()
{
    if (m_filelist.empty())
        return;

    m_index++;
    if (m_index >= m_filelist.size())
        m_index = 0;
}

void SimpleFileList::prev()
{
    if (m_filelist.empty())
        return;

    m_index--;
    if (m_index >= m_filelist.size())
        m_index = m_filelist.size() - 1;
}

void SimpleFileList::random()
{
    if (!m_filelist.empty())
        m_index = RANDOM_INT(m_filelist.size());
}

std::string SimpleFileList::at(size_t index) const
{
    return index < m_filelist.size()
        ? m_filelist[index]
        : std::string();
}

void SimpleFileList::add(std::string path)
{
    m_filelist.emplace_back(std::move(path));
}

bool SimpleFileList::find(const std::string& name)
{
    bool fFound = false;
    size_t oldCurrent = m_index;
    do {
        next(); //sets us to the beginning if we hit the end -> loop through the maps
        if (strstr(m_filelist[m_index].c_str(), name.c_str()))   //compare names after
            fFound = true;
    }
    while (m_index != oldCurrent && !fFound);

    return fFound;
}

///////////// SkinList ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkinList::SkinList()
{
    DirectoryListing dir(convertPath("gfx/skins/"));
    std::string curname;
    while (dir(curname)) {
        if (curname.length() < 5)
            continue;

        std::string file_ext = curname.substr(curname.length() - 4);
        inPlaceLowerCase(file_ext);
        if (file_ext != ".bmp" && file_ext != ".png") //Allow bmp and png skins
            continue;

        SkinListNode node {
            stripCreatorAndExt(curname),
            dir.fullName(curname),
        };

        auto it = m_skins.begin();
        for (; it != m_skins.end(); it++) {
            if (node.name.compare(it->name) < 0)
                break;
        }
        m_skins.insert(it, std::move(node));
    }
}

std::string SkinList::getPath(size_t index) const
{
    return index < m_skins.size()
        ? m_skins[index].path
        : std::string();
}

std::string SkinList::getName(size_t index) const
{
    return index < m_skins.size()
        ? m_skins[index].name
        : std::string();
}

///////////// SimpleDirectoryList ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SimpleDirectoryList::SimpleDirectoryList(const std::string &path)
{
    DirectoryListing d(path);
    std::string curname;
    while (d.NextDirectory(curname)) {
        m_filelist.insert(m_filelist.end(), d.fullName(curname));
    }
    if (m_filelist.empty()) {
        printf("ERROR: Empty directory.  %s\n", path.c_str());
        //exit(0);
    }

    m_index = 0;
}


///////////// MusicList ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MusicList::MusicList()
{
    DirectoryListing d(convertPath("music/game/"));
    std::string currentdir;
    while (d.NextDirectory(currentdir)) {
        auto m = MusicEntry::load(d.fullName(currentdir));
        if (m)
            m_entries.push_back(std::move(m));
    }

    if (m_entries.empty()) {
        throw "Empty Music directory!";
    }
}

void MusicList::setRandomMusic(size_t musicCategory, const std::string& mapName, const std::string& background)
{
    m_currentMusic = m_entries[m_currentIndex]->randomMusic(musicCategory, mapName, background);
}

void MusicList::setNextMusic(size_t musicCategory, const std::string& mapName, const std::string& background)
{
    m_currentMusic = m_entries[m_currentIndex]->nextMusic(musicCategory, mapName, background);
}

void MusicList::next() {
    m_currentIndex = (m_currentIndex + 1) % m_entries.size();
}

void MusicList::prev() {
    m_currentIndex = (m_currentIndex == 0 ? m_entries.size() : m_currentIndex) - 1;
}

void MusicList::random() {
    m_currentIndex = RANDOM_INT(m_entries.size());
}

void MusicList::updateEntriesWithOverrides()
{
    for (std::unique_ptr<MusicEntry>& entry : m_entries) {
        entry->updateWithOverrides();
    }
}


///////////// MusicEntry ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MusicEntry::MusicEntry(std::string name)
    : m_name(std::move(name))
{
    for (size_t k = 0; k < MAXMUSICCATEGORY; k++)
        numsongsforcategory[k] = 0;
}

std::unique_ptr<MusicEntry> MusicEntry::load(const std::string& musicdirectory)
{
    size_t separator_pos = musicdirectory.rfind(dirSeparator());
    std::string name = separator_pos != std::string::npos
        ? musicdirectory.substr(separator_pos + 1)
        : musicdirectory;

    name = name.substr(0, name.rfind("."));

    const std::string musicfile = musicdirectory + dirSeparator() + std::string("Music.txt");
    FILE* in = fopen(musicfile.c_str(), "r");
    if (!in) {
        printf("Error: Could not open: %s\n", musicfile.c_str());
        return {};
    }

    int iNumFile = 0;

    auto me = std::make_unique<MusicEntry>(std::move(name));

    //Run through the global overrides and add them to the map overrides of this music entry
    //if there are any overrides then for each override
    /*
    {
        if (mapoverride.find(pszName) == mapoverride.end())
            mapoverride[pszName] = new MusicOverride();

        songFileNames.push_back(sPath);

        fUsesMapOverrides = true;
        mapoverride[pszName]->songs.push_back(iNumFile);
        iNumFile++;
    }
    */

    int iAddToCategory = -1;
    char szBuffer[256];
    while (fgets(szBuffer, 256, in)) {
        //Ignore comment lines
        if (szBuffer[0] == '#' || szBuffer[0] == ' ' || szBuffer[0] == '\t' || szBuffer[0] == '\n' || szBuffer[0] == '\r')
            continue;

        //Chop off line ending
        size_t stringLength = strlen(szBuffer);
        for (size_t k = 0; k < stringLength; k++) {
            if (szBuffer[k] == '\r' || szBuffer[k] == '\n') {
                szBuffer[k] = '\0';
                break;
            }
        }

        //If we found a category header
        if (szBuffer[0] == '[') {
            if (cstr_ci_equals(szBuffer, "[land]"))
                iAddToCategory = 0;
            else if (cstr_ci_equals(szBuffer, "[underground]"))
                iAddToCategory = 1;
            else if (cstr_ci_equals(szBuffer, "[underwater]"))
                iAddToCategory = 2;
            else if (cstr_ci_equals(szBuffer, "[castle]"))
                iAddToCategory = 3;
            else if (cstr_ci_equals(szBuffer, "[platforms]"))
                iAddToCategory = 4;
            else if (cstr_ci_equals(szBuffer, "[ghost]"))
                iAddToCategory = 5;
            else if (cstr_ci_equals(szBuffer, "[bonus]"))
                iAddToCategory = 6;
            else if (cstr_ci_equals(szBuffer, "[battle]"))
                iAddToCategory = 7;
            else if (cstr_ci_equals(szBuffer, "[desert]"))
                iAddToCategory = 8;
            else if (cstr_ci_equals(szBuffer, "[clouds]"))
                iAddToCategory = 9;
            else if (cstr_ci_equals(szBuffer, "[snow]"))
                iAddToCategory = 10;
            else if (cstr_ci_equals(szBuffer, "[maps]"))
                iAddToCategory = MAXMUSICCATEGORY;
            else if (cstr_ci_equals(szBuffer, "[backgrounds]"))
                iAddToCategory = MAXMUSICCATEGORY + 1;

            continue;
        }

        //Cap the number of songs at MAXCATEGORYTRACKS for a category
        if (iAddToCategory > -1 && iAddToCategory < MAXMUSICCATEGORY && me->numsongsforcategory[iAddToCategory] >= MAXCATEGORYTRACKS)
            continue;

        if (iNumFile < 4 || iAddToCategory > -1) {
            if (iAddToCategory == MAXMUSICCATEGORY || iAddToCategory == MAXMUSICCATEGORY + 1) {
                char * pszName = strtok(szBuffer, ",\n");

                if (!pszName)
                    continue;

                if (iAddToCategory == MAXMUSICCATEGORY) {
                    if (me->mapoverride.find(pszName) == me->mapoverride.end())
                        me->mapoverride[pszName] = new MusicOverride();
                } else {
                    if (me->backgroundoverride.find(pszName) == me->backgroundoverride.end())
                        me->backgroundoverride[pszName] = new MusicOverride();
                }

                char * pszMusic = strtok(NULL, ",\n");
                while (pszMusic) {
                    std::string sPath = musicdirectory + dirSeparator() + pszMusic;

                    if (FileExists(sPath.c_str())) {
                        me->songFileNames.push_back(sPath);

                        if (iAddToCategory == MAXMUSICCATEGORY) {
                            me->mapoverride[pszName]->songs.push_back(iNumFile);
                        } else {
                            me->backgroundoverride[pszName]->songs.push_back(iNumFile);
                        }

                        iNumFile++;
                    }
                    pszMusic = strtok(NULL, ",\n");
                }
            } else {
                std::string sPath = musicdirectory + dirSeparator() + szBuffer;
                if (FileExists(sPath)) {
                    me->songFileNames.push_back(sPath);

                    //Don't add to category lists if this is one of the four special music tracks
                    if (iNumFile >= 4) {
                        me->songsforcategory[iAddToCategory][me->numsongsforcategory[iAddToCategory]] = iNumFile;
                        me->numsongsforcategory[iAddToCategory]++;
                    }
                    iNumFile++;
                }
            }
        }
    }

    fclose(in);

    //Now read labeled subdirectories like "Land", "Underground", "Castle", etc for more songs

    for (short iMusicCategory = 0; iMusicCategory < MAXMUSICCATEGORY; iMusicCategory++) {
        std::string musicPath = musicdirectory + dirSeparator() + std::string(g_szMusicCategoryNames[iMusicCategory]);
        if (FileExists(musicPath)) {
            SimpleFileList musList(musicPath + dirSeparator(), ".ogg");

            short iCount = musList.count();

            //printf("Found %d files in %s\n", iCount, musicPath.c_str());

            for (short iFile = 0; iFile < iCount; iFile++) {
                if (me->numsongsforcategory[iMusicCategory] < MAXCATEGORYTRACKS) {
                    me->songFileNames.push_back(musList.currentPath());
                    me->songsforcategory[iMusicCategory][me->numsongsforcategory[iMusicCategory]] = iNumFile;
                    me->numsongsforcategory[iMusicCategory]++;
                    iNumFile++;
                }

                musList.next();
            }
        }
    }

    if (iNumFile == 0) {
        printf("Error: No songs found in: %s\n", musicdirectory.c_str());
        return {};
    }

    //Verify we have at least one track for each category
    for (size_t i = 0; i < MAXMUSICCATEGORY; i++) {
        if (me->numsongsforcategory[i] == 0) {
            if (i < 4) {
                printf("Error: Missing track definition for music category: %s\n", g_szMusicCategoryNames[i]);
                return {};
            } else { //Use default category
                me->numsongsforcategory[i] = me->numsongsforcategory[g_iDefaultMusicCategory[i]];
                for (size_t k = 0; k < me->numsongsforcategory[i]; k++) {
                    me->songsforcategory[i][k] = me->songsforcategory[g_iDefaultMusicCategory[i]][k];
                }
            }
        }
    }

    /*
    std::vector<std::string>::iterator itr = songFileNames.begin();

    printf("------ Songlist ------\n");
    short iCounter = 0;
    while (itr != songFileNames.end())
    {
        printf("%d: %s\n", iCounter++, itr->c_str());
        itr++;
    }

    printf("\n\n");

    for (i = 0; i < MAXMUSICCATEGORY; i++)
    {
        printf("---- %d ----\n", i);
        printf("Count: %d\n", numsongsforcategory[i]);
        for (k = 0; k < numsongsforcategory[i]; k++)
            printf("%d\n", songsforcategory[i][k]);
    }
    */
}

const std::string& MusicEntry::music(size_t musicID) const
{
    return (musicID >= songFileNames.size())
        ? songFileNames[songFileNames.size() - 1]
        : songFileNames[musicID];
}

const std::string& MusicEntry::randomMusic(size_t musicCategory, const std::string& mapName, const std::string& background)
{
    //First check if there is specific map music
    const auto mapIter = mapoverride.find(mapName);
    if (mapIter != mapoverride.end() && !mapIter->second->songs.empty()) {
        m_currentMusic = RANDOM_INT(mapIter->second->songs.size());
        return songFileNames[mapIter->second->songs[m_currentMusic]];
    }

    //Then check if there is specific background music
    const auto bgIter = backgroundoverride.find(background);
    if (bgIter != backgroundoverride.end() && !bgIter->second->songs.empty()) {
        m_currentMusic = RANDOM_INT(bgIter->second->songs.size());
        return songFileNames[bgIter->second->songs[m_currentMusic]];
    }

    //Then default to the music category
    if (0 <= musicCategory && musicCategory < MAXMUSICCATEGORY && numsongsforcategory[musicCategory] > 0) {
        m_currentMusic = RANDOM_INT(numsongsforcategory[musicCategory]);
        return songFileNames[songsforcategory[musicCategory][m_currentMusic]];
    }

    return songFileNames[4];
}

const std::string& MusicEntry::nextMusic(size_t musicCategory, const std::string& mapName, const std::string& background)
{
    //First check if there is specific map music
    const auto mapIter = mapoverride.find(mapName);
    if (mapIter != mapoverride.end() && !mapIter->second->songs.empty()) {
        if (++m_currentMusic >= mapIter->second->songs.size())
            m_currentMusic = 0;

        return songFileNames[mapIter->second->songs[m_currentMusic]];
    }
    //Then check if there is specific background music
    const auto bgIter = backgroundoverride.find(background);
    if (bgIter != backgroundoverride.end() && !bgIter->second->songs.empty()) {
        if (++m_currentMusic >= bgIter->second->songs.size())
            m_currentMusic = 0;

        return songFileNames[bgIter->second->songs[m_currentMusic]];
    }

    //Then default to the music category
    if (0 <= musicCategory && musicCategory < MAXMUSICCATEGORY && numsongsforcategory[musicCategory] > 0) {
        if (++m_currentMusic >= numsongsforcategory[musicCategory])
            m_currentMusic = 0;

        return songFileNames[songsforcategory[musicCategory][m_currentMusic]];
    }

    return songFileNames[4];
}

void MusicEntry::updateWithOverrides()
{
    size_t songIndex = songFileNames.size();

    for (const MapMusicOverride& override : mapmusicoverrides) {
        if (mapoverride.find(override.mapname) == mapoverride.end())
            mapoverride[override.mapname] = new MusicOverride();

        for (const std::string& song : override.songs) {
            songFileNames.push_back(song);
            mapoverride[override.mapname]->songs.push_back(songIndex);
            songIndex++;
        }
    }
}


///////////// MusicList ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WorldMusicList::WorldMusicList()
{
    DirectoryListing d(convertPath("music/world/"));
    std::string currentdir;
    while (d.NextDirectory(currentdir)) {
        std::unique_ptr<WorldMusicEntry> m = std::make_unique<WorldMusicEntry>(d.fullName(currentdir));
        if (m)
            m_entries.push_back(std::move(m));
    }

    if (m_entries.empty()) {
        throw "Empty music directory!";
    }
}

void WorldMusicList::next() {
    m_currentIndex = (m_currentIndex + 1) % m_entries.size();
}

void WorldMusicList::prev() {
    m_currentIndex = (m_currentIndex == 0 ? m_entries.size() : m_currentIndex) - 1;
}

void WorldMusicList::random() {
    m_currentIndex = RANDOM_INT(m_entries.size());
}

void WorldMusicList::updateEntriesWithOverrides()
{
    for (std::unique_ptr<WorldMusicEntry>& entry : m_entries) {
        entry->updateWithOverrides();
    }
}


///////////// WorldMusicEntry ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WorldMusicEntry::WorldMusicEntry(std::string name)
    : m_name(std::move(name))
{}

std::unique_ptr<WorldMusicEntry> WorldMusicEntry::load(const std::string& musicdirectory)
{
    const size_t separator_pos = musicdirectory.rfind(dirSeparator());
    std::string name = separator_pos != std::string::npos
        ? musicdirectory.substr(separator_pos + 1)
        : musicdirectory;

    name = name.substr(0, name.rfind("."));

    const std::string musicfile = musicdirectory + dirSeparator() + std::string("Music.txt");
    FILE* in = fopen(musicfile.c_str(), "r");
    if (!in) {
        printf("Error: Could not open: %s\n", musicfile.c_str());
        return {};
    }

    auto wme = std::make_unique<WorldMusicEntry>(std::move(name));

    int iAddToCategory = -1;
    char szBuffer[256];
    while (fgets(szBuffer, 256, in)) {
        //Ignore comment lines
        if (szBuffer[0] == '#' || szBuffer[0] == ' ' || szBuffer[0] == '\t' || szBuffer[0] == '\n' || szBuffer[0] == '\r')
            continue;

        //Chop off line ending
        int stringLength = strlen(szBuffer);
        for (int k = 0; k < stringLength; k++) {
            if (szBuffer[k] == '\r' || szBuffer[k] == '\n') {
                szBuffer[k] = '\0';
                break;
            }
        }

        //If we found a category header
        if (szBuffer[0] == '[') {
            if (cstr_ci_equals(szBuffer, "[grass]"))
                iAddToCategory = 0;
            else if (cstr_ci_equals(szBuffer, "[desert]"))
                iAddToCategory = 1;
            else if (cstr_ci_equals(szBuffer, "[water]"))
                iAddToCategory = 2;
            else if (cstr_ci_equals(szBuffer, "[giant]"))
                iAddToCategory = 3;
            else if (cstr_ci_equals(szBuffer, "[sky]"))
                iAddToCategory = 4;
            else if (cstr_ci_equals(szBuffer, "[ice]"))
                iAddToCategory = 5;
            else if (cstr_ci_equals(szBuffer, "[pipe]"))
                iAddToCategory = 6;
            else if (cstr_ci_equals(szBuffer, "[dark]"))
                iAddToCategory = 7;
            else if (cstr_ci_equals(szBuffer, "[space]"))
                iAddToCategory = 8;
            else if (cstr_ci_equals(szBuffer, "[bonus]"))
                iAddToCategory = WORLDMUSICBONUS;
            else if (cstr_ci_equals(szBuffer, "[sleep]"))
                iAddToCategory = WORLDMUSICSLEEP;
            else if (cstr_ci_equals(szBuffer, "[worlds]"))
                iAddToCategory = WORLDMUSICWORLDS;

            continue;
        }

        if (iAddToCategory > -1 && iAddToCategory <= WORLDMUSICSLEEP) {
            std::string sPath = musicdirectory + dirSeparator() + szBuffer;
            if (FileExists(sPath))
                wme->m_songFileNames[iAddToCategory] = sPath;
        } else if (iAddToCategory == WORLDMUSICWORLDS) {
            char* pszName = strtok(szBuffer, ",\n");
            if (!pszName)
                continue;

            char* pszMusic = strtok(NULL, ",\n");
            if (!pszMusic)
                continue;

            std::string sPath = musicdirectory + dirSeparator() + pszMusic;
            if (!FileExists(sPath))
                continue;

            wme->m_worldOverrides[pszName] = sPath;
        }
    }

    fclose(in);
    return wme;
}

const std::string& WorldMusicEntry::music(size_t musicID, const std::string& worldName) const
{
    //First check if there is specific map music
    const auto iter = m_worldOverrides.find(worldName);
    if (iter != m_worldOverrides.cend()) {
        return iter->second;
    }

    return (musicID <= WORLDMUSICSLEEP)
        ? m_songFileNames[musicID]
        : m_songFileNames[0];
}

void WorldMusicEntry::updateWithOverrides()
{
    for (const WorldMusicOverride& override : worldmusicoverrides) {
        m_worldOverrides[override.worldname] = override.song;
    }
}
