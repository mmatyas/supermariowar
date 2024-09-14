#include "MapList.h"

#include "dirlist.h"
#include "FileList.h"
#include "GameValues.h"
#include "linfunc.h"
#include "map.h"
#include "path.h"
#include "RandomNumberGenerator.h"
#include "Version.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

extern CMap* g_map;
extern FiltersList* filterslist;
extern CGameValues game_values;


namespace {
void addMapsFrom(const std::string& relDir, std::multimap<std::string, MapListNode>& container)
{
    DirectoryListing d(convertPath(relDir), ".map");
    std::string curname;
    while (d(curname)) {
        MapListNode node(d.fullName(curname));
        container.emplace(stripCreatorAndExt(curname), std::move(node));
    }
}
} // namespace


MapListNode::MapListNode(std::string fullName)
{
    pfFilters.resize(NUM_AUTO_FILTERS + filterslist->count(), false);
    filename = std::move(fullName);
}

///////////// MapList ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MapList::MapList(bool fWorldEditor)
{
    strcpy(szUnknownMapString, "-");

    addMapsFrom("maps/", maps);

#ifdef _DEBUG
    addMapsFrom("maps/test/", maps);
    addMapsFrom("maps/special/", maps);
#endif

    //If this is for the world editor, load all the world maps into the map viewer UI control
    if (fWorldEditor) {
        //Load in the "tour only" maps directory
        addMapsFrom("maps/tour/", maps);

        SimpleDirectoryList worldeditormapdirs(convertPath("worlds/"));
        for (size_t iDir = 0; iDir < worldeditormapdirs.count(); iDir++) {
            std::string szName = worldeditormapdirs.currentPath() + '/';
            addMapsFrom(szName, maps);
            worldeditormapdirs.next();
        }

#ifndef _DEBUG
        addMapsFrom("maps/special/", maps);
#endif
    }

    //TODO: add proper test via size
    if (maps.empty()) {
        printf("ERROR: Empty map directory!\n");
        throw std::runtime_error("ERROR: Empty map directory!");
    }

    current = maps.begin();

    short iIndex = 0;
    while (current != maps.end()) {
        (*current).second.iIndex = iIndex++;
        current++;
    }

    current = maps.begin();
    savedcurrent = current;
    outercurrent = current;

    iFilteredMapCount = maps.size();

    mlnFilteredMaps.resize(maps.size());
    mlnMaps.resize(maps.size());

    //Load in the "tour only" maps directory
    addMapsFrom("maps/tour/", worldmaps);

    //Read all world map directories and load them into the world/tour only list
    SimpleDirectoryList worldmapdirs(convertPath("worlds/"));
    for (size_t iDir = 0; iDir < worldmapdirs.count(); iDir++) {
        std::string szName = worldmapdirs.currentPath() + '/';
        addMapsFrom(szName, worldmaps);
        worldmapdirs.next();
    }

    addMapsFrom("maps/special/", worldmaps);
}

//Called by level editor to load world maps into the map list
void MapList::addWorldMaps()
{
    SimpleDirectoryList worldmapdirs(convertPath("worlds/"));
    for (size_t iDir = 0; iDir < worldmapdirs.count(); iDir++) {
        std::string szName = worldmapdirs.currentPath() + '/';
        addMapsFrom(szName, maps);
        worldmapdirs.next();
    }
}

void MapList::add(const char * name)
{
    std::string fullName = convertPath("maps/") + name;
    MapListNode node(std::move(fullName));
    maps.emplace(stripCreatorAndExt(name), std::move(node));
}

bool MapList::find(const char * name)
{
    bool fFound = false;

    auto oldCurrent = current;
    do {
        next(false);	//sets us to the beginning if we hit the end -> loop through the maps

        if (strstr((*current).second.filename.c_str(), name))	//compare names after
            fFound = true;
    } while (current != oldCurrent && !fFound);

    return fFound;
}

bool MapList::findexact(const char * name, bool fWorld)
{
    char * szLookForName = new char[strlen(name) + 1];
    strcpy(szLookForName, name);
    inPlaceLowerCase(szLookForName);

    bool fFound = false;

    //If we're looking for a world, then search the world maps first
    //if the world map isn't found, then search the regular map list
    if (fWorld) {
        auto iterateAll = worldmaps.begin(), lim = worldmaps.end();

        while (iterateAll != lim && !fFound) {
            char * szCurrentName = new char[iterateAll->first.length() + 1];
            strcpy(szCurrentName, iterateAll->first.c_str());
            inPlaceLowerCase(szCurrentName);

            if (!strcmp(szCurrentName, szLookForName)) {
                fFound = true;
                outercurrent = iterateAll;
            }

            delete[] szCurrentName;

            iterateAll++;
        }

        if (fFound) {
            delete[] szLookForName;
            return true;
        }
    }

    auto oldCurrent = current;

    fFound = false;
    do {
        next(false);	//sets us to the beginning if we hit the end -> loop through the maps

        char * szCurrentName = new char[current->first.length() + 1];
        strcpy(szCurrentName, current->first.c_str());
        inPlaceLowerCase(szCurrentName);

        if (!strcmp(szCurrentName, szLookForName))
            fFound = true;

        delete[] szCurrentName;
    } while (current != oldCurrent && !fFound);

    delete[] szLookForName;

    return fFound;
}

