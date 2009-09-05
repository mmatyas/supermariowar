#include <string.h>
#include <math.h>

#include "global.h"

extern CPlayer * GetPlayerFromGlobalID(short iGlobalID);

//------------------------------------------------------------------------------
// class Object base class
//------------------------------------------------------------------------------
CObject::CObject(gfxSprite *nspr1, short x, short y)
{
	dead = false;
	
	spr = nspr1;
	xi(x);
	yi(y);

	if(spr)
	{
		iw = (short)spr->getWidth();
		ih = (short)spr->getHeight();
	}

	velx = 0.0f;
	vely = 0.0f;
	
	state = 0;

	collisionWidth = iw;
	collisionHeight = ih;
	collisionOffsetX = 0;
	collisionOffsetY = 0;

	iNetworkID = g_iNextNetworkID++;
}

/*
short CObject::writeNetworkUpdate(char * pData)
{
	//To send different messages from the same object
	//put in a message type ID that tells it to write and
	//read the message differently
	WriteIntToBuffer(&pData[0], iNetworkID);
	WriteShortToBuffer(&pData[4], ix);
	WriteShortToBuffer(&pData[6], iy);
	WriteFloatToBuffer(&pData[8], velx);
	WriteFloatToBuffer(&pData[12], vely);
	return 16;
}

void CObject::readNetworkUpdate(short size, char * pData)
{
	//ReadIntFromBuffer(&iNetworkID, &pData[0]);
	ReadShortFromBuffer(&ix, &pData[4]);
	ReadShortFromBuffer(&iy, &pData[6]);
	ReadFloatFromBuffer(&velx, &pData[8]);
	ReadFloatFromBuffer(&vely, &pData[12]);
}*/

//returns the blocks touching each of the four corners
void CObject::GetCollisionBlocks(IO_Block * blocks[4])
{
	short xl = 0;
	if(ix < 0)
		xl = (ix + 640) / TILESIZE;
	else
		xl = ix / TILESIZE;

	short xr = 0;
	if(ix + iw >= 640)
		xr = (ix + iw - 640) / TILESIZE;
	else
		xr = (ix + iw) / TILESIZE;

	blocks[0] = NULL;
	blocks[1] = NULL;

	if(iy >= 0 && iy < 480)
	{
		short yt = iy / TILESIZE;

		blocks[0] = g_map.block(xl, yt);
		blocks[1] = g_map.block(xr, yt);
	}

	blocks[2] = NULL;
	blocks[3] = NULL;

	if(iy + ih >= 0 && iy + ih < 480)
	{
		short yb = (iy + ih) / TILESIZE;

		blocks[2] = g_map.block(xl, yb);
		blocks[3] = g_map.block(xr, yb);
	}
}

//------------------------------------------------------------------------------
// class MovingObject (all moving objects inheirit from this class)
//------------------------------------------------------------------------------
IO_MovingObject::IO_MovingObject(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth) :
	CObject(nspr, x, y)
{
	iNumSprites = iNumSpr;

	if(iAnimationWidth > -1)
		iw = iAnimationWidth;
	else if(spr)
		iw = (short)spr->getWidth() / iNumSprites;

	if(iAnimationHeight > -1)
		ih = iAnimationHeight;

	animationtimer = 0;

	if(spr)
		animationWidth = (short)spr->getWidth();

	fOldX = fx;
	fOldY = fy - ih;

	animationspeed = aniSpeed;
	
	objectType = object_moving;
	movingObjectType = movingobject_none;

	if(iCollisionWidth > -1)
	{
		collisionWidth = iCollisionWidth;
		collisionHeight = iCollisionHeight;
		collisionOffsetX = iCollisionOffsetX;
		collisionOffsetY = iCollisionOffsetY;
	}
	else
	{
		collisionWidth = iw;
		collisionHeight = ih;
		collisionOffsetX = 0;
		collisionOffsetY = 0;
	}

	if(iAnimationOffsetX > -1)
	{
		animationOffsetX = iAnimationOffsetX;
		animationOffsetY = iAnimationOffsetY;
	}
	else
	{
		animationOffsetX = 0;
		animationOffsetY = 0;
	}

	drawframe = animationOffsetX;

	inair = false;
	onice = false;

	platform = NULL;
	iHorizontalPlatformCollision = -1;
	iVerticalPlatformCollision = -1;

	fObjectDiesOnSuperDeathTiles = true;
	fObjectCollidesWithMap = true;

	bounce = GRAVITATION;
}


