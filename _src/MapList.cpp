
#ifdef _XBOX
	#include <xtl.h>
#endif

#ifdef _WIN32
	#ifndef _XBOX
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
	#endif
#else
    #include <dirent.h>
#endif

#include "global.h"
#include "dirlist.h"
#include <ctype.h>
#include <iostream>
using std::cout;
using std::endl;
using std::string;

extern short g_iVersion[];

char * lowercase(char * name)
{
	for(unsigned int k = 0; k < strlen(name); k++)
	{
		name[k] = (char)tolower(name[k]);
	}

	return name;
}

MapListNode::MapListNode(std::string fullName)
{
	pfFilters = new bool[NUM_AUTO_FILTERS + filterslist.GetCount()];
	for(short iFilter = 0; iFilter < filterslist.GetCount() + NUM_AUTO_FILTERS; iFilter++)
		pfFilters[iFilter] = false;
	
	fInCurrentFilterSet = true;
	filename = fullName;
	iIndex = 0;
	
	fReadFromCache = false;

	fValid = true;
}

MapListNode::~MapListNode()
{
	delete [] pfFilters;
}

extern char * g_szMusicCategoryNames[MAXMUSICCATEGORY];
extern short g_iDefaultMusicCategory[MAXMUSICCATEGORY];

///////////// MapList ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MapList::MapList()
{
	DirectoryListing d(convertPath("maps/"), ".map");
	std::string curname;

	while(d(curname))
	{
		MapListNode * node = new MapListNode(d.fullName(curname));
		maps[stripCreatorAndDotMap(curname)] = node;
	}

#ifdef _DEBUG
	DirectoryListing debugMapDir(convertPath("maps/test/"), ".map");
	while(debugMapDir(curname))
	{
		MapListNode * node = new MapListNode(debugMapDir.fullName(curname));
		maps[stripCreatorAndDotMap(curname)] = node;
	}

	DirectoryListing specialMapDir(convertPath("maps/special/"), ".map");
	while(specialMapDir(curname))
	{
		MapListNode * node = new MapListNode(specialMapDir.fullName(curname));
		maps[stripCreatorAndDotMap(curname)] = node;
	}
#endif

	//TODO: add proper test via size
	if(maps.empty())
	{	
		printf("ERROR: Empty map directory!\n");
		exit(0);
	}

	current = maps.begin();

	short iIndex = 0;
	while(current != maps.end())
	{
		(*current).second->iIndex = iIndex++;
		current++;
	}

	current = maps.begin();
	savedcurrent = current;

	iFilteredMapCount = maps.size();

	mlnFilteredMaps = new std::map<std::string, MapListNode*>::iterator[maps.size()];
	mlnMaps = new std::map<std::string, MapListNode*>::iterator[maps.size()];
}

MapList::~MapList()
{
	std::map<std::string, MapListNode*>::iterator iterateAll = maps.begin(), lim = maps.end();
	
	while (iterateAll != lim)
	{
		delete (iterateAll->second);
		iterateAll++;
	}

	maps.clear();

	delete [] mlnFilteredMaps;
	delete [] mlnMaps;
}

void MapList::add(const char * name)
{
	std::string fullName = convertPath("maps/") + name;

	for(std::map<std::string, MapListNode*>::iterator i = maps.begin(); i != maps.end(); ++i)
	{
		if((*i).second->filename == fullName)
				return;
	}
	//not found - insert new map
	MapListNode * node = new MapListNode(fullName);
	maps[stripCreatorAndDotMap(name)] = node;
}

bool MapList::find(const char * name)
{
	char * szLookForName = strlwr(strdup(name));
	bool fFound = false;

	std::map<std::string, MapListNode*>::iterator oldCurrent = current;
	do
	{
		next(false);	//sets us to the beginning if we hit the end -> loop through the maps

		char * szCurrentName = strlwr(strdup((*current).second->filename.c_str()));

		if(strstr(szCurrentName, szLookForName))	//compare names after
			fFound = true;

		free(szCurrentName);
	}
	while(current != oldCurrent && !fFound);

	free(szLookForName);

	return fFound;
}

bool MapList::findexact(const char * name)
{
	char * szLookForName = new char[strlen(name) + 1];
	strcpy(szLookForName, name);
	strlwr(szLookForName);

	bool fFound = false;

	std::map<std::string, MapListNode*>::iterator oldCurrent = current;

	do
	{
		next(false);	//sets us to the beginning if we hit the end -> loop through the maps

		char * szCurrentName = new char[current->first.length() + 1];
		strcpy(szCurrentName, current->first.c_str());
		strlwr(szCurrentName);

		if(!strcmp(szCurrentName, szLookForName))
			fFound = true;

		delete[] szCurrentName;
	}
	while(current != oldCurrent && !fFound);

	delete[] szLookForName;

	return fFound;
}

