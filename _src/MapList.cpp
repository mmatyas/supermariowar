
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
	std::map<std::string, MapListNode*>::iterator iterateAll = maps.begin();
	
	while (iterateAll != maps.end())
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
	//TODO: remove std::string conversion after re-enabling convertPath
	std::string fullName = (std::string)(convertPath("maps/")) + name;	//why does name end with .map???

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

		free(szCurrentName);
	}
	while(current != oldCurrent && !fFound);

	free(szLookForName);

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
			
			std::map<std::string, MapListNode*>::iterator itr = maps.begin();

			while(itr != maps.end())
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
	std::map<std::string, MapListNode*>::iterator itr = maps.begin();

	while(itr != maps.end())
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

	std::map<std::string, MapListNode*>::iterator itr = maps.begin();

	while(itr != maps.end())
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
	std::map<std::string, MapListNode*>::iterator itr = maps.begin();

	iFilteredMapCount = 0;
	short iTotalCount = 0;
	while(itr != maps.end())
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

///////////// SimpleFileList ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SimpleFileList::SimpleFileList(const std::string &path, const std::string &extension)
{
	DirectoryListing d(path, extension);
	std::string curname;
	while(d(curname))
	{
		filelist.insert(filelist.end(), d.fullName(curname));
	}

	currentIndex = 0;

	if(filelist.empty())
	{
		//printf("ERROR: Empty directory!\n");
		//exit(0);
		currentIndex = -1;
	}
}


void SimpleFileList::next()
{
	if(filelist.empty())
		return;

	if(currentIndex + 1 == int(filelist.size()))
		currentIndex = 0;
	else
		currentIndex++;
};

void SimpleFileList::prev()
{
	if(filelist.empty())
		return;

	if(currentIndex == 0)
		currentIndex = filelist.size() - 1;
	else
		currentIndex--;
};

const char * SimpleFileList::GetIndex(unsigned int index)
{
	if(index < filelist.size())
		return filelist[index].c_str();

	return NULL;
}


void SimpleFileList::SetCurrentName(const std::string &name)
{
	if(filelist.empty())
		return;

	for(unsigned short i = 0; i < filelist.size(); i++)
	{
		if(!strcmp(filelist[i].c_str(), name.c_str()))
		{
			currentIndex = i;
			break;
		}
	}
}

///////////// SkinList ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkinListNode::SkinListNode(std::string skinName, std::string skinPath)
{
	sSkinName = skinName;
	sSkinPath = skinPath;
}

		
SkinList::SkinList()
{
	DirectoryListing d(convertPath("gfx/skins/"), ".bmp");
	std::string curname;
	while(d(curname))
	{
		std::string sShortSkinName = stripCreatorAndDotMap(curname);
		SkinListNode * node = new SkinListNode(sShortSkinName, d.fullName(curname));
		
		if(skins.empty())
		{
			skins.push_back(node);
		}
		else
		{
			std::vector<SkinListNode*>::iterator itr = skins.begin();

			while(itr != skins.end())
			{
				if(sShortSkinName.compare((*itr)->sSkinName) < 0)
					break;

				itr++;
			}

			skins.insert(itr, node);
		}
	}
}

const char * SkinList::GetIndex(unsigned int index)
{
	if(index < skins.size())
		return skins[index]->sSkinPath.c_str();

	return NULL;
}

const char * SkinList::GetSkinName(unsigned int index)
{
	if(index < skins.size())
		return skins[index]->sSkinName.c_str();

	return NULL;
}

///////////// SimpleDirectoryList ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SimpleDirectoryList::SimpleDirectoryList(const std::string &path)
{
	DirectoryListing d(path);
	std::string curname;
	while(d.NextDirectory(curname))
	{
		filelist.insert(filelist.end(), d.fullName(curname));
	}
	if(filelist.empty())
	{
		printf("ERROR: Empty directory.  %s\n", path.c_str());
		//exit(0);

		char hi;
		scanf("%c", &hi);
	}

	currentIndex = 0;
}


