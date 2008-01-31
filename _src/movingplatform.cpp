#include "global.h"
#include <math.h>

extern short x_shake;
extern short y_shake;

//------------------------------------------------------------------------------
// Moving Platform Path base class
//------------------------------------------------------------------------------

MovingPlatformPath::MovingPlatformPath(float vel, float startX, float startY, float endX, float endY, bool preview)
{
	dVelocity = vel;
	dVelX = 0.0f;
	dVelY = 0.0f;

	dPathPointX[0] = startX;
	dPathPointY[0] = startY;
	dPathPointX[1] = endX;
	dPathPointY[1] = endY;

	if(preview)
	{
		dPathPointX[0] /= 2.0f;
		dPathPointY[0] /= 2.0f;
		dPathPointX[1] /= 2.0f;
		dPathPointY[1] /= 2.0f;
		dVelocity /= 2.0f;
	}

	pPlatform = NULL;

	iSteps = 0;

	Reset();
}

void MovingPlatformPath::Reset()
{
	iOnStep[0] = 0;
	iOnStep[1] = 0;

	dCurrentX = dPathPointX[0];
	dCurrentY = dPathPointY[0];

	iGoalPoint = 1;
}

//------------------------------------------------------------------------------
// Straight Path
//------------------------------------------------------------------------------

StraightPath::StraightPath(float vel, float startX, float startY, float endX, float endY, bool preview) :
	MovingPlatformPath(vel, startX, startY, endX, endY, preview)
{	
	dWidth = dPathPointX[1] - dPathPointX[0];
	dHeight = dPathPointY[1] - dPathPointY[0];

	//Lock angle to vertical
	if(dWidth == 0)
	{
		if(dHeight > 0)
			dAngle = HALF_PI;
		else
			dAngle = THREE_HALF_PI;

		dLength = fabs(dHeight);
	}
	else if(dHeight == 0) //Lock angle to horizontal
	{
		if(dWidth > 0)
			dAngle = 0.0f;
		else
			dAngle = PI;

		dLength = fabs(dWidth);
	}
	else
	{
		dAngle = atan2(dHeight, dWidth);
		dLength = sqrt(dHeight * dHeight + dWidth * dWidth);
	}
	
	iSteps = (short)(dLength / dVelocity) + 1;

	SetVelocity();
}

bool StraightPath::Move(short type)
{
	//If the path is a straight line, we don't need to calculate the adjustment each frame!
	/*
	if(iGoalPoint == 1)
	{
		fVelX = pPath->fVelocity * cos(pPath->fAngle);
		fVelY = pPath->fVelocity * sin(pPath->fAngle);
	}
	else
	{
		fVelX = -pPath->fVelocity * cos(pPath->fAngle);
		fVelY = -pPath->fVelocity * sin(pPath->fAngle);
	}
	*/

	dCurrentX += dVelX;
	dCurrentY += dVelY;

	//See if we're at the end of the path
	/*
	if(dVelX < -0.01f)
	{
		if(dVelY < -0.01f)
		{
			if(dCurrentX <= dPathPointX[iGoalPoint] && dCurrentY <= dPathPointY[iGoalPoint])
			{
				dCurrentX = dPathPointX[iGoalPoint];
				dCurrentY = dPathPointY[iGoalPoint];
				iGoalPoint = 1 - iGoalPoint;
				SetVelocity();
			}
		}
		else if(dVelY > 0.01f)
		{
			if(dCurrentX <= dPathPointX[iGoalPoint] && dCurrentY >= dPathPointY[iGoalPoint])
			{
				dCurrentX = dPathPointX[iGoalPoint];
				dCurrentY = dPathPointY[iGoalPoint];
				iGoalPoint = 1 - iGoalPoint;
				SetVelocity();
			}
		}
		else
		{
			if(dCurrentX <= dPathPointX[iGoalPoint])
			{
				dCurrentX = dPathPointX[iGoalPoint];
				dCurrentY = dPathPointY[iGoalPoint];
				iGoalPoint = 1 - iGoalPoint;
				SetVelocity();
			}
		}
	}
	else if(dVelX > 0.01f)
	{
		if(dVelY < -0.01f)
		{
			if(dCurrentX >= dPathPointX[iGoalPoint] && dCurrentY <= dPathPointY[iGoalPoint])
			{
				dCurrentX = dPathPointX[iGoalPoint];
				dCurrentY = dPathPointY[iGoalPoint];
				iGoalPoint = 1 - iGoalPoint;
				SetVelocity();
			}
		}
		else if(dVelY > 0.01f)
		{
			if(dCurrentX >= dPathPointX[iGoalPoint] && dCurrentY >= dPathPointY[iGoalPoint])
			{
				dCurrentX = dPathPointX[iGoalPoint];
				dCurrentY = dPathPointY[iGoalPoint];
				iGoalPoint = 1 - iGoalPoint;
				SetVelocity();
			}
		}
		else
		{
			if(dCurrentX >= dPathPointX[iGoalPoint])
			{
				dCurrentX = dPathPointX[iGoalPoint];
				dCurrentY = dPathPointY[iGoalPoint];
				iGoalPoint = 1 - iGoalPoint;
				SetVelocity();
			}
		}
	}
	else
	{
		if(dVelY > 0.01f)
		{
			if(dCurrentY >= dPathPointY[iGoalPoint])
			{
				dCurrentX = dPathPointX[iGoalPoint];
				dCurrentY = dPathPointY[iGoalPoint];
				iGoalPoint = 1 - iGoalPoint;
				SetVelocity();
			}
		}
		else if(dVelY < -0.01f)
		{
			if(dCurrentY <= dPathPointY[iGoalPoint])
			{
				dCurrentX = dPathPointX[iGoalPoint];
				dCurrentY = dPathPointY[iGoalPoint];
				iGoalPoint = 1 - iGoalPoint;
				SetVelocity();
			}
		}
		else
		{
			//Platform is not moving!
			printf("Platform is not moving.  This is probably a problem!\n");
		}
	}*/

	if(++iOnStep[type] >= iSteps)
	{
		iOnStep[type] = 0;

		dCurrentX = dPathPointX[iGoalPoint];
		dCurrentY = dPathPointY[iGoalPoint];

		iGoalPoint = 1 - iGoalPoint;

		SetVelocity();
	}

	return false;
}

