#include "global.h"

LevelMap::LevelMap()
{
	iWidth = 0;
	iHeight = 0;
	tiles = NULL;	
}

LevelMap::~LevelMap()
{
	if(tiles)
	{
		for(short iCol = 0; iCol < iWidth; iCol++)
			delete [] tiles[iCol];

		delete [] tiles;

		tiles = NULL;
	}
}

bool LevelMap::Load()
{
	FILE * file = fopen(tourlist.GetIndex(game_values.levelindex), "r");

	if(!file)
		return false;

	char buffer[256];
	short iReadType = 0;
	short iVersion[4] = {0, 0, 0, 0};
	short iMapTileReadRow = 0;
	
	while(fgets(buffer, 256, file))
	{
		if(buffer[0] == '#' || buffer[0] == '\n' || buffer[0] == ' ' || buffer[0] == '\t')
			continue;

		if(iReadType == 0)
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

			iReadType = 1;
		}
		else if(iReadType == 1)
		{
			iWidth = atoi(buffer);
			iReadType = 2;
		}
		else if(iReadType == 2)
		{
			iHeight = atoi(buffer);
			iReadType = 3;

			tiles = new LevelMapTile*[iWidth];

			for(short iCol = 0; iCol < iWidth; iCol++)
				tiles[iCol] = new LevelMapTile[iHeight];
		}
		else if(iReadType == 3)
		{
			short iMapTileReadCol = 0;

			char * psz = strtok(buffer, ",\n");
			
			for(short iMapTileReadCol = 0; iMapTileReadCol < iWidth; iMapTileReadCol++)
			{
				if(!psz)
					return false;

				tiles[iMapTileReadCol][iMapTileReadRow].iType = atoi(psz);
				psz = strtok(NULL, ".\n");
			}

			if(++iMapTileReadRow == iHeight)
				iReadType = 4;
		}

	}

	fclose(file);

	return iReadType == 4;
}

bool LevelMap::Save(char * szFileName)
{
	return false;
}