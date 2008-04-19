#include "global.h"
#include <iostream>
using std::cout;
using std::endl;

#ifdef PNG_SAVE_FORMAT
	#include "savepng.h"
#endif

#ifdef _WIN32
	#ifndef _XBOX
		#pragma comment(linker, "/NODEFAULTLIB:libc.lib")

		#ifdef PNG_SAVE_FORMAT
			#pragma comment(lib, "libpng.lib")
			#pragma comment(lib, "zlib.lib")
		#endif
    #endif
#endif

#define fopen(f,m) fopen(f.c_str(),m)

extern gfxSprite spr_frontmap[2];
extern short g_iTileConversion[];
extern short g_iVersion[];
extern char * g_szBackgroundConversion[26];
extern short g_iMusicCategoryConversion[26];

short g_iTileTypeConversion[13] = {0, 1, 2, 5, 121, 9, 17, 33, 65, 6, 21, 37, 69};

CMap::CMap()
{
	platforms = NULL;
	iNumPlatforms = 0;
	iNumMapItems = 0;
	iNumMapHazards = 0;

	animatedTilesSurface = NULL;

	for(short iSwitch = 0; iSwitch < 4; iSwitch++)
		iSwitches[iSwitch] = 0;
}

CMap::~CMap()
{}


//With the new 32x30 tile set, we need to convert old maps to use the 
//correct indexes into the tile set
/*
void CMap::convertMap()
{
	int i, j, k;

	bool fTilesNeedConversion = true;
	bool fBlocksNeedConversion = true;

	for(j = 0; j < MAPHEIGHT; j++)
	{
		for(i = 0; i < MAPWIDTH; i++)
		{
			for(k = 0; k < MAPLAYERS; k++)
			{
				//if we find an empty tile (value 960), then we already have a converted map
				if(mapdata[i][j][k] == TILESETSIZE)
					fTilesNeedConversion = false;
			}

			if(objectdata[i][j.iType == -1)
				fBlocksNeedConversion = false;
		}
	}

	if(!fTilesNeedConversion && !fBlocksNeedConversion)
		return;

	//Otherwise, convert this map
	for(j = 0; j < MAPHEIGHT; j++)
	{
		for(i = 0; i < MAPWIDTH; i++)
		{
			for(k = 0; k < MAPLAYERS; k++)
			{
				mapdata[i][j][k] = g_iTileConversion[mapdata[i][j][k]];
			}
		}
	}
}*/

void CMap::clearMap()
{
	int i, j, k;

	for(j = 0; j < MAPHEIGHT; j++)
	{
		for(i = 0; i < MAPWIDTH; i++)
		{
			for(k = 0; k < MAPLAYERS; k++)
			{
				//reset tile
				mapdata[i][j][k].iID = TILESETNONE;  //no tile selected
			}

			mapdatatop[i][j].iType = tile_nonsolid;
			mapdatatop[i][j].iFlags = tile_flag_nonsolid;

			objectdata[i][j].iType = -1;
			warpdata[i][j].direction = -1;
			warpdata[i][j].connection = -1;

			for(short iSpawn = 0; iSpawn < NUMSPAWNAREATYPES; iSpawn++)
				nospawn[iSpawn][i][j] = false;
		}
	}

	eyecandyID = 0;
	iNumMapItems = 0;
	iNumMapHazards = 0;

	bltrect.w = TILESIZE;
	bltrect.h = TILESIZE;
}

void CMap::clearPlatforms()
{
	if(platforms)
	{
		for(short iPlatform = 0; iPlatform < iNumPlatforms; iPlatform++)
		{
			delete platforms[iPlatform];
		}

		delete [] platforms;
		platforms = NULL;
	
		iNumPlatforms = 0;
	}

	std::list<MovingPlatform*>::iterator iter = tempPlatforms.begin(), lim = tempPlatforms.end();
	
	while (iter != lim)
	{
		delete (*iter);
		++iter;
	}

	tempPlatforms.clear();
}

void CMap::ClearAnimatedTiles()
{
	std::vector<AnimatedTile*>::iterator iter = animatedtiles.begin(), lim = animatedtiles.end();
	
	while (iter != lim)
	{
		delete (*iter);
		++iter;
	}

	animatedtiles.clear();
}

