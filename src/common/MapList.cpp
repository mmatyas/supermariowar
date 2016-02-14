#include "MapList.h"

#include "dirlist.h"
#include "FileList.h"
#include "GameValues.h"
#include "linfunc.h"
#include "map.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <iostream>
#include <stdexcept>

#ifdef _XBOX
#include <xtl.h>
#endif

#ifdef _WIN32
#ifndef _XBOX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#endif

#if defined(__APPLE__)
#include <sys/stat.h>
#endif

using std::cout;
using std::endl;
using std::string;

extern int32_t g_iVersion[];

extern CMap* g_map;
extern FiltersList* filterslist;
extern CGameValues game_values;

MapListNode::MapListNode(std::string fullName)
{
    pfFilters = new bool[NUM_AUTO_FILTERS + filterslist->GetCount()];
    for (short iFilter = 0; iFilter < filterslist->GetCount() + NUM_AUTO_FILTERS; iFilter++)
        pfFilters[iFilter] = false;

    fInCurrentFilterSet = true;
    filename = fullName;
    iIndex = 0;

    fReadFromCache = false;

    iShortNameLength = strlen(stripCreatorAndDotMap(fullName).c_str());

    fValid = true;
}

MapListNode::~MapListNode()
{
    delete [] pfFilters;
}

///////////// MapList ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MapList::MapList(bool fWorldEditor)
{
    strcpy(szUnknownMapString, "-");

    DirectoryListing d(convertPath("maps/"), ".map");
    std::string curname;

    while (d(curname)) {
        MapListNode * node = new MapListNode(d.fullName(curname));
        maps[stripCreatorAndDotMap(curname)] = node;
    }

#ifdef _DEBUG
    DirectoryListing debugMapDir(convertPath("maps/test/"), ".map");
    while (debugMapDir(curname)) {
        MapListNode * node = new MapListNode(debugMapDir.fullName(curname));
        maps[stripCreatorAndDotMap(curname)] = node;
    }

    DirectoryListing specialDebugMapDir(convertPath("maps/special/"), ".map");
    while (specialDebugMapDir(curname)) {
        MapListNode * node = new MapListNode(specialDebugMapDir.fullName(curname));
        maps[stripCreatorAndDotMap(curname)] = node;
    }
#endif

    //If this is for the world editor, load all the world maps into the map viewer UI control
    if (fWorldEditor) {
        //Load in the "tour only" maps directory
        DirectoryListing tourMapDir(convertPath("maps/tour/"), ".map");

        while (tourMapDir(curname)) {
            MapListNode * node = new MapListNode(tourMapDir.fullName(curname));
            maps[stripCreatorAndDotMap(curname)] = node;
        }

        SimpleDirectoryList worldeditormapdirs(convertPath("worlds/"));

        short iEditorDirCount = worldeditormapdirs.GetCount();
        for (short iDir = 0; iDir < iEditorDirCount; iDir++) {
            const char * szName = worldeditormapdirs.current_name();

            DirectoryListing worldMapDir(convertPath(std::string(szName) + std::string("/")), ".map");

            while (worldMapDir(curname)) {
                MapListNode * node = new MapListNode(worldMapDir.fullName(curname));
                maps[stripCreatorAndDotMap(curname)] = node;
            }

            worldeditormapdirs.next();
        }

#ifndef _DEBUG
        DirectoryListing specialEditorMapDir(convertPath("maps/special/"), ".map");
        while (specialEditorMapDir(curname)) {
            MapListNode * node = new MapListNode(specialEditorMapDir.fullName(curname));
            maps[stripCreatorAndDotMap(curname)] = node;
        }
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
        (*current).second->iIndex = iIndex++;
        current++;
    }

    current = maps.begin();
    savedcurrent = current;
    outercurrent = current;

    iFilteredMapCount = maps.size();

    mlnFilteredMaps = new std::map<std::string, MapListNode*>::iterator[maps.size()];
    mlnMaps = new std::map<std::string, MapListNode*>::iterator[maps.size()];

    //Load in the "tour only" maps directory
    DirectoryListing tourMapDir(convertPath("maps/tour/"), ".map");

    while (tourMapDir(curname)) {
        MapListNode * node = new MapListNode(tourMapDir.fullName(curname));
        worldmaps[stripCreatorAndDotMap(curname)] = node;
    }

    //Read all world map directories and load them into the world/tour only list
    SimpleDirectoryList worldmapdirs(convertPath("worlds/"));

    short iDirCount = worldmapdirs.GetCount();
    for (short iDir = 0; iDir < iDirCount; iDir++) {
        const char * szName = worldmapdirs.current_name();

        DirectoryListing worldMapDir(convertPath(std::string(szName) + std::string("/")), ".map");

        while (worldMapDir(curname)) {
            MapListNode * node = new MapListNode(worldMapDir.fullName(curname));
            worldmaps[stripCreatorAndDotMap(curname)] = node;
        }

        worldmapdirs.next();
    }

    DirectoryListing specialMapDir(convertPath("maps/special/"), ".map");
    while (specialMapDir(curname)) {
        MapListNode * node = new MapListNode(specialMapDir.fullName(curname));
        worldmaps[stripCreatorAndDotMap(curname)] = node;
    }
}