//Returns true if the map needs to be reloaded
bool MapList::FindFilteredMap()
{
	if((*current).second->fInCurrentFilterSet)
		return false;

	next(true);
	return true;
}

//Update to allow key combos like "hy" to do "hyrule castle"
bool MapList::startswith(char letter)
{
	//Captialize the letter becuase all maps have first letter in caps
	if(letter >= SDLK_a && letter <= SDLK_z)
		letter -= 32;

	std::map<std::string, MapListNode*>::iterator oldCurrent = current;
	do
	{
		next(true);	//sets us to the beginning if we hit the end -> loop through the maps

		if(currentShortmapname()[0] == letter)
			return true;
	}
	while(current != oldCurrent);

	return false;
}

void MapList::prev(bool fUseFilters)
{
	if(fUseFilters)
	{
		std::map<std::string, MapListNode*>::iterator oldCurrent = current;

		do
		{
			prev(false);

			if((*current).second->fInCurrentFilterSet)
				return;
		}
		while(current != oldCurrent);
	}
	else
	{
		if(current == maps.begin())	//we are at the first element
			current = --maps.end();	//continue from end
		else
			--current;
	}

	return;
}

void MapList::next(bool fUseFilters)
{
	if(fUseFilters)
	{
		std::map<std::string, MapListNode*>::iterator oldCurrent = current;
		
		do
		{
			next(false);

			if((*current).second->fInCurrentFilterSet)
				return;
		}
		while(current != oldCurrent);
	}
	else
	{
		if(current == --maps.end())	//we are at the last valid element
			current = maps.begin();	//continue from start
		else
			++current;
	}

	return;
}

void MapList::random(bool fUseFilters)
{
	int iShuffle = 0;
	if(fUseFilters)
	{
		if(iFilteredMapCount < 2)
			return;

		iShuffle = rand() % (iFilteredMapCount - 1);
	}
	else
	{
		iShuffle = rand() % (maps.size() - 1);
	}

	for(int i = 0; i <= iShuffle; i++)
		next(fUseFilters);
}


void MapList::WriteFilters()
{
	//Save user defined filters back to files
	if(game_values.fNeedWriteFilters)
	{
		game_values.fNeedWriteFilters = false;

		for(short iFilter = 0; iFilter < filterslist.GetCount(); iFilter++)
		{
			FILE * fp = fopen(filterslist.GetIndex(iFilter), "w");

			if(!fp)
				continue;

			fprintf(fp, "#Version\n");
			fprintf(fp, "%d.%d.%d.%d\n\n", g_iVersion[0], g_iVersion[1], g_iVersion[2], g_iVersion[3]);
			
			fprintf(fp, "#Icon\n");
			fprintf(fp, "%d\n\n", game_values.piFilterIcons[iFilter + NUM_AUTO_FILTERS]);

			fprintf(fp, "#Maps\n");
			
			std::map<std::string, MapListNode*>::iterator itr = maps.begin(), lim = maps.end();

			while(itr != lim)
			{
				if((*itr).second->pfFilters[iFilter + NUM_AUTO_FILTERS])
					fprintf(fp, "%s\n", (*itr).first.c_str());

				itr++;
			}

			fclose(fp);
		}
	}
}