void CMap::loadMap(const std::string& file, ReadType iReadType)
{
	iTileAnimationTimer = 0;
	iTileAnimationFrame = 0;
	
	ClearAnimatedTiles();

	FILE * mapfile;
	short i, j, k;

	/*
    cout << "loading map " << file;
	
	if(iReadType == read_type_preview)
		cout << " (preview)";
	if(iReadType == read_type_summary)
		cout << " (summary)";

	cout << " ...";
	*/

	mapfile = fopen(file, "rb");
	if(mapfile == NULL)
	{
        cout << endl << " ERROR: Couldn't open map" << endl;
		return;
	}

	//Load version number
	int version[4];
	//version[0] = ReadInt(mapfile); //Major
	//version[1] = ReadInt(mapfile); //Minor
	//version[2] = ReadInt(mapfile); //Micro
	//version[3] = ReadInt(mapfile); //Build

	ReadIntChunk(version, 4, mapfile);

	if(version[0] == 1 && version[1] == 8)
	{
		//Read summary information here
		
		int iAutoFilterValues[NUM_AUTO_FILTERS + 1];
		ReadIntChunk(iAutoFilterValues, NUM_AUTO_FILTERS + 1, mapfile);

		for(short iFilter = 0; iFilter < NUM_AUTO_FILTERS; iFilter++)
			fAutoFilter[iFilter] = iAutoFilterValues[iFilter] > 0;

		if(iReadType == read_type_summary)
		{
			fclose(mapfile);
			return;
		}

		clearPlatforms();

		cout << "loading map " << file;
	
		if(iReadType == read_type_preview)
			cout << " (preview)";
		
		cout << " [Version " << version[0] << '.' << version[1] << '.'
			<< version[2] << '.' << version[3] << " Map Detected]\n";

		//Load tileset information
		short iNumTilesets = ReadInt(mapfile);

		TilesetTranslation * translation = new TilesetTranslation[iNumTilesets];

		short iMaxTilesetID = 0; //Figure out how big the translation array needs to be
		for(short iTileset = 0; iTileset < iNumTilesets; iTileset++)
		{
			short iTilesetID = ReadInt(mapfile);
			translation[iTileset].iID = iTilesetID;

			if(iTilesetID > iMaxTilesetID)
				iMaxTilesetID = iTilesetID;

			ReadString(translation[iTileset].szName, 128, mapfile);
		}

		short * translationid = new short[iMaxTilesetID + 1];

		for(short iTileset = 0; iTileset < iNumTilesets; iTileset++)
		{
			translationid[translation[iTileset].iID] = g_tilesetmanager.GetIndexFromName(translation[iTileset].szName);
		}

		delete [] translation;

		//2. load map data
		for(j = 0; j < MAPHEIGHT; j++)
		{
			for(i = 0; i < MAPWIDTH; i++)
			{
				for(k = 0; k < MAPLAYERS; k++)
				{
					//mapdata[i][j][k] = (short)ReadInt(mapfile);

					TilesetTile * tile = &mapdata[i][j][k];
					tile->iID = ReadByteAsShort(mapfile);

					//Convert tileset ids into the current game's tileset's ids
					if(tile->iID >= 0)
						tile->iID = translationid[tile->iID];

					tile->iCol = ReadByteAsShort(mapfile);
					tile->iRow = ReadByteAsShort(mapfile);
				}

				objectdata[i][j].iType = ReadByteAsShort(mapfile);
				objectdata[i][j].fHidden = ReadBool(mapfile);
			}
		}

		//Read in background to use
		ReadString(szBackgroundFile, 128, mapfile);

		//Read on/off switches
		for(short iSwitch = 0; iSwitch < 4; iSwitch++)
		{
			iSwitches[iSwitch] = (short)ReadInt(mapfile);
		}

		loadPlatforms(mapfile, iReadType == read_type_preview, version, translationid);

		//All tiles have been loaded so the translation is no longer needed
		delete [] translationid;

		//Load map items (like carryable spikes and springs)
		iNumMapItems = ReadInt(mapfile);

		for(j = 0; j < iNumMapItems; j++)
		{
			mapitems[j].itype = ReadInt(mapfile);
			mapitems[j].ix = ReadInt(mapfile);
			mapitems[j].iy = ReadInt(mapfile);
		}

		//Load map hazards (like fireball strings, rotodiscs, pirhana plants)
		iNumMapHazards = ReadInt(mapfile);

		for(short iMapHazard = 0; iMapHazard < iNumMapHazards; iMapHazard++)
		{
			maphazards[iMapHazard].itype = ReadInt(mapfile);
			maphazards[iMapHazard].ix = ReadInt(mapfile);
			maphazards[iMapHazard].iy = ReadInt(mapfile);

			for(short iParam = 0; iParam < NUMMAPHAZARDPARAMS; iParam++)
				maphazards[iMapHazard].iparam[iParam] = ReadInt(mapfile);
			
			for(short iParam = 0; iParam < NUMMAPHAZARDPARAMS; iParam++)
				maphazards[iMapHazard].dparam[iParam] = ReadFloat(mapfile);
		}

		//Read in eyecandy to use
		eyecandyID = (short)ReadInt(mapfile);

		musicCategoryID = ReadInt(mapfile);

		for(j = 0; j < MAPHEIGHT; j++)
		{
			for(i = 0; i < MAPWIDTH; i++)
			{
				TileType iType = (TileType)ReadInt(mapfile);
				mapdatatop[i][j].iType = iType;

				if(iType >= 0 && iType <= 12)
					mapdatatop[i][j].iFlags = g_iTileTypeConversion[iType];
				else
				{
					mapdatatop[i][j].iType = tile_nonsolid;
					mapdatatop[i][j].iFlags = tile_flag_nonsolid;
				}

				warpdata[i][j].direction = (short)ReadInt(mapfile);
				warpdata[i][j].connection = (short)ReadInt(mapfile);
				warpdata[i][j].id = (short)ReadInt(mapfile);
				
				for(short iType = 0; iType < NUMSPAWNAREATYPES; iType++)
					nospawn[iType][i][j] = ReadBool(mapfile);
			}
		}

		//Read switch block state data
		int iNumSwitchBlockData = ReadInt(mapfile);
		for(short iBlock = 0; iBlock < iNumSwitchBlockData; iBlock++)
		{
			short iCol = ReadByteAsShort(mapfile);
			short iRow = ReadByteAsShort(mapfile);

			objectdata[iCol][iRow].iSettings[0] = ReadByteAsShort(mapfile);
		}

		if(iReadType == read_type_preview)
		{
			fclose(mapfile);
			return;
		}

		maxConnection = 0;

		numwarpexits = (short)ReadInt(mapfile);
		for(i = 0; i < numwarpexits && i < MAXWARPS; i++)
		{
			warpexits[i].direction = (short)ReadInt(mapfile);
			warpexits[i].connection = (short)ReadInt(mapfile);
			warpexits[i].id = (short)ReadInt(mapfile);
			warpexits[i].x = (short)ReadInt(mapfile);
			warpexits[i].y = (short)ReadInt(mapfile);

			warpexits[i].lockx = (short)ReadInt(mapfile);
			warpexits[i].locky = (short)ReadInt(mapfile);

			warpexits[i].warpx = (short)ReadInt(mapfile);
			warpexits[i].warpy = (short)ReadInt(mapfile);
			warpexits[i].numblocks = (short)ReadInt(mapfile);

			if(warpexits[i].connection > maxConnection)
				maxConnection = warpexits[i].connection;
		}

		//Ignore any more warps than the max
		for(i = 0; i < numwarpexits - MAXWARPS; i++)
		{
			for(j = 0; j < 10; j++)
				ReadInt(mapfile);
		}

		if(numwarpexits > MAXWARPS)
			numwarpexits = MAXWARPS;

		//Read spawn areas

		for(i = 0; i < NUMSPAWNAREATYPES; i++)
		{
			totalspawnsize[i] = 0;
			numspawnareas[i] = (short)ReadInt(mapfile);

			if(numspawnareas[i] > MAXSPAWNAREAS)
			{
				cout << endl << " ERROR: Number of spawn areas (" << numspawnareas[i]
					<< ") was greater than max allowed (" << MAXSPAWNAREAS << ')'
					<< endl;
				return;
			}

			for(int m = 0; m < numspawnareas[i]; m++)
			{
				spawnareas[i][m].left = (short)ReadInt(mapfile);
				spawnareas[i][m].top = (short)ReadInt(mapfile);
				spawnareas[i][m].width = (short)ReadInt(mapfile);
				spawnareas[i][m].height = (short)ReadInt(mapfile);
				spawnareas[i][m].size = (short)ReadInt(mapfile);
			
				totalspawnsize[i] += spawnareas[i][m].size;
			}
		}

		//Read draw areas (foreground tiles drawing optimization)
		numdrawareas = (short)ReadInt(mapfile);

		if(numdrawareas > MAXDRAWAREAS)
		{
			cout << endl << " ERROR: Number of draw areas (" << numdrawareas
                << ") was greater than max allowed (" << MAXDRAWAREAS << ')'
                << endl;
			return;
		}

		//Load rects to help optimize drawing the foreground
		for(int m = 0; m < numdrawareas; m++)
		{
			drawareas[m].x = (Sint16)ReadInt(mapfile);
			drawareas[m].y = (Sint16)ReadInt(mapfile);
			drawareas[m].w = (Uint16)ReadInt(mapfile);
			drawareas[m].h = (Uint16)ReadInt(mapfile);
		}

		int iNumExtendedDataBlocks = ReadInt(mapfile);

		for(short iBlock = 0; iBlock < iNumExtendedDataBlocks; iBlock++)
		{
			short iCol = ReadByteAsShort(mapfile);
			short iRow = ReadByteAsShort(mapfile);

			short iNumSettings = ReadByteAsShort(mapfile);
			for(short iSetting = 0; iSetting < iNumSettings; iSetting++)
				objectdata[iCol][iRow].iSettings[iSetting] = ReadByteAsShort(mapfile);
		}

		//read mode item locations like flags and race goals
		iNumRaceGoals = (short)ReadInt(mapfile);
		for(j = 0; j < iNumRaceGoals; j++)
		{
			racegoallocations[j].x = (short)ReadInt(mapfile);
			racegoallocations[j].y = (short)ReadInt(mapfile);
		}

		iNumFlagBases = (short)ReadInt(mapfile);
		for(j = 0; j < iNumFlagBases; j++)
		{
			flagbaselocations[j].x = (short)ReadInt(mapfile);
			flagbaselocations[j].y = (short)ReadInt(mapfile);
		}
	}
	else if(version[0] == 1 && version[1] == 7)
	{
		iNumMapItems = 0;
		iNumMapHazards = 0;

		//Read summary information here
		if((version[2] == 0 && version[3] > 1) || version[2] >= 1)
		{
			int iAutoFilterValues[9];
			ReadIntChunk(iAutoFilterValues, 9, mapfile);

			for(short iFilter = 0; iFilter < 8; iFilter++)
				fAutoFilter[iFilter] = iAutoFilterValues[iFilter] > 0;

			for(short iFilter = 8; iFilter < NUM_AUTO_FILTERS; iFilter++)
				fAutoFilter[iFilter] = false;

			//Read density and don't do anything with it at the moment
			//int iDensity = iAutoFilterValues[8];
		}
		else
		{
			for(short iFilter = 0; iFilter < NUM_AUTO_FILTERS; iFilter++)
				fAutoFilter[iFilter] = false;
		}

		if(iReadType == read_type_summary)
		{
			fclose(mapfile);
			return;
		}

		clearPlatforms();

		cout << "loading map " << file;
	
		if(iReadType == read_type_preview)
			cout << " (preview)";
		
		cout << " ";

		cout << "[Version " << version[0] << '.' << version[1] << '.'
			<< version[2] << '.' << version[3] << " Map Detected]\n";

		short iClassicTilesetID = g_tilesetmanager.GetIndexFromName("Classic");

		//2. load map data
		for(j = 0; j < MAPHEIGHT; j++)
		{
			for(i = 0; i < MAPWIDTH; i++)
			{
				for(k = 0; k < MAPLAYERS; k++)
				{
					short iTileID = (short)ReadInt(mapfile);

					TilesetTile * tile = &mapdata[i][j][k];
					
					if(iTileID == TILESETSIZE)
					{
						tile->iID = TILESETNONE;
						tile->iCol = 0;
						tile->iRow = 0;
					}
					else
					{
						tile->iID = iClassicTilesetID;
						tile->iCol = iTileID % TILESETWIDTH;
						tile->iRow = iTileID / TILESETWIDTH;
					}
				}

				objectdata[i][j].iType = (short)ReadInt(mapfile);
				if(objectdata[i][j].iType == 15)
					objectdata[i][j].iType = -1;

				objectdata[i][j].fHidden = false;
				
				if(objectdata[i][j].iType == 1)
				{
					for(short iSetting = 0; iSetting < NUM_BLOCK_SETTINGS; iSetting++)
						objectdata[i][j].iSettings[iSetting] = g_iDefaultPowerupPresets[0][iSetting];
				}
			}
		}

		if((version[2] == 0 && version[3] > 1) || version[2] >= 1)
		{
			//Read in background to use
			ReadString(szBackgroundFile, 128, mapfile);
			//printf("Background: %s", szBackgroundFile);
		}
		else if(version[2] == 0 && version[3] == 1)
		{
			//Read in background to use
			ReadString(szBackgroundFile, 128, mapfile);
			
			for(short iBackground = 0; iBackground < 26; iBackground++)
			{
				char * szFindUnderscore = strstr(g_szBackgroundConversion[iBackground], "_");

				if(szFindUnderscore)
					szFindUnderscore++;

				if(!strcmp(szFindUnderscore, szBackgroundFile))
				{
					strcpy(szBackgroundFile, g_szBackgroundConversion[iBackground]);
				}
			}
		}
		else
		{
			//Read old background IDs and convert that to a background filename
			backgroundID = (short)ReadInt(mapfile);
			strcpy(szBackgroundFile, g_szBackgroundConversion[backgroundID]);
		}

		if((version[2] == 0 && version[3] > 0) || version[2] >= 1)
		{
			//Read on/off switches
			for(short iSwitch = 0; iSwitch < 4; iSwitch++)
			{
				iSwitches[iSwitch] = 1 - (short)ReadInt(mapfile);
			}

			//Set all the on/off blocks correctly
			for(j = 0; j < MAPHEIGHT; j++)
			{
				for(i = 0; i < MAPWIDTH; i++)
				{
					if(objectdata[i][j].iType >= 11 && objectdata[i][j].iType <= 14)
					{
						objectdata[i][j].iSettings[0] = iSwitches[objectdata[i][j].iType - 11];
					}
				}
			}
		}
		else if(iReadType == read_type_preview)  //if it is a preview, for older maps, set the on/off blocks to on by default
		{
			//Read on/off switches
			for(short iSwitch = 0; iSwitch < 4; iSwitch++)
			{
				iSwitches[iSwitch] = 1;
			}

			//Set all the on/off blocks correctly
			for(j = 0; j < MAPHEIGHT; j++)
			{
				for(i = 0; i < MAPWIDTH; i++)
				{
					if(objectdata[i][j].iType >= 11 && objectdata[i][j].iType <= 14)
					{
						objectdata[i][j].iSettings[0] = 1;
					}
				}
			}
		}

		if((version[2] == 0 && version[3] > 1) || version[2] >= 1)
		{
			short translationid[1] = {g_tilesetmanager.GetIndexFromName("Classic")};
			loadPlatforms(mapfile, iReadType == read_type_preview, version, translationid);
		}

		//Read in eyecandy to use
		eyecandyID = (short)ReadInt(mapfile);

		if((version[2] == 0 && version[3] > 0) || version[2] >= 1)
			musicCategoryID = ReadInt(mapfile);
		else
			musicCategoryID = g_iMusicCategoryConversion[backgroundID];

		for(j = 0; j < MAPHEIGHT; j++)
		{
			for(i = 0; i < MAPWIDTH; i++)
			{
				TileType iType = (TileType)ReadInt(mapfile);
				mapdatatop[i][j].iType = iType;
				mapdatatop[i][j].iFlags = g_iTileTypeConversion[iType];

				warpdata[i][j].direction = (short)ReadInt(mapfile);
				warpdata[i][j].connection = (short)ReadInt(mapfile);
				warpdata[i][j].id = (short)ReadInt(mapfile);
				
				for(short iType = 0; iType < 6; iType += 5)
					nospawn[iType][i][j] = ReadInt(mapfile) == 0 ? false : true;

				//Copy player no spawn areas into team no spawn areas
				for(short iType = 1; iType < 5; iType++)
					nospawn[iType][i][j] = nospawn[0][i][j];
			}
		}

		if(iReadType == read_type_preview)
		{
			fclose(mapfile);
			return;
		}

		maxConnection = 0;

		numwarpexits = (short)ReadInt(mapfile);
		for(i = 0; i < numwarpexits && i < MAXWARPS; i++)
		{
			warpexits[i].direction = (short)ReadInt(mapfile);
			warpexits[i].connection = (short)ReadInt(mapfile);
			warpexits[i].id = (short)ReadInt(mapfile);
			warpexits[i].x = (short)ReadInt(mapfile);
			warpexits[i].y = (short)ReadInt(mapfile);

			warpexits[i].lockx = (short)ReadInt(mapfile);
			warpexits[i].locky = (short)ReadInt(mapfile);

			warpexits[i].warpx = (short)ReadInt(mapfile);
			warpexits[i].warpy = (short)ReadInt(mapfile);
			warpexits[i].numblocks = (short)ReadInt(mapfile);

			if(warpexits[i].connection > maxConnection)
				maxConnection = warpexits[i].connection;
		}

		//Ignore any more warps than the max
		for(i = 0; i < numwarpexits - MAXWARPS; i++)
		{
			for(j = 0; j < 10; j++)
				ReadInt(mapfile);
		}

		if(numwarpexits > MAXWARPS)
			numwarpexits = MAXWARPS;

		//Read spawn areas
		for(i = 0; i < 6; i += 5)
		{
			totalspawnsize[i] = 0;
			numspawnareas[i] = (short)ReadInt(mapfile);

			if(numspawnareas[i] > MAXSPAWNAREAS)
			{
				cout << endl << " ERROR: Number of spawn areas (" << numspawnareas[i]
					<< ") was greater than max allowed (" << MAXSPAWNAREAS << ')'
					<< endl;
				return;
			}

			for(int m = 0; m < numspawnareas[i]; m++)
			{
				spawnareas[i][m].left = (short)ReadInt(mapfile);
				spawnareas[i][m].top = (short)ReadInt(mapfile);
				spawnareas[i][m].width = (short)ReadInt(mapfile);
				spawnareas[i][m].height = (short)ReadInt(mapfile);
				spawnareas[i][m].size = (short)ReadInt(mapfile);
			
				totalspawnsize[i] += spawnareas[i][m].size;
			}
		}

		//Copy player spawn areas to team specific spawn areas
		for(short iType = 1; iType < 5; iType++)
		{
			totalspawnsize[iType] = totalspawnsize[0];
			numspawnareas[iType] = numspawnareas[0];

			for(int m = 0; m < numspawnareas[0]; m++)
			{
				spawnareas[iType][m].left = spawnareas[0][m].left;
				spawnareas[iType][m].top = spawnareas[0][m].top;
				spawnareas[iType][m].width = spawnareas[0][m].width;
				spawnareas[iType][m].height = spawnareas[0][m].height;
				spawnareas[iType][m].size = spawnareas[0][m].size;
			}
		}

		//Read draw areas (foreground tiles drawing optimization)
		numdrawareas = (short)ReadInt(mapfile);

		if(numdrawareas > MAXDRAWAREAS)
		{
			cout << endl << " ERROR: Number of draw areas (" << numdrawareas
                << ") was greater than max allowed (" << MAXDRAWAREAS << ')'
                << endl;
			return;
		}

		//Load rects to help optimize drawing the foreground
		for(int m = 0; m < numdrawareas; m++)
		{
			drawareas[m].x = (Sint16)ReadInt(mapfile);
			drawareas[m].y = (Sint16)ReadInt(mapfile);
			drawareas[m].w = (Uint16)ReadInt(mapfile);
			drawareas[m].h = (Uint16)ReadInt(mapfile);
		}

		if(version[2] == 0 && version[3] < 2)
		{
			short translationid[1] = {g_tilesetmanager.GetIndexFromName("Classic")};
			loadPlatforms(mapfile, iReadType == read_type_preview, version, translationid);
		}

		if(version[2] == 0 && version[3] == 0)
		{
			for(short iSwitch = 0; iSwitch < 4; iSwitch++)
			{
				iSwitches[iSwitch] = 1 - (short)ReadInt(mapfile);
			}

			//Set all the on/off blocks correctly
			for(j = 0; j < MAPHEIGHT; j++)
			{
				for(i = 0; i < MAPWIDTH; i++)
				{
					if(objectdata[i][j].iType >= 11 && objectdata[i][j].iType <= 14)
					{
						objectdata[i][j].iSettings[0] = iSwitches[objectdata[i][j].iType - 11];
					}
				}
			}
		}
	}
	else if(version[0] == 1 && version[1] == 6)
	{
		iNumMapItems = 0;
		iNumMapHazards = 0;

		for(short iFilter = 0; iFilter < NUM_AUTO_FILTERS; iFilter++)
			fAutoFilter[iFilter] = false;

		if(iReadType == read_type_summary)
		{
			fclose(mapfile);
			return;
		}

		clearPlatforms();

		cout << "loading map " << file;

        cout << "[Version " << version[0] << '.' << version[1] << '.'
            << version[2] << '.' << version[3] << " Map Detected]\n";

		//2. load map data
		for(j = 0; j < MAPHEIGHT; j++)
		{
			for(i = 0; i < MAPWIDTH; i++)
			{
				for(k = 0; k < MAPLAYERS; k++)
				{
					//This converts from 1.6 tileset to 1.7 tileset
					short iTile = (short)ReadInt(mapfile);

					if(iTile == 300)
					{
						mapdata[i][j][k].iID = TILESETNONE;
					}
					else
					{
						short iTileID = g_iTileConversion[iTile];

						TilesetTile * tile = &mapdata[i][j][k];
						tile->iID = g_tilesetmanager.GetClassicTilesetIndex();
						tile->iCol = iTileID % 32;
						tile->iRow = iTileID / 32;
					}
				}

				mapdatatop[i][j].iType = tile_nonsolid;
				mapdatatop[i][j].iFlags = tile_flag_nonsolid;

				for(k = MAPLAYERS - 1; k >= 0; k--)
				{
					TilesetTile * tile = &mapdata[i][j][k];
					TileType type = g_tilesetmanager.GetClassicTileset()->GetTileType(tile->iCol, tile->iRow);
					if(type != tile_nonsolid)
					{
						mapdatatop[i][j].iType = type;
						mapdatatop[i][j].iFlags = g_iTileTypeConversion[type];
						break;
					}
				}

				objectdata[i][j].iType = (short)ReadInt(mapfile);
				if(objectdata[i][j].iType == 6)
					objectdata[i][j].iType = -1;

				objectdata[i][j].fHidden = false;
				
				if(objectdata[i][j].iType == 1)
				{
					for(short iSetting = 0; iSetting < NUM_BLOCK_SETTINGS; iSetting++)
						objectdata[i][j].iSettings[iSetting] = g_iDefaultPowerupPresets[0][iSetting];
				}

				warpdata[i][j].direction = (short)ReadInt(mapfile);
				warpdata[i][j].connection = (short)ReadInt(mapfile);
				warpdata[i][j].id = (short)ReadInt(mapfile);

				if(version[2] == 1)
				{
					nospawn[0][i][j] = ReadInt(mapfile) == 0 ? false : true;

					for(short iType = 1; iType < NUMSPAWNAREATYPES; iType++)
						nospawn[iType][i][j] = nospawn[0][i][j];
				}
			}
		}

		//Read in background to use
		backgroundID = (short)ReadInt(mapfile);
		musicCategoryID = g_iMusicCategoryConversion[backgroundID];

		strcpy(szBackgroundFile, g_szBackgroundConversion[backgroundID]);

		//Read in eyecandy to use
		eyecandyID = (short)ReadInt(mapfile);
		
		maxConnection = 0;

		numwarpexits = (short)ReadInt(mapfile);
		for(i = 0; i < numwarpexits && i < MAXWARPS; i++)
		{
			warpexits[i].direction = (short)ReadInt(mapfile);
			warpexits[i].connection = (short)ReadInt(mapfile);
			warpexits[i].id = (short)ReadInt(mapfile);
			warpexits[i].x = (short)ReadInt(mapfile);
			warpexits[i].y = (short)ReadInt(mapfile);

			warpexits[i].lockx = (short)ReadInt(mapfile);
			warpexits[i].locky = (short)ReadInt(mapfile);

			warpexits[i].warpx = (short)ReadInt(mapfile);
			warpexits[i].warpy = (short)ReadInt(mapfile);
			warpexits[i].numblocks = (short)ReadInt(mapfile);

			if(warpexits[i].connection > maxConnection)
				maxConnection = warpexits[i].connection;
		}

		//Ignore any more warps than the max
		for(i = 0; i < numwarpexits - MAXWARPS; i++)
		{
			for(j = 0; j < 10; j++)
				ReadInt(mapfile);
		}

		if(numwarpexits > MAXWARPS)
			numwarpexits = MAXWARPS;

		//Read spawn areas
		totalspawnsize[0] = 0;
		numspawnareas[0] = (short)ReadInt(mapfile);

		if(numspawnareas[0] > MAXSPAWNAREAS)
		{
            cout << endl << " ERROR: Number of spawn areas (" << numspawnareas[0]
                 << ") was greater than max allowed (" << MAXSPAWNAREAS << ')'
                 << endl;
			return;
		}

		//Read the only spawn area definition in the file
		for(int m = 0; m < numspawnareas[0]; m++)
		{
			spawnareas[0][m].left = (short)ReadInt(mapfile);
			spawnareas[0][m].top = (short)ReadInt(mapfile);
			spawnareas[0][m].width = (short)ReadInt(mapfile);
			spawnareas[0][m].height = (short)ReadInt(mapfile);
			spawnareas[0][m].size = (short)ReadInt(mapfile);
		
			if(version[2] == 0)
			{
				spawnareas[0][m].width -= spawnareas[0][m].left;
				spawnareas[0][m].height -= spawnareas[0][m].top;
			}

			totalspawnsize[0] += spawnareas[0][m].size;
		}
		
		//Then duplicate it for all the other spawn areas
		for(short i = 1; i < NUMSPAWNAREATYPES; i++)
		{
			totalspawnsize[i] = totalspawnsize[0];
			numspawnareas[i] = numspawnareas[0];

			//Read the only spawn area definition in the file
			for(int m = 0; m < numspawnareas[0]; m++)
			{
				spawnareas[i][m].left = spawnareas[0][m].left;
				spawnareas[i][m].top = spawnareas[0][m].top;
				spawnareas[i][m].width = spawnareas[0][m].width;
				spawnareas[i][m].height = spawnareas[0][m].height;
				spawnareas[i][m].size = spawnareas[0][m].size;
			
				if(version[2] == 0)
				{
					spawnareas[i][m].width -= spawnareas[i][m].left;
					spawnareas[i][m].height -= spawnareas[i][m].top;
				}
			}
		}

		if(version[2] == 1)
		{
			numdrawareas = (short)ReadInt(mapfile);

			if(numdrawareas > MAXDRAWAREAS)
			{
            cout << endl << " ERROR: Number of draw areas (" << numdrawareas
                 << ") was greater than max allowed (" << MAXDRAWAREAS << ')'
                 << endl;
				return;
			}

			for(int m = 0; m < numdrawareas; m++)
			{
				drawareas[m].x = (Sint16)ReadInt(mapfile);
				drawareas[m].y = (Sint16)ReadInt(mapfile);
				drawareas[m].w = (Uint16)ReadInt(mapfile);
				drawareas[m].h = (Uint16)ReadInt(mapfile);
			}
		}

		for(short iSwitch = 0; iSwitch < 4; iSwitch++)
			iSwitches[iSwitch] = 0;

	}
	else //If the version is unrecognized (1.5 maps didn't have version numbers)
	{
		iNumMapItems = 0;
		iNumMapHazards = 0;

		for(short iFilter = 0; iFilter < NUM_AUTO_FILTERS; iFilter++)
			fAutoFilter[iFilter] = false;

		if(iReadType == read_type_summary)
		{
			fclose(mapfile);
			return;
		}

		clearPlatforms();

		cout << "loading map " << file;
		cout << "[Version 1.5 Map Detected]\n";

		//Reset position of read cursor
		rewind(mapfile);

		//clear map (we won't be reading in all the layers so it needs to be cleared)
		clearMap();
		
		//2. load map data
		for(j = 0; j < MAPHEIGHT; j++)
		{
			for(i = 0; i < MAPWIDTH; i++)
			{
				//Read everything into layer 1
				short iTileID = (short)ReadInt(mapfile);

				TilesetTile * tile = &mapdata[i][j][1];
				tile->iID = g_tilesetmanager.GetClassicTilesetIndex();
				tile->iCol = iTileID % 32;
				tile->iRow = iTileID / 32;

				TileType iType = g_tilesetmanager.GetClassicTileset()->GetTileType(tile->iCol, tile->iRow);
				mapdatatop[i][j].iType = iType;
				mapdatatop[i][j].iFlags = g_iTileTypeConversion[iType];

				mapdata[i][j][0].iID = TILESETNONE;
				mapdata[i][j][2].iID = TILESETNONE;
				mapdata[i][j][3].iID = TILESETNONE;
			}
		}

		//3. load objects data
		for(j = 0; j < MAPHEIGHT; j++)
		{
			for(i = 0; i < MAPWIDTH; i++)
			{
				objectdata[i][j].iType = (short)ReadInt(mapfile);
				if(objectdata[i][j].iType == 6)
					objectdata[i][j].iType = -1;

				objectdata[i][j].fHidden = false;
				
				if(objectdata[i][j].iType == 1)
				{
					for(short iSetting = 0; iSetting < NUM_BLOCK_SETTINGS; iSetting++)
						objectdata[i][j].iSettings[iSetting] = g_iDefaultPowerupPresets[0][iSetting];
				}
			}
		}

		backgroundID = (short)ReadInt(mapfile);
		musicCategoryID = g_iMusicCategoryConversion[backgroundID];

		strcpy(szBackgroundFile, g_szBackgroundConversion[backgroundID]);
		
		//All 1.5 maps will use cloud eyecandy
		eyecandyID = 1;

		for(short iSwitch = 0; iSwitch < 4; iSwitch++)
			iSwitches[iSwitch] = 0;
	}

	fclose(mapfile);
	clearWarpLocks();

    cout << " done" << endl;
}