void IO_MovingObject::draw()
{
	spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY, iw, ih);
}


void IO_MovingObject::update()
{
	fOldX = fx;
	fOldY = fy;

	collision_detection_map();

	animate();
}

void IO_MovingObject::animate()
{
	if(animationspeed > 0 && ++animationtimer >= animationspeed)
	{
		animationtimer = 0;
		
		drawframe += iw;
		if(drawframe >= animationWidth)
			drawframe = animationOffsetX;
	}
}

bool IO_MovingObject::collide(CPlayer *)
{
	dead = true;
	return false;
}

void IO_MovingObject::applyfriction()
{
	//Add air/ground friction
	if(velx > 0.0f)
	{
		if(inair)
			velx -= VELAIRFRICTION;
		else if(onice)
			velx -= VELICEFRICTION;
		else
			velx -= VELMOVINGFRICTION;

		if(velx < 0.0f)
			velx = 0.0f;
	}
	else if(velx < 0.0f)
	{
		if(inair)
			velx += VELAIRFRICTION;
		else if(onice)
			velx += VELICEFRICTION;
		else
			velx += VELMOVINGFRICTION;

		if(velx > 0.0f)
			velx = 0.0f;
	}
}

void IO_MovingObject::collision_detection_map()
{
	xf(fx + velx);
	flipsidesifneeded();

	fPrecalculatedY = fy + vely;  //Fixes weird float rounding error.  Must be computed here before casting to int.  Otherwise, this will miss the bottom collision, but then hit the side collision and the player can slide out of 1x1 spaces.

	float fPlatformVelX = 0.0f;
	float fPlatformVelY = 0.0f;

	float fTempY = fy;

	if(platform)
	{
		if(!onice)
		{
			fPlatformVelX = platform->fVelX;
			xf(fx + fPlatformVelX);
			flipsidesifneeded();
		}
		
		fPlatformVelY = platform->fVelY;

		if(platform->fOldVelY < 0.0f)
			fy += platform->fOldVelY;
		
		fPrecalculatedY += platform->fOldVelY;
	}

	iHorizontalPlatformCollision = -1;
	iVerticalPlatformCollision = -1;

	g_map.movingPlatformCollision(this);

	fy = fTempY;

	if(fPrecalculatedY + collisionHeight < 0.0f)
	{				// on top outside of the screen
		yf(fPrecalculatedY);
		vely = CapFallingVelocity(GRAVITATION + vely);
		
		if(!platform)
		{
			inair = true;
			onice = false;
		}

		return;
	}
	else if(fPrecalculatedY + collisionHeight >= 480.0f)
	{	//on ground outside of the screen?
		yi(-collisionHeight);
		fOldY = fy - 1.0f;
		onice = false;
		return;
	}

	//Could be optimized with bit shift >> 5
	short ty = (short)fy / TILESIZE;
	short ty2 = ((short)fy + collisionHeight) / TILESIZE;
	short tx = -1;
	

	//-----------------------------------------------------------------
	//  x axis first (--)
	//-----------------------------------------------------------------
	if(fy + collisionHeight >= 0.0f)
	{
		if(velx + fPlatformVelX > 0.01f || iHorizontalPlatformCollision == 3)
		{		//moving right
			if(fx + collisionWidth >= 640.0f)
			{
				tx = (short)(fx + collisionWidth - 640.0f) / TILESIZE;
				fOldX -= 640.0f;
			}
			else
                tx = ((short)fx + collisionWidth) / TILESIZE;

			IO_Block * topblock = g_map.block(tx, ty);
			IO_Block * bottomblock = g_map.block(tx, ty2);

			bool fTopBlockSolid = topblock && !topblock->isTransparent() && !topblock->isHidden();
			bool fBottomBlockSolid = bottomblock && !bottomblock->isTransparent() && !bottomblock->isHidden();
			if(fTopBlockSolid || fBottomBlockSolid)
			{
				bool processOtherBlock = true;
				if(fTopBlockSolid) //collide with top block
				{	
					if(iHorizontalPlatformCollision == 3)
					{
						KillObjectMapHazard();
						return;
					}

					topblock->collide(this, 1);
					flipsidesifneeded();
					removeifprojectile(this, true, true);

					processOtherBlock = false;

					SideBounce(true);
				}
				
				if(processOtherBlock && fBottomBlockSolid) //then bottom
				{	
					if(iHorizontalPlatformCollision == 3)
					{
						KillObjectMapHazard();
						return;
					}

					bottomblock->collide(this, 1);
					flipsidesifneeded();
					removeifprojectile(this, true, true);

					SideBounce(true);
				}
			}
			else if((g_map.map(tx, ty) & tile_flag_solid) || (g_map.map(tx, ty2) & tile_flag_solid))
			{	//collision on the right side.

				if(iHorizontalPlatformCollision == 3)
				{
					KillObjectMapHazard();
					return;
				}

				xf((float)((tx << 5) - collisionWidth) - 0.2f); //move to the edge of the tile (tile on the right -> mind the object width)
				fOldX = fx;
				
				if(velx > 0.0f)
					velx = -velx;

				flipsidesifneeded();
				removeifprojectile(this, true, true);

				SideBounce(true);
			}
		}
		else if(velx + fPlatformVelX < -0.01f || iHorizontalPlatformCollision == 1)
		{	//moving left
			tx = (short)fx / TILESIZE;

			//Just in case fx < 0 and wasn't caught by flipsidesifneeded()
			if(tx < 0)
				tx = 0;

			IO_Block * topblock = g_map.block(tx, ty);
			IO_Block * bottomblock = g_map.block(tx, ty2);

			bool fTopBlockSolid = topblock && !topblock->isTransparent() && !topblock->isHidden();
			bool fBottomBlockSolid = bottomblock && !bottomblock->isTransparent() && !bottomblock->isHidden();
			if(fTopBlockSolid || fBottomBlockSolid)
			{
				bool processOtherBlock = true;
				if(fTopBlockSolid) //collide with top block
				{	
					if(iHorizontalPlatformCollision == 1)
					{
						KillObjectMapHazard();
						return;
					}

					topblock->collide(this, 3);
					flipsidesifneeded();
					removeifprojectile(this, true, true);

					processOtherBlock = false;

					SideBounce(false);
				}
				
				if(processOtherBlock && fBottomBlockSolid) //then bottom
				{	
					if(iHorizontalPlatformCollision == 1)
					{
						KillObjectMapHazard();
						return;
					}

					bottomblock->collide(this, 3);
					flipsidesifneeded();
					removeifprojectile(this, true, true);

					SideBounce(false);
				}
			}
			else if((g_map.map(tx, ty) & tile_flag_solid) || (g_map.map(tx, ty2) & tile_flag_solid))
			{
				if(iHorizontalPlatformCollision == 1)
				{
					KillObjectMapHazard();
					return;
				}

				xf((float)((tx << 5) + TILESIZE) + 0.2f);			//move to the edge of the tile
				fOldX = fx;

				if(velx < 0.0f)
					velx = -velx;

				flipsidesifneeded();
				removeifprojectile(this, true, true);

				SideBounce(false);
			}
		}
	}

	short txl = -1, txr = -1;

	txl = ix / TILESIZE;

	if(ix + collisionWidth >= 640)
        txr = (ix + collisionWidth - 640) / TILESIZE;
	else
		txr = (ix + collisionWidth) / TILESIZE;

	//-----------------------------------------------------------------
	//  then y axis (|)
	//-----------------------------------------------------------------

	float fMovingUp = vely;
	if(platform)
		fMovingUp = vely + fPlatformVelY - bounce;

	if(fMovingUp < -0.01f)
	{
		ty = (short)(fPrecalculatedY) / TILESIZE;

		IO_Block * leftblock = g_map.block(txl, ty);
		IO_Block * rightblock = g_map.block(txr, ty);
		
		if(leftblock && !leftblock->isTransparent() && !leftblock->isHidden()) //then left
		{	
			if(iVerticalPlatformCollision == 2)
				KillObjectMapHazard();

			leftblock->collide(this, 0);
			return;
		}
		
		if(rightblock && !rightblock->isTransparent() && !rightblock->isHidden()) //then right
		{	
			if(iVerticalPlatformCollision == 2)
				KillObjectMapHazard();

			rightblock->collide(this, 0);
			return;
		}

		if((g_map.map(txl, ty) & tile_flag_solid) || (g_map.map(txr, ty) & tile_flag_solid))
		{
			if(iVerticalPlatformCollision == 2)
				KillObjectMapHazard();

			yf((float)((ty << 5) + TILESIZE) + 0.2f);
			fOldY = fy - 1.0f;
			
			if(vely < 0.0f)
				vely = -vely;	
		}
		else
		{
			yf(fPrecalculatedY);
			vely += GRAVITATION;
		}

		if(!platform)
		{
			inair = true;
        	onice = false;
		}
	}
	else
	{	//moving down / on ground
		ty = ((short)fPrecalculatedY + collisionHeight) / TILESIZE;

		IO_Block * leftblock = g_map.block(txl, ty);
		IO_Block * rightblock = g_map.block(txr, ty);

		bool fLeftBlockSolid = leftblock && !leftblock->isTransparent() && !leftblock->isHidden();
		bool fRightBlockSolid = rightblock && !rightblock->isTransparent() && !rightblock->isHidden();

		if(fLeftBlockSolid || fRightBlockSolid)
		{
			bool processOtherBlock = true;
			if(fLeftBlockSolid) //collide with left block
			{	
				processOtherBlock = leftblock->collide(this, 2);

				if(!platform)
				{
					inair = false;
					onice = false;
				}
			}
			
			if(processOtherBlock && fRightBlockSolid) //then right
			{	
				rightblock->collide(this, 2);
				
				if(!platform)
				{
					inair = false;
					onice = false;
				}
			}
			
			if(iVerticalPlatformCollision == 0)
				KillObjectMapHazard();

			return;
		}

		int leftTile = g_map.map(txl, ty);
		int rightTile = g_map.map(txr, ty);

		if((leftTile & tile_flag_solid_on_top) || (rightTile & tile_flag_solid_on_top))
		{
			if((fOldY + collisionHeight) / TILESIZE < ty)
			{
				vely = BottomBounce();
				yf((float)((ty << 5) - collisionHeight) - 0.2f);
				fOldY = fy - GRAVITATION;
				
				if(!platform)
				{
					inair = false;
					onice = false;
				}

				platform = NULL;

				if(iVerticalPlatformCollision == 0)
					KillObjectMapHazard();

				return;
			}
		}

		bool fSuperDeathTileUnderObject = fObjectDiesOnSuperDeathTiles && (((leftTile & tile_flag_super_death_top) && (rightTile & tile_flag_super_death_top)) ||
									 ((leftTile & tile_flag_super_death_top) && !(rightTile & tile_flag_solid)) ||
									 (!(leftTile & tile_flag_solid) && (rightTile & tile_flag_super_death_top)));

		if(((leftTile & tile_flag_solid) || (rightTile & tile_flag_solid)) && !fSuperDeathTileUnderObject)
		{	
			vely = BottomBounce();
			yf((float)((ty << 5) - collisionHeight) - 0.2f);
			fOldY = fy;

			if(!platform)
			{
				inair = false;

				if((leftTile & tile_flag_ice && ((rightTile & tile_flag_ice) || rightTile == tile_flag_nonsolid || rightTile == tile_flag_gap)) ||
					(rightTile & tile_flag_ice && ((leftTile & tile_flag_ice) || leftTile == tile_flag_nonsolid || leftTile == tile_flag_gap)))
					onice = true;
				else 
					onice = false;
			}
			
			platform = NULL;

			if(iVerticalPlatformCollision == 0)
				KillObjectMapHazard();
		}
		else if(fSuperDeathTileUnderObject)
		{
			KillObjectMapHazard();
			return;
		}
		else
		{
			yf(fPrecalculatedY);
			vely = CapFallingVelocity(GRAVITATION + vely);
			
			if(!platform)
				inair = true;
		}
	}

	if(!platform && inair)
		onice = false;
}