///////////// MusicList ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MusicList::MusicList()
{
	DirectoryListing d(convertPath("music/"));
	std::string currentdir;
	while(d.NextDirectory(currentdir))
	{
		MusicEntry *m = new MusicEntry(d.fullName(currentdir));
		if (!m->fError)
			entries.push_back(m);
		else
			delete m;
	}
	
	if(entries.empty())
	{
		printf("ERROR: Empty Music directory!\n");
		exit(0);
	}

	currentIndex = 0;
}

MusicList::~MusicList()
{
	for(unsigned int i = 0; i < entries.size(); i++)
	{
		delete entries[i];
	}
}

string MusicList::GetMusic(int musicID)
{
	return entries[currentIndex]->GetMusic(musicID);
}

void MusicList::SetRandomMusic(int iMusicCategory, const char * szMapName, const char * szBackground)
{
	CurrentMusic = entries[currentIndex]->GetRandomMusic(iMusicCategory, szMapName, szBackground);
}

void MusicList::SetNextMusic(int iMusicCategory, const char * szMapName, const char * szBackground)
{
	CurrentMusic = entries[currentIndex]->GetNextMusic(iMusicCategory, szMapName, szBackground);
}

string MusicList::GetCurrentMusic()
{
	return CurrentMusic;
}


void MusicList::next()
{
	if(currentIndex+1 == int(entries.size()))
		currentIndex = 0;
	else
		currentIndex++;
};

void MusicList::prev()
{
	if(currentIndex == 0)
		currentIndex = entries.size()-1;
	else
		currentIndex--;
};
		