void CMap::UpdateAllTileGaps()
{
	//Detect 1 tile gaps between solid tiles
	for(short j = 0; j < MAPHEIGHT; j++)
	{
		for(short i = 0; i < MAPWIDTH; i++)
		{
			SetTileGap(i, j);
		}
	}
}

void CMap::UpdateTileGap(short i, short j)
{
	for(short iRow = j; iRow <= j + 1; iRow++)
	{
		for(short iCol = i - 1; iCol <= i + 1; iCol++)
		{
			if(iRow >= MAPHEIGHT)
				break;

			short ix = iCol;
			if(ix < 0)
				ix = MAPWIDTH - 1;
			else if(ix >= MAPWIDTH)
				ix = 0;
				
			SetTileGap(ix, iRow);
		}
	}
}

void CMap::SetTileGap(short i, short j)
{
	short iLeftTile = i - 1;
	if(iLeftTile < 0)
		iLeftTile = MAPWIDTH - 1;

	short iRightTile = i + 1;
	if(iRightTile >= MAPWIDTH)
		iRightTile = 0;

	int topLeftTile = 0;
	int topCenterTile = 0;
	int topRightTile = 0;

	IO_Block * topLeftBlock = NULL;
	IO_Block * topCenterBlock = NULL;
	IO_Block * topRightBlock = NULL;

	if(j > 0)
	{
		topLeftTile = mapdatatop[iLeftTile][j - 1].iFlags;
		topCenterTile = mapdatatop[i][j - 1].iFlags;
		topRightTile = mapdatatop[iRightTile][j - 1].iFlags;

		topLeftBlock = blockdata[iLeftTile][j - 1];
		topCenterBlock = blockdata[i][j - 1];
		topRightBlock = blockdata[iRightTile][j - 1];
	}

	int leftTile = mapdatatop[iLeftTile][j].iFlags;
	int centerTile = mapdatatop[i][j].iFlags;
	int rightTile = mapdatatop[iRightTile][j].iFlags;

	IO_Block * leftBlock = blockdata[iLeftTile][j];
	IO_Block * centerBlock = blockdata[i][j];
	IO_Block * rightBlock = blockdata[iRightTile][j];

	bool fLeftSolid = (leftTile != tile_flag_nonsolid && leftTile != tile_flag_gap) || (leftBlock && !leftBlock->isTransparent() && !leftBlock->isHidden());
	bool fCenterSolid = centerTile != tile_flag_nonsolid || (centerBlock && !centerBlock->isTransparent() && !centerBlock->isHidden());
	bool fRightSolid = (rightTile != tile_flag_nonsolid && rightTile != tile_flag_gap) || (rightBlock && !rightBlock->isTransparent() && !rightBlock->isHidden());

	bool fTopLeftSolid = (topLeftTile & tile_flag_solid) || (topLeftBlock && !topLeftBlock->isTransparent() && !topLeftBlock->isHidden());
	bool fTopCenterSolid = (topCenterTile & tile_flag_solid) || (topCenterBlock && !topCenterBlock->isTransparent() && !topCenterBlock->isHidden());
	bool fTopRightSolid = (topRightTile & tile_flag_solid) || (topRightBlock && !topRightBlock->isTransparent() && !topRightBlock->isHidden());

	if(fLeftSolid && !fCenterSolid && fRightSolid && !fTopLeftSolid && !fTopCenterSolid && !fTopRightSolid)
	{
		mapdatatop[i][j].iType = tile_gap;
		mapdatatop[i][j].iFlags = tile_flag_gap;
	}
	else if(mapdatatop[i][j].iFlags == tile_flag_gap)
	{
		mapdatatop[i][j].iType = tile_nonsolid;
		mapdatatop[i][j].iFlags = tile_flag_nonsolid;
	}
}