//This method checks the object against the map and moves it outside of any tiles or blocks it might be inside of
bool IO_MovingObject::collision_detection_checksides()
{
	//First figure out where the corners of this object are touching
	Uint8 iCase = 0;

	short txl = -1, nofliptxl = ix >> 5;
	if(ix < 0)
		txl = (ix + 640) >> 5;
	else
		txl = nofliptxl;

	short txr = -1, nofliptxr = (ix + collisionWidth) >> 5;
	if(ix + collisionWidth >= 640)
		txr = (ix + collisionWidth - 640) >> 5;
	else
		txr = nofliptxr;

	short ty = iy >> 5;
	short ty2 = (iy + collisionHeight) >> 5;

	if(iy >= 0)
	{
		if(ty < MAPHEIGHT)
		{
			if(txl >= 0 && txl < MAPWIDTH)
			{
				IO_Block * block = g_map.block(txl, ty);

				if((block && !block->isTransparent() && !block->isHidden()) || (g_map.map(txl, ty) & tile_flag_solid) > 0)
				{
					iCase |= 0x01;
				}
			}

			if(txr >= 0 && txr < MAPWIDTH)
			{
				IO_Block * block = g_map.block(txr, ty);

				if((block && !block->isTransparent() && !block->isHidden()) || (g_map.map(txr, ty) & tile_flag_solid) > 0)
				{
					iCase |= 0x02;
				}
			}
		}
	}

	if(iy + collisionHeight >= 0.0f)
	{
		if(ty2 < MAPHEIGHT)
		{
			if(txl >= 0 && txl < MAPWIDTH)
			{
				IO_Block * block = g_map.block(txl, ty2);

				if((block && !block->isTransparent() && !block->isHidden()) || (g_map.map(txl, ty2) & tile_flag_solid) > 0)
				{
					iCase |= 0x04;
				}
			}

			if(txr >= 0 && txr < MAPWIDTH)
			{
				IO_Block * block = g_map.block(txr, ty2);

				if((block && !block->isTransparent() && !block->isHidden()) || (g_map.map(txr, ty2) & tile_flag_solid) > 0)
				{
					iCase |= 0x08;
				}
			}
		}
	}

	bool fRet = true;
	//Then determine which way is the best way to move this object out of the solid map areas
	switch(iCase)
	{
		//Do nothing
		//[ ][ ]
		//[ ][ ]
		case 0:
			fRet = false;
			break;
		
		//[X][ ]
		//[ ][ ]
		case 1:
		{
			if(ix + (collisionWidth >> 1) > (nofliptxl << 5) + TILESIZE)
			{
				xf((float)((nofliptxl << 5) + TILESIZE) + 0.2f);
				flipsidesifneeded();
			}
			else
			{
				yf((float)((ty << 5) + TILESIZE) + 0.2f);
			}

			break;
		}

		//[ ][X]
		//[ ][ ]
		case 2:
		{
			if(ix + (collisionWidth >> 1) < (nofliptxr << 5))
			{
				xf((float)((nofliptxr << 5) - collisionWidth) - 0.2f);
				flipsidesifneeded();
			}
			else
			{
				yf((float)((ty << 5) + TILESIZE) + 0.2f);
			}

			break;
		}

		//[X][X]
		//[ ][ ]
		case 3:
		{
			yf((float)((ty << 5) + TILESIZE) + 0.2f);
			break;
		}

		//[ ][ ]
		//[X][ ]
		case 4:
		{
			if(ix + (collisionWidth >> 1) > (nofliptxl << 5) + TILESIZE)
			{
				xf((float)((nofliptxl << 5) + TILESIZE) + 0.2f);
				flipsidesifneeded();
			}
			else
			{
				yf((float)((ty2 << 5) - collisionHeight) - 0.2f);
			}

			break;
		}

		//[X][ ]
		//[X][ ]
		case 5:
		{
			xf((float)((nofliptxl << 5) + TILESIZE) + 0.2f);
			flipsidesifneeded();
			break;
		}

		//[ ][X]
		//[X][ ]
		case 6:
		{
			if(ix + (collisionWidth >> 1) > (nofliptxl << 5) + TILESIZE)
			{
				yf((float)((ty << 5) + TILESIZE) + 0.2f);
				xf((float)((nofliptxl << 5) + TILESIZE) + 0.2f);
				flipsidesifneeded();
			}
			else
			{
				yf((float)((ty2 << 5) - collisionHeight) - 0.2f);
				xf((float)((nofliptxr << 5) - collisionWidth) - 0.2f);
				flipsidesifneeded();
			}

			break;
		}

		//[X][X]
		//[X][ ]
		case 7:
		{
			yf((float)((ty << 5) + TILESIZE) + 0.2f);
			xf((float)((nofliptxl << 5) + TILESIZE) + 0.2f);
			flipsidesifneeded();
			break;
		}

		//[ ][ ]
		//[ ][X]
		case 8:
		{
			if(ix + (collisionWidth >> 1) < (nofliptxr << 5))
			{
				xf((float)((nofliptxr << 5) - collisionWidth) - 0.2f);
				flipsidesifneeded();
			}
			else
			{
				yf((float)((ty2 << 5) - collisionHeight) - 0.2f);
			}

			break;
		}

		//[X][ ]
		//[ ][X]
		case 9:
		{
			if(ix + (collisionWidth >> 1) > (nofliptxl << 5) + TILESIZE)
			{
				yf((float)((ty2 << 5) - collisionHeight) - 0.2f);
				xf((float)((nofliptxl << 5) + TILESIZE) + 0.2f);
				flipsidesifneeded();
			}
			else
			{
				yf((float)((ty << 5) + TILESIZE) + 0.2f);
				xf((float)((nofliptxr << 5) - collisionWidth) - 0.2f);
				flipsidesifneeded();
			}

			break;
		}

		//[ ][X]
		//[ ][X]
		case 10:
		{
			xf((float)((nofliptxr << 5) - collisionWidth) - 0.2f);
			flipsidesifneeded();
			break;
		}

		//[X][X]
		//[ ][X]
		case 11:
		{
			yf((float)((ty << 5) + TILESIZE) + 0.2f);
			xf((float)((nofliptxr << 5) - collisionWidth) - 0.2f);
			flipsidesifneeded();
			break;
		}

		//[ ][ ]
		//[X][X]
		case 12:
		{
			yf((float)((ty2 << 5) - collisionHeight) - 0.2f);
			break;
		}

		//[X][ ]
		//[X][X]
		case 13:
		{
			yf((float)((ty2 << 5) - collisionHeight) - 0.2f);
			xf((float)((nofliptxl << 5) + TILESIZE) + 0.2f);
			flipsidesifneeded();
			break;
		}

		//[ ][X]
		//[X][X]
		case 14:
		{
			yf((float)((ty2 << 5) - collisionHeight) - 0.2f);
			xf((float)((nofliptxr << 5) - collisionWidth) - 0.2f);
			flipsidesifneeded();
			break;
		}

		//If object is completely inside a block, default to moving it down
		//[X][X]
		//[X][X]
		case 15:
		{
			yf((float)((ty2 << 5) + TILESIZE) + 0.2f);
			break;
		}

		default:
		{
			fRet = false;
			break;
		}
	}

	//Updated object to have correct precalculatedY since it wasn't getting collision detection updates
	//while it was being held by the player
	fPrecalculatedY = fy;

	//Check moving platforms and make sure this object is not inside one
	fRet |= g_map.movingPlatformCheckSides(this);

	return fRet;
}