//Returns true if the map needs to be reloaded
bool MapList::FindFilteredMap()
{
    if ((*current).second.fInCurrentFilterSet)
        return false;

    next(true);
    return true;
}

//Searches for a map that starts with this single character
bool MapList::startswith(char letter)
{
    //Captialize the letter becuase all maps have first letter in caps
    if (letter >= 'a' && letter <= 'z')
        letter -= 32;

    auto oldCurrent = current;
    do {
        next(true);	//sets us to the beginning if we hit the end -> loop through the maps

        if (currentShortmapname()[0] == letter)
            return true;
    } while (current != oldCurrent);

    return false;
}

//Searches for maps that start with this entire string
bool MapList::startswith(const std::string& match)
{
    auto oldCurrent = current;
    do {
        next(true);	//sets us to the beginning if we hit the end -> loop through the maps

        const std::string& szMapName = currentShortmapname();
        if (match.length() > szMapName.length())
            continue;

        bool matches = true;
        for (size_t iIndex = 0; iIndex < match.length(); iIndex++) {
            if (tolower(szMapName[iIndex]) != tolower(match[iIndex])) {
                matches = false;
                break;
            }
        }

        if (matches)
            return true;
    } while (current != oldCurrent);

    return false;
}

void MapList::prev(bool fUseFilters)
{
    if (fUseFilters) {
        auto oldCurrent = current;

        do {
            prev(false);

            if ((*current).second.fInCurrentFilterSet)
                return;
        } while (current != oldCurrent);
    } else {
        if (current == maps.begin())	//we are at the first element
            current = maps.end();	//continue from end

        --current;
        outercurrent = current;
    }

    return;
}

void MapList::next(bool fUseFilters)
{
    if (fUseFilters) {
        auto oldCurrent = current;

        do {
            next(false);

            if ((*current).second.fInCurrentFilterSet)
                return;
        } while (current != oldCurrent);
    } else {
        ++current;

        if (current == maps.end())	//we are at the last valid element
            current = maps.begin();	//continue from start

        outercurrent = current;
    }

    return;
}

void MapList::random(bool fUseFilters)
{
    int iShuffle = 0;
    if (fUseFilters) {
        if (iFilteredMapCount < 2)
            return;

        iShuffle = RANDOM_INT(iFilteredMapCount - 1);
    } else {
        iShuffle = RANDOM_INT(maps.size() - 1);
    }

    for (int i = 0; i <= iShuffle; i++)
        next(fUseFilters);
}

std::string MapList::randomFilename() const
{
    auto rnd = maps.cbegin();

    short iRand = RANDOM_INT(maps.size());

    for (short iMap = 0; iMap < iRand; iMap++)
        rnd++;

    return (*rnd).second.filename;
}


void MapList::WriteFilters()
{
    //Save user defined filters back to files
    if (game_values.fNeedWriteFilters) {
        game_values.fNeedWriteFilters = false;

        for (size_t iFilter = 0; iFilter < filterslist->count(); iFilter++) {
            FILE * fp = fopen(filterslist->at(iFilter).c_str(), "w");

            if (!fp)
                continue;

            fprintf(fp, "#Version\n");
            fprintf(fp, "%d.%d.%d.%d\n\n", GAME_VERSION.major, GAME_VERSION.minor, GAME_VERSION.patch, GAME_VERSION.build);

            fprintf(fp, "#Icon\n");
            fprintf(fp, "%d\n\n", game_values.piFilterIcons[iFilter + NUM_AUTO_FILTERS]);

            fprintf(fp, "#Maps\n");

            auto itr = maps.begin(), lim = maps.end();

            while (itr != lim) {
                if ((*itr).second.pfFilters[iFilter + NUM_AUTO_FILTERS])
                    fprintf(fp, "%s\n", (*itr).first.c_str());

                itr++;
            }

            fclose(fp);

#if defined(__APPLE__)
            chmod(filterslist->at(iFilter).c_str(), S_IRWXU | S_IRWXG | S_IROTH);
#endif
        }
    }
}