void CMap::loadPlatforms(FILE * mapfile, bool fPreview, int version[4], short * translationid)
{
	clearPlatforms();

	//Load moving platforms
	iNumPlatforms = (short)ReadInt(mapfile);
	platforms = new MovingPlatform*[iNumPlatforms];

	for(short iPlatform = 0; iPlatform < iNumPlatforms; iPlatform++)
	{
		short iWidth = (short)ReadInt(mapfile);
		short iHeight = (short)ReadInt(mapfile);

		TilesetTile ** tiles = new TilesetTile*[iWidth];
		MapTile ** types = new MapTile*[iWidth];

		for(short iCol = 0; iCol < iWidth; iCol++)
		{
			tiles[iCol] = new TilesetTile[iHeight];
			types[iCol] = new MapTile[iHeight];

			for(short iRow = 0; iRow < iHeight; iRow++)
			{
				TilesetTile * tile = &tiles[iCol][iRow];

				if(version[0] > 1 || (version[0] == 1 && version[1] >= 8))
				{
					tile->iID = ReadByteAsShort(mapfile);

					//Convert tileset ids into the current game's tileset's ids
					if(tile->iID >= 0)
						tile->iID = translationid[tile->iID];

					tile->iCol = ReadByteAsShort(mapfile);
					tile->iRow = ReadByteAsShort(mapfile);

					TileType iType = (TileType)ReadInt(mapfile);

					if(iType >= 0 && iType <= 12)
					{
						types[iCol][iRow].iType = iType;
						types[iCol][iRow].iFlags = g_iTileTypeConversion[iType];
					}
					else
					{
						types[iCol][iRow].iType = tile_nonsolid;
						types[iCol][iRow].iFlags = tile_flag_nonsolid;
					}
				}
				else
				{
					short iTile = ReadInt(mapfile);
					TileType type;

					if(iTile == TILESETSIZE)
					{
						tile->iID = TILESETNONE;
						tile->iCol = 0;
						tile->iRow = 0;

						type = tile_nonsolid;
					}
					else
					{
						tile->iID = g_tilesetmanager.GetClassicTilesetIndex();
						tile->iCol = iTile % TILESETWIDTH;
						tile->iRow = iTile / TILESETWIDTH;

						type = g_tilesetmanager.GetClassicTileset()->GetTileType(tile->iCol, tile->iRow);
					}
	
					types[iCol][iRow].iType = type;
					types[iCol][iRow].iFlags = g_iTileTypeConversion[type];
				}
			}
		}
	
		short iPathType = 0;
		
		if(version[0] > 1 || (version[0] == 1 && version[1] >= 8))
			iPathType = ReadInt(mapfile);

		MovingPlatformPath * path = NULL;
		if(iPathType == 0) //segment path
		{
			float fStartX = ReadFloat(mapfile);
			float fStartY = ReadFloat(mapfile);
			float fEndX = ReadFloat(mapfile);
			float fEndY = ReadFloat(mapfile);
			float fVelocity = ReadFloat(mapfile);

			path = new StraightPath(fVelocity, fStartX, fStartY, fEndX, fEndY, fPreview);
		}
		else if(iPathType == 1) //continuous path
		{
			float fStartX = ReadFloat(mapfile);
			float fStartY = ReadFloat(mapfile);
			float fAngle = ReadFloat(mapfile);
			float fVelocity = ReadFloat(mapfile);

			path = new StraightPathContinuous(fVelocity, fStartX, fStartY, fAngle, fPreview);
		}
		else if(iPathType == 2) //elliptical path
		{
			float fRadiusX = ReadFloat(mapfile);
			float fRadiusY = ReadFloat(mapfile);
			float fCenterX = ReadFloat(mapfile);
			float fCenterY = ReadFloat(mapfile);
			float fAngle = ReadFloat(mapfile);
			float fVelocity = ReadFloat(mapfile);

			path = new EllipsePath(fVelocity, fAngle, fRadiusX, fRadiusY, fCenterX, fCenterY, fPreview);
		}

		platforms[iPlatform] = new MovingPlatform(tiles, types, iWidth, iHeight, path, fPreview);
	}
}