///////////// MusicEntry ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MusicEntry::MusicEntry(const std::string & musicdirectory)
{
	fError = false;
	fUsesMapOverrides = false;
	fUsesBackgroundOverrides = false;

	iCurrentMusic = 0;

	int i, k;

	char * szDir = (char*)(musicdirectory.c_str());
	
	for(k = 0; k < MAXMUSICCATEGORY; k++)
		numsongsforcategory[k] = 0;

	char musiclistname[FILEBUFSIZE];
#ifdef _XBOX
	sprintf(musiclistname, szDir + 9);
#else
    char * p = strrchr(szDir, '/');
    if (!p) p=szDir; else p++;
    strcpy(musiclistname, p);
#endif

	for(i = (int)strlen(musiclistname); i >= 0; i--)
	{
		if(musiclistname[i] == '.')
		{
			musiclistname[i] = '\0';
			break;
		}
	}

	name = musiclistname;

	std::string musicfile = musicdirectory + getDirectorySeperator() + std::string("Music.txt");

	FILE * in = fopen(musicfile.c_str(), "r");

	if(!in)
	{
		printf("Error: Could not open: %s\n", musicfile.c_str());
		fError = true;
		return;
	}

	int iNumFile = 0;
	int iAddToCategory = -1;
	char szBuffer[256];
	while(fgets(szBuffer, 256, in))
	{
		//Ignore comment lines
		if(szBuffer[0] == '#' || szBuffer[0] == ' ' || szBuffer[0] == '\t' || szBuffer[0] == '\n' || szBuffer[0] == '\r')
			continue;

		//Chop off line ending
		int stringLength = strlen(szBuffer);
		for(k = 0; k < stringLength; k++)
		{
			if(szBuffer[k] == '\r' || szBuffer[k] == '\n')
			{
				szBuffer[k] = '\0';
				break;
			}
		}

		//If we found a category header
		if(szBuffer[0] == '[')
		{
			if(!stricmp(szBuffer, "[land]"))
				iAddToCategory = 0;
			else if(!stricmp(szBuffer, "[underground]"))
				iAddToCategory = 1;
			else if(!stricmp(szBuffer, "[underwater]"))
				iAddToCategory = 2;
			else if(!stricmp(szBuffer, "[castle]"))
				iAddToCategory = 3;
			else if(!stricmp(szBuffer, "[platforms]"))
				iAddToCategory = 4;
			else if(!stricmp(szBuffer, "[ghost]"))
				iAddToCategory = 5;
			else if(!stricmp(szBuffer, "[bonus]"))
				iAddToCategory = 6;
			else if(!stricmp(szBuffer, "[battle]"))
				iAddToCategory = 7;
			else if(!stricmp(szBuffer, "[desert]"))
				iAddToCategory = 8;
			else if(!stricmp(szBuffer, "[clouds]"))
				iAddToCategory = 9;
			else if(!stricmp(szBuffer, "[snow]"))
				iAddToCategory = 10;
			else if(!stricmp(szBuffer, "[maps]"))
				iAddToCategory = MAXMUSICCATEGORY;
			else if(!stricmp(szBuffer, "[backgrounds]"))
				iAddToCategory = MAXMUSICCATEGORY + 1;

			continue;
		}

		//Cap the number of songs at MAXCATEGORYTRACKS for a category
		if(iAddToCategory > -1 && iAddToCategory < MAXMUSICCATEGORY && numsongsforcategory[iAddToCategory] >= MAXCATEGORYTRACKS)
			continue;

		if(iNumFile < 4 || iAddToCategory > -1)
		{
			if(iAddToCategory == MAXMUSICCATEGORY || iAddToCategory == MAXMUSICCATEGORY + 1)
			{
				char * pszName = strtok(szBuffer, ",\n");
				
				if(!pszName)
					continue;

				if(iAddToCategory == MAXMUSICCATEGORY)
				{
					if(mapoverride.find(pszName) == mapoverride.end())
						mapoverride[pszName] = new MusicOverride();
				}
				else
				{
					if(backgroundoverride.find(pszName) == backgroundoverride.end())
						backgroundoverride[pszName] = new MusicOverride();
				}

				char * pszMusic = strtok(NULL, ",\n");
				while(pszMusic)
				{
					std::string sPath = musicdirectory + getDirectorySeperator() + convertPartialPath(std::string(pszMusic));

					if(File_Exists(sPath.c_str()))
					{
						songFileNames.push_back(sPath);

						if(iAddToCategory == MAXMUSICCATEGORY)
						{
							fUsesMapOverrides = true;
							mapoverride[pszName]->songs.push_back(iNumFile);
						}
						else
						{
							fUsesBackgroundOverrides = true;
							backgroundoverride[pszName]->songs.push_back(iNumFile);
						}

						iNumFile++;
					}
					pszMusic = strtok(NULL, ",\n");
				}
			}
			else
			{
				std::string sPath = musicdirectory + getDirectorySeperator() + convertPartialPath(std::string(szBuffer));

				if(File_Exists(sPath.c_str()))
				{
					songFileNames.push_back(sPath);

					//Don't add to category lists if this is one of the four special music tracks
					if(iNumFile >= 4)
					{
						songsforcategory[iAddToCategory][numsongsforcategory[iAddToCategory]] = iNumFile;
						numsongsforcategory[iAddToCategory]++;
					}
					iNumFile++;
				}
			}
		}
	}

	fclose(in);

	//Now read labeled subdirectories like "Land", "Underground", "Castle", etc for more songs

	for(short iMusicCategory = 0; iMusicCategory < MAXMUSICCATEGORY; iMusicCategory++)
	{
		std::string musicPath = musicdirectory + getDirectorySeperator() + std::string(g_szMusicCategoryNames[iMusicCategory]);
		if(File_Exists(musicPath))
		{
			SimpleFileList musiclist(musicPath + getDirectorySeperator(), ".ogg");

			short iCount = musiclist.GetCount();

			//printf("Found %d files in %s\n", iCount, musicPath.c_str());

			for(short iFile = 0; iFile < iCount; iFile++)
			{
				if(numsongsforcategory[iMusicCategory] < MAXCATEGORYTRACKS)
				{
					songFileNames.push_back(musiclist.current_name());
					songsforcategory[iMusicCategory][numsongsforcategory[iMusicCategory]] = iNumFile;
					numsongsforcategory[iMusicCategory]++;
					iNumFile++;
				}

				musiclist.next();
			}
		}
	}

	if(iNumFile == 0)
	{
		printf("Error: No songs found in: %s\n", musicdirectory.c_str());
		fError = true;
		return;
	}

	//Verify we have at least one track for each category
	for(i = 0; i < MAXMUSICCATEGORY; i++)
	{
		if(numsongsforcategory[i] == 0)
		{
			if(i < 4)
			{
				printf("Error: Missing track definition for music category: %s\n", g_szMusicCategoryNames[i]);
				fError = true;
				return;
			}
			else  //Use default category
			{
				numsongsforcategory[i] = numsongsforcategory[g_iDefaultMusicCategory[i]];
				for(k = 0; k < numsongsforcategory[i]; k++)
				{
					songsforcategory[i][k] = songsforcategory[g_iDefaultMusicCategory[i]][k];
				}
			}
		}
	}

	/*
	std::vector<std::string>::iterator itr = songFileNames.begin();

	printf("------ Songlist ------\n");
	short iCounter = 0;
	while(itr != songFileNames.end())
	{
		printf("%d: %s\n", iCounter++, itr->c_str());
		itr++;
	}

	printf("\n\n");

	for(i = 0; i < MAXMUSICCATEGORY; i++)
	{
		printf("---- %d ----\n", i);
		printf("Count: %d\n", numsongsforcategory[i]);
		for(k = 0; k < numsongsforcategory[i]; k++)
			printf("%d\n", songsforcategory[i][k]);
	}
	*/
};