MapList::~MapList()
{
    //Delete all map list nodes
    std::map<std::string, MapListNode*>::iterator iterateAll = maps.begin(), lim = maps.end();

    while (iterateAll != lim) {
        delete (iterateAll->second);
        iterateAll++;
    }

    maps.clear();

    //Delete all world map list nodes
    iterateAll = worldmaps.begin();
    lim = worldmaps.end();

    while (iterateAll != lim) {
        delete (iterateAll->second);
        iterateAll++;
    }

    worldmaps.clear();

    delete [] mlnFilteredMaps;
    delete [] mlnMaps;
}

//Called by level editor to load world maps into the map list
void MapList::addWorldMaps()
{
    SimpleDirectoryList worldmapdirs(convertPath("worlds/"));

    short iDirCount = worldmapdirs.GetCount();
    for (short iDir = 0; iDir < iDirCount; iDir++) {
        const char * szName = worldmapdirs.current_name();

        DirectoryListing worldMapDir(convertPath(std::string(szName) + std::string("/")), ".map");

        DirectoryListing specialDebugMapDir(convertPath("maps/special/"), ".map");

        std::string curname;
        while (worldMapDir(curname)) {
            MapListNode * node = new MapListNode(worldMapDir.fullName(curname));
            maps[stripCreatorAndDotMap(curname)] = node;
        }

        worldmapdirs.next();
    }
}

void MapList::add(const char * name)
{
    std::string fullName = convertPath("maps/") + name;

    for (std::map<std::string, MapListNode*>::iterator i = maps.begin(); i != maps.end(); ++i) {
        if ((*i).second->filename == fullName)
            return;
    }

    //not found - insert new map
    MapListNode * node = new MapListNode(fullName);
    maps[stripCreatorAndDotMap(name)] = node;
}

