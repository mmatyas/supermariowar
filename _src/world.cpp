#include "global.h"

extern void ResetTourStops();
extern TourStop * ParseTourStopLine(char * buffer, short iVersion[4], bool fIsWorld);
extern WorldMap g_worldmap;
extern bool LoadMenuSkin(short playerID, short skinID, short colorID, bool fLoadBothDirections);


/**********************************
* WorldMovingObject
**********************************/

WorldMovingObject::WorldMovingObject()
{
	ix = 0;
	iy = 0;
	iCurrentTileX = 0;
	iCurrentTileY = 0;
	iDestTileX = 0;
	iDestTileY = 0;
		
	iState = 0;
	iDrawSprite = 0;
	iDrawDirection = 0;
	iAnimationFrame = 0;
	iAnimationTimer = 0;
}

WorldMovingObject::~WorldMovingObject()
{}

void WorldMovingObject::Init(short iCol, short iRow, short iSprite, short iInitialDirection)
{
	ix = iCol * TILESIZE;
	iy = iRow * TILESIZE;
	iCurrentTileX = iCol;
	iCurrentTileY = iRow;
	iDestTileX = iCol;
	iDestTileY = iRow;
		
	iState = 0;
	iDrawSprite = iSprite;
	iDrawDirection = iInitialDirection;
	iAnimationFrame = 0;
	iAnimationTimer = 0;
}

void WorldMovingObject::Move(short iDirection)
{
	if(iDirection == 0)
	{
		iDestTileY--;
		iState = 1;
	}
	else if(iDirection == 1)
	{
		iDestTileY++;
		iState = 2;
	}
	else if(iDirection == 2)
	{
		iDestTileX--;
		iState = 3;
		iDrawDirection = 1;
	}
	else if(iDirection == 3)
	{
		iDestTileX++;
		iState = 4;
		iDrawDirection = 0;
	}
}

bool WorldMovingObject::Update()
{
	if(++iAnimationTimer > 15)
	{
		iAnimationTimer = 0;
		iAnimationFrame += 2;
		if(iAnimationFrame > 2)
			iAnimationFrame = 0;
	}

	if(iState == 1)
	{
		iy -= 2;
		if(iy <= iDestTileY * TILESIZE)
		{
			iy = iDestTileY * TILESIZE;
			iState = 0;
			iCurrentTileY = iDestTileY;

			return true;
		}
	}
	else if(iState == 2) //down
	{
		iy += 2;
		if(iy >= iDestTileY * TILESIZE)
		{
			iy = iDestTileY * TILESIZE;
			iState = 0;
			iCurrentTileY = iDestTileY;

			return true;
		}
	}
	else if(iState == 3) //left
	{
		ix -= 2;
		if(ix <= iDestTileX * TILESIZE)
		{
			ix = iDestTileX * TILESIZE;
			iState = 0;
			iCurrentTileX = iDestTileX;

			return true;
		}
	}
	else if(iState == 4) //right
	{
		ix += 2;
		if(ix >= iDestTileX * TILESIZE)
		{
			ix = iDestTileX * TILESIZE;
			iState = 0;
			iCurrentTileX = iDestTileX;

			return true;
		}
	}

	return false;
}

/**********************************
* WorldPlayer
**********************************/

WorldPlayer::WorldPlayer() :
	WorldMovingObject()
{}

WorldPlayer::~WorldPlayer()
{}

void WorldPlayer::Init(short iCol, short iRow)
{
	WorldMovingObject::Init(iCol, iRow, 0, 0);
}

void WorldPlayer::Draw(short iMapOffsetX, short iMapOffsetY)
{
	spr_player[iDrawSprite][iAnimationFrame + iDrawDirection]->draw(ix + iMapOffsetX, iy + iMapOffsetY, 0, 0, 32, 32);
}

void WorldPlayer::SetSprite(short iPlayer)
{
	while(!LoadMenuSkin(iPlayer, game_values.skinids[iPlayer], game_values.colorids[iPlayer], true))
	{
		if(++game_values.skinids[iPlayer] >= skinlist.GetCount())
			game_values.skinids[iPlayer] = 0;
	}

	iDrawSprite = iPlayer;
}


/**********************************
* WorldVehicle
**********************************/