void StraightPath::SetVelocity()
{
	if(iGoalPoint == 1)
	{
		dVelX = dVelocity * cos(dAngle);
		dVelY = dVelocity * sin(dAngle);
	}
	else
	{
		dVelX = -dVelocity * cos(dAngle);
		dVelY = -dVelocity * sin(dAngle);
	}

	//Fix rounding errors
	if(dVelX < 0.01f && dVelX > -0.01f)
		dVelX = 0.0f;

	if(dVelY < 0.01f && dVelY > -0.01f)
		dVelY = 0.0f;
}


//------------------------------------------------------------------------------
// Elipse Path
//------------------------------------------------------------------------------
/*
ElipsePath::ElipsePath(float vel, float angle, float radiusx, float radiusy, bool preview) :
	MovingPlatformPath(vel, 0.0f, 0.0f, 0.0f, 0.0f, preview)
{	
	dAngle = angle;
	dRadiusX = radiusx;
	dRadiusY = radiusy;

	SetPosition();
}

bool ElipsePath::Move(short type)
{
	dAngle += dVelocity;

	while(dAngle < 0.0f)
		dAngle += TWO_PI;

	while(dAngle >= TWO_PI)
		dAngle -= TWO_PI;

	dOldCurrentX = dCurrentX;
	dOldCurrentY = dCurrentY;

	dCurrentX = dRadiusX * cos(dAngle);
	dCurrentY = dRadiusY * sin(dAngle);

	dVelX = dCurrentX - dOldCurrentX;
	dVelY = dCurrentY - dOldCurrentY;

	return false;
}

void ElipsePath::SetVelocity()
{
	if(iGoalPoint == 1)
	{
		dVelX = dVelocity * cos(dAngle);
		dVelY = dVelocity * sin(dAngle);
	}
	else
	{
		dVelX = -dVelocity * cos(dAngle);
		dVelY = -dVelocity * sin(dAngle);
	}

	//Fix rounding errors
	if(dVelX < 0.01f && dVelX > -0.01f)
		dVelX = 0.0f;

	if(dVelY < 0.01f && dVelY > -0.01f)
		dVelY = 0.0f;
}*/

//------------------------------------------------------------------------------
// Falling path (for falling donut blocks)
//------------------------------------------------------------------------------

FallingPath::FallingPath(float startX, float startY) :
	MovingPlatformPath(0.0f, startX, startY, 0.0f, 0.0f, false)
{

}

bool FallingPath::Move(short type)
{
	dVelY = CapFallingVelocity(dVelY + GRAVITATION);
	
	if(pPlatform->fy - pPlatform->iHalfHeight >= 480.0f)
	{
		//If a player is standing on this platform, clear him off
		for(short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++)
		{
			if(list_players[iPlayer]->platform == pPlatform)
			{
				list_players[iPlayer]->platform = NULL;
				list_players[iPlayer]->vely = dVelY;
			}
		}

		pPlatform->fDead = true;
	}

	dCurrentY += dVelY;

	return false;
}

//------------------------------------------------------------------------------
// Moving Platform
//------------------------------------------------------------------------------

MovingPlatform::MovingPlatform(TilesetTile ** tiledata, MapTile ** tiletypes, short w, short h, MovingPlatformPath * path, bool fPreview)
{
	fDead = false;

	short iTileSize = TILESIZE;
	short iTileSizeIndex = 0;

	if(fPreview)
	{
		iTileSize = PREVIEWTILESIZE;
		iTileSizeIndex = 1;
	}

	iTileData = tiledata;
	iTileType = tiletypes;

	iTileWidth = w;
	iTileHeight = h;

	iWidth = w * iTileSize;
	iHeight = h * iTileSize;

	iHalfWidth = iWidth >> 1;
	iHalfHeight = iHeight >> 1;
	
	pPath = path;
	pPath->SetPlatform(this);

	ResetPath();

	sSurface = SDL_CreateRGBSurface(screen->flags, w * iTileSize, h * iTileSize, screen->format->BitsPerPixel, 0, 0, 0, 0);

	if( SDL_SetColorKey(sSurface, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(sSurface->format, 255, 0, 255)) < 0)
		printf("\n ERROR: Couldn't set ColorKey for moving platform: %s\n", SDL_GetError());

	SDL_FillRect(sSurface, NULL, SDL_MapRGB(sSurface->format, 255, 0, 255));

	for(short iCol = 0; iCol < iTileWidth; iCol++)
	{
		for(short iRow = 0; iRow < iTileHeight; iRow++)
		{
			TilesetTile * tile = &iTileData[iCol][iRow];

			if(tile->iID == TILESETNONE)
				continue;

			if(tile->iID >= 0)
			{
				g_tilesetmanager.Draw(sSurface, tile->iID, iTileSizeIndex, tile->iCol, tile->iRow, iCol, iRow);
				//SDL_BlitSurface(g_tilesetmanager.GetTileset(tile->iID)->getsur, &g_tilesetmanager.rRects[iTileSizeIndex][tile->iCol][tile->iRow], sSurface, &rDstRect);
			}
			else if(tile->iID == TILESETUNKNOWN)
			{
				SDL_BlitSurface(spr_unknowntile[iTileSizeIndex].getSurface(), &g_tilesetmanager.rRects[iTileSizeIndex][0][0], sSurface, &g_tilesetmanager.rRects[iTileSizeIndex][iCol][iRow]);				
			}
		}
	}

	rSrcRect.x = 0;
	rSrcRect.y = 0;
	rSrcRect.w = w * iTileSize;
	rSrcRect.h = h * iTileSize;

	rDstRect.x = ix - iHalfWidth;
	rDstRect.y = iy - iHalfHeight;
	rDstRect.w = w * iTileSize;
	rDstRect.h = h * iTileSize;

	fVelX = pPath->dVelX;
	fVelY = pPath->dVelY;

	fOldVelX = fVelX;
	fOldVelY = fVelY;
}

MovingPlatform::~MovingPlatform()
{
	for(short iCol = 0; iCol < iTileWidth; iCol++)
	{
		delete [] iTileData[iCol];
		delete [] iTileType[iCol];
	}
	
	delete [] iTileData;
	delete [] iTileType;

	delete pPath;

	SDL_FreeSurface(sSurface);
}

