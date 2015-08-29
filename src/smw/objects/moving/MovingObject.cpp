#include "MovingObject.h"

#include "IO_Block.h"
#include "eyecandy.h"
#include "GameValues.h"
#include "gamemodes.h"
#include "gfx.h"
#include "map.h"
#include "movingplatform.h"
#include "player.h"
#include "ResourceManager.h"

#include "objects/carriable/Egg.h"
#include "objects/carriable/Flag.h"
#include "objects/carriable/PhantoKey.h"
#include "objects/carriable/Star.h"
#include "objects/carriable/ThrowBox.h"
#include "objects/moving/Coin.h"

extern CGameValues game_values;
extern CResourceManager* rm;

extern CMap* g_map;
extern CEyecandyContainer eyecandy[3];

extern CPlayer* GetPlayerFromGlobalID(short iGlobalID);

//------------------------------------------------------------------------------
// class MovingObject (all moving objects inheirit from this class)
//------------------------------------------------------------------------------
IO_MovingObject::IO_MovingObject(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth) :
    CObject(nspr, x, y)
{
    iNumSprites = iNumSpr;

    if (iAnimationWidth > -1)
        iw = iAnimationWidth;
    else if (spr)
        iw = (short)spr->getWidth() / iNumSprites;

    if (iAnimationHeight > -1)
        ih = iAnimationHeight;

    animationtimer = 0;

    if (spr)
        animationWidth = (short)spr->getWidth();

    fOldX = fx;
    fOldY = fy - ih;

    animationspeed = aniSpeed;

    objectType = object_moving;
    movingObjectType = movingobject_none;

    if (iCollisionWidth > -1) {
        collisionWidth = iCollisionWidth;
        collisionHeight = iCollisionHeight;
        collisionOffsetX = iCollisionOffsetX;
        collisionOffsetY = iCollisionOffsetY;
    } else {
        collisionWidth = iw;
        collisionHeight = ih;
        collisionOffsetX = 0;
        collisionOffsetY = 0;
    }

    if (iAnimationOffsetX > -1) {
        animationOffsetX = iAnimationOffsetX;
        animationOffsetY = iAnimationOffsetY;
    } else {
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
    if (animationspeed > 0 && ++animationtimer >= animationspeed) {
        animationtimer = 0;

        drawframe += iw;
        if (drawframe >= animationWidth)
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
    if (velx > 0.0f) {
        if (inair)
            velx -= VELAIRFRICTION;
        else if (onice)
            velx -= VELICEFRICTION;
        else
            velx -= VELMOVINGFRICTION;

        if (velx < 0.0f)
            velx = 0.0f;
    } else if (velx < 0.0f) {
        if (inair)
            velx += VELAIRFRICTION;
        else if (onice)
            velx += VELICEFRICTION;
        else
            velx += VELMOVINGFRICTION;

        if (velx > 0.0f)
            velx = 0.0f;
    }
}

// TODO / FIXME: The collision code is almost the same as in CPlayer
void IO_MovingObject::collision_detection_map()
{
    setXf(fx + velx);
    flipsidesifneeded();

    fPrecalculatedY = fy + vely;  //Fixes weird float rounding error.  Must be computed here before casting to int.  Otherwise, this will miss the bottom collision, but then hit the side collision and the player can slide out of 1x1 spaces.

    float fPlatformVelX = 0.0f;
    float fPlatformVelY = 0.0f;

    float fTempY = fy;

    if (platform) {
        if (!onice) {
            fPlatformVelX = platform->fVelX;
            setXf(fx + fPlatformVelX);
            flipsidesifneeded();
        }

        fPlatformVelY = platform->fVelY;

        if (platform->fOldVelY < 0.0f)
            fy += platform->fOldVelY;

        fPrecalculatedY += platform->fOldVelY;
    }

    iHorizontalPlatformCollision = -1;
    iVerticalPlatformCollision = -1;

    g_map->movingPlatformCollision(this);

    fy = fTempY;

    if (fPrecalculatedY + collisionHeight < 0.0f) {
        // on top outside of the screen
        setYf(fPrecalculatedY);
        vely = CapFallingVelocity(GRAVITATION + vely);

        if (!platform) {
            inair = true;
            onice = false;
        }

        return;
    } else if (fPrecalculatedY + collisionHeight >= smw->ScreenHeight) {
        //on ground outside of the screen?
        setYi(-collisionHeight);
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
    if (fy + collisionHeight >= 0.0f) {
        if (velx + fPlatformVelX > 0.01f || iHorizontalPlatformCollision == 3) {
            //moving right
            if (fx + collisionWidth >= smw->ScreenWidth) {
                tx = (short)(fx + collisionWidth - smw->ScreenWidth) / TILESIZE;
                fOldX -= smw->ScreenWidth;
            } else
                tx = ((short)fx + collisionWidth) / TILESIZE;

            IO_Block * topblock = g_map->block(tx, ty);
            IO_Block * bottomblock = g_map->block(tx, ty2);

            bool fTopBlockSolid = topblock && !topblock->isTransparent() && !topblock->isHidden();
            bool fBottomBlockSolid = bottomblock && !bottomblock->isTransparent() && !bottomblock->isHidden();
            if (fTopBlockSolid || fBottomBlockSolid) {
                bool processOtherBlock = true;
                if (fTopBlockSolid) { //collide with top block
                    if (iHorizontalPlatformCollision == 3) {
                        KillObjectMapHazard();
                        return;
                    }

                    topblock->collide(this, 1);
                    flipsidesifneeded();
                    removeifprojectile(this, true, true);

                    processOtherBlock = false;

                    SideBounce(true);
                }

                if (processOtherBlock && fBottomBlockSolid) { //then bottom
                    if (iHorizontalPlatformCollision == 3) {
                        KillObjectMapHazard();
                        return;
                    }

                    bottomblock->collide(this, 1);
                    flipsidesifneeded();
                    removeifprojectile(this, true, true);

                    SideBounce(true);
                }
            } else if ((g_map->map(tx, ty) & tile_flag_solid) || (g_map->map(tx, ty2) & tile_flag_solid)) {
                //collision on the right side.

                if (iHorizontalPlatformCollision == 3) {
                    KillObjectMapHazard();
                    return;
                }

                setXf((float)((tx << 5) - collisionWidth) - 0.2f); //move to the edge of the tile (tile on the right -> mind the object width)
                fOldX = fx;

                if (velx > 0.0f)
                    velx = -velx;

                flipsidesifneeded();
                removeifprojectile(this, true, true);

                SideBounce(true);
            }
        } else if (velx + fPlatformVelX < -0.01f || iHorizontalPlatformCollision == 1) {
            //moving left
            tx = (short)fx / TILESIZE;

            //Just in case fx < 0 and wasn't caught by flipsidesifneeded()
            if (tx < 0)
                tx = 0;

            IO_Block * topblock = g_map->block(tx, ty);
            IO_Block * bottomblock = g_map->block(tx, ty2);

            bool fTopBlockSolid = topblock && !topblock->isTransparent() && !topblock->isHidden();
            bool fBottomBlockSolid = bottomblock && !bottomblock->isTransparent() && !bottomblock->isHidden();
            if (fTopBlockSolid || fBottomBlockSolid) {
                bool processOtherBlock = true;
                if (fTopBlockSolid) { //collide with top block
                    if (iHorizontalPlatformCollision == 1) {
                        KillObjectMapHazard();
                        return;
                    }

                    topblock->collide(this, 3);
                    flipsidesifneeded();
                    removeifprojectile(this, true, true);

                    processOtherBlock = false;

                    SideBounce(false);
                }

                if (processOtherBlock && fBottomBlockSolid) { //then bottom
                    if (iHorizontalPlatformCollision == 1) {
                        KillObjectMapHazard();
                        return;
                    }

                    bottomblock->collide(this, 3);
                    flipsidesifneeded();
                    removeifprojectile(this, true, true);

                    SideBounce(false);
                }
            } else if ((g_map->map(tx, ty) & tile_flag_solid) || (g_map->map(tx, ty2) & tile_flag_solid)) {
                if (iHorizontalPlatformCollision == 1) {
                    KillObjectMapHazard();
                    return;
                }

                setXf((float)((tx << 5) + TILESIZE) + 0.2f);			//move to the edge of the tile
                fOldX = fx;

                if (velx < 0.0f)
                    velx = -velx;

                flipsidesifneeded();
                removeifprojectile(this, true, true);

                SideBounce(false);
            }
        }
    }

    short txl = -1, txr = -1;

    txl = ix / TILESIZE;

    if (ix + collisionWidth >= smw->ScreenWidth)
        txr = (ix + collisionWidth - smw->ScreenWidth) / TILESIZE;
    else
        txr = (ix + collisionWidth) / TILESIZE;

    //-----------------------------------------------------------------
    //  then y axis (|)
    //-----------------------------------------------------------------

    float fMovingUp = vely;
    if (platform)
        fMovingUp = vely + fPlatformVelY - bounce;

    if (fMovingUp < -0.01f) {
        ty = (short)(fPrecalculatedY) / TILESIZE;

        IO_Block * leftblock = g_map->block(txl, ty);
        IO_Block * rightblock = g_map->block(txr, ty);

        if (leftblock && !leftblock->isTransparent() && !leftblock->isHidden()) { //then left
            if (iVerticalPlatformCollision == 2)
                KillObjectMapHazard();

            leftblock->collide(this, 0);
            return;
        }

        if (rightblock && !rightblock->isTransparent() && !rightblock->isHidden()) { //then right
            if (iVerticalPlatformCollision == 2)
                KillObjectMapHazard();

            rightblock->collide(this, 0);
            return;
        }

        if ((g_map->map(txl, ty) & tile_flag_solid) || (g_map->map(txr, ty) & tile_flag_solid)) {
            if (iVerticalPlatformCollision == 2)
                KillObjectMapHazard();

            setYf((float)((ty << 5) + TILESIZE) + 0.2f);
            fOldY = fy - 1.0f;

            if (vely < 0.0f)
                vely = -vely;
        } else {
            setYf(fPrecalculatedY);
            vely += GRAVITATION;
        }

        if (!platform) {
            inair = true;
            onice = false;
        }
    } else {
        //moving down / on ground
        ty = ((short)fPrecalculatedY + collisionHeight) / TILESIZE;

        IO_Block * leftblock = g_map->block(txl, ty);
        IO_Block * rightblock = g_map->block(txr, ty);

        bool fLeftBlockSolid = leftblock && !leftblock->isTransparent() && !leftblock->isHidden();
        bool fRightBlockSolid = rightblock && !rightblock->isTransparent() && !rightblock->isHidden();

        if (fLeftBlockSolid || fRightBlockSolid) {
            bool processOtherBlock = true;
            if (fLeftBlockSolid) { //collide with left block
                processOtherBlock = leftblock->collide(this, 2);

                if (!platform) {
                    inair = false;
                    onice = false;
                }
            }

            if (processOtherBlock && fRightBlockSolid) { //then right
                rightblock->collide(this, 2);

                if (!platform) {
                    inair = false;
                    onice = false;
                }
            }

            if (iVerticalPlatformCollision == 0)
                KillObjectMapHazard();

            return;
        }

        int leftTile = g_map->map(txl, ty);
        int rightTile = g_map->map(txr, ty);

        if ((leftTile & tile_flag_solid_on_top) || (rightTile & tile_flag_solid_on_top)) {
            if ((fOldY + collisionHeight) / TILESIZE < ty) {
                vely = BottomBounce();
                setYf((float)((ty << 5) - collisionHeight) - 0.2f);
                fOldY = fy - GRAVITATION;

                if (!platform) {
                    inair = false;
                    onice = false;
                }

                platform = NULL;

                if (iVerticalPlatformCollision == 0)
                    KillObjectMapHazard();

                return;
            }
        }

        bool fSuperDeathTileUnderObject = fObjectDiesOnSuperDeathTiles && (((leftTile & tile_flag_super_death_top) && (rightTile & tile_flag_super_death_top)) ||
                                          ((leftTile & tile_flag_super_death_top) && !(rightTile & tile_flag_solid)) ||
                                          (!(leftTile & tile_flag_solid) && (rightTile & tile_flag_super_death_top)));

        if (((leftTile & tile_flag_solid) || (rightTile & tile_flag_solid)) && !fSuperDeathTileUnderObject) {
            vely = BottomBounce();
            setYf((float)((ty << 5) - collisionHeight) - 0.2f);
            fOldY = fy;

            if (!platform) {
                inair = false;

                if ((leftTile & tile_flag_ice && ((rightTile & tile_flag_ice) || rightTile == tile_flag_nonsolid || rightTile == tile_flag_gap)) ||
                        (rightTile & tile_flag_ice && ((leftTile & tile_flag_ice) || leftTile == tile_flag_nonsolid || leftTile == tile_flag_gap)))
                    onice = true;
                else
                    onice = false;
            }

            platform = NULL;

            if (iVerticalPlatformCollision == 0)
                KillObjectMapHazard();
        } else if (fSuperDeathTileUnderObject) {
            KillObjectMapHazard();
            return;
        } else {
            setYf(fPrecalculatedY);
            vely = CapFallingVelocity(GRAVITATION + vely);

            if (!platform)
                inair = true;
        }
    }

    if (!platform && inair)
        onice = false;
}

//This method checks the object against the map and moves it outside of any tiles or blocks it might be inside of
bool IO_MovingObject::collision_detection_checksides()
{
    //First figure out where the corners of this object are touching
    Uint8 iCase = 0;

    short txl = -1, nofliptxl = ix >> 5;
    if (ix < 0)
        txl = (ix + smw->ScreenWidth) >> 5;
    else
        txl = nofliptxl;

    short txr = -1, nofliptxr = (ix + collisionWidth) >> 5;
    if (ix + collisionWidth >= smw->ScreenWidth)
        txr = (ix + collisionWidth - smw->ScreenWidth) >> 5;
    else
        txr = nofliptxr;

    short ty = iy >> 5;
    short ty2 = (iy + collisionHeight) >> 5;

    if (iy >= 0) {
        if (ty < MAPHEIGHT) {
            if (txl >= 0 && txl < MAPWIDTH) {
                IO_Block * block = g_map->block(txl, ty);

                if ((block && !block->isTransparent() && !block->isHidden()) || (g_map->map(txl, ty) & tile_flag_solid) > 0) {
                    iCase |= 0x01;
                }
            }

            if (txr >= 0 && txr < MAPWIDTH) {
                IO_Block * block = g_map->block(txr, ty);

                if ((block && !block->isTransparent() && !block->isHidden()) || (g_map->map(txr, ty) & tile_flag_solid) > 0) {
                    iCase |= 0x02;
                }
            }
        }
    }

    if (iy + collisionHeight >= 0.0f) {
        if (ty2 < MAPHEIGHT) {
            if (txl >= 0 && txl < MAPWIDTH) {
                IO_Block * block = g_map->block(txl, ty2);

                if ((block && !block->isTransparent() && !block->isHidden()) || (g_map->map(txl, ty2) & tile_flag_solid) > 0) {
                    iCase |= 0x04;
                }
            }

            if (txr >= 0 && txr < MAPWIDTH) {
                IO_Block * block = g_map->block(txr, ty2);

                if ((block && !block->isTransparent() && !block->isHidden()) || (g_map->map(txr, ty2) & tile_flag_solid) > 0) {
                    iCase |= 0x08;
                }
            }
        }
    }

    bool fRet = true;
    //Then determine which way is the best way to move this object out of the solid map areas
    switch (iCase) {
        //Do nothing
        //[ ][ ]
        //[ ][ ]
    case 0:
        fRet = false;
        break;

        //[X][ ]
        //[ ][ ]
    case 1: {
        if (ix + (collisionWidth >> 1) > (nofliptxl << 5) + TILESIZE) {
            setXf((float)((nofliptxl << 5) + TILESIZE) + 0.2f);
            flipsidesifneeded();
        } else {
            setYf((float)((ty << 5) + TILESIZE) + 0.2f);
        }

        break;
    }

    //[ ][X]
    //[ ][ ]
    case 2: {
        if (ix + (collisionWidth >> 1) < (nofliptxr << 5)) {
            setXf((float)((nofliptxr << 5) - collisionWidth) - 0.2f);
            flipsidesifneeded();
        } else {
            setYf((float)((ty << 5) + TILESIZE) + 0.2f);
        }

        break;
    }

    //[X][X]
    //[ ][ ]
    case 3: {
        setYf((float)((ty << 5) + TILESIZE) + 0.2f);
        break;
    }

    //[ ][ ]
    //[X][ ]
    case 4: {
        if (ix + (collisionWidth >> 1) > (nofliptxl << 5) + TILESIZE) {
            setXf((float)((nofliptxl << 5) + TILESIZE) + 0.2f);
            flipsidesifneeded();
        } else {
            setYf((float)((ty2 << 5) - collisionHeight) - 0.2f);
        }

        break;
    }

    //[X][ ]
    //[X][ ]
    case 5: {
        setXf((float)((nofliptxl << 5) + TILESIZE) + 0.2f);
        flipsidesifneeded();
        break;
    }

    //[ ][X]
    //[X][ ]
    case 6: {
        if (ix + (collisionWidth >> 1) > (nofliptxl << 5) + TILESIZE) {
            setYf((float)((ty << 5) + TILESIZE) + 0.2f);
            setXf((float)((nofliptxl << 5) + TILESIZE) + 0.2f);
            flipsidesifneeded();
        } else {
            setYf((float)((ty2 << 5) - collisionHeight) - 0.2f);
            setXf((float)((nofliptxr << 5) - collisionWidth) - 0.2f);
            flipsidesifneeded();
        }

        break;
    }

    //[X][X]
    //[X][ ]
    case 7: {
        setYf((float)((ty << 5) + TILESIZE) + 0.2f);
        setXf((float)((nofliptxl << 5) + TILESIZE) + 0.2f);
        flipsidesifneeded();
        break;
    }

    //[ ][ ]
    //[ ][X]
    case 8: {
        if (ix + (collisionWidth >> 1) < (nofliptxr << 5)) {
            setXf((float)((nofliptxr << 5) - collisionWidth) - 0.2f);
            flipsidesifneeded();
        } else {
            setYf((float)((ty2 << 5) - collisionHeight) - 0.2f);
        }

        break;
    }

    //[X][ ]
    //[ ][X]
    case 9: {
        if (ix + (collisionWidth >> 1) > (nofliptxl << 5) + TILESIZE) {
            setYf((float)((ty2 << 5) - collisionHeight) - 0.2f);
            setXf((float)((nofliptxl << 5) + TILESIZE) + 0.2f);
            flipsidesifneeded();
        } else {
            setYf((float)((ty << 5) + TILESIZE) + 0.2f);
            setXf((float)((nofliptxr << 5) - collisionWidth) - 0.2f);
            flipsidesifneeded();
        }

        break;
    }

    //[ ][X]
    //[ ][X]
    case 10: {
        setXf((float)((nofliptxr << 5) - collisionWidth) - 0.2f);
        flipsidesifneeded();
        break;
    }

    //[X][X]
    //[ ][X]
    case 11: {
        setYf((float)((ty << 5) + TILESIZE) + 0.2f);
        setXf((float)((nofliptxr << 5) - collisionWidth) - 0.2f);
        flipsidesifneeded();
        break;
    }

    //[ ][ ]
    //[X][X]
    case 12: {
        setYf((float)((ty2 << 5) - collisionHeight) - 0.2f);
        break;
    }

    //[X][ ]
    //[X][X]
    case 13: {
        setYf((float)((ty2 << 5) - collisionHeight) - 0.2f);
        setXf((float)((nofliptxl << 5) + TILESIZE) + 0.2f);
        flipsidesifneeded();
        break;
    }

    //[ ][X]
    //[X][X]
    case 14: {
        setYf((float)((ty2 << 5) - collisionHeight) - 0.2f);
        setXf((float)((nofliptxr << 5) - collisionWidth) - 0.2f);
        flipsidesifneeded();
        break;
    }

    //If object is completely inside a block, default to moving it down
    //[X][X]
    //[X][X]
    case 15: {
        setYf((float)((ty2 << 5) + TILESIZE) + 0.2f);
        break;
    }

    default: {
        fRet = false;
        break;
    }
    }

    //Updated object to have correct precalculatedY since it wasn't getting collision detection updates
    //while it was being held by the player
    fPrecalculatedY = fy;

    //Check moving platforms and make sure this object is not inside one
    fRet |= g_map->movingPlatformCheckSides(this);

    return fRet;
}

void IO_MovingObject::flipsidesifneeded()
{
    //Use ix here to avoid rounding issues (can crash if txr evals to over the right side of screen)
    if (ix < 0 || fx < 0.0f) {
        setXf(fx + smw->ScreenWidth);
        fOldX += smw->ScreenWidth;
    } else if (ix >= smw->ScreenWidth || fx >= smw->ScreenWidth) {
        setXf(fx - smw->ScreenWidth);
        fOldX -= smw->ScreenWidth;
    }
}

//This method probably needs to be rewritten so that it just calls into the object being killed
//And let it handle not dying and placing itself somewhere else if necessary
void IO_MovingObject::KillObjectMapHazard(short playerID)
{
    if (!dead) {
        //If it is a throw box, trigger it's behavior and return
        if (movingObjectType == movingobject_throwbox) {
            ((CO_ThrowBox*)this)->Die();
            return;
        }

        dead = true;
        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix + (iw >> 1) - 16, iy + (ih >> 1) - 16, 3, 4));

        if (movingObjectType == movingobject_fireball) {
            CPlayer * player = GetPlayerFromGlobalID(iPlayerID);
            if (player)
                player->decreaseProjectilesCount();

            ifSoundOnPlay(rm->sfx_hit);
        } else if (movingObjectType == movingobject_egg) {
            dead = false;
            ((CO_Egg*)this)->placeEgg();
            ifSoundOnPlay(rm->sfx_transform);
        } else if (movingObjectType == movingobject_star) {
            dead = false;
            ((CO_Star*)this)->placeStar();
            ifSoundOnPlay(rm->sfx_transform);
        } else if (movingObjectType == movingobject_flag) {
            dead = false;
            ((CO_Flag*)this)->placeFlag();
            ifSoundOnPlay(rm->sfx_transform);
        } else if (movingObjectType == movingobject_bomb) {
            CPlayer * player = GetPlayerFromGlobalID(iPlayerID);
            if (player)
                player->decreaseProjectilesCount();

            ifSoundOnPlay(rm->sfx_hit);
        } else if (movingObjectType == movingobject_phantokey) {
            dead = false;
            ((CO_PhantoKey*)this)->placeKey();
            ifSoundOnPlay(rm->sfx_transform);
        } else if (game_mode_boxes_minigame == game_values.gamemode->gamemode && movingObjectType == movingobject_coin) {
            dead = false;
            ((MO_Coin*)this)->placeCoin();
            ifSoundOnPlay(rm->sfx_transform);
        } else if (game_values.gamemode->gamemode == game_mode_stomp && (movingObjectType == movingobject_goomba || movingObjectType == movingobject_koopa || movingObjectType == movingobject_spiny || movingObjectType == movingobject_buzzybeetle || movingObjectType == movingobject_cheepcheep)) {
            if (!game_values.gamemode->gameover) {
                CPlayer * player = GetPlayerFromGlobalID(playerID);

                if (NULL != player) {
                    player->score->AdjustScore(1);
                }
            }

            ifSoundOnPlay(rm->sfx_hit);
        } else {
            ifSoundOnPlay(rm->sfx_hit);
        }
    }
}