void MapList::ReadFilters()
{
	char buffer[256];

	//Get auto filters from maps
	current = maps.begin();

	//Used cached summary before trying to read the actual map file (to speed up load time)
	FILE * fp = fopen(convertPath("maps/cache/mapsummary.txt").c_str(), "r");

	if(fp)
	{
		while(fgets(buffer, 256, fp))
		{
			char * pszMapName = strtok(buffer, ",\n");

			if(maps.find(pszMapName) != maps.end())
			{
				bool fErrorReading = false;
				for(short iFilter = 0; iFilter < NUM_AUTO_FILTERS; iFilter++)
				{
					char * psz = strtok(NULL, ",\n");
					
					if(psz)
					{
						maps[pszMapName]->pfFilters[iFilter] = strcmp(psz, "0") != 0;
					}
					else
					{
						fErrorReading = true;
						break;
					}
				}

				if(!fErrorReading)
					maps[pszMapName]->fReadFromCache = true;
			}
		}

		fclose(fp);
	}

	while(current != maps.end())
	{
		if(!current->second->fReadFromCache)
		{
			MapListNode * mln = current->second;
			g_map.loadMap(mln->filename, read_type_summary);
			memcpy(mln->pfFilters, g_map.fAutoFilter, sizeof(bool) * NUM_AUTO_FILTERS);
		}

		current++;
	}

	current = maps.begin();
	//Get user defined filters from files in filters directory
	for(short iFilter = 0; iFilter < filterslist.GetCount(); iFilter++)
	{
		FILE * fp = fopen(filterslist.GetIndex(iFilter), "r");

		if(!fp)
			continue;

		short iVersion[4] = {0, 0, 0, 0};
		short iReadState = 0;
		while(fgets(buffer, 256, fp))
		{
			if(buffer[0] == '#' || buffer[0] == '\n' || buffer[0] == '\r' || buffer[0] == ' ' || buffer[0] == '\t')
				continue;

			if(0 == iReadState)
			{
				char * psz = strtok(buffer, ".\n");
				if(psz)
					iVersion[0] = atoi(psz);

				psz = strtok(NULL, ".\n");
				if(psz)
					iVersion[1] = atoi(psz);

				psz = strtok(NULL, ".\n");
				if(psz)
					iVersion[2] = atoi(psz);

				psz = strtok(NULL, ".\n");
				if(psz)
					iVersion[3] = atoi(psz);

				iReadState = 1;
				continue;
			}
			else if(1 == iReadState)
			{
				game_values.piFilterIcons[iFilter + NUM_AUTO_FILTERS] = atoi(buffer);
				iReadState = 2;
				continue;
			}
			else
			{
				char * pszMap = strtok(buffer, "\r\n");

				//If that map is found
				if(findexact(pszMap))
					(*current).second->pfFilters[iFilter + NUM_AUTO_FILTERS] = true;
			}
		}

		fclose(fp);
	}

	//Reset the current back to the beginning after setting up the filters for each map
	current = maps.begin();
}

//Forces all the maps to reload the auto filters from the live map files (flush the cache)
void MapList::ReloadMapAutoFilters()
{
	std::map<std::string, MapListNode*>::iterator itr = maps.begin(), lim = maps.end();

	while(itr != lim)
	{
		MapListNode * mln = itr->second;
		g_map.loadMap(mln->filename, read_type_summary);
		memcpy(mln->pfFilters, g_map.fAutoFilter, sizeof(bool) * NUM_AUTO_FILTERS);

		itr++;
	}
}

void MapList::WriteMapSummaryCache()
{
	FILE * fp = fopen(convertPath("maps/cache/mapsummary.txt").c_str(), "w");
	
	if(!fp)
		return;

	std::map<std::string, MapListNode*>::iterator itr = maps.begin(), lim = maps.end();

	while(itr != lim)
	{
		fprintf(fp, itr->first.c_str());

		for(short iFilter = 0; iFilter < NUM_AUTO_FILTERS; iFilter++)
			fprintf(fp, ",%d", itr->second->pfFilters[iFilter]);

		fprintf(fp, "\n");
		itr++;
	}

	fclose(fp);
}

//Applies the currently selected filters to the entire map set
//After this call, when flipping through maps, only the matched maps
//will show up in the map field or in the thumbnail browser
void MapList::ApplyFilters(bool * pfFilters)
{
	std::map<std::string, MapListNode*>::iterator itr = maps.begin(), lim = maps.end();

	iFilteredMapCount = 0;
	short iTotalCount = 0;
	while(itr != lim)
	{
		bool fMatched = true;
		for(short iFilter = 0; iFilter < NUM_AUTO_FILTERS + filterslist.GetCount(); iFilter++)
		{
			if(pfFilters[iFilter])
			{
				if(!(*itr).second->pfFilters[iFilter])
				{
					fMatched = false;
					break;
				}
			}
		}

		(*itr).second->fInCurrentFilterSet = fMatched;

		if(fMatched)
		{
			(*itr).second->iFilteredIndex = iFilteredMapCount;
			mlnFilteredMaps[iFilteredMapCount] = itr;
			iFilteredMapCount++;
		}

		mlnMaps[iTotalCount++] = itr;

		itr++;
	}

	game_values.fFiltersOn = false;
	for(short iFilter = 0; iFilter < NUM_AUTO_FILTERS + filterslist.GetCount(); iFilter++)
	{
		if(pfFilters[iFilter])
		{
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
	if(fUseFilters)
	{
		if(iIndex >= iFilteredMapCount)
			return maps.end();

		return mlnFilteredMaps[iIndex];
	}
	else
	{
		if(iIndex >= maps.size())
			return maps.end();

		return mlnMaps[iIndex];
	}
}