void MovingPlatform::draw()
{
	rDstRect.x = ix - iHalfWidth + x_shake;
	rDstRect.y = iy - iHalfHeight + y_shake;
	rDstRect.w = iWidth;
	rDstRect.h = iHeight;

	// Blit onto the screen surface
	if(SDL_BlitSurface(sSurface, &rSrcRect, blitdest, &rDstRect) < 0)
	{
		fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
		return;
	}

	//Deal with wrapping over sides of screen
	bool fBlitSide = false;
	if(ix - iHalfWidth < 0)
	{
		rDstRect.x = ix - iHalfWidth + 640 + x_shake;
		fBlitSide = true;
	}
	else if(ix + iHalfWidth >= 640)
	{
		rDstRect.x = ix - iHalfWidth - 640 + x_shake;
		fBlitSide = true;
	}
		
	if(fBlitSide)
	{
		//rDstRect.y = iy - iHalfHeight + y_shake;
		rDstRect.w = iWidth;
		rDstRect.h = iHeight;

		if(SDL_BlitSurface(sSurface, &rSrcRect, blitdest, &rDstRect) < 0)
		{
			fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
		}

		//rDstRect.x = ix - iHalfWidth;
	}

	/*
	if(iy - iHalfHeight < 0)
	{
		rDstRect.y = iy - iHalfHeight + 480;
		rDstRect.x = ix - iHalfWidth;

		if(SDL_BlitSurface(sSurface, &rSrcRect, blitdest, &rDstRect) < 0)
		{
			fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
		}

		rDstRect.y = iy - iHalfHeight;
	}
	else if(iy + iHalfHeight >= 480)
	{
		rDstRect.y = iy - iHalfHeight - 480;
		rDstRect.x = ix - iHalfWidth;

		if(SDL_BlitSurface(sSurface, &rSrcRect, blitdest, &rDstRect) < 0)
		{
			fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
		}

		rDstRect.y = iy - iHalfHeight;
	}

	if(ix - iHalfWidth < 0 && iy - iHalfHeight < 0)
	{
		rDstRect.x = ix - iHalfWidth + 640;
		rDstRect.y = iy - iHalfHeight + 480;

		if(SDL_BlitSurface(sSurface, &rSrcRect, blitdest, &rDstRect) < 0)
		{
			fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
		}

		rDstRect.x = ix - iHalfWidth;
		rDstRect.y = iy - iHalfHeight;
	}
	else if(ix + iHalfWidth >= 640 && iy + iHalfHeight >= 480)
	{
		rDstRect.x = ix - iHalfWidth - 640;
		rDstRect.y = iy - iHalfHeight - 480;

		if(SDL_BlitSurface(sSurface, &rSrcRect, blitdest, &rDstRect) < 0)
		{
			fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
		}

		rDstRect.x = ix - iHalfWidth;
		rDstRect.y = iy - iHalfHeight;
	}
	*/
}

//Draw path for map preview
void MovingPlatform::draw(short iOffsetX, short iOffsetY)
{
	rDstRect.x = ix - iHalfWidth + x_shake + iOffsetX;
	rDstRect.y = iy - iHalfHeight + y_shake + iOffsetY;

	// Blit onto the screen surface
	if(SDL_BlitSurface(sSurface, &rSrcRect, blitdest, &rDstRect) < 0)
	{
		fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
		return;
	}
}

void MovingPlatform::update()
{
	fOldX = pPath->dCurrentX;
	fOldY = pPath->dCurrentY;

	fOldVelX = pPath->dVelX;
	fOldVelY = pPath->dVelY;

	//Path will affect new fVelX and fVelY to move the platform to it's next location
	if(pPath->Move(0))
	{
		//This path is done so move to the next one

	}

	fVelX = pPath->dVelX;
	fVelY = pPath->dVelY;

	xf(pPath->dCurrentX);
	yf(pPath->dCurrentY);
}

void MovingPlatform::ResetPath()
{
	pPath->Reset();

	xf(pPath->dCurrentX);
	yf(pPath->dCurrentY);

	fOldX = fx;
	fOldY = fy;

	//Need to advance spawn platform here if spawn time is greater than 0
	//Advance it the spawn time amount here

}