void CMap::saveMap(const std::string& file)
{
	FILE *mapfile;
	int i, j, k;

    cout << "saving map " << file << " ... ";

	mapfile = fopen(file, "wb");
	if(mapfile == NULL)
	{
        cout << endl << " ERROR: couldn't save map" << endl;
		return;
	}

	//First write the map compatibility version number 
	//(this will allow the map loader to identify if the map needs conversion)
	WriteInt(g_iVersion[0], mapfile); //Major
	WriteInt(g_iVersion[1], mapfile); //Minor
	WriteInt(g_iVersion[2], mapfile); //Micro
	WriteInt(g_iVersion[3], mapfile); //Build

	//Calculate warp zones
	bool usedtile[MAPWIDTH][MAPHEIGHT];
	for(j = 0; j < MAPHEIGHT; j++)
		for(i = 0; i < MAPWIDTH; i++)
			usedtile[i][j] = false;

	//bool fAutoMapFilters[NUM_AUTO_FILTERS];
	//for(short iFilter = 0; iFilter < NUM_AUTO_FILTERS; iFilter++)
		//fAutoMapFilters[iFilter] = false;

	int iHazardCount = 0;
	int iWarpCount = 0;
	int iIceCount = 0;
	int iPowerupBlockCount = 0;
	int iPlatformCount = 0;
	int iDensity = 0;
	int iOnOffBlockCount = 0;
	int iThrowBlockCount = 0;
	int iBreakableBlockCount = 0;
	int iItemDestroyableBlockCount = 0;
	int iHiddenBlockCount = 0;
	
	for(short iPlatform = 0; iPlatform < iNumPlatforms; iPlatform++)
	{
		for(short iCol = 0; iCol < platforms[iPlatform]->iTileWidth; iCol++)
		{
			for(short iRow = 0; iRow < platforms[iPlatform]->iTileHeight; iRow++)	
			{
				//Set the tile type flags for each tile
				int iType = platforms[iPlatform]->iTileType[iCol][iRow].iType;
				if(iType >= 0 && iType <= 12)
				{
					platforms[iPlatform]->iTileType[iCol][iRow].iFlags = g_iTileTypeConversion[iType];
				}
				else
				{
					platforms[iPlatform]->iTileType[iCol][iRow].iType = tile_nonsolid;
					platforms[iPlatform]->iTileType[iCol][iRow].iFlags = tile_flag_nonsolid;
				}

				TilesetTile * tile = &platforms[iPlatform]->iTileData[iCol][iRow];
				int iFlags = platforms[iPlatform]->iTileType[iCol][iRow].iFlags;

				if(tile->iID != TILESETNONE)
					iPlatformCount++;

				if(iFlags & tile_flag_has_death)
					iHazardCount++;

				if(iFlags & tile_flag_ice)
					iIceCount++;
			}
		}
	}

	//Examine tiles to calculate map summary data (i.e. presence of death blocks, powerups, ice, switches, etc.)
	short numWarpExits = 0;
	for(j = 0; j < MAPHEIGHT; j++)
	{
		for(i = 0; i < MAPWIDTH; i++)
		{
			//Set the tile type flags for each tile
			int iType = mapdatatop[i][j].iType;
			if(iType >= 0 && iType <= 12)
			{
				mapdatatop[i][j].iFlags = g_iTileTypeConversion[iType];
			}
			else
			{
				mapdatatop[i][j].iType = tile_nonsolid;
				mapdatatop[i][j].iFlags = tile_flag_nonsolid;
			}

			//Calculate what warp tiles belong together (any warps that have the same connection that are 
			//next to each other are merged into a single warp)
			//If there are too many warps, then remove any warp encountered that is over that limit
			if(warpdata[i][j].connection != -1 && !usedtile[i][j])
			{
				int movex = 0, movey = 0;
				int currentx = i, currenty = j;

				if(warpdata[i][j].direction == 0 || warpdata[i][j].direction == 2)
					movex = 1;
				else
					movey = 1;

				while(currentx < MAPWIDTH && currenty < MAPHEIGHT)
				{
					if(warpdata[currentx][currenty].direction != warpdata[i][j].direction ||
						warpdata[currentx][currenty].connection != warpdata[i][j].connection)
					{
						break;
					}

					usedtile[currentx][currenty] = true;
					
					if(numWarpExits < MAXWARPS)
					{
						warpdata[currentx][currenty].id = numWarpExits;
					}
					else
					{
						warpdata[currentx][currenty].connection = -1;
						warpdata[currentx][currenty].direction = -1;
					}

					currentx += movex;
					currenty += movey;
				}

				numWarpExits++;
			}

			short iBlockType = objectdata[i][j].iType;
			int iFlags = mapdatatop[i][j].iFlags;

			//Calculate auto map filters
			if(iFlags & tile_flag_has_death)
				iHazardCount++;

			if(warpdata[i][j].connection != -1)
				iWarpCount++;

			if(iFlags & tile_flag_ice)
				iIceCount++;

			if(iBlockType == 1 || iBlockType == 15) //Powerup/View Block
				iPowerupBlockCount++;

			if(iBlockType == 0) //Breakable Block
				iBreakableBlockCount++;
			
			if(iBlockType == 6 || iBlockType == 16) //Blue/Red Throw Block
				iThrowBlockCount++;

			if(iBlockType >= 11 && iBlockType <= 14) //On/Off Block
				iOnOffBlockCount++;

			if(iFlags & tile_flag_solid)
				iDensity++;

			if((iBlockType == 1 || iBlockType == 3 || iBlockType == 4 || iBlockType == 5 || iBlockType == 15 || iBlockType == 17 || iBlockType == 18) && objectdata[i][j].fHidden) //Hidden blocks
				iHiddenBlockCount++;

			if(iBlockType >= 20 && iBlockType <= 29) //Item Destroyable Blocks
				iItemDestroyableBlockCount++;
		}
	}

	//Save summary info for auto map filters (hazards, ice, warps, platforms, bonus blocks, density, etc)
	WriteInt(iHazardCount, mapfile);
	WriteInt(iWarpCount, mapfile);
	WriteInt(iIceCount, mapfile);
	WriteInt(iPowerupBlockCount, mapfile);
	WriteInt(iBreakableBlockCount, mapfile);
	WriteInt(iThrowBlockCount, mapfile);
	WriteInt(iOnOffBlockCount, mapfile);
	WriteInt(iPlatformCount, mapfile);
	WriteInt(iNumMapHazards, mapfile);
	WriteInt(iItemDestroyableBlockCount, mapfile);
	WriteInt(iHiddenBlockCount, mapfile);
	WriteInt(iNumMapItems, mapfile);
	WriteInt(iDensity, mapfile);

	//Write tileset names and indexes for translation at load time
	//Number of tilesets used by this map
	short iTilesetCount = g_tilesetmanager.GetCount();
	bool * fTilesetUsed = new bool[iTilesetCount];
	for(short iTileset = 0; iTileset < iTilesetCount; iTileset++)
		fTilesetUsed[iTileset] = false;

	//Scan map to see how many tilesets were used
	for(j = 0; j < MAPHEIGHT; j++)
	{
		for(i = 0; i < MAPWIDTH; i++)
		{
			for(k = 0; k < MAPLAYERS; k++)
			{
				if(mapdata[i][j][k].iID >= 0)
					fTilesetUsed[mapdata[i][j][k].iID] = true;
			}
		}
	}

	//Scan platforms too for tilesets used
	for(short iPlatform = 0; iPlatform < iNumPlatforms; iPlatform++)
	{
		for(short iCol = 0; iCol < platforms[iPlatform]->iTileWidth; iCol++)
		{
			for(short iRow = 0; iRow < platforms[iPlatform]->iTileHeight; iRow++)	
			{
				TilesetTile * tile = &platforms[iPlatform]->iTileData[iCol][iRow];

				if(tile->iID >= 0)
					fTilesetUsed[tile->iID] = true;
			}
		}
	}

	short iUsedTilesets = 0;
	for(short iTileset = 0; iTileset < iTilesetCount; iTileset++)
	{
		if(fTilesetUsed[iTileset])
			iUsedTilesets++;
	}

	WriteInt(iUsedTilesets, mapfile);

	//Write each of the tileset names with the index that will be used by this mapfile to represent that tileset
	for(short iTileset = 0; iTileset < iTilesetCount; iTileset++)
	{
		if(fTilesetUsed[iTileset])
		{
			//Tileset ID
			WriteInt(iTileset, mapfile);

			//Tileset Name
			WriteString(g_tilesetmanager.GetTileset(iTileset)->GetName(), mapfile);
		}
	}

	delete [] fTilesetUsed;

	//save map tiles and blocks
	for(j = 0; j < MAPHEIGHT; j++)
	{
		for(i = 0; i < MAPWIDTH; i++)
		{
			for(k = 0; k < MAPLAYERS; k++)
			{
				//Tile sprites (4 layers)
				//WriteInt(mapdata[i][j][k], mapfile);

				TilesetTile * tile = &mapdata[i][j][k];

				//Make sure the tile's col and row are within the tileset
				if(tile->iID >= 0)
				{
					if(tile->iCol < 0 || tile->iCol >= g_tilesetmanager.GetTileset(tile->iID)->GetWidth())
						tile->iCol = 0;

					if(tile->iRow < 0 || tile->iRow >= g_tilesetmanager.GetTileset(tile->iID)->GetHeight())
						tile->iRow = 0;
				}

				WriteByteFromShort(tile->iID, mapfile);
				WriteByteFromShort(tile->iCol, mapfile);
				WriteByteFromShort(tile->iRow, mapfile);
			}
			
			//Interaction blocks
			WriteByteFromShort(objectdata[i][j].iType, mapfile);
			WriteBool(objectdata[i][j].fHidden, mapfile);
		}
	}

	//Write background File
	WriteString(szBackgroundFile, mapfile);

	//Save the default on/off switch states
	for(short iSwitch = 0; iSwitch < 4; iSwitch++)
		WriteInt(iSwitches[iSwitch], mapfile);

	//Write moving platforms
	WriteInt(iNumPlatforms, mapfile);

	for(short iPlatform = 0; iPlatform < iNumPlatforms; iPlatform++)
	{
		WriteInt(platforms[iPlatform]->iTileWidth, mapfile);
		WriteInt(platforms[iPlatform]->iTileHeight, mapfile);

		for(short iCol = 0; iCol < platforms[iPlatform]->iTileWidth; iCol++)
		{
			for(short iRow = 0; iRow < platforms[iPlatform]->iTileHeight; iRow++)	
			{
				TilesetTile * tile = &platforms[iPlatform]->iTileData[iCol][iRow];
				
				//Make sure the tile's col and row are within the tileset
				if(tile->iID >= 0)
				{
					if(tile->iCol < 0 || tile->iCol >= g_tilesetmanager.GetTileset(tile->iID)->GetWidth())
						tile->iCol = 0;

					if(tile->iRow < 0 || tile->iRow >= g_tilesetmanager.GetTileset(tile->iID)->GetHeight())
						tile->iRow = 0;
				}

				WriteByteFromShort(tile->iID, mapfile);
				WriteByteFromShort(tile->iCol, mapfile);
				WriteByteFromShort(tile->iRow, mapfile);
				
				WriteInt(platforms[iPlatform]->iTileType[iCol][iRow].iType, mapfile);
			}
		}
		
		short iPathType = platforms[iPlatform]->pPath->iType;
		WriteInt(iPathType, mapfile);

		if(iPathType == 0)
		{
			StraightPath * path = (StraightPath*)platforms[iPlatform]->pPath;
			WriteFloat(path->dPathPointX[0], mapfile);
			WriteFloat(path->dPathPointY[0], mapfile);
			WriteFloat(path->dPathPointX[1], mapfile);
			WriteFloat(path->dPathPointY[1], mapfile);
			WriteFloat(path->dVelocity, mapfile);
		}
		else if(iPathType == 1)
		{
			StraightPathContinuous * path = (StraightPathContinuous*)platforms[iPlatform]->pPath;
			WriteFloat(path->dPathPointX[0], mapfile);
			WriteFloat(path->dPathPointY[0], mapfile);
			WriteFloat(path->dAngle, mapfile);
			WriteFloat(path->dVelocity, mapfile);
		}
		else if(iPathType == 2) //elliptical path
		{
			EllipsePath * path = (EllipsePath*)platforms[iPlatform]->pPath;
			WriteFloat(path->dRadiusX, mapfile);
			WriteFloat(path->dRadiusY, mapfile);
			WriteFloat(path->dPathPointX[0], mapfile);
			WriteFloat(path->dPathPointY[0], mapfile);
			WriteFloat(path->dAngle[0], mapfile);
			WriteFloat(path->dVelocity, mapfile);
		}
	}

	//Write map items (carried springs, spikes, kuribo's shoe, etc)
	WriteInt(iNumMapItems, mapfile);
	
	for(short iMapItem = 0; iMapItem < iNumMapItems; iMapItem++)
	{
		WriteInt(mapitems[iMapItem].itype, mapfile);
		WriteInt(mapitems[iMapItem].ix, mapfile);  //tile aligned
		WriteInt(mapitems[iMapItem].iy, mapfile);
	}

	//Write map hazards (fireball strings, rotodiscs, pirhana plants, etc)
	WriteInt(iNumMapHazards, mapfile);

	for(short iMapHazard = 0; iMapHazard < iNumMapHazards; iMapHazard++)
	{
		WriteInt(maphazards[iMapHazard].itype, mapfile);
		WriteInt(maphazards[iMapHazard].ix, mapfile);
		WriteInt(maphazards[iMapHazard].iy, mapfile);

		for(short iParam = 0; iParam < NUMMAPHAZARDPARAMS; iParam++)
			WriteInt(maphazards[iMapHazard].iparam[iParam], mapfile);
		
		for(short iParam = 0; iParam < NUMMAPHAZARDPARAMS; iParam++)
			WriteFloat(maphazards[iMapHazard].dparam[iParam], mapfile);
	}

	//Write eyecandy ID
	WriteInt(eyecandyID, mapfile);

	//Write music category
	WriteInt(musicCategoryID, mapfile);

	//Write the rest of the map data later so that we can just load part of the map for the preview
	for(j = 0; j < MAPHEIGHT; j++)
	{
		for(i = 0; i < MAPWIDTH; i++)
		{			
			//Write tile collision types (ice, solid, death, etc.)
			WriteInt(mapdatatop[i][j].iType, mapfile);
			
			//Write per tile warp data
			WriteInt(warpdata[i][j].direction, mapfile);
			WriteInt(warpdata[i][j].connection, mapfile);
			WriteInt(warpdata[i][j].id, mapfile);
			
			//Write per tile allowed spawn types (player, team specific (1-4), item)
			for(short iType = 0; iType < NUMSPAWNAREATYPES; iType++)
				WriteBool(nospawn[iType][i][j], mapfile);
		}
	}

	//Count blocks that have supplement data (like powerup weights for powerup blocks or state for switched blocks)
	short iBlockCount = 0;
	short iSwitchBlockCount = 0;
	for(j = 0; j < MAPHEIGHT; j++)
	{
		for(i = 0; i < MAPWIDTH; i++)
		{
			//powerup block
			if(objectdata[i][j].iType == 1 || objectdata[i][j].iType == 15)
				iBlockCount++;

			//switched blocks
			if(objectdata[i][j].iType >= 11 && objectdata[i][j].iType <= 14)
				iSwitchBlockCount++;
		}
	}

	//Write out the switch block state
	WriteInt(iSwitchBlockCount, mapfile);
	for(j = 0; j < MAPHEIGHT; j++)
	{
		for(i = 0; i < MAPWIDTH; i++)
		{
			if(objectdata[i][j].iType >= 11 && objectdata[i][j].iType <= 14)
			{
				WriteByteFromShort(i, mapfile);
				WriteByteFromShort(j, mapfile);
				WriteByteFromShort(objectdata[i][j].iSettings[0], mapfile);
				
				//TODO: REmove this and comment in the LINE ABOVE
				//WriteByteFromShort(iSwitches[objectdata[i][j].iType - 11], mapfile);
			}
		}
	}

	//Write number of warp exits
	WriteInt(numWarpExits, mapfile);

	for(j = 0; j < MAPHEIGHT; j++)
		for(i = 0; i < MAPWIDTH; i++)
			usedtile[i][j] = false;

	//scan warp data and write out warp exits
	for(j = 0; j < MAPHEIGHT; j++)
	{
		for(i = 0; i < MAPWIDTH; i++)
		{
			//if we found a warp area and it hasn't been consumed by another warp area
			//then scan for surrounding warp areas and consume them to create a warp exit
			if(warpdata[i][j].connection != -1 && !usedtile[i][j])
			{
				int movex = 0, movey = 0;
				int currentx = i, currenty = j;

				if(warpdata[i][j].direction == 0 || warpdata[i][j].direction == 2)
					movex = 1;
				else
					movey = 1;


				int numblocks = 0;
				while(currentx < MAPWIDTH && currenty < MAPHEIGHT)
				{
					if(warpdata[currentx][currenty].direction != warpdata[i][j].direction ||
						warpdata[currentx][currenty].connection != warpdata[i][j].connection)
					{
						break;
					}
					
					usedtile[currentx][currenty] = true;

					currentx += movex;
					currenty += movey;
					numblocks++;
				}

				//We're on the next block that isn't a warp
				currentx -= movex;
				currenty -= movey;

				WriteInt(warpdata[i][j].direction, mapfile);
				WriteInt(warpdata[i][j].connection, mapfile);
				WriteInt(warpdata[i][j].id, mapfile);

				//Write out warp exit x,y position for player and position for lock icon to display
				if(warpdata[i][j].direction == 0)
				{
					WriteInt(((currentx * TILESIZE + TILESIZE - i * TILESIZE) >> 1) + i * TILESIZE - HALFPW, mapfile);
					WriteInt(j * TILESIZE - 1 + PHOFFSET, mapfile);

					WriteInt(((currentx * TILESIZE + TILESIZE - i * TILESIZE) >> 1) + i * TILESIZE - 16, mapfile);
					WriteInt(j * TILESIZE, mapfile);
				}
				else if(warpdata[i][j].direction == 2)
				{
					WriteInt(((currentx * TILESIZE + TILESIZE - i * TILESIZE) >> 1) + i * TILESIZE - HALFPW, mapfile);
					WriteInt(j * TILESIZE + 1 + PHOFFSET, mapfile);

					WriteInt(((currentx * TILESIZE + TILESIZE - i * TILESIZE) >> 1) + i * TILESIZE - 16, mapfile);
					WriteInt(j * TILESIZE, mapfile);
				}
				else if(warpdata[i][j].direction == 1)
				{
					WriteInt(i * TILESIZE + TILESIZE - PW - PWOFFSET, mapfile);
					WriteInt(currenty * TILESIZE + TILESIZE - PH - 1, mapfile);

					WriteInt(i * TILESIZE, mapfile);
					WriteInt(((currenty * TILESIZE + TILESIZE - j * TILESIZE) >> 1) + j * TILESIZE - 16, mapfile);
				}
				else if(warpdata[i][j].direction == 3)
				{
					WriteInt(i * TILESIZE - 1 + PWOFFSET, mapfile);
					WriteInt(currenty * TILESIZE + TILESIZE - PH - 1, mapfile);

					WriteInt(i * TILESIZE, mapfile);
					WriteInt(((currenty * TILESIZE + TILESIZE - j * TILESIZE) >> 1) + j * TILESIZE - 16, mapfile);
				}

				WriteInt(i, mapfile);
				WriteInt(j, mapfile);
				WriteInt(numblocks, mapfile);

			}
		}
	}

	//Calculate player/team spawn zones
	for(short iType = 0; iType < 5; iType++)
	{
		calculatespawnareas(iType, false);
		
		if(numspawnareas[iType] == 0)
			calculatespawnareas(iType, true);
	}

	//Calculate item spawn zones
	calculatespawnareas(5, false);

	//Write spawn areas
	for(i = 0; i < NUMSPAWNAREATYPES; i++)
	{
		WriteInt(numspawnareas[i], mapfile);

		for(int m = 0; m < numspawnareas[i]; m++)
		{
			WriteInt(spawnareas[i][m].left, mapfile);
			WriteInt(spawnareas[i][m].top, mapfile);
			WriteInt(spawnareas[i][m].width, mapfile);
			WriteInt(spawnareas[i][m].height, mapfile);
			WriteInt(spawnareas[i][m].size, mapfile);
		}
	}
	
	for(j = 0; j < MAPHEIGHT; j++)
		for(i = 0; i < MAPWIDTH; i++)
			usedtile[i][j] = false;

	//Figure out areas that contain tiles in layers 2 and 3
	for(j = 0; j < MAPHEIGHT; j++)
	{
		for(i = 0; i < MAPWIDTH; i++)
		{
			if(mapdata[i][j][2].iID <= TILESETNONE && mapdata[i][j][3].iID <= TILESETNONE)
				usedtile[i][j] = true;
		}
	}

	//Figure out where the overlayer draw rects are
	numdrawareas = 0;
	for(j = 0; j < MAPHEIGHT; j++)
	{
		for(i = 0; i < MAPWIDTH; i++)
		{
			if(!usedtile[i][j])
			{
				bool fDownDone = false;
				bool fRightDone = false;

				int downsize = j + 1;
				int rightsize = i + 1;
				int attempt = 1;

				while(true)
				{
					if(!fRightDone)
					{
						for(int right = i; right < rightsize; right++)
						{
							if(right >= MAPWIDTH || j + attempt >= MAPHEIGHT || usedtile[right][j + attempt])
								fRightDone = true;
						}

						if(!fRightDone)
							downsize++;
					}

					if(!fDownDone)
					{
						for(int down = j; down < downsize; down++)
						{
							if(i + attempt >= MAPWIDTH || down >= MAPHEIGHT || usedtile[i + attempt][down])
								fDownDone = true;
						}

						if(!fDownDone)
							rightsize++;
					}

					if(fDownDone && fRightDone)
					{
						if(numdrawareas < MAXDRAWAREAS)
						{
							drawareas[numdrawareas].x = (Sint16)(i * TILESIZE);
							drawareas[numdrawareas].y = (Sint16)(j * TILESIZE);
							drawareas[numdrawareas].w = (Uint16)((rightsize - i) * TILESIZE);
							drawareas[numdrawareas].h = (Uint16)((downsize - j) * TILESIZE);

							numdrawareas++;
						}

						for(int down = j; down < downsize; down++)
						{
							for(int right = i; right < rightsize; right++)
							{
								usedtile[right][down] = true;
							}
						}
						
						break;
					}

					attempt++;
				}
			}
		}
	}

	//Write draw areas
	WriteInt(numdrawareas, mapfile);

	for(int m = 0; m < numdrawareas; m++)
	{
		WriteInt(drawareas[m].x, mapfile);
		WriteInt(drawareas[m].y, mapfile);
		WriteInt(drawareas[m].w, mapfile);
		WriteInt(drawareas[m].h, mapfile);
	}

	//Write the number of blocks we have supplement info for
	WriteInt(iBlockCount, mapfile);

	for(j = 0; j < MAPHEIGHT; j++)
	{
		for(i = 0; i < MAPWIDTH; i++)
		{
			if(objectdata[i][j].iType == 1 || objectdata[i][j].iType == 15) //powerup or view block
			{
				WriteByteFromShort(i, mapfile);
				WriteByteFromShort(j, mapfile);
				
				WriteByteFromShort(NUM_BLOCK_SETTINGS, mapfile);
				for(short iSetting = 0; iSetting < NUM_BLOCK_SETTINGS; iSetting++)
					WriteByteFromShort(objectdata[i][j].iSettings[iSetting], mapfile);
			}
		}
	}

	//Write mode item locations like flags and race goals
	WriteInt(iNumRaceGoals, mapfile);
	for(j = 0; j < iNumRaceGoals; j++)
	{
		WriteInt(racegoallocations[j].x, mapfile);
		WriteInt(racegoallocations[j].y, mapfile);
	}

	WriteInt(iNumFlagBases, mapfile);
	for(j = 0; j < iNumFlagBases; j++)
	{
		WriteInt(flagbaselocations[j].x, mapfile);
		WriteInt(flagbaselocations[j].y, mapfile);
	}

	fclose(mapfile);

    cout << "done" << endl;

/*
	//Save thumbnail
	char szSaveThumbnail[256];
	strcpy(szSaveThumbnail, "maps/cache/");
	char * pszSaveThumbnail = szSaveThumbnail + strlen(szSaveThumbnail);
	GetNameFromFileName(pszSaveThumbnail, file.c_str());

#ifdef PNG_SAVE_FORMAT	
	strcat(szSaveThumbnail, ".png");
#else
	strcat(szSaveThumbnail, ".bmp");
#endif

	saveThumbnail(convertPath(szSaveThumbnail), true);
*/
}