WorldVehicle::WorldVehicle() :
	WorldMovingObject()
{}

WorldVehicle::~WorldVehicle()
{}

void WorldVehicle::Init(short iCol, short iRow, short iAction, short iSprite, short minMoves, short maxMoves, bool spritePaces, short iInitialDirection)
{
	WorldMovingObject::Init(iCol, iRow, iSprite, iInitialDirection);

	fEnabled = true;

	short iRectOffsetX = 0;
	short iRectOffsetY = 0;

	if(iDrawSprite >= 0 && iDrawSprite <= 2)
	{
		iRectOffsetX = 0;
		iRectOffsetY = iDrawSprite * TILESIZE;
	}

	for(short iRect = 0; iRect < 4; iRect++)
		gfx_setrect(&srcRects[iRect], iRect * TILESIZE + iRectOffsetX, iRectOffsetY, 32, 32);

	iNumMoves = 0;
	iActionId = iAction;

	iMinMoves = minMoves;
	iMaxMoves = maxMoves;

	fSpritePaces = spritePaces;
	iPaceOffset = 0;
	iPaceTimer = 0;
}

void WorldVehicle::Move()
{
	iNumMoves = rand() % (iMaxMoves - iMinMoves + 1) + iMinMoves;

	if(iNumMoves > 0)
	{
		iPaceOffset = 0;
		iPaceTimer = 0;
	}

	SetNextDest();
}

void WorldVehicle::SetNextDest()
{
	if(iState != 0 || iMaxMoves == 0)
		return;

	WorldMapTile * tile = &g_worldmap.tiles[iCurrentTileX][iCurrentTileY];

	short iPlayerCurrentTileX, iPlayerCurrentTileY;
	g_worldmap.GetPlayerCurrentTile(&iPlayerCurrentTileX, &iPlayerCurrentTileY);

	if(iNumMoves-- <= 0)
	{
		if(tile->iType == 0 && (iPlayerCurrentTileX != iCurrentTileX || iPlayerCurrentTileY != iCurrentTileY) && 
			g_worldmap.NumVehiclesInTile(iCurrentTileX, iCurrentTileY) <= 1)
			return;
	}

	//Don't allow vehicle to move forever, cap it at 10 moves over the number attempted
	if(iNumMoves <= -10)
		return;

	short iConnections[4];
	short iNumConnections = 0;
	for(short iDirection = 0; iDirection < 4; iDirection ++)
	{
		if(tile->fConnection[iDirection])
			iConnections[iNumConnections++] = iDirection;
	}

	short iConnection = iConnections[rand() % iNumConnections];

	WorldMovingObject::Move(iConnection);
}

bool WorldVehicle::Update()
{
	bool fMoveDone = WorldMovingObject::Update();

	if(fMoveDone)
		SetNextDest();

	//If we're done moving, start pacing in place
	if(fSpritePaces && iState == 0 && ++iPaceTimer > 1)
	{
		iPaceTimer = 0;

		if(iDrawDirection)
		{
			if(--iPaceOffset <= -16)
				iDrawDirection = 0;
		}
		else
		{
			if(++iPaceOffset >= 16)
				iDrawDirection = 1;
		}
	}

	return false;
}

void WorldVehicle::Draw(short iWorldOffsetX, short iWorldOffsetY)
{
	SDL_Rect rDst = {ix + iWorldOffsetX + iPaceOffset, iy + iWorldOffsetY, 32, 32};
	SDL_BlitSurface(spr_worldobjects.getSurface(), &srcRects[iDrawDirection + iAnimationFrame], blitdest, &rDst);
}


/**********************************
* WorldMap
**********************************/