void MapList::ReadFilters()
{
    char buffer[256];

    //Get auto filters from maps
    current = maps.begin();

    //Used cached summary before trying to read the actual map file (to speed up load time)
    FILE * mfp = fopen(convertPath("maps/cache/mapsummary.txt").c_str(), "r");

    if (mfp) {
        while (fgets(buffer, 256, mfp)) {
            char * pszMapName = strtok(buffer, ",\n");
            MapListNode& node = maps.find(pszMapName)->second;

            if (maps.find(pszMapName) != maps.end()) {
                bool fErrorReading = false;
                for (size_t iFilter = 0; iFilter < NUM_AUTO_FILTERS; iFilter++) {
                    char * psz = strtok(NULL, ",\n");

                    if (psz) {
                        node.pfFilters[iFilter] = strcmp(psz, "0") != 0;
                    } else {
                        fErrorReading = true;
                        break;
                    }
                }

                if (!fErrorReading)
                    node.fReadFromCache = true;
            }
        }

        fclose(mfp);
    }

    while (current != maps.end()) {
        if (!current->second.fReadFromCache) {
            MapListNode& mln = current->second;
            g_map->loadMap(mln.filename, read_type_summary);
            std::copy(g_map->fAutoFilter.cbegin(), g_map->fAutoFilter.cend(), mln.pfFilters.begin());
        }

        current++;
    }

    current = maps.begin();
    //Get user defined filters from files in filters directory
    for (size_t iFilter = 0; iFilter < filterslist->count(); iFilter++) {
        FILE * ffp = fopen(filterslist->at(iFilter).c_str(), "r");

        if (!ffp)
            continue;

        short iReadState = 0;
        while (fgets(buffer, 256, ffp)) {
            if (buffer[0] == '#' || buffer[0] == '\n' || buffer[0] == '\r' || buffer[0] == ' ' || buffer[0] == '\t')
                continue;

            if (0 == iReadState) {
                // Version number
                iReadState = 1;
                continue;
            } else if (1 == iReadState) {
                game_values.piFilterIcons[iFilter + NUM_AUTO_FILTERS] = atoi(buffer);
                iReadState = 2;
                continue;
            } else {
                char * pszMap = strtok(buffer, "\r\n");

                //If that map is found
                if (findexact(pszMap, false))
                    (*current).second.pfFilters[iFilter + NUM_AUTO_FILTERS] = true;
            }
        }

        fclose(ffp);
    }

    //Reset the current back to the beginning after setting up the filters for each map
    current = maps.begin();
    outercurrent = current;
}

//Forces all the maps to reload the auto filters from the live map files (flush the cache)
void MapList::ReloadMapAutoFilters()
{
    auto itr = maps.begin(), lim = maps.end();

    while (itr != lim) {
        MapListNode& mln = itr->second;
        g_map->loadMap(mln.filename, read_type_summary);
        std::copy(g_map->fAutoFilter.cbegin(), g_map->fAutoFilter.cend(), mln.pfFilters.begin());

        itr++;
    }
}

void MapList::WriteMapSummaryCache()
{
    FILE * fp = fopen(convertPath("maps/cache/mapsummary.txt").c_str(), "w");

    if (!fp)
        return;

    auto itr = maps.begin(), lim = maps.end();

    while (itr != lim) {
        fprintf(fp, "%s", itr->first.c_str());

        for (size_t iFilter = 0; iFilter < NUM_AUTO_FILTERS; iFilter++)
            fprintf(fp, ",%d", itr->second.pfFilters[iFilter] ? 1 : 0);

        fprintf(fp, "\n");
        itr++;
    }

    fclose(fp);

#if defined(__APPLE__)
    chmod(convertPath("maps/cache/mapsummary.txt").c_str(), S_IRWXU | S_IRWXG | S_IROTH);
#endif
}

//Applies the currently selected filters to the entire map set
//After this call, when flipping through maps, only the matched maps
//will show up in the map field or in the thumbnail browser
void MapList::ApplyFilters(const std::vector<bool>& pfFilters)
{
    auto itr = maps.begin(), lim = maps.end();

    iFilteredMapCount = 0;
    short iTotalCount = 0;
    while (itr != lim) {
        bool fMatched = true;
        for (size_t iFilter = 0; iFilter < pfFilters.size(); iFilter++) {
            if (pfFilters[iFilter]) {
                if (!(*itr).second.pfFilters[iFilter]) {
                    fMatched = false;
                    break;
                }
            }
        }

        (*itr).second.fInCurrentFilterSet = fMatched;

        if (fMatched) {
            (*itr).second.iFilteredIndex = iFilteredMapCount;
            mlnFilteredMaps[iFilteredMapCount] = itr;
            iFilteredMapCount++;
        }

        mlnMaps[iTotalCount++] = itr;

        itr++;
    }

    game_values.fFiltersOn = std::find(pfFilters.cbegin(), pfFilters.cend(), true) != pfFilters.cend();
    FindFilteredMap();
}

bool MapList::MapInFilteredSet()
{
    return (*current).second.fInCurrentFilterSet;
}

std::multimap<std::string, MapListNode>::iterator MapList::GetIteratorAt(unsigned short iIndex, bool fUseFilters)
{
    if (fUseFilters) {
        if (iIndex >= iFilteredMapCount)
            return maps.end();

        return mlnFilteredMaps[iIndex];
    } else {
        if (iIndex >= maps.size())
            return maps.end();

        return mlnMaps[iIndex];
    }
}

const char * MapList::GetUnknownMapName()
{
    return szUnknownMapString;
}