//Save thumbnail image
void CMap::saveThumbnail(const std::string &sFile, bool fUseClassicPack)
{
	SDL_Surface * sThumbnail = SDL_CreateRGBSurface(screen->flags, 160, 120, 16, 0, 0, 0, 0);

	char szBackgroundFile[128];
	std::string path;

	if(fUseClassicPack)
	{
		sprintf(szBackgroundFile, "gfx/packs/Classic/backgrounds/%s", g_map.szBackgroundFile);
		path = convertPath(szBackgroundFile);

		//if the background file doesn't exist, use the classic background
		if(!File_Exists(path))
			path = convertPath("gfx/packs/Classic/backgrounds/Land_Classic.png");
	}
	else
	{
		sprintf(szBackgroundFile, "gfx/packs/backgrounds/%s", g_map.szBackgroundFile);
		path = convertPath(szBackgroundFile, gamegraphicspacklist.current_name());

		//if the background file doesn't exist, use the classic background
		if(!File_Exists(path))
			path = convertPath("gfx/packs/backgrounds/Land_Classic.png", gamegraphicspacklist.current_name());
	}

	SDL_Surface * temp = IMG_Load(path.c_str());

	SDL_Surface * sBackground = SDL_DisplayFormat(temp);
	if(!sBackground)
	{
		printf("ERROR: Couldn't convert thumbnail background to diplay pixel format: %s\n", SDL_GetError());
		return;
	}

	SDL_FreeSurface(temp);

	SDL_Rect srcRectBackground = {0, 0, 640, 480};
	SDL_Rect dstRectBackground = {0, 0, 160, 120};

	if(SDL_SoftStretch(sBackground, &srcRectBackground, sThumbnail, &dstRectBackground) < 0)
	{
		fprintf(stderr, "SDL_SoftStretch error: %s\n", SDL_GetError());
		return;
	}

	SDL_FreeSurface(sBackground);

	preDrawPreviewBackground(sThumbnail, true);
	preDrawPreviewMapItems(sThumbnail, true);
	drawThumbnailHazards(sThumbnail);
	drawThumbnailPlatforms(sThumbnail);
	preDrawPreviewForeground(sThumbnail, true);
	preDrawPreviewWarps(sThumbnail, true);

	//Save the screenshot with the same name as the map file

#ifdef PNG_SAVE_FORMAT
	IMG_SavePNG(sThumbnail, sFile.c_str());
#else
	SDL_SaveBMP(sThumbnail, sFile.c_str());
#endif

}

void CMap::calculatespawnareas(short iType, bool fUseTempBlocks)
{
	bool usedtile[MAPWIDTH][MAPHEIGHT];
	short i, j;

	for(j = 0; j < MAPHEIGHT; j++)
		for(i = 0; i < MAPWIDTH; i++)
			usedtile[i][j] = false;

	//Figure out areas where spawning is allowed
	for(j = 0; j < MAPHEIGHT; j++)
	{
		for(i = 0; i < MAPWIDTH; i++)
		{
			bool fUsed = false;

			if(j >= 13 || j == 0)
				fUsed = true;

			if(!fUsed)
			{
				if(nospawn[iType][i][j])
					fUsed = true;
			}

			if(!fUsed && (mapdatatop[i][j].iFlags & tile_flag_solid))
			{
				fUsed = true;
			}
			
			//If there is a block there
			if(!fUsed)
			{
				if(objectdata[i][j].iType != -1)
					fUsed = true;
			}

			//If this is a player spawn area, have extra restrictions like don't spawn over spikes etc.
			if(0 <= iType && 4 >= iType)
			{
				//If there is a death tile directly above
				if(!fUsed)
				{
					if(j > 0)
					{
						if(mapdatatop[i][j - 1].iFlags & tile_flag_death_on_bottom)
						{
							fUsed = true;
						}
					}
				}

				//If there is a death tile anywhere below this tile
				if(!fUsed)
				{
					int m;
					for(m = j; m < MAPHEIGHT; m++)
					{
						TileType type = mapdatatop[i][m].iType;
						int flags = mapdatatop[i][m].iFlags;
						short block = objectdata[i][m].iType;

						if(m == j && (flags & tile_flag_solid_on_top))
							continue;

						if(type == tile_death_on_top || type == tile_death)
						{
							fUsed = true;
							break;
						}
						
						if(fUseTempBlocks)
						{
							if((type != tile_nonsolid && type != tile_gap) || block != -1)
							{
								break;
							}
						}
						else
						{
							//Ignore the blocks that might not be there anymore (destroyed, turned off, etc)
							if((type != tile_nonsolid && type != tile_gap) || (block != -1 && block != 0 && block != 2 && block != 6 && (block < 11 || block > 14) && block != 16 && block < 19))
							{
								break;
							}
						}
					}

					//If we didn't find a landing spot from here to bottom, then try to wrap around and see
					if(m == MAPHEIGHT)
					{
						for(m = 0; m < j; m++)
						{
							TileType type = mapdatatop[i][m].iType;
							short block = objectdata[i][m].iType;

							if(type == tile_death_on_top || type == tile_death)
							{
								fUsed = true;
								break;
							}
							
							if(fUseTempBlocks)
							{
								if((type != tile_nonsolid && type != tile_gap) || block != -1)
								{
									break;
								}
							}
							else
							{
								if((type != tile_nonsolid && type != tile_gap) || (block != -1 && block != 0 && block != 2 && block != 6 && (block < 11 || block > 14) && block != 16 && block < 19))
								{
									break;
								}
							}
						}

						//if(m == j)
							//fUsed = true;
					}
				}
			}

			usedtile[i][j] = fUsed;
		}
	}

	//Figure out where the spawn areas are
	numspawnareas[iType] = 0;
	for(j = 0; j < MAPHEIGHT; j++)
	{
		for(i = 0; i < MAPWIDTH; i++)
		{
			if(!usedtile[i][j])
			{
				bool fDownDone = false;
				bool fRightDone = false;

				short downsize = j + 1;
				short rightsize = i + 1;
				short attempt = 1;

				while(true)
				{
					if(!fRightDone)
					{
						for(int right = i; right < rightsize; right++)
						{
							if(right >= MAPWIDTH || j + attempt >= MAPHEIGHT || usedtile[right][j + attempt])
								fRightDone = true;
						}

						if(!fRightDone)
							downsize++;
					}

					if(!fDownDone)
					{
						for(int down = j; down < downsize; down++)
						{
							if(i + attempt >= MAPWIDTH || down >= MAPHEIGHT || usedtile[i + attempt][down])
								fDownDone = true;
						}

						if(!fDownDone)
							rightsize++;
					}

					if(fDownDone && fRightDone)
					{
						if(numspawnareas[iType] < MAXSPAWNAREAS)
						{
							spawnareas[iType][numspawnareas[iType]].left = i;
							spawnareas[iType][numspawnareas[iType]].top = j;
							spawnareas[iType][numspawnareas[iType]].width = rightsize - 1 - i;
							spawnareas[iType][numspawnareas[iType]].height = downsize - 1 - j;
							spawnareas[iType][numspawnareas[iType]].size = (rightsize - i) * (downsize - j);

							numspawnareas[iType]++;
						}

						for(short down = j; down < downsize; down++)
						{
							for(short right = i; right < rightsize; right++)
							{
								usedtile[right][down] = true;
							}
						}
						
						break;
					}

					attempt++;
				}
			}
		}
	}
}

void CMap::AnimateTiles(short iFrame)
{
	//For each animated tile
	if(iAnimatedVectorIndices[iFrame] == iAnimatedVectorIndices[iFrame + 1])
		return;

	//FIXME:: There is an issue here where there is excessive memory page faults when you put a
	//non animated tile on layer 0 (or layer 2) and then an animated tile on layer 1 (or 3)
	//This causes the working set to contain the static tileset + the backmap surface that is swapped
	//every 8 frames, needing to be loaded back into the working set.  This causes serious slowdown.
	for(short iTile = iAnimatedVectorIndices[iFrame]; iTile < iAnimatedVectorIndices[iFrame + 1]; iTile++)
	{
		AnimatedTile * tile = animatedtiles[iTile];
		SDL_Rect * rDst = &(tile->rDest);

		if(tile->fBackgroundAnimated)
		{
			SDL_BlitSurface(animatedTilesSurface, &(tile->rAnimationSrc[0][iTileAnimationFrame]), animatedBackmapSurface, rDst);
		}

		if(tile->fForegroundAnimated)
		{
			SDL_BlitSurface(animatedTilesSurface, &(tile->rAnimationSrc[1][iTileAnimationFrame]), animatedFrontmapSurface, rDst);
		}
	}
}

void CMap::draw(SDL_Surface *targetSurface, int layer)
{
	int i, j;

	//draw left to right full vertical
	bltrect.x = 0;
	for(i = 0; i < MAPWIDTH; i++)
	{
		bltrect.y = -TILESIZE;	//this is okay, see

		for(j = 0; j < MAPHEIGHT; j++)
		{
			bltrect.y += TILESIZE;	// here

			TilesetTile * tile = &mapdata[i][j][layer];
			
			//If there is no tile to draw, continue
			if(tile->iID == TILESETNONE)
				continue;

			//If this is an animated tile, then setup an animated tile struct for use in drawing
			if(tile->iID >= 0)
			{
				g_tilesetmanager.Draw(targetSurface, tile->iID, 0, tile->iCol, tile->iRow, i, j);
				//SDL_BlitSurface(spr_maptiles[0].getSurface(), &g_tilesetmanager.rRects[0][tile->iCol][tile->iRow], targetSurface, &bltrect);
			}
			else if(tile->iID == TILESETANIMATED)
			{
				//See if we already have this tile
				bool fNeedNewAnimatedTile = true;

				short iNewTileId = j * MAPWIDTH + i;
				std::vector<AnimatedTile*>::iterator iter = animatedtiles.begin(), lim = animatedtiles.end();
				while (iter != lim)
				{
					if(iNewTileId == (*iter)->id)
					{
						fNeedNewAnimatedTile = false;
						break;
					}

					++iter;
				}

				if(fNeedNewAnimatedTile)
				{
					AnimatedTile * animatedtile = new AnimatedTile();
					animatedtile->id = iNewTileId;
					
					animatedtile->fBackgroundAnimated = false;
					animatedtile->fForegroundAnimated = false;

					for(short iLayer = 0; iLayer < 4; iLayer++)
					{
						TilesetTile * tile = &mapdata[i][j][iLayer];
						TilesetTile * toTile = &animatedtile->layers[iLayer];

						toTile->iID = tile->iID;
						toTile->iCol = tile->iCol;
						toTile->iRow = tile->iRow;

						if(tile->iID >= 0) //If it is part of a tileset
						{
							gfx_setrect(&(animatedtile->rSrc[iLayer][0]), tile->iCol * TILESIZE, tile->iRow * TILESIZE, TILESIZE, TILESIZE);
						}
						else if(tile->iID == TILESETANIMATED)
						{
							for(short iRect = 0; iRect < 4; iRect++)
							{
								gfx_setrect(&(animatedtile->rSrc[iLayer][iRect]), (iRect + (tile->iCol << 2)) * TILESIZE, tile->iRow * TILESIZE, TILESIZE, TILESIZE);
							}

							//Background is animated if it is a background layer or if it is a foreground layer and we are not displaying the foreground
							if(iLayer < 2 || !game_values.toplayer)
								animatedtile->fBackgroundAnimated = true;

							//Foreground is animated if it is a foreground layer and we are displaying the foreground
							if(iLayer >= 2 && game_values.toplayer)
								animatedtile->fForegroundAnimated = true;
						}
					}

					gfx_setrect(&(animatedtile->rDest), bltrect.x, bltrect.y, TILESIZE, TILESIZE);
					animatedtiles.push_back(animatedtile);
				}
			}
			else if(tile->iID == TILESETUNKNOWN) //Draw red X where tile should be
			{
				SDL_BlitSurface(spr_unknowntile[0].getSurface(), &g_tilesetmanager.rRects[0][0][0], targetSurface, &bltrect);
			}
		}

		bltrect.x += TILESIZE;
	}

	bltrect.x = 0;
	bltrect.y = 0;
	bltrect.w = 640;
	bltrect.h = 480;
}

void CMap::drawThumbnailHazards(SDL_Surface * targetSurface)
{
	blitdest = targetSurface;

	for(short iHazard = 0; iHazard < iNumMapHazards; iHazard++)
	{
		DrawMapHazard(&maphazards[iHazard], 2, false);
	}

	blitdest = screen;
}

void CMap::drawThumbnailPlatforms(SDL_Surface * targetSurface)
{
	blitdest = targetSurface;

	for(short iPlatform = 0; iPlatform < iNumPlatforms; iPlatform++)
	{
		MovingPlatform * platform = platforms[iPlatform];
		MovingPlatformPath * basepath = platform->pPath;
		
		if(basepath->iType == 0)
		{
			StraightPath * path = (StraightPath*) basepath;
			DrawPlatform(path->iType, platform->iTileData, ((short)path->dPathPointX[0]) << 1, ((short)path->dPathPointY[0]) << 1, ((short)path->dPathPointX[1]) << 1, ((short)path->dPathPointY[1]) << 1, 0.0f, 0.0f, 0.0f, 2, platform->iTileWidth, platform->iTileHeight, true, true);
		}
		else if(basepath->iType == 1)
		{
			StraightPathContinuous * path = (StraightPathContinuous*) basepath;
			DrawPlatform(path->iType, platform->iTileData, ((short)path->dPathPointX[0]) << 1, ((short)path->dPathPointY[0]) << 1, 0, 0, path->dAngle, 0.0f, 0.0f, 2, platform->iTileWidth, platform->iTileHeight, true, true);
		}
		else if(basepath->iType == 2)
		{
			EllipsePath * path = (EllipsePath*) basepath;
			DrawPlatform(path->iType, platform->iTileData, ((short)path->dPathPointX[0]) << 1, ((short)path->dPathPointY[0]) << 1, 0, 0, path->dStartAngle, path->dRadiusX * 2, path->dRadiusY * 2, 2, platform->iTileWidth, platform->iTileHeight, true, true);
		}
	}

	blitdest = screen;
}