void MovingPlatform::collide(CPlayer * player)
{
	if(!coldec_player(player))
	{
		if(player->platform == this)
			player->platform = NULL;

		/*
		printf("player->fx: %.5f  player->fy: %.5f  player->fPrecalcualtedY: %.5f\n", player->fx, player->fy, player->fPrecalculatedY);
		printf("player->fx + PW: %.5f  player->fy + PH: %.5f  player->fPrecalculatedY + PH: %.5f\n", player->fx + PW, player->fy + PH, player->fPrecalculatedY + PH);
		printf("fx: %.5f  fy: %.5f  fOldY: %.5f\n", fx - iHalfWidth, fy - iHalfHeight, fOldY - iHalfHeight);
		printf("fx + iWidth: %.5f  fy + iHeight: %.5f  fOldY + iHeight: %.5f\n",  fx + iHalfWidth, fy + iHalfHeight, fOldY + iHalfHeight);
		printf("fVelY: %.5f  fOldVelY: %.5f  player->vely: %.5f\n\n",  fVelY, fOldVelY, player->vely);
		*/
		return;
	}

	if(fDead)
		return;

	float fColVelX = player->velx - fOldVelX;
	float fColVelY = player->vely - fOldVelY;

	if(player->platform)
	{
		fColVelY += player->platform->fOldVelY - GRAVITATION;
		
		if(player->velx < -0.6f || player->velx > 0.6f)
			fColVelX += player->platform->fOldVelX;

		/*
		if(player->platform != this)
			fColVelX += player->platform->fOldVelX;
		else if(player->platform == this && (player->velx < -0.6f || player->velx > 0.6f))
			fColVelX += player->platform->fOldVelX;
		*/
	}
	else if(!player->inair)
	{
		fColVelY -= GRAVITATION;  //Ignore the gravity if the player is on the ground
	}

	float fRelativeY1;
	float fRelativeY2;

	if(player->platform != this)
	{
		fRelativeY1 = player->fy - fOldY + iHalfHeight;
		fRelativeY2 = player->fy + PH - fOldY + iHalfHeight;
	}
	else
	{
		fRelativeY1 = player->fy - fy + iHalfHeight;
		fRelativeY2 = player->fy + PH - fy + iHalfHeight;
	}

	/*
	printf("\n>>>>Platform Collision! fCorVelX: %.5f  fCorVelY: %.5f\n", fColVelX, fColVelY);
	printf(">>>>player->fx: %.5f  player->fy: %.5f  player->fPrecalcualtedY: %.5f\n", player->fx, player->fy, player->fPrecalculatedY);
	printf(">>>>player->fx + PW: %.5f  player->fy + PH: %.5f  player->fPrecalculatedY + PH: %.5f\n", player->fx + PW, player->fy + PH, player->fPrecalculatedY + PH);
	printf(">>>>fx: %.5f  fy: %.5f  fOldY: %.5f\n", fx - iHalfWidth, fy - iHalfHeight, fOldY - iHalfHeight);
	printf(">>>>fx + iWidth: %.5f  fy + iHeight: %.5f  fOldY + iHeight: %.5f\n",  fx + iHalfWidth, fy + iHalfHeight, fOldY + iHalfHeight);
	printf(">>>>fVelY: %.5f  fOldVelY: %.5f  player->vely: %.5f\n\n",  fVelY, fOldVelY, player->vely);
	*/

	if(fColVelX > 0.01f || player->iHorizontalPlatformCollision == 3)
	{
		float fRelativeX;

		if(player->fx + PW >= 640.0f)
			fRelativeX = player->fx + PW - 640.0f - fx + iHalfWidth;
		else
			fRelativeX = player->fx + PW - fx + iHalfWidth;

		//printf(">>>>Checking Right Side fRelativeX: %.5f  fRelativeY1: %.5f  fRelativeY2: %.5f\n", fRelativeX, fRelativeY1, fRelativeY2);

		if(fRelativeX >= 0.0f && fRelativeX < iWidth)
		{
			short tx = (short)fRelativeX / TILESIZE;
		
			int t1 = tile_flag_nonsolid;
			int t2 = tile_flag_nonsolid;

			if(fRelativeY1 >= 0.0f && fRelativeY1 < iHeight)
				t1 = iTileType[tx][(short)fRelativeY1 / TILESIZE].iFlags;

			if(fRelativeY2 >= 0.0f && fRelativeY2 < iHeight)
				t2 = iTileType[tx][(short)fRelativeY2 / TILESIZE].iFlags;

			if((t1 & tile_flag_solid) || (t2 & tile_flag_solid))
			{
				if(player->iHorizontalPlatformCollision == 3)
				{
					player->KillPlayerMapHazard(true, kill_style_environment);
					//printf("Platform Right Side Killed Player\n");
					return;
				}
				else if(((t1 & tile_flag_death_on_left) || (t2 & tile_flag_death_on_left)) &&
					!player->invincible && !player->spawninvincible)
				{
					if(player_kill_nonkill != player->KillPlayerMapHazard(true, kill_style_environment))
						return;
				}
				else
				{
					player->xf((float)(tx * TILESIZE - PW) - 0.2f + fx - iHalfWidth);
					player->flipsidesifneeded();
					
					//test to see if we put the object on top of a background tile
					short iTestBackgroundY = (short)player->fy / TILESIZE;
					short iTestBackgroundY2 = ((short)player->fy + PH) / TILESIZE;
					
					short iTestBackgroundX;

					if(player->fx + PW >= 640.0f)
						iTestBackgroundX = ((short)player->fx + PW - 640) / TILESIZE;
					else
						iTestBackgroundX = ((short)player->fx + PW) / TILESIZE;
					
					IO_Block * topblock = g_map.block(iTestBackgroundX, iTestBackgroundY);
					IO_Block * bottomblock = g_map.block(iTestBackgroundX, iTestBackgroundY2);

					if((topblock && !topblock->isTransparent() && !topblock->isHidden()) || 
						(bottomblock && !bottomblock->isTransparent() && !bottomblock->isHidden()) ||
						(g_map.map(iTestBackgroundX, iTestBackgroundY) & tile_flag_solid)|| 
						(g_map.map(iTestBackgroundX, iTestBackgroundY2) & tile_flag_solid))
					{
						player->xf((float)(iTestBackgroundX * TILESIZE - PW) - 0.2f);
						player->flipsidesifneeded();
					}

					if(fOldVelX < 0.0f)
						player->fOldX = fx + 10.0f;  //Need to push out the old enough to collide correctly with flip blocks
					else
						player->fOldX = fx - 10.0f;  //Need to push out the old enough to collide correctly with flip blocks
					
					player->iHorizontalPlatformCollision = 1;

					if(player->velx > 0.0f)
						player->velx = 0.0f;

					//printf("Hit Platform Right Side\n");
				}
			}
		}
	}
	else if(fColVelX < -0.01f || player->iHorizontalPlatformCollision == 1)
	{
		float fRelativeX = player->fx - fx + iHalfWidth;
		
		//printf(">>>>Checking Left Side fRelativeX: %.5f  fRelativeY1: %.5f  fRelativeY2: %.5f\n", fRelativeX, fRelativeY1, fRelativeY2);

		if(fRelativeX >= 0.0f && fRelativeX < iWidth)
		{
			short tx = (short)fRelativeX / TILESIZE;

			int t1 = tile_flag_nonsolid;
			int t2 = tile_flag_nonsolid;

			if(fRelativeY1 >= 0 && fRelativeY1 < iHeight)
				t1 = iTileType[tx][(short)fRelativeY1 / TILESIZE].iFlags;

			if(fRelativeY2 >= 0 && fRelativeY2 < iHeight)
				t2 = iTileType[tx][(short)fRelativeY2 / TILESIZE].iFlags;

			if((t1 & tile_flag_solid) || (t2 & tile_flag_solid))
			{
				if(player->iHorizontalPlatformCollision == 1)
				{
					player->KillPlayerMapHazard(true, kill_style_environment);
					//printf("Platform Left Side Killed Player\n");
					return;				
				}
				else if(((t1 & tile_flag_death_on_right) || (t2 & tile_flag_death_on_right)) &&
					!player->invincible && !player->spawninvincible)
				{
					if(player_kill_nonkill != player->KillPlayerMapHazard(true, kill_style_environment))
						return;				
				}
				else
				{
					player->xf((float)(tx * TILESIZE + TILESIZE) + 0.2f + fx - iHalfWidth);
					player->flipsidesifneeded();
					
					//test to see if we put the object on top of a background tile
					short iTestBackgroundY = (short)player->fy / TILESIZE;
					short iTestBackgroundY2 = ((short)player->fy + PH) / TILESIZE;
					short iTestBackgroundX = (short)player->fx / TILESIZE;
					
					IO_Block * topblock = g_map.block(iTestBackgroundX, iTestBackgroundY);
					IO_Block * bottomblock = g_map.block(iTestBackgroundX, iTestBackgroundY2);

					if((topblock && !topblock->isTransparent() && !topblock->isHidden()) || 
						(bottomblock && !bottomblock->isTransparent() && !bottomblock->isHidden()) ||
						(g_map.map(iTestBackgroundX, iTestBackgroundY) & tile_flag_solid) || 
						(g_map.map(iTestBackgroundX, iTestBackgroundY2) & tile_flag_solid))
					{
						player->xf((float)(iTestBackgroundX * TILESIZE + TILESIZE) + 0.2f);
						player->flipsidesifneeded();
					}


					if(fOldVelX < 0.0f)
						player->fOldX = fx + 10.0f;  //Need to push out the old enough to collide correctly with flip blocks
					else
						player->fOldX = fx - 10.0f;  //Need to push out the old enough to collide correctly with flip blocks
					
					player->iHorizontalPlatformCollision = 3;

					if(player->velx < 0.0f)
						player->velx = 0.0f;

					//printf("Hit Platform Left Side\n");
				}
			}
		}
	}

	float fRelativeX1 = player->fx - fx + iHalfWidth;

	float fRelativeX2;
	if(player->fx + PW > 640.0f)
		fRelativeX2 = player->fx + PW - 640.0f - fx + iHalfWidth;
	else
		fRelativeX2 = player->fx + PW - fx + iHalfWidth;

	if(fColVelY < 0.0f)
	{
		float fRelativeY;

		if(player->inair || player->platform == this)
			fRelativeY = player->fPrecalculatedY - fy + iHalfHeight;
		else
			fRelativeY = player->fy - fy + iHalfHeight;
		
		//printf(">>>>Checking Bottom Side fRelativeY: %.5f  fRelativeX1: %.5f  fRelativeX2: %.5f\n", fRelativeY, fRelativeX1, fRelativeX2);

		if(fRelativeY >= 0.0f && fRelativeY < iHeight)
		{
			short ty = (short)fRelativeY / TILESIZE;

			int t1 = tile_flag_nonsolid;
			int t2 = tile_flag_nonsolid;

			if(fRelativeX1 >= 0.0f && fRelativeX1 < iWidth)
				t1 = iTileType[(short)fRelativeX1 / TILESIZE][ty].iFlags;

			if(fRelativeX2 >= 0.0f && fRelativeX2 < iWidth)
				t2 = iTileType[(short)fRelativeX2 / TILESIZE][ty].iFlags;
		
			if(((t1 & tile_flag_solid) || (t2 & tile_flag_solid)) && (((t1 & tile_flag_death_on_bottom) == 0 && (t2 & tile_flag_death_on_bottom) == 0) || 
				player->invincible || player->spawninvincible))
			{
				if(player->iVerticalPlatformCollision == 2)
				{
					player->KillPlayerMapHazard(true, kill_style_environment);
					//printf("Platform Bottom Killed Player\n");
					return;
				}
				else
				{
					player->fPrecalculatedY = ty * TILESIZE + TILESIZE + 0.2f + fy - iHalfHeight;
					player->fOldY = player->fPrecalculatedY - fVelY - GRAVITATION;

					if(player->vely < 0.0f && fVelY > 0.0f)
						player->fPrecalculatedY += fVelY;

					if(player->vely < 0.0f)
						player->vely = CapFallingVelocity(-player->vely * BOUNCESTRENGTH + fVelY);

					player->iVerticalPlatformCollision = 0;

					//printf("Hit Platform Bottom\n");
				}

				return;
			}
			else if((t1 & tile_flag_death_on_bottom) || (t2 & tile_flag_death_on_bottom))
			{
				if(player_kill_nonkill != player->KillPlayerMapHazard(true, kill_style_environment))
					return;
			}
		}
	}
	else
	{
		float fRelativeY = player->fPrecalculatedY + PH - fy + iHalfHeight;

		//printf(">>>>Checking Top Side fRelativeY: %.5f  fRelativeX1: %.5f  fRelativeX2: %.5f\n", fRelativeY, fRelativeX1, fRelativeX2);

		if(fRelativeY >= 0.0f && fRelativeY < iHeight)
		{
			short ty = (short)fRelativeY / TILESIZE;

			int t1 = tile_flag_nonsolid;
			int t2 = tile_flag_nonsolid;

			if(fRelativeX1 >= 0.0f && fRelativeX1 < iWidth)
				t1 = iTileType[(short)fRelativeX1 / TILESIZE][ty].iFlags;

			if(fRelativeX2 >= 0.0f && fRelativeX2 < iWidth)
				t2 = iTileType[(short)fRelativeX2 / TILESIZE][ty].iFlags;

			bool fSolidTileUnderPlayer = (t1 & tile_flag_solid) || (t2 & tile_flag_solid);

			if((t1 & tile_flag_solid_on_top || t2 & tile_flag_solid_on_top) && player->fOldY + PH <= ty * TILESIZE + fOldY - iHalfHeight)
			{
				//Deal with player down jumping through solid on top tiles
				if(player->fallthrough && !fSolidTileUnderPlayer)
				{
					player->fPrecalculatedY = ty * TILESIZE - PH + (fVelY > 0.0f ? fVelY : 0.0f) + 0.2f + fy - iHalfHeight;
					player->inair = true;
					player->platform = NULL;
					//printf("Fell Through Solid On Top Platform\n");
				}
				else
				{
					if(player->iVerticalPlatformCollision == 0)
					{
						player->KillPlayerMapHazard(true, kill_style_environment);
						//printf("Solid On Top Platform Killed Player\n");
						return;
					}
					else
					{
						player->platform = this;
						player->iVerticalPlatformCollision = 2;

						player->fPrecalculatedY = ty * TILESIZE - PH - 0.2f + fy - iHalfHeight;
						player->vely = GRAVITATION;
						player->inair = false;
						player->killsinrowinair = 0;
						player->extrajumps = 0;
						//printf("Hit Solid On Top Platform\n");
					}
				}

				player->fOldY = player->fPrecalculatedY - fVelY;
				player->fallthrough = false;
				player->onice = false;
				
				return;
			}
			else if(fSolidTileUnderPlayer && (((t1 & tile_flag_death_on_top) == 0 && (t2 & tile_flag_death_on_top) == 0) ||  
				player->invincible || player->spawninvincible || player->fKuriboShoe))
			{	//on ground

				if(player->iVerticalPlatformCollision == 0)
				{
					player->KillPlayerMapHazard(true, kill_style_environment);
					//printf("Platform Top Killed Player\n");
					return;
				}
				else
				{
					player->platform = this;
					player->iVerticalPlatformCollision = 2;

					player->fPrecalculatedY = ty * TILESIZE - PH - 0.2f + fy - iHalfHeight;
					player->fOldY = player->fPrecalculatedY - fVelY;
					
					player->vely = GRAVITATION;
					player->inair = false;
					player->killsinrowinair = 0;
					player->extrajumps = 0;
					
					if((t1 & tile_flag_ice && ((t2 & tile_flag_ice) || t2 == tile_flag_nonsolid || t2 == tile_flag_gap)) ||
						(t2 & tile_flag_ice && ((t1 & tile_flag_ice) || t1 == tile_flag_nonsolid || t1 == tile_flag_gap)))
						player->onice = true;
					else 
						player->onice = false;
					
					player->fallthrough = false;

					//printf("Hit Solid Top Platform\n");

					return;
				}
			}
			else if(t1 & tile_flag_death_on_top || t2 & tile_flag_death_on_top)
			{
				if(player_kill_nonkill != player->KillPlayerMapHazard(true, kill_style_environment))
					return;
			}
			else
			{
				if(player->platform == this)
					player->platform = NULL;
			}
		}
	}
}