bool MapList::find(const char * name)
{
    bool fFound = false;

    std::map<std::string, MapListNode*>::iterator oldCurrent = current;
    do {
        next(false);	//sets us to the beginning if we hit the end -> loop through the maps

        if (strstr((*current).second->filename.c_str(), name))	//compare names after
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
        std::map<std::string, MapListNode*>::iterator iterateAll = worldmaps.begin(), lim = worldmaps.end();

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

    std::map<std::string, MapListNode*>::iterator oldCurrent = current;

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
    if ((*current).second->fInCurrentFilterSet)
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

    std::map<std::string, MapListNode*>::iterator oldCurrent = current;
    do {
        next(true);	//sets us to the beginning if we hit the end -> loop through the maps

        if (currentShortmapname()[0] == letter)
            return true;
    } while (current != oldCurrent);

    return false;
}

//Searches for maps that start with this entire string
bool MapList::startswith(std::string match)
{
    int iMatchLen = strlen(match.c_str());

    std::map<std::string, MapListNode*>::iterator oldCurrent = current;
    do {
        next(true);	//sets us to the beginning if we hit the end -> loop through the maps

        const char * szMapName = currentShortmapname();
        const int iMapNameLen = currentShortMapNameLen();

        if (iMatchLen > iMapNameLen)
            continue;

        for (short iIndex = 0; iIndex < iMatchLen && iIndex < iMapNameLen; iIndex++) {
            if (tolower(szMapName[iIndex]) == tolower(match[iIndex]))
                continue;

            goto TRYNEXTMAP;
        }

        //gets here if we matched
        return true;

        //Label that we break to if we don't match (it'd be nice if we had labeled continues in c++)
TRYNEXTMAP:
        continue;
    } while (current != oldCurrent);

    return false;
}

void MapList::prev(bool fUseFilters)
{
    if (fUseFilters) {
        std::map<std::string, MapListNode*>::iterator oldCurrent = current;

        do {
            prev(false);

            if ((*current).second->fInCurrentFilterSet)
                return;
        } while (current != oldCurrent);
    } else {
        if (current == maps.begin())	//we are at the first element
            current = --maps.end();	//continue from end
        else
            --current;

        outercurrent = current;
    }

    return;
}

void MapList::next(bool fUseFilters)
{
    if (fUseFilters) {
        std::map<std::string, MapListNode*>::iterator oldCurrent = current;

        do {
            next(false);

            if ((*current).second->fInCurrentFilterSet)
                return;
        } while (current != oldCurrent);
    } else {
        if (current == --maps.end())	//we are at the last valid element
            current = maps.begin();	//continue from start
        else
            ++current;

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

const char* MapList::randomFilename()
{
    std::map<std::string, MapListNode*>::iterator rnd = maps.begin();

    short iRand = RANDOM_INT(maps.size());

    for (short iMap = 0; iMap < iRand; iMap++)
        rnd++;

    return (*rnd).second->filename.c_str();
}


void MapList::WriteFilters()
{
    //Save user defined filters back to files
    if (game_values.fNeedWriteFilters) {
        game_values.fNeedWriteFilters = false;

        for (short iFilter = 0; iFilter < filterslist->GetCount(); iFilter++) {
            FILE * fp = fopen(filterslist->GetIndex(iFilter), "w");

            if (!fp)
                continue;

            fprintf(fp, "#Version\n");
            fprintf(fp, "%d.%d.%d.%d\n\n", g_iVersion[0], g_iVersion[1], g_iVersion[2], g_iVersion[3]);

            fprintf(fp, "#Icon\n");
            fprintf(fp, "%d\n\n", game_values.piFilterIcons[iFilter + NUM_AUTO_FILTERS]);

            fprintf(fp, "#Maps\n");

            std::map<std::string, MapListNode*>::iterator itr = maps.begin(), lim = maps.end();

            while (itr != lim) {
                if ((*itr).second->pfFilters[iFilter + NUM_AUTO_FILTERS])
                    fprintf(fp, "%s\n", (*itr).first.c_str());

                itr++;
            }

            fclose(fp);

#if defined(__APPLE__)
            chmod(filterslist->GetIndex(iFilter), S_IRWXU | S_IRWXG | S_IROTH);
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

            if (maps.find(pszMapName) != maps.end()) {
                bool fErrorReading = false;
                for (short iFilter = 0; iFilter < NUM_AUTO_FILTERS; iFilter++) {
                    char * psz = strtok(NULL, ",\n");

                    if (psz) {
                        maps[pszMapName]->pfFilters[iFilter] = strcmp(psz, "0") != 0;
                    } else {
                        fErrorReading = true;
                        break;
                    }
                }

                if (!fErrorReading)
                    maps[pszMapName]->fReadFromCache = true;
            }
        }

        fclose(mfp);
    }

    while (current != maps.end()) {
        if (!current->second->fReadFromCache) {
            MapListNode * mln = current->second;
            g_map->loadMap(mln->filename, read_type_summary);
            memcpy(mln->pfFilters, g_map->fAutoFilter, sizeof(bool) * NUM_AUTO_FILTERS);
        }

        current++;
    }

    current = maps.begin();
    //Get user defined filters from files in filters directory
    for (short iFilter = 0; iFilter < filterslist->GetCount(); iFilter++) {
        FILE * ffp = fopen(filterslist->GetIndex(iFilter), "r");

        if (!ffp)
            continue;

        short iVersion[4] = {0, 0, 0, 0};
        short iReadState = 0;
        while (fgets(buffer, 256, ffp)) {
            if (buffer[0] == '#' || buffer[0] == '\n' || buffer[0] == '\r' || buffer[0] == ' ' || buffer[0] == '\t')
                continue;

            if (0 == iReadState) {
                char * psz = strtok(buffer, ".\n");
                if (psz)
                    iVersion[0] = atoi(psz);

                psz = strtok(NULL, ".\n");
                if (psz)
                    iVersion[1] = atoi(psz);

                psz = strtok(NULL, ".\n");
                if (psz)
                    iVersion[2] = atoi(psz);

                psz = strtok(NULL, ".\n");
                if (psz)
                    iVersion[3] = atoi(psz);

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
                    (*current).second->pfFilters[iFilter + NUM_AUTO_FILTERS] = true;
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
    std::map<std::string, MapListNode*>::iterator itr = maps.begin(), lim = maps.end();

    while (itr != lim) {
        MapListNode * mln = itr->second;
        g_map->loadMap(mln->filename, read_type_summary);
        memcpy(mln->pfFilters, g_map->fAutoFilter, sizeof(bool) * NUM_AUTO_FILTERS);

        itr++;
    }
}

void MapList::WriteMapSummaryCache()
{
    FILE * fp = fopen(convertPath("maps/cache/mapsummary.txt").c_str(), "w");

    if (!fp)
        return;

    std::map<std::string, MapListNode*>::iterator itr = maps.begin(), lim = maps.end();

    while (itr != lim) {
        fprintf(fp, "%s", itr->first.c_str());

        for (short iFilter = 0; iFilter < NUM_AUTO_FILTERS; iFilter++)
            fprintf(fp, ",%d", itr->second->pfFilters[iFilter]);

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
void MapList::ApplyFilters(bool * pfFilters)
{
    std::map<std::string, MapListNode*>::iterator itr = maps.begin(), lim = maps.end();

    iFilteredMapCount = 0;
    short iTotalCount = 0;
    while (itr != lim) {
        bool fMatched = true;
        for (short iFilter = 0; iFilter < NUM_AUTO_FILTERS + filterslist->GetCount(); iFilter++) {
            if (pfFilters[iFilter]) {
                if (!(*itr).second->pfFilters[iFilter]) {
                    fMatched = false;
                    break;
                }
            }
        }

        (*itr).second->fInCurrentFilterSet = fMatched;

        if (fMatched) {
            (*itr).second->iFilteredIndex = iFilteredMapCount;
            mlnFilteredMaps[iFilteredMapCount] = itr;
            iFilteredMapCount++;
        }

        mlnMaps[iTotalCount++] = itr;

        itr++;
    }

    game_values.fFiltersOn = false;
    for (short iFilter = 0; iFilter < NUM_AUTO_FILTERS + filterslist->GetCount(); iFilter++) {
        if (pfFilters[iFilter]) {
            game_values.fFiltersOn = true;
            break;
        }
    }

    FindFilteredMap();
}

bool MapList::MapInFilteredSet()
{
    return (*current).second->fInCurrentFilterSet;
}

std::map<std::string, MapListNode*>::iterator MapList::GetIteratorAt(unsigned short iIndex, bool fUseFilters)
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