void CMap::preDrawPreviewWarps(SDL_Surface * targetSurface, bool fThumbnail)
{
	short iTileSize = 16;
	short iScreenshotSize = 0;

	if(fThumbnail)
	{
		iTileSize = 8;
		iScreenshotSize = 1;
	}

	for(int j = 0; j < MAPHEIGHT; j++)
	{
		for(int i = 0; i < MAPWIDTH; i++)
		{
			Warp * warp = &g_map.warpdata[i][j];
			
			if(warp->connection != -1)
			{
				SDL_Rect rSrc = {warp->connection * iTileSize, warp->direction * iTileSize, iTileSize, iTileSize};
				SDL_Rect rDst = {i * iTileSize, j * iTileSize, iTileSize, iTileSize};

				SDL_BlitSurface(spr_thumbnail_warps[iScreenshotSize].getSurface(), &rSrc, targetSurface, &rDst);
			}
		}
	}
}

void CMap::preDrawPreviewMapItems(SDL_Surface * targetSurface, bool fThumbnail)
{
	short iTileSize = 16;
	short iScreenshotSize = 0;

	if(fThumbnail)
	{
		iTileSize = 8;
		iScreenshotSize = 1;
	}

	for(int j = 0; j < iNumMapItems; j++)
	{
		SDL_Rect rSrc = {mapitems[j].itype * iTileSize, 0, iTileSize, iTileSize};
		SDL_Rect rDst = {mapitems[j].ix * iTileSize, mapitems[j].iy * iTileSize, iTileSize, iTileSize};

		SDL_BlitSurface(spr_thumbnail_mapitems[iScreenshotSize].getSurface(), &rSrc, targetSurface, &rDst);
	}
}

void CMap::preDrawPreviewBackground(SDL_Surface * targetSurface, bool fThumbnail)
{
	drawPreview(targetSurface, 0, fThumbnail);

	SDL_Delay(10);

	drawPreview(targetSurface, 1, fThumbnail);

	SDL_Delay(10);

	if(!game_values.toplayer)
	{
		drawPreview(targetSurface, 2, fThumbnail);

		SDL_Delay(10);

		drawPreview(targetSurface, 3, fThumbnail);

		SDL_Delay(10);
	}

	drawPreviewBlocks(targetSurface, fThumbnail);
}

void CMap::preDrawPreviewBackground(gfxSprite * spr_background, SDL_Surface * targetSurface, bool fThumbnail)
{
	SDL_Rect srcrect;
	srcrect.x = 0;
	srcrect.y = 0;
	srcrect.w = 640;
	srcrect.h = 480;

	SDL_Rect dstrect;
	dstrect.x = 0;
	dstrect.y = 0;

	if(fThumbnail)
	{
		dstrect.w = 160;
		dstrect.h = 120;
	}
	else
	{
		dstrect.w = 320;
		dstrect.h = 240;
	}

	if(SDL_SoftStretch(spr_background->getSurface(), &srcrect, targetSurface, &dstrect) < 0)
	{
		fprintf(stderr, "SDL_SoftStretch error: %s\n", SDL_GetError());
		return;
	}

	SDL_Delay(10);  //Sleeps to help the music from skipping
	
	preDrawPreviewBackground(targetSurface, fThumbnail);
}

void CMap::preDrawPreviewForeground(SDL_Surface * targetSurface, bool fThumbnail)
{
	if(!fThumbnail)
	{
		SDL_FillRect(targetSurface, NULL, SDL_MapRGB(targetSurface->format, 255, 0, 255));
		SDL_SetColorKey(targetSurface, SDL_SRCCOLORKEY, SDL_MapRGB(targetSurface->format, 255, 0, 255));
		SDL_Delay(10);
	}

	if(!game_values.toplayer)
		return;

	drawPreview(targetSurface, 2, fThumbnail);

	SDL_Delay(10);

	drawPreview(targetSurface, 3, fThumbnail);
}

void CMap::drawPreview(SDL_Surface * targetSurface, int layer, bool fThumbnail)
{
	int i, j;

	//draw left to right full vertical
	short iTilesetIndex = 1;

	if(fThumbnail)
		iTilesetIndex = 2;

	for(i = 0; i < MAPWIDTH; i++)
	{
		for(j = 0; j < MAPHEIGHT; j++)
		{
			TilesetTile * tile = &mapdata[i][j][layer];
			if(tile->iID == TILESETNONE)
				continue;

			//Handle drawing preview for animated tiles
			if(tile->iID >= 0)
			{
				g_tilesetmanager.Draw(targetSurface, tile->iID, iTilesetIndex, tile->iCol, tile->iRow, i, j);
				//SDL_BlitSurface(spr_maptiles[iTilesetIndex].getSurface(), &g_tilesetmanager.rRects[iTilesetIndex][tile->iCol][tile->iRow], targetSurface, &rectDst);
			}
			else if(tile->iID == TILESETANIMATED)
			{				
				SDL_BlitSurface(spr_tileanimation[iTilesetIndex].getSurface(), &g_tilesetmanager.rRects[iTilesetIndex][tile->iCol << 2][tile->iRow], targetSurface, &g_tilesetmanager.rRects[iTilesetIndex][i][j]);
			}
			else if(tile->iID == TILESETUNKNOWN)
			{	
				SDL_BlitSurface(spr_unknowntile[iTilesetIndex].getSurface(), &g_tilesetmanager.rRects[iTilesetIndex][0][0], targetSurface, &g_tilesetmanager.rRects[iTilesetIndex][i][j]);
			}
		}
	}
}

void CMap::drawPreviewBlocks(SDL_Surface * targetSurface, bool fThumbnail)
{
	int i, j, ts;
	
	short iBlockSize = PREVIEWTILESIZE;

	if(fThumbnail)
		iBlockSize = THUMBTILESIZE;

	SDL_Rect rectDst;
	rectDst.w = iBlockSize;
	rectDst.h = iBlockSize;

	SDL_Rect rectSrc;
	rectSrc.w = iBlockSize;
	rectSrc.h = iBlockSize;
	
	rectDst.x = 0;
	for(i = 0; i < MAPWIDTH; i++)
	{
		rectDst.y = -iBlockSize;	//this is okay, see

		for(j = 0; j < MAPHEIGHT; j++)
		{
			rectDst.y += iBlockSize;	// here

			ts = objectdata[i][j].iType;
			if(ts == -1)
				continue;

			//Don't draw hidden blocks
			if(objectdata[i][j].fHidden)
				continue;

			rectSrc.x = (Sint16)(ts * iBlockSize);
			rectSrc.y = 0;

			//Draw the turned off switch blocks too
			if(ts >= 7 && ts <= 10)
			{
				if(iSwitches[(ts - 7) % 4] == 0)
					rectSrc.y = iBlockSize;
			}
			else if(ts >= 11 && ts <= 14)
			{
				if(objectdata[i][j].iSettings[0] == 0)
					rectSrc.y = iBlockSize;
			}
			else if(ts >= 15 && ts <= 19)
			{
				rectSrc.x = iBlockSize * (ts - 15);
				rectSrc.y = iBlockSize;
			}
			else if(ts >= 20 && ts <= 29)
			{
				rectSrc.x = iBlockSize * (ts - 20);
				rectSrc.y = iBlockSize << 1;
			}
		
			if(fThumbnail)
				SDL_BlitSurface(spr_blocks[2].getSurface(), &rectSrc, targetSurface, &rectDst);
			else
				SDL_BlitSurface(spr_blocks[1].getSurface(), &rectSrc, targetSurface, &rectDst);
		}

		rectDst.x += iBlockSize;
	}
}

void CMap::predrawbackground(gfxSprite &background, gfxSprite &mapspr)
{
	SDL_Rect r;
	r.x = 0;
	r.y = 0;
	r.w = 640;
	r.h = 480;

	SDL_BlitSurface(background.getSurface(), NULL, mapspr.getSurface(), &r);
	
	draw(mapspr.getSurface(), 0);
	draw(mapspr.getSurface(), 1);

	if(!game_values.toplayer)
	{
		draw(mapspr.getSurface(), 2);
		draw(mapspr.getSurface(), 3);
	}

	/*
	//Draws the spawn areas
	SDL_Rect dest;
	dest.w = 32;
	dest.h = 32;
	short iType = 2;
	
	for(int m = 0; m < numspawnareas[iType]; m++)  //use [1] for item spawn areas
	{
		dest.x = spawnareas[iType][m].left * TILESIZE;
		dest.y = spawnareas[iType][m].top * TILESIZE;

		dest.w = (spawnareas[iType][m].width) * TILESIZE + TILESIZE;
		dest.h = (spawnareas[iType][m].height) * TILESIZE + TILESIZE;

		//int color = 0x00 << 24 | rand() % 256 << 16 | rand() % 256 << 8 | rand() % 256;
		int color = 0x128 << 24 | 0x255 << 8;

		SDL_FillRect(mapspr.getSurface(), &dest, color);		//fill empty area with black
	}
	*/
}

void CMap::predrawforeground(gfxSprite &foregroundspr)
{
	SDL_FillRect(foregroundspr.getSurface(), NULL, SDL_MapRGB(foregroundspr.getSurface()->format, 255, 0, 255));
	SDL_SetColorKey(foregroundspr.getSurface(), SDL_SRCCOLORKEY, SDL_MapRGB(foregroundspr.getSurface()->format, 255, 0, 255));

	draw(foregroundspr.getSurface(), 2);
	draw(foregroundspr.getSurface(), 3);
}

void CMap::SetupAnimatedTiles()
{
	iAnimatedBackgroundLayers = 2;
	if(!game_values.toplayer)
		iAnimatedBackgroundLayers = 4;

	g_iCurrentDrawIndex = 0;

	iAnimatedTileCount = animatedtiles.size();

	if(animatedTilesSurface)
	{
		SDL_FreeSurface(animatedTilesSurface);
		animatedTilesSurface = NULL;
	}

	if(iAnimatedTileCount > 0)
	{
		SDL_Surface * backgroundSurface = spr_background.getSurface();
		SDL_Surface * animatedTileSrcSurface = spr_tileanimation[0].getSurface();

		animatedFrontmapSurface = spr_frontmap[g_iCurrentDrawIndex].getSurface();
		animatedBackmapSurface = spr_backmap[g_iCurrentDrawIndex].getSurface();
		animatedTilesSurface = SDL_CreateRGBSurface(screen->flags, 1024, 1024, screen->format->BitsPerPixel, 0, 0, 0, 0);

		int iTransparentColor = SDL_MapRGB(animatedTilesSurface->format, 255, 0, 255);

		std::vector<AnimatedTile*>::iterator iter = animatedtiles.begin(), lim = animatedtiles.end();
		
		SDL_Rect rDst = {0, 0, 32, 32};
		while (iter != lim)
		{
			AnimatedTile * tile = *iter;
			SDL_Rect * rSrc = &(tile->rDest);

			if(tile->fBackgroundAnimated)
			{
				for(short iTileAnimationFrame = 0; iTileAnimationFrame < 4; iTileAnimationFrame++)
				{
					gfx_setrect(&tile->rAnimationSrc[0][iTileAnimationFrame], &rDst);

					SDL_BlitSurface(backgroundSurface, rSrc, animatedTilesSurface, &rDst);

					for(short iLayer = 0; iLayer < iAnimatedBackgroundLayers; iLayer++)
					{
						TilesetTile * tilesetTile = &tile->layers[iLayer];
						if(tilesetTile->iID >= 0)
						{
							SDL_BlitSurface(g_tilesetmanager.GetTileset(tilesetTile->iID)->GetSurface(0), &(tile->rSrc[iLayer][0]), animatedTilesSurface, &rDst);
						}
						else if(tilesetTile->iID == TILESETANIMATED)
						{
							SDL_BlitSurface(animatedTileSrcSurface, &(tile->rSrc[iLayer][iTileAnimationFrame]), animatedTilesSurface, &rDst);
						}
						else if(tilesetTile->iID == TILESETUNKNOWN)
						{
							SDL_BlitSurface(spr_unknowntile[0].getSurface(), &g_tilesetmanager.rRects[0][0][0], animatedTilesSurface, &rDst);
						}
					}

					rDst.x += 32;
					if(rDst.x >= 1024)
					{
						rDst.x = 0;
						rDst.y += 32;
						if(rDst.y >= 1024)
						{
							tile->fForegroundAnimated = false;
							++iter;
							break;
						}
					}
				}
			}

			if(tile->fForegroundAnimated)
			{
				for(short iTileAnimationFrame = 0; iTileAnimationFrame < 4; iTileAnimationFrame++)
				{
					gfx_setrect(&tile->rAnimationSrc[1][iTileAnimationFrame], &rDst);

					SDL_FillRect(animatedTilesSurface, &rDst, iTransparentColor);

					for(short iLayer = 2; iLayer < 4; iLayer++)
					{
						TilesetTile * tilesetTile = &tile->layers[iLayer];
						if(tilesetTile->iID >= 0)
						{
							SDL_BlitSurface(g_tilesetmanager.GetTileset(tilesetTile->iID)->GetSurface(0), &(tile->rSrc[iLayer][0]), animatedTilesSurface, &rDst);
						}
						else if(tilesetTile->iID == TILESETANIMATED)
						{
							SDL_BlitSurface(animatedTileSrcSurface, &(tile->rSrc[iLayer][iTileAnimationFrame]), animatedTilesSurface, &rDst);
						}
						else if(tilesetTile->iID == TILESETUNKNOWN)
						{
							SDL_BlitSurface(spr_unknowntile[0].getSurface(), &g_tilesetmanager.rRects[0][0][0], animatedTilesSurface, &rDst);
						}
					}

					rDst.x += 32;
					if(rDst.x >= 1024)
					{
						rDst.x = 0;
						rDst.y += 32;
						if(rDst.y >= 1024)
						{
							++iter;
							break;
						}
					}
				}
			}

			++iter;
		}

		//Turn off animation for the tiles that couldn't fit onto our animation page (256 tiles max)
		while (iter != lim)
		{
			AnimatedTile * tile = *iter;

			tile->fBackgroundAnimated = false;
			tile->fForegroundAnimated = false;

			++iter;
		}

		for(short iAnimatedFrame = 0; iAnimatedFrame <= NUM_FRAMES_BETWEEN_TILE_ANIMATION; iAnimatedFrame++)
			iAnimatedVectorIndices[iAnimatedFrame] = (iAnimatedFrame * iAnimatedTileCount) / NUM_FRAMES_BETWEEN_TILE_ANIMATION;
	
		for(short iFrame = 0; iFrame < NUM_FRAMES_BETWEEN_TILE_ANIMATION; iFrame++)
			AnimateTiles(iFrame);

		animatedFrontmapSurface = spr_frontmap[1 - g_iCurrentDrawIndex].getSurface();
		animatedBackmapSurface = spr_backmap[1 - g_iCurrentDrawIndex].getSurface();

		AnimateTiles(0);
	}
}