WorldMap::WorldMap()
{
	iWidth = 0;
	iHeight = 0;
	tiles = NULL;
	vehicles = NULL;
	iNumVehicles = 0;
	iNumStages = 0;
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
	short iCurrentStage = 0;
	short iCurrentVehicle = 0;
	
	while(fgets(buffer, 256, file))
	{
		if(buffer[0] == '#' || buffer[0] == '\n' || buffer[0] == '\r' || buffer[0] == ' ' || buffer[0] == '\t')
			continue;

		if(iReadType == 0)  //Read version number
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
		else if(iReadType == 1) //world width
		{
			iWidth = atoi(buffer);
			iReadType = 2;
		}
		else if(iReadType == 2) //world height
		{
			iHeight = atoi(buffer);
			iReadType = 3;

			tiles = new WorldMapTile*[iWidth];

			for(short iCol = 0; iCol < iWidth; iCol++)
				tiles[iCol] = new WorldMapTile[iHeight];
		}
		else if(iReadType == 3) //background sprites
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
		else if(iReadType == 4) //foreground sprites
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
		else if(iReadType == 5) //path connections
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
		else if(iReadType == 6) //stages
		{
			char * psz = strtok(buffer, ",\n");
			
			for(short iMapTileReadCol = 0; iMapTileReadCol < iWidth; iMapTileReadCol++)
			{
				if(!psz)
					goto RETURN;

				WorldMapTile * tile = &tiles[iMapTileReadCol][iMapTileReadRow];
				tile->iType = atoi(psz);

				if(tile->iType == 1)
				{
					iStartX = iMapTileReadCol;
					iStartY = iMapTileReadRow;
				}
				
				tile->fCompleted = tile->iType <= 1;

				psz = strtok(NULL, ",\n");
			}

			if(++iMapTileReadRow == iHeight)
				iReadType = 7;
		}
		else if(iReadType == 7) //number of stages
		{
			iNumStages = atoi(buffer);
			iReadType = 8;
		}
		else if(iReadType == 8) //stage details
		{
			TourStop * ts = ParseTourStopLine(buffer, iVersion, true);
		
			game_values.tourstops.push_back(ts);
			game_values.tourstoptotal++;

			if(++iCurrentStage >= iNumStages)
				iReadType = 9;
		}
		else if(iReadType == 9) //number of vehicles
		{
			iNumVehicles = atoi(buffer);
			vehicles = new WorldVehicle[iNumVehicles];
			iReadType = 10;
		}
		else if(iReadType == 10) //moving objects
		{
			char * psz = strtok(buffer, ",\n");
			
			if(!psz)
				goto RETURN;

			short iSprite = atoi(psz);

			psz = strtok(NULL, ",\n");

			short iStage = atoi(psz);

			if(iStage > iNumStages)
				iStage = 0;

			psz = strtok(NULL, ",\n");
			short iCol = atoi(psz);

			psz = strtok(NULL, ",\n");
			short iRow = atoi(psz);

			psz = strtok(NULL, ",\n");
			short iMinMoves = atoi(psz);

			if(iMinMoves < 0)
				iMinMoves = 0;

			psz = strtok(NULL, ",\n");
			short iMaxMoves = atoi(psz);

			if(iMaxMoves < iMinMoves)
				iMaxMoves = iMinMoves;

			psz = strtok(NULL, ",\n");
			bool fSpritePaces = atoi(psz) == 1;

			psz = strtok(NULL, ",\n");
			short iInitialDirection = atoi(psz);

			if(iInitialDirection != 0)
				iInitialDirection = 1;

			vehicles[iCurrentVehicle].Init(iCol, iRow, iStage, iSprite, iMinMoves, iMaxMoves, fSpritePaces, iInitialDirection);

			if(++iCurrentVehicle >= iNumVehicles)
				iReadType = 11;
		}
	}

RETURN:

	fclose(file);

	return iReadType == 11;
}

//Saves world to file
bool WorldMap::Save(const char * szPath)
{
	return true;
}

void WorldMap::Clear()
{
	if(tiles)
	{
		for(short iCol = 0; iCol < iWidth; iCol++)
		{
			tiles[iCol]->iBackgroundSprite = 0;
			tiles[iCol]->iForegroundSprite = 0;
			tiles[iCol]->iConnectionType = 0;
			tiles[iCol]->iType = 0;
		}
	}

	if(vehicles)
	{
		delete [] vehicles;
		vehicles = NULL;
	}
}

//Creates clears world and resizes (essentially creating a new world to work on for editor)
void WorldMap::New(short w, short h)
{
	Cleanup();
	
	tiles = new WorldMapTile*[w];

	for(short iCol = 0; iCol < w; iCol++)
		tiles[iCol] = new WorldMapTile[h];
}

