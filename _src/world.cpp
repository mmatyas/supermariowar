#include "global.h"

extern void ResetTourStops();
extern TourStop * ParseTourStopLine(char * buffer, short iVersion[4], bool fIsWorld);
extern WorldMap g_worldmap;

WorldVehicle::WorldVehicle(short iCol, short iRow, short iAction, short iSprite)
{
	iX = iCol * TILESIZE;
	iY = iRow * TILESIZE;
	iCurrentTileX = iCol;
	iCurrentTileY = iRow;
	iDestTileX = iCol;
	iDestTileY = iRow;
		
	iState = 0;
	iDrawSprite = iSprite;
	
	short iRectOffsetX = 0;
	short iRectOffsetY = 0;

	if(iDrawSprite == 0)
	{
		iRectOffsetX = 0;
		iRectOffsetY = 0;
	}

	for(short iRect = 0; iRect < 4; iRect++)
		gfx_setrect(&srcRects[iRect], iRect * TILESIZE + iRectOffsetX, iRectOffsetY, 32, 32);

	iDrawDirection = 0;
	iAnimationFrame = 0;
	iAnimationTimer = 0;
	
	iNumMoves = 0;
	iActionId = iAction;

}

WorldVehicle::~WorldVehicle()
{}

void WorldVehicle::SetNextDest()
{
	if(iState != 0)
		return;
	
	WorldMapTile * tile = &g_worldmap.tiles[iCurrentTileX][iCurrentTileY];

	short iConnections[4];
	short iNumConnections = 0;
	for(short iDirection = 0; iDirection < 4; iDirection ++)
	{
		if(tile->fConnection[iDirection])
			iConnections[iNumConnections++] = iDirection;
	}

	short iConnection = iConnections[rand() % iNumConnections];

	if(iConnection == 0)
	{
		iDestTileY--;
		iState = 1;
	}
	else if(iConnection == 1)
	{
		iDestTileY++;
		iState = 2;
	}
	else if(iConnection == 2)
	{
		iDestTileX--;
		iState = 3;
		iDrawDirection = 0;
	}
	else if(iConnection == 3)
	{
		iDestTileX++;
		iState = 4;
		iDrawDirection = 2;
	}
}

void WorldVehicle::Update()
{
	if(++iAnimationTimer > 15)
	{
		iAnimationTimer = 0;
		if(++iAnimationFrame > 1)
			iAnimationFrame = 0;
	}

	//Player is moving from one tile to the next (up)
	if(iState == 1)
	{
		iY -= 2;
		if(iY <= iDestTileY * TILESIZE)
		{
			iY = iDestTileY * TILESIZE;
			iState = 0;
			iCurrentTileY = iDestTileY;
		}
	}
	else if(iState == 2) //down
	{
		iY += 2;
		if(iY >= iDestTileY * TILESIZE)
		{
			iY = iDestTileY * TILESIZE;
			iState = 0;
			iCurrentTileY = iDestTileY;
		}
	}
	else if(iState == 3) //left
	{
		iX -= 2;
		if(iX <= iDestTileX * TILESIZE)
		{
			iX = iDestTileX * TILESIZE;
			iState = 0;
			iCurrentTileX = iDestTileX;
		}
	}
	else if(iState == 4) //right
	{
		iX += 2;
		if(iX >= iDestTileX * TILESIZE)
		{
			iX = iDestTileX * TILESIZE;
			iState = 0;
			iCurrentTileX = iDestTileX;
		}
	}
}

void WorldVehicle::Draw(short iWorldOffsetX, short iWorldOffsetY)
{
	SDL_Rect rDst = {iX + iWorldOffsetX, iY + iWorldOffsetY, 32, 32};
	SDL_BlitSurface(spr_worldobjects.getSurface(), &srcRects[iDrawDirection + iAnimationFrame], blitdest, &rDst);
}


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
				tile->iBackgroundSprite = atoi(psz);
				tile->fAnimated = tile->iBackgroundSprite == 0 || (tile->iBackgroundSprite > 18 && tile->iBackgroundSprite <= 44);
				
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
				tile->iForegroundSprite = atoi(psz);

				if(!tile->fAnimated)
					tile->fAnimated = tile->iForegroundSprite >= 3 && tile->iForegroundSprite <= 6;
				
				psz = strtok(NULL, ",\n");
			}

			if(++iMapTileReadRow == iHeight)
			{
				iReadType = 5;
				iMapTileReadRow = 0;
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
				tile->iConnectionType = atoi(psz);
		
				psz = strtok(NULL, ",\n");
			}

			if(++iMapTileReadRow == iHeight)
			{
				iReadType = 6;
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
		else if(iReadType == 6)
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
				iReadType = 7;
				iMaxStageNumber--;  //offset down by 1 because #1 is the start 
			}
		}
		else if(iReadType == 7)
		{
			TourStop * ts = ParseTourStopLine(buffer, iVersion, true);
		
			game_values.tourstops.push_back(ts);
			game_values.tourstoptotal++;

			if(++iCurrentStage >= iMaxStageNumber)
				iReadType = 8;
		}
	}

RETURN:

	fclose(file);

	return iReadType == 8;
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