void CMap::updatePlatforms()
{
	for(short iPlatform = 0; iPlatform < iNumPlatforms; iPlatform++)
	{
		platforms[iPlatform]->update();
	}

	std::list<MovingPlatform*>::iterator iter = tempPlatforms.begin(), lim = tempPlatforms.end();
	
	while (iter != lim)
	{
		if((*iter)->fDead)
		{
			delete (*iter);
			
			iter = tempPlatforms.erase(iter);
			lim = tempPlatforms.end();
		}
		else
		{
			(*iter)->update();
			++iter;
		}
	}

	/*
	while (iter != lim)
	{
		if((*iterateAll)->fDead)
		{
			iterateAll = tempPlatforms.erase(iterateAll);
			--iterateAll;
		}
		else
		{
			(*iterateAll)->update();
			++iterateAll;
		}
	}*/
}

void CMap::drawPlatforms()
{
	for(short iPlatform = 0; iPlatform < iNumPlatforms; iPlatform++)
	{
		platforms[iPlatform]->draw();
	}

	std::list<MovingPlatform*>::iterator iterateAll = tempPlatforms.begin(), lim = tempPlatforms.end();
	while (iterateAll != lim)
	{
		(*iterateAll)->draw();
		iterateAll++;
	}
}

void CMap::drawPlatforms(short iOffsetX, short iOffsetY)
{
	for(short iPlatform = 0; iPlatform < iNumPlatforms; iPlatform++)
	{
		platforms[iPlatform]->draw(iOffsetX, iOffsetY);
	}
}

void CMap::movingPlatformCollision(CPlayer * player)
{
	//Collide player with normal moving platforms
	for(short iPlatform = 0; iPlatform < iNumPlatforms; iPlatform++)
	{
		platforms[iPlatform]->collide(player);

		if(!player->isready())
			return;
	}

	//Collide player with temporary platforms (like falling donut blocks)
	std::list<MovingPlatform*>::iterator iterateAll = tempPlatforms.begin(), lim = tempPlatforms.end();
	while (iterateAll != lim)
	{
		(*iterateAll)->collide(player);

		if(!player->isready())
			return;
		
		iterateAll++;
	}
}

void CMap::movingPlatformCollision(IO_MovingObject * object)
{
	for(short iPlatform = 0; iPlatform < iNumPlatforms; iPlatform++)
	{
		platforms[iPlatform]->collide(object);
	}

	std::list<MovingPlatform*>::iterator iterateAll = tempPlatforms.begin(), lim = tempPlatforms.end();
	while (iterateAll != lim)
	{
		(*iterateAll)->collide(object);		
		iterateAll++;
	}
}

void CMap::resetPlatforms()
{
	for(short iPlatform = 0; iPlatform < iNumPlatforms; iPlatform++)
	{
		platforms[iPlatform]->ResetPath();
	}

	std::list<MovingPlatform*>::iterator iter = tempPlatforms.begin(), lim = tempPlatforms.end();
	
	while (iter != lim)
	{
		delete (*iter);
		++iter;
	}

	tempPlatforms.clear();
}

void CMap::lockconnection(int connection)
{
	//Lock all warp connections
	if(connection == -1)
	{
		for(short iConnection = 0; iConnection <= maxConnection; iConnection++)
		{
			warplocked[iConnection] = true;
		}
	}
	else //otherwise just lock the one connection
	{
		warplocked[connection] = true;
	}
}

WarpExit * CMap::getRandomWarpExit(int connection, int currentID)
{
	int indices[MAXWARPS];
	int numIndices = 0;

	WarpExit * currentWarp = NULL;

	for(int k = 0; k < numwarpexits; k++)
	{
		if(warpexits[k].connection == connection)
		{
			if(warpexits[k].id == currentID)
				currentWarp = &warpexits[k];
			else
				indices[numIndices++] = k;
		}
	}

	if(numIndices == 0)
		return currentWarp;

	return &warpexits[indices[rand() % numIndices]];
}

void CMap::clearWarpLocks()
{
	for(short iConnection = 0; iConnection < 10; iConnection++)
	{
		warplocktimer[iConnection] = 0;
		warplocked[iConnection] = false;
	}

	for(short iWarpExit = 0; iWarpExit < numwarpexits; iWarpExit++)
	{
		warpexits[iWarpExit].locktimer = 0;
	}
}

void CMap::drawWarpLocks()
{
	for(int iWarpExit = 0; iWarpExit < numwarpexits; iWarpExit++)
	{
		if(warplocked[warpexits[iWarpExit].connection] || warpexits[iWarpExit].locktimer > 0)
		{
			spr_warplock.draw(warpexits[iWarpExit].lockx, warpexits[iWarpExit].locky);
		}
	}
}

void CMap::update()
{
	//Unlock locked warps if the time is up
	for(short iConnection = 0; iConnection <= maxConnection; iConnection++)
	{
		if(warplocked[iConnection])
		{
			if(++warplocktimer[iConnection] > game_values.warplocktime)
			{
				warplocked[iConnection] = false;
				warplocktimer[iConnection] = 0;
			}
		}
	}

	//If warp is individually locked, then reduce lock timer
	for(short iWarpExit = 0; iWarpExit < numwarpexits; iWarpExit++)
	{
		if(warpexits[iWarpExit].locktimer > 0)
			--warpexits[iWarpExit].locktimer;
	}

	//Animate the animated tiles
	if(++iTileAnimationTimer >= NUM_FRAMES_BETWEEN_TILE_ANIMATION)
	{
		iTileAnimationTimer = 0;

		animatedFrontmapSurface = spr_frontmap[g_iCurrentDrawIndex].getSurface();
		animatedBackmapSurface = spr_backmap[g_iCurrentDrawIndex].getSurface();

		g_iCurrentDrawIndex = 1 - g_iCurrentDrawIndex;

		if(++iTileAnimationFrame >= NUM_FRAMES_IN_TILE_ANIMATION)
			iTileAnimationFrame = 0;
	}

	if(iAnimatedTileCount > 0)
		AnimateTiles(iTileAnimationTimer);
}

bool CMap::findspawnpoint(short iType, short * x, short * y, short width, short height, bool tilealigned)
{
	if(totalspawnsize[iType] <= 0)
	{
		*x = 320;
		*y = 224;
		return true;
	}

	int spawnarea = rand() % totalspawnsize[iType];

	int currentsize = 0;
	for(int m = 0; m < numspawnareas[iType]; m++)
	{
		currentsize += spawnareas[iType][m].size;

		if(spawnarea >= currentsize)
			continue;

		short areawidth = spawnareas[iType][m].width * TILESIZE + TILESIZE;
		short areaheight = spawnareas[iType][m].height * TILESIZE + TILESIZE;

		if(width > areawidth || height > areaheight)
			continue;

		if(tilealigned)
		{
			short xoffset = spawnareas[iType][m].width;
			short yoffset = spawnareas[iType][m].height;

			if(xoffset > 0)
				xoffset = (short)(rand() % xoffset);

			if(yoffset > 0)
				yoffset = (short)(rand() % yoffset);

			*x = xoffset * TILESIZE + spawnareas[iType][m].left * TILESIZE + (TILESIZE >> 1) - (width >> 1);
			*y = yoffset * TILESIZE + spawnareas[iType][m].top * TILESIZE + (TILESIZE >> 1) - (height >> 1);
		}
		else
		{
			short xoffset = areawidth - width - 2;
			short yoffset = areaheight - height - 2;

			if(xoffset > 0)
				xoffset = (short)(rand() % xoffset) + 1;

			if(yoffset > 0)
				yoffset = (short)(rand() % yoffset) + 1;

			*x = xoffset + spawnareas[iType][m].left * TILESIZE;
			*y = yoffset + spawnareas[iType][m].top * TILESIZE;
		}

		break;
	}
	//Check to see if we are spawning into a platform
	for(short iPlatform = 0; iPlatform < iNumPlatforms; iPlatform++)
	{
		if(platforms[iPlatform]->IsInNoSpawnZone(*x, *y, width, height))
			return false;
	}

	//Check to see if we are spawning into a temporary (falling) platform
	std::list<MovingPlatform*>::iterator iterateAll = tempPlatforms.begin(), lim = tempPlatforms.end();
	while (iterateAll != lim)
	{
		if((*iterateAll)->IsInNoSpawnZone(*x, *y, width, height))
			return false;

		iterateAll++;
	}

	return true;
}

void CMap::AddTemporaryPlatform(MovingPlatform * platform)
{
	tempPlatforms.push_back(platform);
}

bool CMap::IsInPlatformNoSpawnZone(short x, short y, short width, short height)
{
	for(short iPlatform = 0; iPlatform < iNumPlatforms; iPlatform++)
	{
		if(platforms[iPlatform]->IsInNoSpawnZone(x, y, width, height))
			return true;
	}

	std::list<MovingPlatform*>::iterator iterateAll = tempPlatforms.begin(), lim = tempPlatforms.end();
	while (iterateAll != lim)
	{
		if((*iterateAll)->IsInNoSpawnZone(x, y, width, height))
			return true;

		iterateAll++;
	}

	return false;
}

void CMap::drawfrontlayer()
{
	for(int k = 0; k < numdrawareas; k++)
		spr_frontmap[g_iCurrentDrawIndex].draw(drawareas[k].x, drawareas[k].y, drawareas[k].x, drawareas[k].y, drawareas[k].w, drawareas[k].h);

	//Draw gaps in pink for debugging
	/*
	for(short i = 0; i < MAPHEIGHT; i++)
	{
		for(short j = 0; j < MAPWIDTH; j++)
		{
			if(mapdatatop[j][i].iType == tile_gap)
			{
				SDL_Rect r = {j * TILESIZE, i * TILESIZE, TILESIZE, TILESIZE};
				SDL_FillRect(blitdest, &r, SDL_MapRGB(blitdest->format, 255, 0, 255));
			}
		}	
	}*/
}

bool CMap::checkforwarp(short iData1, short iData2, short iData3, short iDirection)
{
	Warp * warp1 = NULL;
	Warp * warp2 = NULL;

	if(iDirection == 0 || iDirection == 2)
	{
		warp1 = &warpdata[iData1][iData3];
		warp2 = &warpdata[iData2][iData3];
	}
	else
	{
		warp1 = &warpdata[iData1][iData2];
		warp2 = &warpdata[iData1][iData3];
	}

	return warp1->direction == warp2->direction && warp1->id == warp2->id && warp1->direction == iDirection &&
		!warplocked[warp1->connection] && warpexits[warp1->id].locktimer <= 0;
}

void CMap::optimize()
{
	for(int j = 0; j < MAPHEIGHT; j++)
	{
		for(int i = 0; i < MAPWIDTH; i++)
		{
			for(int m = 1; m < MAPLAYERS; m++)
			{
				TilesetTile * tile = &mapdata[i][j][m];
				TileType type = g_tilesetmanager.GetTileset(tile->iID)->GetTileType(tile->iCol, tile->iRow);
				if(type != tile_nonsolid && type != tile_gap && type != tile_solid_on_top)
				{
					for(int k = m - 1; k >= 0; k--)
					{
						TilesetTile * compareTile = &mapdata[i][j][k];
						if(compareTile->iID == TILESETNONE)
						{
							TilesetTile * fromTile = &mapdata[i][j][k + 1];
							compareTile->iID = fromTile->iID;
							compareTile->iCol = fromTile->iCol;
							compareTile->iRow = fromTile->iRow;

							fromTile->iID = TILESETNONE;
							fromTile->iCol = 0;
							fromTile->iRow = 0;
						}
						else
						{
							break;
						}
					}
				}
			}
		}
	}
}