//Resizes world keeping intact current tiles (if possible)
void WorldMap::Resize(short w, short h)
{
	//Copy tiles from old map
	WorldMapTile ** tempTiles = NULL;

	if(tiles)
	{
		WorldMapTile ** tempTiles = new WorldMapTile*[iWidth];

		for(short iCol = 0; iCol < iWidth; iCol++)
		{
			tempTiles[iCol] = new WorldMapTile[iHeight];
			
			for(short iRow = 0; iRow < iHeight; iRow++)
			{
				tempTiles[iCol][iRow].iBackgroundSprite = tiles[iCol][iRow].iBackgroundSprite;
				tempTiles[iCol][iRow].iForegroundSprite = tiles[iCol][iRow].iForegroundSprite;
				tempTiles[iCol][iRow].iConnectionType = tiles[iCol][iRow].iConnectionType;
				tempTiles[iCol][iRow].iType = tiles[iCol][iRow].iType;
			}
		}
	}

	//Create new map
	New(iWidth, iHeight);

	//Copy into new map
	if(tempTiles)
	{
		for(short iCol = 0; iCol < w && iCol < iWidth; iCol++)
		{
			for(short iRow = 0; iRow < h && iRow < iHeight; iRow++)
			{
				tiles[iCol][iRow].iBackgroundSprite = tempTiles[iCol][iRow].iBackgroundSprite;
				tiles[iCol][iRow].iForegroundSprite = tempTiles[iCol][iRow].iForegroundSprite;
				tiles[iCol][iRow].iConnectionType = tempTiles[iCol][iRow].iConnectionType;
				tiles[iCol][iRow].iType = tempTiles[iCol][iRow].iType;
			}

			delete [] tempTiles[iCol];
		}

		delete [] tempTiles;
	}
}

void WorldMap::InitPlayer()
{
	player.Init(iStartX, iStartY);
}

bool WorldMap::Update()
{
	bool fPlayMovingVehicleSound = false;

	for(short iVehicle = 0; iVehicle < iNumVehicles; iVehicle++)
	{
		if(!vehicles[iVehicle].fEnabled)
			continue;

		vehicles[iVehicle].Update();

		if(vehicles[iVehicle].iState > 0)
			fPlayMovingVehicleSound = true;
	}

	if(fPlayMovingVehicleSound && !sfx_boomerang.isplaying())
		ifsoundonplay(sfx_boomerang);

	return player.Update();
}

void WorldMap::Draw(short iMapOffsetX, short iMapOffsetY)
{
	for(short iVehicle = 0; iVehicle < iNumVehicles; iVehicle++)
	{
		if(!vehicles[iVehicle].fEnabled)
			continue;

		vehicles[iVehicle].Draw(iMapOffsetX, iMapOffsetY);
	}

	player.Draw(iMapOffsetX, iMapOffsetY);
}

