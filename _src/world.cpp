#include "global.h"

extern void ResetTourStops();
extern TourStop * ParseTourStopLine(char * buffer, short iVersion[4], bool fIsWorld);

WorldMap::WorldMap()
{
	iWidth = 0;
	iHeight = 0;
	tiles = NULL;	
}

WorldMap::~WorldMap()
{
	Cleanup();
}

bool WorldMap::Load()
{
	Cleanup();
	ResetTourStops();

	FILE * file = fopen(worldlist.GetIndex(game_values.worldindex), "r");

	if(!file)
		return false;

	char buffer[256];
	short iReadType = 0;
	short iVersion[4] = {0, 0, 0, 0};
	short iMapTileReadRow = 0;
	short iMaxStageNumber = 0;
	short iCurrentStage = 0;
	
	while(fgets(buffer, 256, file))
	{
		if(buffer[0] == '#' || buffer[0] == '\n' || buffer[0] == '\r' || buffer[0] == ' ' || buffer[0] == '\t')
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

			tiles = new WorldMapTile*[iWidth];

			for(short iCol = 0; iCol < iWidth; iCol++)
				tiles[iCol] = new WorldMapTile[iHeight];
		}
		else if(iReadType == 3)
		{
			char * psz = strtok(buffer, ",\n");
			
			for(short iMapTileReadCol = 0; iMapTileReadCol < iWidth; iMapTileReadCol++)
			{
				if(!psz)
					goto RETURN;

				WorldMapTile * tile = &tiles[iMapTileReadCol][iMapTileReadRow];
				tile->iSprite = atoi(psz);
				
				psz = strtok(NULL, ",\n");
			}

			if(++iMapTileReadRow == iHeight)
			{
				iReadType = 4;
				iMapTileReadRow = 0;
			}
		}
		else if(iReadType == 4)
		{
			char * psz = strtok(buffer, ",\n");
			
			for(short iMapTileReadCol = 0; iMapTileReadCol < iWidth; iMapTileReadCol++)
			{
				if(!psz)
					goto RETURN;

				WorldMapTile * tile = &tiles[iMapTileReadCol][iMapTileReadRow];
				tile->iConnectionType = atoi(psz);
		
				psz = strtok(NULL, ",\n");
			}

			if(++iMapTileReadRow == iHeight)
			{
				iReadType = 5;
				iMapTileReadRow = 0;

				//1 == |  2 == -  3 == -!  4 == L  5 == ,-  6 == -,
				//7 == -|  8 == -`-  9 == |-  10 == -,-  11 == +
				for(short iRow = 0; iRow < iHeight; iRow++)
				{
					for(short iCol = 0; iCol < iWidth; iCol++)
					{
						WorldMapTile * tile = &tiles[iCol][iRow];

						for(short iDirection = 0; iDirection < 4; iDirection++)
							tile->fConnection[iDirection] = false;

						if(iRow > 0)
						{
							WorldMapTile * topTile = &tiles[iCol][iRow - 1];

							tile->fConnection[0] = (topTile->iConnectionType == 1 || topTile->iConnectionType == 5 || topTile->iConnectionType == 6 || 
								topTile->iConnectionType == 7 || topTile->iConnectionType == 9 || topTile->iConnectionType == 10 ||
								topTile->iConnectionType == 11) && (tile->iConnectionType == 1 || tile->iConnectionType == 3 ||
								tile->iConnectionType == 4 || tile->iConnectionType == 7 || tile->iConnectionType == 8 ||
								tile->iConnectionType == 9 || tile->iConnectionType == 11);
						}

						if(iRow < iHeight - 1)
						{
							WorldMapTile * bottomTile = &tiles[iCol][iRow + 1];

							tile->fConnection[1] = (bottomTile->iConnectionType == 1 || bottomTile->iConnectionType == 3 || bottomTile->iConnectionType == 4 || 
								bottomTile->iConnectionType == 7 || bottomTile->iConnectionType == 8 || bottomTile->iConnectionType == 9 ||
								bottomTile->iConnectionType == 11) && (tile->iConnectionType == 1 || tile->iConnectionType == 5 ||
								tile->iConnectionType == 6 || tile->iConnectionType == 7 || tile->iConnectionType == 9 ||
								tile->iConnectionType == 10 || tile->iConnectionType == 11);
						}

						if(iCol > 0)
						{
							WorldMapTile * leftTile = &tiles[iCol - 1][iRow];

							tile->fConnection[2] = (leftTile->iConnectionType == 2 || leftTile->iConnectionType == 4 || leftTile->iConnectionType == 5 || 
								leftTile->iConnectionType == 8 || leftTile->iConnectionType == 9 || leftTile->iConnectionType == 10 ||
								leftTile->iConnectionType == 11) && (tile->iConnectionType == 2 || tile->iConnectionType == 3 ||
								tile->iConnectionType == 6 || tile->iConnectionType == 7 || tile->iConnectionType == 8 ||
								tile->iConnectionType == 10 || tile->iConnectionType == 11);
						}

						if(iCol < iWidth - 1)
						{
							WorldMapTile * rightTile = &tiles[iCol + 1][iRow];

							tile->fConnection[3] = (rightTile->iConnectionType == 2 || rightTile->iConnectionType == 3 || rightTile->iConnectionType == 6 || 
								rightTile->iConnectionType == 7 || rightTile->iConnectionType == 8 || rightTile->iConnectionType == 10 ||
								rightTile->iConnectionType == 11) && (tile->iConnectionType == 2 || tile->iConnectionType == 4 ||
								tile->iConnectionType == 5 || tile->iConnectionType == 8 || tile->iConnectionType == 9 ||
								tile->iConnectionType == 10 || tile->iConnectionType == 11);
						}
					}	
				}
			}
		}
		else if(iReadType == 5)
		{
			char * psz = strtok(buffer, ",\n");
			
			for(short iMapTileReadCol = 0; iMapTileReadCol < iWidth; iMapTileReadCol++)
			{
				if(!psz)
					goto RETURN;

				WorldMapTile * tile = &tiles[iMapTileReadCol][iMapTileReadRow];
				tile->iType = atoi(psz);

				if(tile->iType > iMaxStageNumber)
					iMaxStageNumber = tile->iType;

				if(tile->iType == 1)
				{
					iStartX = iMapTileReadCol;
					iStartY = iMapTileReadRow;
				}
				
				tile->fCompleted = tile->iType <= 1;

				psz = strtok(NULL, ",\n");
			}

			if(++iMapTileReadRow == iHeight)
			{
				iReadType = 6;
				iMaxStageNumber--;  //offset down by 1 because #1 is the start 
			}
		}
		else if(iReadType == 6)
		{
			TourStop * ts = ParseTourStopLine(buffer, iVersion, true);
		
			game_values.tourstops.push_back(ts);
			game_values.tourstoptotal++;

			if(++iCurrentStage >= iMaxStageNumber)
				iReadType = 7;
		}
	}

RETURN:

	fclose(file);

	return iReadType == 7;
}

bool WorldMap::Save(char * szFileName)
{
	return false;
}

void WorldMap::Cleanup()
{
	if(tiles)
	{
		for(short iCol = 0; iCol < iWidth; iCol++)
			delete [] tiles[iCol];

		delete [] tiles;

		tiles = NULL;
	}
}