void IO_MovingObject::flipsidesifneeded()
{
	//Use ix here to avoid rounding issues (can crash if txr evals to over the right side of screen)
	if(ix < 0 || fx < 0.0f)
	{
		xf(fx + 640.0f);
		fOldX += 640.0f;
	}
	else if(ix >= 640 || fx >= 640.0f)
	{
		xf(fx - 640.0f);
		fOldX -= 640.0f;
	}
}

//This method probably needs to be rewritten so that it just calls into the object being killed
//And let it handle not dying and placing itself somewhere else if necessary
void IO_MovingObject::KillObjectMapHazard(short playerID)
{
	if(!dead)
	{
		//If it is a throw box, trigger it's behavior and return
		if(movingObjectType == movingobject_throwbox)
		{
			((CO_ThrowBox*)this)->Die();
			return;
		}

		dead = true;
		eyecandy[2].add(new EC_SingleAnimation(&spr_fireballexplosion, ix + (iw >> 1) - 16, iy + (ih >> 1) - 16, 3, 4));

		if(movingObjectType == movingobject_fireball)
		{
			if(iPlayerID > -1 && projectiles[iPlayerID] > 0)
				projectiles[iPlayerID]--;

			ifsoundonplay(sfx_hit);
		}
		else if(movingObjectType == movingobject_egg)
		{
			dead = false;
			((CO_Egg*)this)->placeEgg();
			ifsoundonplay(sfx_transform);
		}
		else if(movingObjectType == movingobject_star)
		{
			dead = false;
			((CO_Star*)this)->placeStar();
			ifsoundonplay(sfx_transform);
		}
		else if(movingObjectType == movingobject_flag)
		{
			dead = false;
			((CO_Flag*)this)->placeFlag();
			ifsoundonplay(sfx_transform);
		}
		else if(movingObjectType == movingobject_bomb)
		{
			if(iPlayerID > -1 && projectiles[iPlayerID] > 0)
				projectiles[iPlayerID]--;

			ifsoundonplay(sfx_hit);
		}
		else if(movingObjectType == movingobject_phantokey)
		{
			dead = false;
			((CO_PhantoKey*)this)->placeKey();
			ifsoundonplay(sfx_transform);
		}
		else if(game_mode_boxes_minigame == game_values.gamemode->gamemode && movingObjectType == movingobject_coin)
		{
			dead = false;
			((MO_Coin*)this)->placeCoin();
			ifsoundonplay(sfx_transform);
		}
		else if(game_values.gamemode->gamemode == game_mode_stomp && (movingObjectType == movingobject_goomba || movingObjectType == movingobject_koopa || movingObjectType == movingobject_spiny || movingObjectType == movingobject_buzzybeetle || movingObjectType == movingobject_cheepcheep))
		{
			if(!game_values.gamemode->gameover)
			{
				CPlayer * player = GetPlayerFromGlobalID(playerID);
				
				if(NULL != player)
				{
					player->score->AdjustScore(1);
				}
			}

			ifsoundonplay(sfx_hit);
		}
		else
		{
			ifsoundonplay(sfx_hit);
		}
	}
}