void WorldMap::DrawMapToSurface(bool fInit, SDL_Surface * surface, short iMapDrawOffsetCol, short iMapDrawOffsetRow, short iAnimationFrame)
{
	for(short iRow = 0; iRow < 19 && iRow + iMapDrawOffsetRow < iHeight; iRow++)
	{
		for(short iCol = 0; iCol < 24 && iCol + iMapDrawOffsetCol < iWidth; iCol++)
		{
			SDL_Rect r = {iCol * TILESIZE, iRow * TILESIZE, TILESIZE, TILESIZE};
		
			WorldMapTile * tile = &tiles[iCol + iMapDrawOffsetCol][iRow + iMapDrawOffsetRow];
			short iBackgroundSprite = tile->iBackgroundSprite;
			short iForegroundSprite = tile->iForegroundSprite;

			if(tile->fAnimated || fInit)
			{
				if(iBackgroundSprite == 0 || (iBackgroundSprite > 18 && iBackgroundSprite <= 44))
				{
					SDL_Rect rSrc = {iAnimationFrame, 0, TILESIZE, TILESIZE};
					SDL_BlitSurface(spr_worldbackground.getSurface(), &rSrc, surface, &r);

					if(iBackgroundSprite > 18 && iBackgroundSprite <= 44)
					{
						if(iBackgroundSprite > 31)
						{
							SDL_Rect rSrc = {TILESIZE + TILESIZE, (iBackgroundSprite - 31) << 5, TILESIZE, TILESIZE};
							SDL_BlitSurface(spr_worldbackground.getSurface(), &rSrc, surface, &r);
						}
						else
						{
							SDL_Rect rSrc = {TILESIZE, (iBackgroundSprite - 18) << 5, TILESIZE, TILESIZE};
							SDL_BlitSurface(spr_worldbackground.getSurface(), &rSrc, surface, &r);
						}
					}
				}
				else if(iBackgroundSprite > 0 && iBackgroundSprite <= 18)
				{
					SDL_Rect rSrc = {0, iBackgroundSprite << 5, TILESIZE, TILESIZE};
					SDL_BlitSurface(spr_worldbackground.getSurface(), &rSrc, surface, &r);
				}

				if(iForegroundSprite == 1 || iForegroundSprite == 2)
				{
					SDL_Rect rSrc = {96, iForegroundSprite << 5, TILESIZE, TILESIZE};
					SDL_BlitSurface(spr_worldbackground.getSurface(), &rSrc, surface, &r);
				}
				else if(iForegroundSprite >= 3 && iForegroundSprite <= 6)
				{
					SDL_Rect rSrc = {iAnimationFrame, (iForegroundSprite + 14) << 5, TILESIZE, TILESIZE};
					SDL_BlitSurface(spr_worldbackground.getSurface(), &rSrc, surface, &r);
				}
			}
		}
	}		
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

	if(vehicles)
	{
		delete [] vehicles;
		vehicles = NULL;
	}
}

void WorldMap::SetPlayerSprite(short iPlayerSprite)
{
	player.SetSprite(iPlayerSprite);
}

bool WorldMap::IsVehicleMoving()
{
	for(short iVehicle = 0; iVehicle < iNumVehicles; iVehicle++)
	{
		if(!vehicles[iVehicle].fEnabled)
			continue;

		if(vehicles[iVehicle].iState > 0)
			return true;
	}

	return false;
}

void WorldMap::GetPlayerPosition(short * iPlayerX, short * iPlayerY)
{
	*iPlayerX = player.ix;
	*iPlayerY = player.iy;
}

void WorldMap::GetPlayerCurrentTile(short * iPlayerCurrentTileX, short * iPlayerCurrentTileY)
{
	*iPlayerCurrentTileX = player.iCurrentTileX;
	*iPlayerCurrentTileY = player.iCurrentTileY;
}

short WorldMap::GetPlayerState()
{
	return player.iState;
}

short WorldMap::GetVehicleInPlayerTile(short * vehicleIndex)
{
	for(short iVehicle = 0; iVehicle < iNumVehicles; iVehicle++)
	{
		WorldVehicle * vehicle = &vehicles[iVehicle];

		if(!vehicle->fEnabled)
			continue;

		if(vehicle->iCurrentTileX == player.iCurrentTileX && vehicle->iCurrentTileY == player.iCurrentTileY)
		{
			*vehicleIndex = iVehicle;
			return vehicle->iActionId;
		}
	}

	*vehicleIndex = -1;
	return -1;
}

void WorldMap::MovePlayer(short iDirection)
{
	player.Move(iDirection);
}

void WorldMap::MoveVehicles()
{
	for(short iVehicle = 0; iVehicle < iNumVehicles; iVehicle++)
	{
		if(!vehicles[iVehicle].fEnabled)
			continue;

		vehicles[iVehicle].Move();
	}
}

void WorldMap::RemoveVehicle(short iVehicleIndex)
{
	vehicles[iVehicleIndex].fEnabled = false;
}

short WorldMap::NumVehiclesInTile(short iTileX, short iTileY)
{
	short iVehicleCount = 0;
	for(short iVehicle = 0; iVehicle < iNumVehicles; iVehicle++)
	{
		WorldVehicle * vehicle = &vehicles[iVehicle];

		if(!vehicle->fEnabled)
			continue;

		if(vehicle->iCurrentTileX == iTileX && vehicle->iCurrentTileY == iTileY)
			iVehicleCount++;
	}

	return iVehicleCount;
}

short WorldMap::GetVehicleStageScore(short iVehicleIndex)
{
	return game_values.tourstops[vehicles[iVehicleIndex].iActionId]->iPoints;
}