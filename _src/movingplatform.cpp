#include "global.h"
#include <math.h>

extern short x_shake;
extern short y_shake;

//------------------------------------------------------------------------------
// Moving Platform
//------------------------------------------------------------------------------

MovingPlatformPath::MovingPlatformPath(float vel, float startX, float startY, float endX, float endY, bool falling)
{
	fVelocity = vel;
	fStartX = startX;
	fStartY = startY;
	fEndX = endX;
	fEndY = endY;
	fFalling = falling;

	CalculateAngle();
}

MovingPlatformPath::MovingPlatformPath()
{
	//Give it some default values for a vertical slow moving platform
	fVelocity = 1.0f;
	fStartX = 304.0f;
	fStartY = 48.0f;
	fEndX = 304.0f;
	fEndY = 432.0f;
	fFalling = false;

	CalculateAngle();
}

MovingPlatformPath::~MovingPlatformPath()
{}

void MovingPlatformPath::CalculateAngle()
{
	if(fEndX - fStartX == 0)
	{
		if(fEndY - fStartY > 0)
			fAngle = HALF_PI;
		else
			fAngle = THREE_HALF_PI;
	}
	else if(fEndY - fStartY == 0)
	{
		if(fEndX - fStartX > 0)
			fAngle = 0.0f;
		else
			fAngle = PI;
	}
	else
	{
		fAngle = atan2(fEndY - fStartY, fEndX - fStartX);
	}
}