string MusicEntry::GetMusic(unsigned int musicID)
{
    if (musicID >= songFileNames.size())
        return songFileNames[songFileNames.size()-1];

    return songFileNames[musicID];
}

string MusicEntry::GetRandomMusic(int iMusicCategory, const char * szMapName, const char * szBackground)
{
	//First check if there is specific map music
	if(fUsesMapOverrides && mapoverride.find(szMapName) != mapoverride.end())
	{
		if(mapoverride[szMapName]->songs.size() > 0)
		{
			iCurrentMusic = rand() % mapoverride[szMapName]->songs.size();
			return songFileNames[mapoverride[szMapName]->songs[iCurrentMusic]];
		}
	}
	//Then check if there is specific background music
	if(fUsesBackgroundOverrides && backgroundoverride.find(szBackground) != backgroundoverride.end())
	{
		if(backgroundoverride[szBackground]->songs.size() > 0)
		{
			iCurrentMusic = rand() % backgroundoverride[szBackground]->songs.size();
			return songFileNames[backgroundoverride[szBackground]->songs[iCurrentMusic]];
		}
	}

	//Then default to the music category
	if(iMusicCategory >= 0 && iMusicCategory < MAXMUSICCATEGORY && numsongsforcategory[iMusicCategory] > 0)
	{
		iCurrentMusic = rand() % numsongsforcategory[iMusicCategory];
		return songFileNames[songsforcategory[iMusicCategory][iCurrentMusic]];
	}

	return songFileNames[4];
}

string MusicEntry::GetNextMusic(int iMusicCategory, const char * szMapName, const char * szBackground)
{
	//First check if there is specific map music
	if(mapoverride.find(szMapName) != mapoverride.end())
	{
		if(mapoverride[szMapName]->songs.size() > 0)
		{
			if(++iCurrentMusic >= mapoverride[szMapName]->songs.size())
				iCurrentMusic = 0;

			return songFileNames[mapoverride[szMapName]->songs[iCurrentMusic]];
		}
	}
	//Then check if there is specific background music
	if(backgroundoverride.find(szBackground) != backgroundoverride.end())
	{
		if(backgroundoverride[szBackground]->songs.size() > 0)
		{
			if(++iCurrentMusic >= backgroundoverride[szBackground]->songs.size())
				iCurrentMusic = 0;

			return songFileNames[backgroundoverride[szBackground]->songs[iCurrentMusic]];
		}
	}

	//Then default to the music category
	if(iMusicCategory >= 0 && iMusicCategory < MAXMUSICCATEGORY && numsongsforcategory[iMusicCategory] > 0)
	{
		if(++iCurrentMusic >= numsongsforcategory[iMusicCategory])
			iCurrentMusic = 0;

		return songFileNames[songsforcategory[iMusicCategory][iCurrentMusic]];
	}

	return songFileNames[4];
}