//------------------------------------------------------------------------------
// class OverMapObject - moving objects that don't collide with map or objects, just player
//------------------------------------------------------------------------------
IO_OverMapObject::IO_OverMapObject(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth) :
	CObject(nspr, x, y)
{
	objectType = object_overmap;
	//movingObjectType = movingobject_none;

	iNumSprites = iNumSpr;

	if(iAnimationWidth > -1)
		iw = iAnimationWidth;
	else if(spr)
		iw = (short)spr->getWidth() / iNumSprites;

	if(iAnimationHeight > -1)
		ih = iAnimationHeight;

	animationspeed = aniSpeed;
	animationtimer = 0;

	if(spr)
		animationWidth = (short)spr->getWidth();

	if(iCollisionWidth > -1)
	{
		collisionWidth = iCollisionWidth;
		collisionHeight = iCollisionHeight;
		collisionOffsetX = iCollisionOffsetX;
		collisionOffsetY = iCollisionOffsetY;
	}
	else
	{
		collisionWidth = iw;
		collisionHeight = ih;
		collisionOffsetX = 0;
		collisionOffsetY = 0;
	}

	if(iAnimationOffsetX > -1)
	{
		animationOffsetX = iAnimationOffsetX;
		animationOffsetY = iAnimationOffsetY;
	}
	else
	{
		animationOffsetX = 0;
		animationOffsetY = 0;
	}

	drawframe = animationOffsetX;
}