bool MovingPlatform::coldec_player(CPlayer * player)
{
	//Special cases to deal with players overlapping the right and left sides of the screen
	if(player->fx + PW < fx - iHalfWidth)
	{
		if (player->fx + 640.0f >= fx + iHalfWidth || player->fx + PW + 640.0f < fx - iHalfWidth || player->fPrecalculatedY >= fy + iHalfHeight || player->fPrecalculatedY + PH < fy - iHalfHeight)
			return false;
		else 
			return true;
	}
	else if(fx + iHalfWidth < player->fx)
	{
		if (player->fx >= fx + iHalfWidth + 640.0f || player->fx + PW < fx - iHalfWidth + 640.0f || player->fPrecalculatedY >= fy + iHalfHeight || player->fPrecalculatedY + PH < fy - iHalfHeight) 
			return false;
		else 
			return true;
	}
	else //Normal case where no overlap
	{
		if (player->fx >= fx + iHalfWidth || fx - iHalfWidth > player->fx + PW || player->fPrecalculatedY >= fy + iHalfHeight || fy - iHalfHeight > player->fPrecalculatedY + PH) 
		{
			//printf("No Platform Collision: %.5f >= %.5f || %.5f > %.5f || %.5f >= %.5f || %.5f > %.5f\n", player->fx, fx + iHalfWidth, fx - iHalfWidth, player->fx + PW, player->fPrecalculatedY, fy + iHalfHeight, fy - iHalfHeight, player->fPrecalculatedY + PH);
			return false;
		}
		else 
			return true;
	}
}