MovingPlatform::MovingPlatform(TilesetTile ** tiledata, TileType ** tiletypes, short w, short h, MovingPlatformPath * path, bool forwardDirection, short startPathNode, bool fPreview)
{
	fDead = false;

	short iTileSize = TILESIZE;
	short iTileSizeIndex = 0;

	if(fPreview)
	{
		iTileSize = PREVIEWTILESIZE;
		iTileSizeIndex = 1;

		path->fEndX /= 2.0f;
		path->fEndY /= 2.0f;
		path->fStartX /= 2.0f;
		path->fStartY /= 2.0f;
		path->fVelocity /= 2.0f;
	}

	iTileData = tiledata;
	iTileType = tiletypes;

	iTileWidth = w;
	iTileHeight = h;

	iWidth = w * iTileSize;
	iHeight = h * iTileSize;

	iHalfWidth = iWidth >> 1;
	iHalfHeight = iHeight >> 1;
	
	xf(path->fStartX);
	yf(path->fStartY);

	fOldX = fx;
	fOldY = fy;

	fStartDirection = forwardDirection;
	fForwardDirection = fStartDirection;

	iStartPathNode = startPathNode;

	pPath = path;

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

	if(forwardDirection)
	{
		fVelX = pPath->fVelocity * cos(pPath->fAngle);
		fVelY = pPath->fVelocity * sin(pPath->fAngle);
	}
	else
	{
		fVelX = -pPath->fVelocity * cos(pPath->fAngle);
		fVelY = -pPath->fVelocity * sin(pPath->fAngle);
	}

	fOldVelX = fVelX;
	fOldVelY = fVelY;
	
	fEndPointX = forwardDirection ? path->fEndX : path->fStartX;
	fEndPointY = forwardDirection ? path->fEndY : path->fStartY;
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

	// Blit onto the screen surface
	if(SDL_BlitSurface(sSurface, &rSrcRect, blitdest, &rDstRect) < 0)
	{
		fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
		return;
	}

	//We won't worry about wrapping the sides of the screen for this release
	/*
	//Deal with wrapping both x and y directions
	if(ix - iHalfWidth < 0)
	{
		rDstRect.x = ix - iHalfWidth + 640;
		rDstRect.y = iy - iHalfHeight;

		if(SDL_BlitSurface(sSurface, &rSrcRect, blitdest, &rDstRect) < 0)
		{
			fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
		}

		rDstRect.x = ix - iHalfWidth;
	}
	else if(ix + iHalfWidth >= 640)
	{
		rDstRect.x = ix - iHalfWidth - 640;
		rDstRect.y = iy - iHalfHeight;

		if(SDL_BlitSurface(sSurface, &rSrcRect, blitdest, &rDstRect) < 0)
		{
			fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
		}

		rDstRect.x = ix - iHalfWidth;
	}

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
	//Evaluate where to move platform based on path
	
	//if the path is a line, calculate the next position
	
	fOldX = fx;
	fOldY = fy;
	
	//If the path is a straight line, we don't need to calculate the adjustment each frame!
	/*
	if(fForwardDirection)
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

	fOldVelX = fVelX;
	fOldVelY = fVelY;

	fx += fVelX;
	fy += fVelY;

	if(pPath->fFalling)
	{
		fVelY = CapFallingVelocity(fVelY + GRAVITATION);
		
		if(fy - iHalfHeight > 480.0f)
		{
			//If a player is standing on this platform, clear him off
			for(short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++)
			{
				if(list_players[iPlayer]->platform == this)
				{
					list_players[iPlayer]->platform = NULL;
					list_players[iPlayer]->vely = fVelY;
				}
			}

			fDead = true;
		}
	}
	else
	{
		//See if we're at the end of the path
		if(fVelX < -0.01f)
		{
			if(fVelY < -0.01f)
			{
				if(fx <= fEndPointX && fy <= fEndPointY)
				{
					xf(fEndPointX);
					yf(fEndPointY);
					FlipDirection();
				}
			}
			else if(fVelY > 0.01f)
			{
				if(fx <= fEndPointX && fy >= fEndPointY)
				{
					xf(fEndPointX);
					yf(fEndPointY);
					FlipDirection();
				}
			}
			else
			{
				if(fx <= fEndPointX)
				{
					xf(fEndPointX);
					yf(fEndPointY);
					FlipDirection();
				}
			}
		}
		else if(fVelX > 0.01f)
		{
			if(fVelY < -0.01f)
			{
				if(fx >= fEndPointX && fy <= fEndPointY)
				{
					xf(fEndPointX);
					yf(fEndPointY);
					FlipDirection();
				}
			}
			else if(fVelY > 0.01f)
			{
				if(fx >= fEndPointX && fy >= fEndPointY)
				{
					xf(fEndPointX);
					yf(fEndPointY);
					FlipDirection();
				}
			}
			else
			{
				if(fx >= fEndPointX)
				{
					xf(fEndPointX);
					yf(fEndPointY);
					FlipDirection();
				}
			}
		}
		else
		{
			if(fVelY > 0.01f)
			{
				if(fy >= fEndPointY)
				{
					xf(fEndPointX);
					yf(fEndPointY);
					FlipDirection();
				}
			}
			else if(fVelY < -0.01f)
			{
				if(fy <= fEndPointY)
				{
					xf(fEndPointX);
					yf(fEndPointY);
					FlipDirection();
				}
			}
			else
			{
				//Platform is not moving!
				printf("Platform is not moving.  This is probably a problem!\n");
			}
		}
	}

	ix = (short)fx;
	iy = (short)fy;
}

void MovingPlatform::ResetPath()
{
	xf(pPath->fStartX);
	yf(pPath->fStartY);

	fOldX = fx;
	fOldY = fy;

	fForwardDirection = fStartDirection;
}

void MovingPlatform::FlipDirection()
{
	fForwardDirection = !fForwardDirection;

	fEndPointX = fForwardDirection ? pPath->fEndX : pPath->fStartX;
	fEndPointY = fForwardDirection ? pPath->fEndY : pPath->fStartY;

	if(fForwardDirection)
	{
		fVelX = pPath->fVelocity * cos(pPath->fAngle);
		fVelY = pPath->fVelocity * sin(pPath->fAngle);
	}
	else
	{
		fVelX = -pPath->fVelocity * cos(pPath->fAngle);
		fVelY = -pPath->fVelocity * sin(pPath->fAngle);
	}

	//Fix rounding errors
	if(fVelX < 0.01f && fVelX > -0.01f)
		fVelX = 0.0f;

	if(fVelY < 0.01f && fVelY > -0.01f)
		fVelY = 0.0f;

	//printf("Platform Flipped Directions: velx: %.5f  vely: %.5f  angle: %.5f\n", fVelX, fVelY, pPath->fAngle);
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
		
			TileType t1 = tile_nonsolid;
			TileType t2 = tile_nonsolid;

			if(fRelativeY1 >= 0.0f && fRelativeY1 < iHeight)
				t1 = iTileType[tx][(short)fRelativeY1 / TILESIZE];

			if(fRelativeY2 >= 0.0f && fRelativeY2 < iHeight)
				t2 = iTileType[tx][(short)fRelativeY2 / TILESIZE];

			if((t1 & 13) || (t2 & 13))
			{
				if(player->iHorizontalPlatformCollision == 3 || ((t1 == tile_death || t2 == tile_death || t1 == tile_death_on_left || t2 == tile_death_on_left) &&
					(!player->invincible && !player->spawninvincible)))
				{
					player->KillPlayerMapHazard();
					//printf("Platform Right Side Killed Player\n");
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

					if((topblock && !topblock->isTransparent()) || 
						(bottomblock && !bottomblock->isTransparent()) ||
						(g_map.map(iTestBackgroundX, iTestBackgroundY) & 13)|| 
						(g_map.map(iTestBackgroundX, iTestBackgroundY2) & 13))
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

			TileType t1 = tile_nonsolid;
			TileType t2 = tile_nonsolid;

			if(fRelativeY1 >= 0 && fRelativeY1 < iHeight)
				t1 = iTileType[tx][(short)fRelativeY1 / TILESIZE];

			if(fRelativeY2 >= 0 && fRelativeY2 < iHeight)
				t2 = iTileType[tx][(short)fRelativeY2 / TILESIZE];

			if((t1 & 13) || (t2 & 13))
			{
				if(player->iHorizontalPlatformCollision == 1 || ((t1 == tile_death || t2 == tile_death || t1 == tile_death_on_right || t2 == tile_death_on_right) &&
					(!player->invincible && !player->spawninvincible)))
				{
					player->KillPlayerMapHazard();
					//printf("Platform Left Side Killed Player\n");
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

					if((topblock && !topblock->isTransparent()) || 
						(bottomblock && !bottomblock->isTransparent()) ||
						(g_map.map(iTestBackgroundX, iTestBackgroundY) & 13) || 
						(g_map.map(iTestBackgroundX, iTestBackgroundY2) & 13))
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

			TileType t1 = tile_nonsolid;
			TileType t2 = tile_nonsolid;

			if(fRelativeX1 >= 0.0f && fRelativeX1 < iWidth)
				t1 = iTileType[(short)fRelativeX1 / TILESIZE][ty];

			if(fRelativeX2 >= 0.0f && fRelativeX2 < iWidth)
				t2 = iTileType[(short)fRelativeX2 / TILESIZE][ty];
		
			if((t1 & 1) || (t2 & 1) || t1 == tile_death_on_right || t2 == tile_death_on_right ||
				((player->invincible || player->spawninvincible) && ((t1 & 4) || (t2 & 4))))
			{
				if(player->iVerticalPlatformCollision == 2)
				{
					player->KillPlayerMapHazard();
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
			else if((t1 & 4) || (t2 & 4))
			{
				player->KillPlayerMapHazard();
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

			TileType t1 = tile_nonsolid;
			TileType t2 = tile_nonsolid;

			if(fRelativeX1 >= 0.0f && fRelativeX1 < iWidth)
				t1 = iTileType[(short)fRelativeX1 / TILESIZE][ty];

			if(fRelativeX2 >= 0.0f && fRelativeX2 < iWidth)
				t2 = iTileType[(short)fRelativeX2 / TILESIZE][ty];

			bool fSolidTileUnderPlayer = t1 == tile_solid || t1 == tile_ice || t1 == tile_death_on_bottom || t1 == tile_death_on_right || t1 == tile_death_on_left ||
				t2 == tile_solid || t2 == tile_ice || t2 == tile_death_on_bottom || t2 == tile_death_on_right || t2 == tile_death_on_left;

			if((t1 == tile_solid_on_top || t2 == tile_solid_on_top) && player->fOldY + PH <= ty * TILESIZE + fOldY - iHalfHeight)
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
						player->KillPlayerMapHazard();
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
			else if(fSolidTileUnderPlayer ||
				((player->invincible || player->spawninvincible || player->fKuriboShoe) && (t1 == tile_death_on_top || t2 == tile_death_on_top ||
				t1 == tile_death || t2 == tile_death)))
			{	//on ground

				if(player->iVerticalPlatformCollision == 0)
				{
					player->KillPlayerMapHazard();
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
					
					if((t1 == tile_ice && (t2 == tile_ice || t2 == tile_nonsolid || t2 == tile_gap)) ||
						t2 == tile_ice && (t1 == tile_ice || t1 == tile_nonsolid || t1 == tile_gap))
						player->onice = true;
					else 
						player->onice = false;
					
					player->fallthrough = false;

					//printf("Hit Solid Top Platform\n");

					return;
				}
			}
			else if(t1 == tile_death_on_top || t2 == tile_death_on_top ||
				t1 == tile_death || t2 == tile_death)
			{
				player->KillPlayerMapHazard();
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

void MovingPlatform::gettiletypes(CPlayer * player, TileType * lefttile, TileType * righttile)
{
	*lefttile = tile_nonsolid;
	*righttile = tile_nonsolid;

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
		*lefttile = iTileType[(short)fRelativeX1 / TILESIZE][ty];

	if(fRelativeX2 >= 0.0f && fRelativeX2 < iWidth)
		*righttile = iTileType[(short)fRelativeX2 / TILESIZE][ty];
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
		
			TileType t1 = tile_nonsolid;
			TileType t2 = tile_nonsolid;

			if(fRelativeY1 >= 0.0f && fRelativeY1 < iHeight)
				t1 = iTileType[tx][(short)fRelativeY1 / TILESIZE];

			if(fRelativeY2 >= 0.0f && fRelativeY2 < iHeight)
				t2 = iTileType[tx][(short)fRelativeY2 / TILESIZE];

			if(t1 == tile_solid || t1 == tile_death_on_top || 
			   t1 == tile_death_on_bottom || t1 == tile_ice ||
			   t2 == tile_solid || t2 == tile_death_on_top || 
			   t2 == tile_death_on_bottom || t2 == tile_ice)
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

					if((topblock && !topblock->isTransparent()) || 
						(bottomblock && !bottomblock->isTransparent()) ||
						(g_map.map(iTestBackgroundX, iTestBackgroundY) & 13) || 
						(g_map.map(iTestBackgroundX, iTestBackgroundY2) & 13))
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

			TileType t1 = tile_nonsolid;
			TileType t2 = tile_nonsolid;

			if(fRelativeY1 >= 0 && fRelativeY1 < iHeight)
				t1 = iTileType[tx][(short)fRelativeY1 / TILESIZE];

			if(fRelativeY2 >= 0 && fRelativeY2 < iHeight)
				t2 = iTileType[tx][(short)fRelativeY2 / TILESIZE];

			if(t1 == tile_solid || t1 == tile_death_on_top || 
			   t1 == tile_death_on_bottom || t1 == tile_ice ||
			   t2 == tile_solid || t2 == tile_death_on_top || 
			   t2 == tile_death_on_bottom || t2 == tile_ice)
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

					if((topblock && !topblock->isTransparent()) || 
						(bottomblock && !bottomblock->isTransparent()) ||
						(g_map.map(iTestBackgroundX, iTestBackgroundY) & 13) || 
						(g_map.map(iTestBackgroundX, iTestBackgroundY2) & 13))
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

			TileType t1 = tile_nonsolid;
			TileType t2 = tile_nonsolid;

			if(fRelativeX1 >= 0.0f && fRelativeX1 < iWidth)
				t1 = iTileType[(short)fRelativeX1 / TILESIZE][ty];

			if(fRelativeX2 >= 0.0f && fRelativeX2 < iWidth)
				t2 = iTileType[(short)fRelativeX2 / TILESIZE][ty];
		
			if((t1 & 13) || (t2 & 13))
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

			TileType t1 = tile_nonsolid;
			TileType t2 = tile_nonsolid;

			if(fRelativeX1 >= 0.0f && fRelativeX1 < iWidth)
				t1 = iTileType[(short)fRelativeX1 / TILESIZE][ty];

			if(fRelativeX2 >= 0.0f && fRelativeX2 < iWidth)
				t2 = iTileType[(short)fRelativeX2 / TILESIZE][ty];

			if((t1 == tile_solid_on_top || t2 == tile_solid_on_top) && object->fOldY + object->collisionHeight <= ty * TILESIZE + fOldY - iHalfHeight)
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
			else if((t1 & 13) || (t2 & 13))
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
					
					if((t1 == tile_ice && (t2 == tile_ice || t2 == tile_nonsolid || t2 == tile_gap)) ||
						t2 == tile_ice && (t1 == tile_ice || t1 == tile_nonsolid || t1 == tile_gap))
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