void IO_OverMapObject::draw()
{
	spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY, iw, ih);
}

void IO_OverMapObject::draw(short iOffsetX, short iOffsetY)
{
	gfx_drawpreview(spr->getSurface(), ((ix - collisionOffsetX) >> 1) + iOffsetX, ((iy - collisionOffsetY) >> 1) + iOffsetY, drawframe >> 1, animationOffsetY >> 1, iw >> 1, ih >> 1, iOffsetX, iOffsetY, 320, 240, true);
}

void IO_OverMapObject::update()
{
	xf(fx + velx);
	yf(fy + vely);

	animate();
}

void IO_OverMapObject::animate()
{
	if(animationspeed > 0 && ++animationtimer == animationspeed)
	{
		animationtimer = 0;
		drawframe += iw;
		if(drawframe >= animationWidth)
			drawframe = animationOffsetX;
	}
}


//------------------------------------------------------------------------------
// class object_container
//------------------------------------------------------------------------------
CObjectContainer::CObjectContainer()
{
	for(short i = 0; i < MAXOBJECTS; i++)
		list[i] = NULL;

	list_end = 0;
}


CObjectContainer::~CObjectContainer()
{
	clean();
}


void CObjectContainer::clean()
{
	for(short i = 0; i < list_end; i++)
	{
		delete list[i];
		list[i] = NULL;
	}
	list_end = 0;
}