void MovingPlatform::gettiletypes(CPlayer * player, int * lefttile, int * righttile)
{
	*lefttile = tile_flag_nonsolid;
	*righttile = tile_flag_nonsolid;

	float fRelativeY = player->fPrecalculatedY + PH - fy + iHalfHeight;

	if(fRelativeY < 0.0f || fRelativeY >= iHeight)
		return;

	short ty = (short)fRelativeY / TILESIZE;

	float fRelativeX1 = player->fx - fx + iHalfWidth;
	
	float fRelativeX2;
	if(player->fx + PW > 640.0f)
		fRelativeX2 = player->fx + PW - 640.0f - fx + iHalfWidth;
	else
		fRelativeX2 = player->fx + PW - fx + iHalfWidth;

	if(fRelativeX1 >= 0.0f && fRelativeX1 < iWidth)
		*lefttile = iTileType[(short)fRelativeX1 / TILESIZE][ty].iFlags;

	if(fRelativeX2 >= 0.0f && fRelativeX2 < iWidth)
		*righttile = iTileType[(short)fRelativeX2 / TILESIZE][ty].iFlags;
}

void MovingPlatform::collide(IO_MovingObject * object)
{
	if(!coldec_object(object))
	{
		if(object->platform == this)
			object->platform = NULL;

		return;
	}

	if(fDead)
		return;

	float fColVelX = object->velx - fOldVelX;
	float fColVelY = object->vely - fOldVelY;

	if(object->platform)
	{
		fColVelY += object->platform->fOldVelY - GRAVITATION;

		if(object->velx < -0.6f || object->velx > 0.6f)
			fColVelX += object->platform->fOldVelX;

		/*
		if(object->platform != this)
			fColVelX += object->platform->fOldVelX;
		else if(object->platform == this && object->velx != 0.0f)
			fColVelX += object->platform->fOldVelX;
			*/
	}
	else if(!object->inair)
	{
		fColVelY -= GRAVITATION;  //Ignore the gravity if the object is on the ground
	}

	float fRelativeY1;
	float fRelativeY2;

	if(object->platform != this)
	{
		fRelativeY1 = object->fy - fOldY + iHalfHeight;
		fRelativeY2 = object->fy + object->collisionHeight - fOldY + iHalfHeight;
	}
	else
	{
		fRelativeY1 = object->fy - fy + iHalfHeight;
		fRelativeY2 = object->fy + object->collisionHeight - fy + iHalfHeight;
	}
	
	if(fColVelX > 0.01f || object->iHorizontalPlatformCollision == 3)
	{
		float fRelativeX;
		if(object->fx + object->collisionWidth > 640.0f)
			fRelativeX = object->fx + object->collisionWidth - 640.0f - fx + iHalfWidth;
		else
			fRelativeX = object->fx + object->collisionWidth - fx + iHalfWidth;

		if(fRelativeX >= 0.0f && fRelativeX < iWidth)
		{
			short tx = (short)fRelativeX / TILESIZE;
		
			int t1 = tile_flag_nonsolid;
			int t2 = tile_flag_nonsolid;

			if(fRelativeY1 >= 0.0f && fRelativeY1 < iHeight)
				t1 = iTileType[tx][(short)fRelativeY1 / TILESIZE].iFlags;

			if(fRelativeY2 >= 0.0f && fRelativeY2 < iHeight)
				t2 = iTileType[tx][(short)fRelativeY2 / TILESIZE].iFlags;

			if((t1 & tile_flag_solid) || (t2 & tile_flag_solid))
			{
				if(object->iHorizontalPlatformCollision == 3)
				{
					object->KillObjectMapHazard();
					return;
				}
				else
				{
					object->xf((float)(tx * TILESIZE - object->collisionWidth) - 0.2f + fx - iHalfWidth);
					object->flipsidesifneeded();
					
					//test to see if we put the object on top of a background tile
					short iTestBackgroundY = (short)object->fy / TILESIZE;
					short iTestBackgroundY2 = ((short)object->fy + object->collisionHeight) / TILESIZE;
					
					short iTestBackgroundX;
					if(object->fx + object->collisionWidth >= 640.0f)
						iTestBackgroundX = ((short)object->fx + object->collisionWidth - 640) / TILESIZE;
					else
						iTestBackgroundX = ((short)object->fx + object->collisionWidth) / TILESIZE;
					
					IO_Block * topblock = g_map.block(iTestBackgroundX, iTestBackgroundY);
					IO_Block * bottomblock = g_map.block(iTestBackgroundX, iTestBackgroundY2);

					if((topblock && !topblock->isTransparent() && !topblock->isHidden()) || 
						(bottomblock && !bottomblock->isTransparent() && !bottomblock->isHidden()) ||
						(g_map.map(iTestBackgroundX, iTestBackgroundY) & tile_flag_solid) || 
						(g_map.map(iTestBackgroundX, iTestBackgroundY2) & tile_flag_solid))
					{
						object->xf((float)(iTestBackgroundX * TILESIZE - object->collisionWidth) - 0.2f);
						object->flipsidesifneeded();
					}

					if(fOldVelX < 0.0f)
						object->fOldX = fx + 10.0f;  //Need to push out the old enough to collide correctly with flip blocks
					else
						object->fOldX = fx - 10.0f;  //Need to push out the old enough to collide correctly with flip blocks
					
					object->iHorizontalPlatformCollision = 1;

					if(object->velx > 0.0f)
						object->velx = -object->velx;

					removeifprojectile(object, true, true);

					object->SideBounce();
				}
			}
		}
	}
	else if(fColVelX < -0.01f || object->iHorizontalPlatformCollision == 1)
	{
		float fRelativeX = object->fx - fx + iHalfWidth;
		
		if(fRelativeX >= 0.0f && fRelativeX < iWidth)
		{
			short tx = (short)fRelativeX / TILESIZE;

			int t1 = tile_flag_nonsolid;
			int t2 = tile_flag_nonsolid;

			if(fRelativeY1 >= 0 && fRelativeY1 < iHeight)
				t1 = iTileType[tx][(short)fRelativeY1 / TILESIZE].iFlags;

			if(fRelativeY2 >= 0 && fRelativeY2 < iHeight)
				t2 = iTileType[tx][(short)fRelativeY2 / TILESIZE].iFlags;

			if((t1 & tile_flag_solid) || (t2 & tile_flag_solid))
			{
				if(object->iHorizontalPlatformCollision == 1)
				{
					object->KillObjectMapHazard();
					return;
				}
				else
				{
					object->xf((float)(tx * TILESIZE + TILESIZE) + 0.2f + fx - iHalfWidth);
					object->flipsidesifneeded();

					//test to see if we put the object on top of a background tile
					short iTestBackgroundY = (short)object->fy / TILESIZE;
					short iTestBackgroundY2 = ((short)object->fy + object->collisionHeight) / TILESIZE;
					short iTestBackgroundX = (short)object->fx / TILESIZE;
					
					IO_Block * topblock = g_map.block(iTestBackgroundX, iTestBackgroundY);
					IO_Block * bottomblock = g_map.block(iTestBackgroundX, iTestBackgroundY2);

					if((topblock && !topblock->isTransparent() && !topblock->isHidden()) || 
						(bottomblock && !bottomblock->isTransparent() && !bottomblock->isHidden()) ||
						(g_map.map(iTestBackgroundX, iTestBackgroundY) & tile_flag_solid) || 
						(g_map.map(iTestBackgroundX, iTestBackgroundY2) & tile_flag_solid))
					{
						object->xf((float)(iTestBackgroundX * TILESIZE + TILESIZE) + 0.2f);
						object->flipsidesifneeded();
					}

					if(fOldVelX < 0.0f)
						object->fOldX = fx + 10.0f;  //Need to push out the old enough to collide correctly with flip blocks
					else
						object->fOldX = fx - 10.0f;  //Need to push out the old enough to collide correctly with flip blocks
					
					object->iHorizontalPlatformCollision = 3;

					if(object->velx < 0.0f)
						object->velx = -object->velx;

					removeifprojectile(object, true, true);

					object->SideBounce();
				}
			}
		}
	}

	float fRelativeX1 = object->fx - fx + iHalfWidth;

	float fRelativeX2;
	if(object->fx + object->collisionWidth > 640.0f)
		fRelativeX2 = object->fx + object->collisionWidth - 640.0f - fx + iHalfWidth;
	else
		fRelativeX2 = object->fx + object->collisionWidth - fx + iHalfWidth;

	if(fColVelY < 0.0f)
	{
		float fRelativeY;

		if(object->inair || object->platform == this)
			fRelativeY = object->fPrecalculatedY - fy + iHalfHeight;
		else
			fRelativeY = object->fy - fy + iHalfHeight;
		
		if(fRelativeY >= 0.0f && fRelativeY < iHeight)
		{
			short ty = (short)fRelativeY / TILESIZE;

			int t1 = tile_flag_nonsolid;
			int t2 = tile_flag_nonsolid;

			if(fRelativeX1 >= 0.0f && fRelativeX1 < iWidth)
				t1 = iTileType[(short)fRelativeX1 / TILESIZE][ty].iFlags;

			if(fRelativeX2 >= 0.0f && fRelativeX2 < iWidth)
				t2 = iTileType[(short)fRelativeX2 / TILESIZE][ty].iFlags;
		
			if((t1 & tile_flag_solid) || (t2 & tile_flag_solid))
			{
				if(object->iVerticalPlatformCollision == 2)
				{
					object->KillObjectMapHazard();
					return;
				}
				else
				{
					object->fPrecalculatedY = ty * TILESIZE + TILESIZE + 0.2f + fy - iHalfHeight;
					object->fOldY = object->fPrecalculatedY - fVelY - GRAVITATION;

					if(object->vely < 0.0f && fVelY > 0.0f)
						object->fPrecalculatedY += fVelY;

					if(object->vely < 0.0f)
						object->vely = CapFallingVelocity(-object->vely + fVelY);

					object->iVerticalPlatformCollision = 0;
				}

				return;
			}
		}
	}
	else
	{
		float fRelativeY = object->fPrecalculatedY + object->collisionHeight - fy + iHalfHeight;;

		if(fRelativeY >= 0.0f && fRelativeY < iHeight)
		{
			short ty = (short)fRelativeY / TILESIZE;

			int t1 = tile_flag_nonsolid;
			int t2 = tile_flag_nonsolid;

			if(fRelativeX1 >= 0.0f && fRelativeX1 < iWidth)
				t1 = iTileType[(short)fRelativeX1 / TILESIZE][ty].iFlags;

			if(fRelativeX2 >= 0.0f && fRelativeX2 < iWidth)
				t2 = iTileType[(short)fRelativeX2 / TILESIZE][ty].iFlags;

			if(((t1 & tile_flag_solid_on_top) || (t2 & tile_flag_solid_on_top)) && object->fOldY + object->collisionHeight <= ty * TILESIZE + fOldY - iHalfHeight)
			{
				if(object->iVerticalPlatformCollision == 0)
				{
					object->KillObjectMapHazard();
					return;
				}
				else
				{
					if(object->bounce >= 0.0f)
						object->platform = this;

					object->iVerticalPlatformCollision = 2;

					object->fPrecalculatedY = ty * TILESIZE - object->collisionHeight - 0.2f + fy - iHalfHeight;
					object->fOldY = object->fPrecalculatedY - fVelY;
					object->vely = object->BottomBounce();
					object->inair = false;
					object->onice = false;
				}

				return;
			}
			else if((t1 & tile_flag_solid) || (t2 & tile_flag_solid))
			{	//on ground
				if(object->iVerticalPlatformCollision == 0)
				{
					object->KillObjectMapHazard();
					return;
				}
				else
				{
					if(object->bounce >= 0.0f)
						object->platform = this;

					object->iVerticalPlatformCollision = 2;

					object->fPrecalculatedY = ty * TILESIZE - object->collisionHeight - 0.2f + fy - iHalfHeight;
					object->fOldY = object->fPrecalculatedY - fVelY;
					object->vely = object->BottomBounce();
					object->inair = false;
					
					if((t1 & tile_flag_ice && ((t2 & tile_flag_ice) || t2 == tile_flag_nonsolid || t2 == tile_flag_gap)) ||
						(t2 & tile_flag_ice && ((t1 & tile_flag_ice) || t1 == tile_flag_nonsolid || t1 == tile_flag_gap)))
						object->onice = true;
					else 
						object->onice = false;
					
					return;
				}
			}
			else
			{
				if(object->platform == this)
					object->platform = NULL;
			}
		}
	}
}