void CObjectContainer::add(CObject *ec)
{
	if(list_end < MAXOBJECTS)
	{
		list[list_end] = ec;
		ec->index = list_end;	//save index for removing
		list_end++;
	}
	else
	{
		delete ec;	//otherwise memory leak!
		//printf("eyecandy list full!\n");
	}
}

bool CObjectContainer::isBlockAt(short x, short y)
{
	for(short i = 0; i < list_end; i++)
	{
		if(x >= list[i]->ix && x < list[i]->ix + list[i]->iw && 
			y >= list[i]->iy && y < list[i]->iy + list[i]->ih && 
			list[i]->getObjectType() == object_block)
		{
			return true;
		}
	}

	return false;
}

float CObjectContainer::getClosestObject(short ix, short iy, short objectType)
{
	int dist = 640000;  //Longest distance from corner to corner squared

	for(short i = 0; i < list_end; i++)
	{
		if(list[i]->getObjectType() != objectType)
			continue;

		short x = list[i]->ix - ix;
		short y = list[i]->iy - iy;

		int calcdist = x * x + y * y;

		if(calcdist < dist)
			dist = calcdist;
	}

	return (float)sqrt((double)dist);
}

float CObjectContainer::getClosestMovingObject(short ix, short iy, short movingObjectType)
{
	int dist = 640000;  //Longest distance from corner to corner squared

	for(short i = 0; i < list_end; i++)
	{
		if(list[i]->getObjectType() != object_moving || ((IO_MovingObject*)list[i])->getMovingObjectType() != movingObjectType)
			continue;

		short x = list[i]->ix - ix;
		short y = list[i]->iy - iy;

		int calcdist = x * x + y * y;

		if(calcdist < dist)
			dist = calcdist;
	}

	return (float)sqrt((double)dist);
}

short CObjectContainer::countTypes(ObjectType type)
{
	short count = 0;

	for(short i = 0; i < list_end; i++)
	{
		if(list[i]->getObjectType() == type)
		{
			count++;
		}
	}

	return count;
}

short CObjectContainer::countMovingTypes(MovingObjectType type)
{
	short count = 0;

	for(short i = 0; i < list_end; i++)
	{
		if(list[i]->getObjectType() == object_moving && ((IO_MovingObject*)list[i])->getMovingObjectType() == type)
		{
			count++;
		}
	}

	return count;
}

void CObjectContainer::adjustPlayerAreas(CPlayer * player, CPlayer * other)
{
	for(short i = 0; i < list_end; i++)
	{
		if(list[i]->getObjectType() == object_area)
		{
			OMO_Area * area = (OMO_Area*)list[i];

			if(area->colorID == other->colorID)
			{
				if(game_values.gamemodesettings.domination.relocateondeath)
					area->placeArea();
	
				if(game_values.gamemodesettings.domination.stealondeath && player)
					area->setOwner(player);
				else if(game_values.gamemodesettings.domination.loseondeath)
					area->reset();
			}
		}
	}
}

void CObjectContainer::removePlayerRaceGoals(short id, short iGoal)
{
	if(game_values.gamemodesettings.race.penalty == 0 && iGoal != -1)
		return;

	for(short i = 0; i < list_end; i++)
	{
		if(list[i]->getObjectType() == object_race_goal)
		{
			OMO_RaceGoal * goal = (OMO_RaceGoal*)list[i];

			if(iGoal == -1 || 2 == game_values.gamemodesettings.race.penalty ||
				(1 == game_values.gamemodesettings.race.penalty && goal->getGoalID() == iGoal))
			{
				goal->reset(id);
			}
		}
	}
}

void CObjectContainer::pushBombs(short x, short y)
{
	for(short i = 0; i < list_end; i++)
	{
		if(list[i]->getObjectType() != object_moving)
			continue;

		IO_MovingObject * mo = (IO_MovingObject*)list[i];

		if(mo->getMovingObjectType() != movingobject_bomb)
			continue;

		CO_Bomb * bomb = (CO_Bomb*)list[i];

		if(bomb->HasOwner())
			continue;

		int bombx = bomb->ix + (bomb->iw >> 1) - x;
		int bomby = bomb->iy + (bomb->ih >> 1) - y;

		int dist = bombx * bombx + bomby * bomby;

		if(dist < 10000)
		{
			if(bombx > 0)
				bomb->velx += ((float)(rand() % 30) / 10.0f + 4.0f);
			else
				bomb->velx -= ((float)(rand() % 30) / 10.0f + 4.0f);

			bomb->vely -= (float)(rand() % 30) / 10.0f + 6.0f;
		}
	}
}

void CObjectContainer::cleandeadobjects()
{
	for(short i = 0; i < list_end; i++)
	{
		if(list[i]->dead)
		{
			delete list[i];
			list_end--;

			if(i != list_end)
			{
				list[i] = list[list_end];
			}

			i--;
		}
	}
}

CObject * CObjectContainer::getRandomObject()
{
	if(list_end == 0)
		return NULL;

	return list[rand() % list_end];
}