bool MovingPlatform::coldec_object(IO_MovingObject * object)
{
	//Special cases to deal with players overlapping the right and left sides of the screen
	if(object->fx + object->collisionWidth < fx - iHalfWidth)
	{
		if (object->fx + 640.0f >= fx + iHalfWidth || object->fx + object->collisionWidth + 640.0f < fx - iHalfWidth || object->fPrecalculatedY >= fy + iHalfHeight || object->fPrecalculatedY + object->collisionHeight < fy - iHalfHeight)
			return false;
		else 
			return true;
	}
	else if(fx + iHalfWidth < object->fx)
	{
		if (object->fx >= fx + iHalfWidth + 640.0f || object->fx + object->collisionWidth < fx - iHalfWidth + 640.0f || object->fPrecalculatedY >= fy + iHalfHeight || object->fPrecalculatedY + object->collisionHeight < fy - iHalfHeight) 
			return false;
		else 
			return true;
	}
	else //Normal case where no overlap
	{
		//if(object->inair || object->platform == this)
		//{
			if (object->fx >= fx + iHalfWidth || fx - iHalfWidth > object->fx + object->collisionWidth || object->fPrecalculatedY >= fy + iHalfHeight || fy - iHalfHeight > object->fPrecalculatedY + object->collisionHeight) 
				return false;
			else 
				return true;
		/*
		}
		else //special case where object is hit when sitting on a tile
		{
			if (object->fx >= fx + iHalfWidth || fx - iHalfWidth > object->fx + object->collisionWidth || object->fy >= fy + iHalfHeight || fy - iHalfHeight > object->fy + object->collisionHeight) 
				return false;
			else 
				return true;
		}
		*/
	}
}

void MovingPlatform::CalculateNoSpawnZone(float dPathTime)
{
	float fXArea = fabs(fVelX) * dPathTime + iHalfWidth;
	float fYArea = fabs(fVelY) * dPathTime + iHalfHeight;

	iNoSpawnZoneLeft = (short)(fx - fXArea);
	iNoSpawnZoneRight = (short)(fx + fXArea);

	iNoSpawnZoneTop = (short)(fy - fYArea);
	iNoSpawnZoneBottom = (short)(fy + fYArea);
}

bool MovingPlatform::IsInNoSpawnZone(short x, short y, short w, short h)
{
	if(x + w < iNoSpawnZoneLeft || x >= iNoSpawnZoneRight ||
		y + h < iNoSpawnZoneTop || y >= iNoSpawnZoneBottom)
		return false;

	return true;
}

