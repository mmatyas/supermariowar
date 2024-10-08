#include "movingplatform.h"

#include "IO_Block.h"
#include "Game.h"
#include "GameValues.h"
#include "map.h"
#include "player.h"
#include "PlayerKillTypes.h"
#include "ResourceManager.h"
#include "TilesetManager.h"
#include "objects/moving/MovingObject.h"

#include "sdl12wrapper.h"

extern SDL_Surface* screen;
extern SDL_Surface* blitdest;

extern short x_shake;
extern short y_shake;

extern short g_iCurrentDrawIndex;

extern CMap* g_map;
extern CTilesetManager* g_tilesetmanager;

extern std::vector<CPlayer*> players;

extern CGameValues game_values;
extern CResourceManager* rm;

enum CollisionStyle {collision_none, collision_normal, collision_overlap_left, collision_overlap_right};

//------------------------------------------------------------------------------
// Moving Platform
//------------------------------------------------------------------------------

MovingPlatform::MovingPlatform(std::vector<TilesetTile>&& tiledata, std::vector<TileType>&& tiletypes, short w, short h, short drawlayer, MovingPlatformPath * path, bool fPreview)
{
    fDead = false;
    iPlayerId = -1;

    short iTileSize = TILESIZE;
    short iTileSizeIndex = 0;

    if (fPreview) {
        iTileSize = PREVIEWTILESIZE;
        iTileSizeIndex = 1;
    }

    iTileData = std::move(tiledata);
    iTileType = std::move(tiletypes);
    assert(iTileData.size() == iTileType.size());
    assert(iTileData.size() == static_cast<size_t>(w * h));

    iTileWidth = w;
    iTileHeight = h;

    iDrawLayer = drawlayer;

    iWidth = w * iTileSize;
    iHeight = h * iTileSize;

    iHalfWidth = iWidth >> 1;
    iHalfHeight = iHeight >> 1;

    pPath = path;
    pPath->SetPlatform(this);

    ResetPath();

    for (short iSurface = 0; iSurface < 2; iSurface++) {
        sSurface[iSurface] = SDL_CreateRGBSurface(screen->flags, w * iTileSize, h * iTileSize, screen->format->BitsPerPixel, 0, 0, 0, 0);

        if ( SDL_SETCOLORKEY(sSurface[iSurface], SDL_FALSE, SDL_MapRGB(sSurface[iSurface]->format, 255, 0, 255)) < 0)
            printf("\n ERROR: Couldn't set ColorKey for moving platform: %s\n", SDL_GetError());

        SDL_FillRect(sSurface[iSurface], NULL, SDL_MapRGB(sSurface[iSurface]->format, 255, 0, 255));
    }

    //Run through all tiles in the platform, detect unknown and blank tiles,
    //and draw all static tiles to the platform surface
    for (short iSurface = 0; iSurface < 2; iSurface++) {
        for (short iCol = 0; iCol < iTileWidth; iCol++) {
            for (short iRow = 0; iRow < iTileHeight; iRow++) {
                const TilesetTile& tile = iTileData[iCol * iTileHeight + iRow];

                if (tile.iID == TILESETNONE)
                    continue;

                if (tile.iID >= 0) {
                    g_tilesetmanager->Draw(sSurface[iSurface], tile.iID, iTileSizeIndex, tile.iCol, tile.iRow, iCol, iRow);
                } else if (tile.iID == TILESETANIMATED) {
                    SDL_BlitSurface(rm->spr_tileanimation[iTileSizeIndex].getSurface(), g_tilesetmanager->rect(iTileSizeIndex, tile.iCol * 4, tile.iRow), sSurface[iSurface], g_tilesetmanager->rect(iTileSizeIndex, iCol, iRow));
                } else if (tile.iID == TILESETUNKNOWN) {
                    SDL_BlitSurface(rm->spr_unknowntile[iTileSizeIndex].getSurface(), g_tilesetmanager->rect(iTileSizeIndex, 0, 0), sSurface[iSurface], g_tilesetmanager->rect(iTileSizeIndex, iCol, iRow));
                }
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

    fVelX = pPath->velocity0().x;
    fVelY = pPath->velocity0().y;

    fOldVelX = fVelX;
    fOldVelY = fVelY;
}

MovingPlatform::~MovingPlatform()
{
    delete pPath;

    SDL_FreeSurface(sSurface[0]);
    SDL_FreeSurface(sSurface[1]);
}

const TilesetTile& MovingPlatform::tileAt(size_t col, size_t row) const
{
    return iTileData[col * iTileHeight + row];
}

TileType MovingPlatform::tileTypeAt(size_t col, size_t row) const
{
    return iTileType[col * iTileHeight + row];
}

void MovingPlatform::draw()
{
    //Comment this back in to see the no spawn area of the platform
    //SDL_Rect r = {(int)pPath->currentPos1().x - iHalfWidth, (int)pPath->currentPos1().y - iHalfHeight, iWidth, iHeight};
    //SDL_FillRect(blitdest, &r, SDL_MapRGB(blitdest->format, 0, 0, 255));

    rDstRect.x = ix - iHalfWidth + x_shake;
    rDstRect.y = iy - iHalfHeight + y_shake;
    rDstRect.w = iWidth;
    rDstRect.h = iHeight;

    // Blit onto the screen surface
    if (SDL_BlitSurface(sSurface[1 - g_iCurrentDrawIndex], &rSrcRect, blitdest, &rDstRect) < 0) {
        fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
        return;
    }

    //Deal with wrapping over sides of screen
    bool fBlitSide = false;
    if (ix - iHalfWidth < 0) {
        rDstRect.x = ix - iHalfWidth + App::screenWidth + x_shake;
        fBlitSide = true;
    } else if (ix + iHalfWidth >= App::screenWidth) {
        rDstRect.x = ix - iHalfWidth - App::screenWidth + x_shake;
        fBlitSide = true;
    }

    if (fBlitSide) {
        rDstRect.y = iy - iHalfHeight + y_shake;
        rDstRect.w = iWidth;
        rDstRect.h = iHeight;

        if (SDL_BlitSurface(sSurface[1 - g_iCurrentDrawIndex], &rSrcRect, blitdest, &rDstRect) < 0) {
            fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
        }

        //rDstRect.x = ix - iHalfWidth;
    }

    /*
    if (iy - iHalfHeight < 0)
    {
    	rDstRect.y = iy - iHalfHeight + App::screenHeight;
    	rDstRect.x = ix - iHalfWidth;

        if (SDL_BlitSurface(sSurface[1 - g_iCurrentDrawIndex], &rSrcRect, blitdest, &rDstRect) < 0)
    	{
    		fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
    	}

    	rDstRect.y = iy - iHalfHeight;
    }
    else if (iy + iHalfHeight >= App::screenHeight)
    {
    	rDstRect.y = iy - iHalfHeight - App::screenHeight;
    	rDstRect.x = ix - iHalfWidth;

        if (SDL_BlitSurface(sSurface[1 - g_iCurrentDrawIndex], &rSrcRect, blitdest, &rDstRect) < 0)
    	{
    		fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
    	}

    	rDstRect.y = iy - iHalfHeight;
    }

    if (ix - iHalfWidth < 0 && iy - iHalfHeight < 0)
    {
    	rDstRect.x = ix - iHalfWidth + App::screenWidth;
    	rDstRect.y = iy - iHalfHeight + App::screenHeight;

        if (SDL_BlitSurface(sSurface[1 - g_iCurrentDrawIndex], &rSrcRect, blitdest, &rDstRect) < 0)
    	{
    		fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
    	}

    	rDstRect.x = ix - iHalfWidth;
    	rDstRect.y = iy - iHalfHeight;
    }
    else if (ix + iHalfWidth >= App::screenWidth && iy + iHalfHeight >= App::screenHeight)
    {
    	rDstRect.x = ix - iHalfWidth - App::screenWidth;
    	rDstRect.y = iy - iHalfHeight - App::screenHeight;

        if (SDL_BlitSurface(sSurface[1 - g_iCurrentDrawIndex], &rSrcRect, blitdest, &rDstRect) < 0)
    	{
    		fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
    	}

    	rDstRect.x = ix - iHalfWidth;
    	rDstRect.y = iy - iHalfHeight;
    }
    */

    /*
    char szVel[256];
    sprintf(szVel, "%.2f, %.2f", pPath->dVelX, pPath->dVelY);
    menu_font_large.draw(20, 20, szVel);
    */
}

//Draw path for map preview
void MovingPlatform::draw(short iOffsetX, short iOffsetY)
{
    gfx_drawpreview(sSurface[0], ix - iHalfWidth + iOffsetX, iy - iHalfHeight + iOffsetY, 0, 0, iWidth, iHeight, iOffsetX, iOffsetY, App::screenWidth/2, App::screenHeight/2, true);
}

void MovingPlatform::update()
{
    fOldX = pPath->currentPos0().x;
    fOldY = pPath->currentPos0().y;

    fOldVelX = pPath->velocity0().x;
    fOldVelY = pPath->velocity0().y;

    //Path will affect new fVelX and fVelY to move the platform to it's next location
    pPath->Move(0);

    //Will move the path "shadow" to the next location (for spawn collision detection)
    pPath->Move(1);

    fVelX = pPath->velocity0().x;
    fVelY = pPath->velocity0().y;

    setXf(pPath->currentPos0().x);
    setYf(pPath->currentPos0().y);
}

void MovingPlatform::ResetPath()
{
    pPath->Reset();

    setXf(pPath->currentPos0().x);
    setYf(pPath->currentPos0().y);

    fOldX = fx;
    fOldY = fy;

    //Need to advance spawn platform here if spawn time is greater than 0
    //Advance it the spawn time amount here

}

void MovingPlatform::collide(CPlayer * player)
{
    short coldec = coldec_player(player);
    if (coldec == collision_none) {
        if (player->platform == this)
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

    if (fDead)
        return;

    float fColVelX = player->velx - fOldVelX;
    float fColVelY = player->vely - fOldVelY;

    if (player->platform) {
        fColVelY += player->platform->fOldVelY - GRAVITATION;

        if (player->velx < -0.6f || player->velx > 0.6f)
            fColVelX += player->platform->fOldVelX;

        /*
        if (player->platform != this)
        	fColVelX += player->platform->fOldVelX;
        else if (player->platform == this && (player->velx < -0.6f || player->velx > 0.6f))
        	fColVelX += player->platform->fOldVelX;
        */
    } else if (!player->inair) {
        fColVelY -= GRAVITATION;  //Ignore the gravity if the player is on the ground
    }

    float fRelativeY1;
    float fRelativeY2;

    if (player->platform != this) {
        fRelativeY1 = player->fy - fOldY + iHalfHeight;
        fRelativeY2 = player->fy + PH - fOldY + iHalfHeight;
    } else {
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

    if (fColVelX > 0.01f || player->iHorizontalPlatformCollision == 3) {
        float fRelativeX;

        //if (player->fx + PW >= App::screenWidth)
        if (coldec == collision_normal)
            fRelativeX = player->fx + PW - fx + iHalfWidth;
        else if (coldec == collision_overlap_left)
            fRelativeX = player->fx + PW - fx + iHalfWidth - App::screenWidth;
        else
            fRelativeX = player->fx + PW - fx + iHalfWidth + App::screenWidth;

        //printf(">>>>Checking Right Side fRelativeX: %.5f  fRelativeY1: %.5f  fRelativeY2: %.5f\n", fRelativeX, fRelativeY1, fRelativeY2);

        if (fRelativeX >= 0.0f && fRelativeX < iWidth) {
            short tx = (short)fRelativeX / TILESIZE;

            int t1 = tile_flag_nonsolid;
            int t2 = tile_flag_nonsolid;

            if (fRelativeY1 >= 0.0f && fRelativeY1 < iHeight)
                t1 = tileToFlags(tileTypeAt(tx, (short)fRelativeY1 / TILESIZE));

            if (fRelativeY2 >= 0.0f && fRelativeY2 < iHeight)
                t2 = tileToFlags(tileTypeAt(tx, (short)fRelativeY2 / TILESIZE));

            if ((t1 & tile_flag_solid) || (t2 & tile_flag_solid)) {
                bool fDeathTileToLeft = ((t1 & tile_flag_death_on_left) && (t2 & tile_flag_death_on_left)) ||
                                        ((t1 & tile_flag_death_on_left) && !(t2 & tile_flag_solid)) ||
                                        (!(t1 & tile_flag_solid) && (t2 & tile_flag_death_on_left));

                bool fSuperDeathTileToLeft = ((t1 & tile_flag_super_or_player_death_left) && (t2 & tile_flag_super_or_player_death_left)) ||
                                             ((t1 & tile_flag_super_or_player_death_left) && !(t2 & tile_flag_solid)) ||
                                             (!(t1 & tile_flag_solid) && (t2 & tile_flag_super_or_player_death_left));

                if (player->iHorizontalPlatformCollision == 3) {
                    player->KillPlayerMapHazard(true, KillStyle::Environment, true, iPlayerId);
                    //printf("Platform Right Side Killed Player\n");
                    return;
                } else if (fSuperDeathTileToLeft || (fDeathTileToLeft && !player->isInvincible() && !player->isShielded() && !player->shyguy)) {
                    if (PlayerKillType::NonKill != player->KillPlayerMapHazard(fSuperDeathTileToLeft, KillStyle::Environment, false, iPlayerId))
                        return;
                } else {
                    player->setXf((float)((tx << 5) - PW) - 0.2f + fx - iHalfWidth);
                    player->flipsidesifneeded();

                    //test to see if we put the object on top of a background tile
                    check_map_collision_right(player);

                    if (fOldVelX < 0.0f)
                        player->fOldX = fx + 10.0f;  //Need to push out the old enough to collide correctly with flip blocks
                    else
                        player->fOldX = fx - 10.0f;  //Need to push out the old enough to collide correctly with flip blocks

                    player->iHorizontalPlatformCollision = 1;
                    player->iPlatformCollisionPlayerId = iPlayerId;

                    if (player->velx > 0.0f)
                        player->velx = 0.0f;

                    //printf("Hit Platform Right Side\n");
                }
            }
        }
    } else if (fColVelX < -0.01f || player->iHorizontalPlatformCollision == 1) {
        //float fRelativeX = player->fx - fx + iHalfWidth;

        float fRelativeX;
        if (coldec == collision_normal)
            fRelativeX = player->fx - fx + iHalfWidth;
        else if (coldec == collision_overlap_left)
            fRelativeX = player->fx - fx + iHalfWidth - App::screenWidth;
        else
            fRelativeX = player->fx - fx + iHalfWidth + App::screenWidth;

        //printf(">>>>Checking Left Side fRelativeX: %.5f  fRelativeY1: %.5f  fRelativeY2: %.5f\n", fRelativeX, fRelativeY1, fRelativeY2);

        if (fRelativeX >= 0.0f && fRelativeX < iWidth) {
            short tx = (short)fRelativeX / TILESIZE;

            int t1 = tile_flag_nonsolid;
            int t2 = tile_flag_nonsolid;

            if (fRelativeY1 >= 0 && fRelativeY1 < iHeight)
                t1 = tileToFlags(tileTypeAt(tx, (short)fRelativeY1 / TILESIZE));

            if (fRelativeY2 >= 0 && fRelativeY2 < iHeight)
                t2 = tileToFlags(tileTypeAt(tx, (short)fRelativeY2 / TILESIZE));

            if ((t1 & tile_flag_solid) || (t2 & tile_flag_solid)) {
                bool fDeathTileToRight = ((t1 & tile_flag_death_on_right) && (t2 & tile_flag_death_on_right)) ||
                                         ((t1 & tile_flag_death_on_right) && !(t2 & tile_flag_solid)) ||
                                         (!(t1 & tile_flag_solid) && (t2 & tile_flag_death_on_right));

                bool fSuperDeathTileToRight = ((t1 & tile_flag_super_or_player_death_right) && (t2 & tile_flag_super_or_player_death_right)) ||
                                              ((t1 & tile_flag_super_or_player_death_right) && !(t2 & tile_flag_solid)) ||
                                              (!(t1 & tile_flag_solid) && (t2 & tile_flag_super_or_player_death_right));

                if (player->iHorizontalPlatformCollision == 1) {
                    player->KillPlayerMapHazard(true, KillStyle::Environment, true, iPlayerId);
                    //printf("Platform Left Side Killed Player\n");
                    return;
                } else if (fSuperDeathTileToRight || (fDeathTileToRight && !player->isInvincible() && !player->isShielded() && !player->shyguy)) {
                    if (PlayerKillType::NonKill != player->KillPlayerMapHazard(fSuperDeathTileToRight, KillStyle::Environment, false, iPlayerId))
                        return;
                } else {
                    player->setXf((float)((tx << 5) + TILESIZE) + 0.2f + fx - iHalfWidth);
                    player->flipsidesifneeded();

                    //test to see if we put the object on top of a background tile
                    check_map_collision_left(player);

                    //WARNING, WARNING! HACKY!!
                    if (fOldVelX < 0.0f)
                        player->fOldX = fx + 10.0f;  //Need to push out the old enough to collide correctly with flip blocks
                    else
                        player->fOldX = fx - 10.0f;  //Need to push out the old enough to collide correctly with flip blocks

                    player->iHorizontalPlatformCollision = 3;
                    player->iPlatformCollisionPlayerId = iPlayerId;

                    if (player->velx < 0.0f)
                        player->velx = 0.0f;

                    //printf("Hit Platform Left Side\n");
                }
            }
        }
    }

    float fRelativeX1, fRelativeX2;

    //Need to ask for collision detection again because the player might be on the other side of the screen now with wrapping
    coldec = coldec_player(player);
    if (coldec == collision_none) {
        if (player->platform == this)
            player->platform = NULL;

        return;
    } else if (coldec == collision_normal) {
        fRelativeX1 = player->fx - fx + iHalfWidth;
        fRelativeX2 = player->fx + PW - fx + iHalfWidth;
    } else if (coldec == collision_overlap_left) {
        fRelativeX1 = player->fx - fx + iHalfWidth - App::screenWidth;
        fRelativeX2 = player->fx + PW - fx + iHalfWidth - App::screenWidth;
    } else {
        fRelativeX1 = player->fx - fx + iHalfWidth + App::screenWidth;
        fRelativeX2 = player->fx + PW - fx + iHalfWidth + App::screenWidth;
    }

    if (fColVelY < 0.0f) {
        float fRelativeY;

        if (player->inair || player->platform == this)
            fRelativeY = player->fPrecalculatedY - fy + iHalfHeight;
        else
            fRelativeY = player->fy - fy + iHalfHeight;

        //printf(">>>>Checking Bottom Side fRelativeY: %.5f  fRelativeX1: %.5f  fRelativeX2: %.5f\n", fRelativeY, fRelativeX1, fRelativeX2);

        if (fRelativeY >= 0.0f && fRelativeY < iHeight) {
            short ty = (short)fRelativeY / TILESIZE;

            int t1 = tile_flag_nonsolid;
            int t2 = tile_flag_nonsolid;

            if (fRelativeX1 >= 0.0f && fRelativeX1 < iWidth)
                t1 = tileToFlags(tileTypeAt((short)fRelativeX1 / TILESIZE, ty));

            if (fRelativeX2 >= 0.0f && fRelativeX2 < iWidth)
                t2 = tileToFlags(tileTypeAt((short)fRelativeX2 / TILESIZE, ty));

            bool fSolidTileOverPlayer = (t1 & tile_flag_solid) || (t2 & tile_flag_solid);

            bool fDeathTileOverPlayer = ((t1 & tile_flag_death_on_bottom) && (t2 & tile_flag_death_on_bottom)) ||
                                        ((t1 & tile_flag_death_on_bottom) && !(t2 & tile_flag_solid)) ||
                                        (!(t1 & tile_flag_solid) && (t2 & tile_flag_death_on_bottom));

            bool fSuperDeathTileOverPlayer = ((t1 & tile_flag_super_or_player_death_bottom) && (t2 & tile_flag_super_or_player_death_bottom)) ||
                                             ((t1 & tile_flag_super_or_player_death_bottom) && !(t2 & tile_flag_solid)) ||
                                             (!(t1 & tile_flag_solid) && (t2 & tile_flag_super_or_player_death_bottom));

            if (fSolidTileOverPlayer && !fSuperDeathTileOverPlayer &&
                    (!fDeathTileOverPlayer || player->isInvincible() || player->isShielded() || player->shyguy)) {
                if (player->iVerticalPlatformCollision == 2) {
                    player->KillPlayerMapHazard(true, KillStyle::Environment, true, iPlayerId);
                    //printf("Platform Bottom Killed Player\n");
                    return;
                } else {
                    player->fPrecalculatedY = (ty << 5) + TILESIZE + 0.2f + fy - iHalfHeight;
                    player->fOldY = player->fPrecalculatedY - fVelY - GRAVITATION;

                    if (player->vely < 0.0f && fVelY > 0.0f)
                        player->fPrecalculatedY += fVelY;

                    if (player->vely < 0.0f)
                        player->vely = CapFallingVelocity(-player->vely * BOUNCESTRENGTH + fVelY);

                    player->iVerticalPlatformCollision = 0;
                    player->iPlatformCollisionPlayerId = iPlayerId;

                    //printf("Hit Platform Bottom\n");
                }

                return;
            } else if ((t1 & tile_flag_player_or_death_on_bottom) || (t2 & tile_flag_player_or_death_on_bottom)) {
                if (PlayerKillType::NonKill != player->KillPlayerMapHazard(true, KillStyle::Environment, false, iPlayerId))
                    return;
            }
        }
    } else {
        float fRelativeY = player->fPrecalculatedY + PH - fy + iHalfHeight;

        //printf(">>>>Checking Top Side fRelativeY: %.5f  fRelativeX1: %.5f  fRelativeX2: %.5f\n", fRelativeY, fRelativeX1, fRelativeX2);

        if (fRelativeY >= 0.0f && fRelativeY < iHeight) {
            short ty = (short)fRelativeY / TILESIZE;

            int t1 = tile_flag_nonsolid;
            int t2 = tile_flag_nonsolid;

            if (fRelativeX1 >= 0.0f && fRelativeX1 < iWidth)
                t1 = tileToFlags(tileTypeAt((short)fRelativeX1 / TILESIZE, ty));

            if (fRelativeX2 >= 0.0f && fRelativeX2 < iWidth)
                t2 = tileToFlags(tileTypeAt((short)fRelativeX2 / TILESIZE, ty));

            bool fSolidTileUnderPlayer = (t1 & tile_flag_solid) || (t2 & tile_flag_solid);

            if ((t1 & tile_flag_solid_on_top || t2 & tile_flag_solid_on_top) && player->fOldY + PH <= (ty << 5) + fOldY - iHalfHeight) {
                //Deal with player down jumping through solid on top tiles
                if (player->fallthrough && !fSolidTileUnderPlayer) {
                    player->fPrecalculatedY = (ty << 5) - PH + (fVelY > 0.0f ? fVelY : 0.0f) + 0.2f + fy - iHalfHeight;
                    player->inair = true;
                    player->platform = NULL;
                    //printf("Fell Through Solid On Top Platform\n");
                } else {
                    if (player->iVerticalPlatformCollision == 0) {
                        player->KillPlayerMapHazard(true, KillStyle::Environment, true, iPlayerId);
                        //printf("Solid On Top Platform Killed Player\n");
                        return;
                    } else {
                        player->platform = this;
                        player->iVerticalPlatformCollision = 2;
                        player->iPlatformCollisionPlayerId = iPlayerId;

                        player->fPrecalculatedY = (ty << 5) - PH - 0.2f + fy - iHalfHeight;
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

            bool fDeathTileUnderPlayer = ((t1 & tile_flag_death_on_top) && (t2 & tile_flag_death_on_top)) ||
                                         ((t1 & tile_flag_death_on_top) && !(t2 & tile_flag_solid)) ||
                                         (!(t1 & tile_flag_solid) && (t2 & tile_flag_death_on_top));

            bool fSuperDeathTileUnderPlayer = ((t1 & tile_flag_super_or_player_death_top) && (t2 & tile_flag_super_or_player_death_top)) ||
                                              ((t1 & tile_flag_super_or_player_death_top) && !(t2 & tile_flag_solid)) ||
                                              (!(t1 & tile_flag_solid) && (t2 & tile_flag_super_or_player_death_top));

            if (fSolidTileUnderPlayer && !fSuperDeathTileUnderPlayer &&
                    (!fDeathTileUnderPlayer || player->isInvincible() || player->isShielded() || player->kuriboshoe.is_on() || player->shyguy)) {
                //on ground

                if (player->iVerticalPlatformCollision == 0) {
                    player->KillPlayerMapHazard(true, KillStyle::Environment, true, iPlayerId);
                    //printf("Platform Top Killed Player\n");
                    return;
                } else {
                    player->platform = this;
                    player->iVerticalPlatformCollision = 2;
                    player->iPlatformCollisionPlayerId = iPlayerId;

                    player->fPrecalculatedY = (ty << 5) - PH - 0.2f + fy - iHalfHeight;
                    player->fOldY = player->fPrecalculatedY - fVelY;

                    player->vely = GRAVITATION;
                    player->inair = false;
                    player->killsinrowinair = 0;
                    player->extrajumps = 0;

                    if ((t1 & tile_flag_ice && ((t2 & tile_flag_ice) || t2 == tile_flag_nonsolid || t2 == tile_flag_gap)) ||
                            (t2 & tile_flag_ice && ((t1 & tile_flag_ice) || t1 == tile_flag_nonsolid || t1 == tile_flag_gap)))
                        player->onice = true;
                    else
                        player->onice = false;

                    player->fallthrough = false;

                    //printf("Hit Solid Top Platform\n");

                    return;
                }
            } else if (fDeathTileUnderPlayer || fSuperDeathTileUnderPlayer) {
                if (PlayerKillType::NonKill != player->KillPlayerMapHazard(fSuperDeathTileUnderPlayer, KillStyle::Environment, false, iPlayerId))
                    return;
            } else {
                if (player->platform == this)
                    player->platform = NULL;
            }
        }
    }
}


void MovingPlatform::check_map_collision_right(CPlayer * player)
{
    if (player->fy + PH < 0.0f)
        return;

    short iTestBackgroundX;
    if (player->fx + PW >= App::screenWidth)
        iTestBackgroundX = ((short)player->fx + PW - App::screenWidth) / TILESIZE;
    else
        iTestBackgroundX = ((short)player->fx + PW) / TILESIZE;

    short iTestBackgroundY = ((short)player->fy + PH) / TILESIZE;

    IO_Block * topblock = g_map->block(iTestBackgroundX, iTestBackgroundY);

    if ((topblock && !topblock->isTransparent() && !topblock->isHidden()) ||
            (g_map->map(iTestBackgroundX, iTestBackgroundY) & tile_flag_solid)) {
        player->setXf((float)((iTestBackgroundX << 5) - PW) - 0.2f);
        player->flipsidesifneeded();
        return;
    }

    if (player->fy < 0.0f)
        return;

    short iTestBackgroundY2 = (short)player->fy / TILESIZE;

    IO_Block * bottomblock = g_map->block(iTestBackgroundX, iTestBackgroundY2);

    if ((bottomblock && !bottomblock->isTransparent() && !bottomblock->isHidden()) ||
            (g_map->map(iTestBackgroundX, iTestBackgroundY2) & tile_flag_solid)) {
        player->setXf((float)((iTestBackgroundX << 5) - PW) - 0.2f);
        player->flipsidesifneeded();
        return;
    }
}

void MovingPlatform::check_map_collision_left(CPlayer * player)
{
    if (player->fy + PH < 0.0f)
        return;

    short iTestBackgroundX = (short)player->fx / TILESIZE;
    short iTestBackgroundY = ((short)player->fy + PH) / TILESIZE;

    IO_Block * topblock = g_map->block(iTestBackgroundX, iTestBackgroundY);

    if ((topblock && !topblock->isTransparent() && !topblock->isHidden()) ||
            (g_map->map(iTestBackgroundX, iTestBackgroundY) & tile_flag_solid)) {
        player->setXf((float)((iTestBackgroundX << 5) + TILESIZE) + 0.2f);
        player->flipsidesifneeded();
        return;
    }

    if (player->fy < 0.0f)
        return;

    short iTestBackgroundY2 = (short)player->fy / TILESIZE;

    IO_Block * bottomblock = g_map->block(iTestBackgroundX, iTestBackgroundY2);

    if ((bottomblock && !bottomblock->isTransparent() && !bottomblock->isHidden()) ||
            (g_map->map(iTestBackgroundX, iTestBackgroundY2) & tile_flag_solid)) {
        player->setXf((float)((iTestBackgroundX << 5) + TILESIZE) + 0.2f);
        player->flipsidesifneeded();
        return;
    }
}

bool MovingPlatform::collision_detection_check_sides(IO_MovingObject * object)
{
    short coldec = coldec_object(object);
    if (coldec == collision_none)
        return false;

    if (fDead)
        return false;

    //Figure out where the corners of this object are touching
    Uint8 iCase = 0;

    //Check the left side
    float fRelativeXLeft = object->fx - fx + iHalfWidth;
    if (coldec == collision_overlap_left)
        fRelativeXLeft -= App::screenWidth;
    else if (coldec == collision_overlap_right)
        fRelativeXLeft += App::screenWidth;

    //Check the right side
    float fRelativeXRight = fRelativeXLeft + object->collisionWidth;

    //Top and Bottom
    float fRelativeYTop = object->fy - fy + iHalfHeight;
    float fRelativeYBottom = fRelativeYTop + object->collisionHeight;

    short txLeft = -1;
    short txRight = -1;

    if (fRelativeXLeft >= 0.0f && fRelativeXLeft < iWidth)
        txLeft = (short)fRelativeXLeft / TILESIZE;

    if (fRelativeXRight >= 0.0f && fRelativeXRight < iWidth)
        txRight = (short)fRelativeXRight / TILESIZE;

    short tyTop = -1;
    short tyBottom = -1;

    if (fRelativeYTop >= 0.0f && fRelativeYTop < iHeight)
        tyTop = (short)fRelativeYTop / TILESIZE;

    if (fRelativeYBottom >= 0.0f && fRelativeYBottom < iHeight)
        tyBottom = (short)fRelativeYBottom / TILESIZE;

    if (txLeft >= 0) {
        int t1 = tile_flag_nonsolid;
        int t2 = tile_flag_nonsolid;

        if (tyTop >= 0)
            t1 = tileToFlags(tileTypeAt(txLeft, tyTop));

        if (tyBottom >= 0)
            t2 = tileToFlags(tileTypeAt(txLeft, tyBottom));

        if (t1 & tile_flag_solid)
            iCase |= 0x01;

        if (t2 & tile_flag_solid)
            iCase |= 0x04;
    }

    if (txRight >= 0) {
        int t1 = tile_flag_nonsolid;
        int t2 = tile_flag_nonsolid;

        if (tyTop >= 0)
            t1 = tileToFlags(tileTypeAt(txRight, tyTop));

        if (tyBottom >= 0)
            t2 = tileToFlags(tileTypeAt(txRight, tyBottom));

        if (t1 & tile_flag_solid)
            iCase |= 0x02;

        if (t2 & tile_flag_solid)
            iCase |= 0x08;
    }

    //Then determine which way is the best way to move this object out of the solid areas
    switch (iCase) {
        //Do nothing
        //[ ][ ]
        //[ ][ ]
    case 0:
        return false;
        break;

        //[X][ ]
        //[ ][ ]
    case 1: {
        if (object->ix + (object->collisionWidth >> 1) > (txLeft << 5) + TILESIZE) {
            object->setXf((float)((txLeft << 5) + TILESIZE) + 0.2f + fx - iHalfWidth);
            object->flipsidesifneeded();
        } else {
            object->setYf((float)((tyTop << 5) + TILESIZE) + 0.2f + fy - iHalfHeight);
        }

        break;
    }

    //[ ][X]
    //[ ][ ]
    case 2: {
        if (object->ix + (object->collisionWidth >> 1) < (txRight << 5)) {
            object->setXf((float)((txRight << 5) - object->collisionWidth) - 0.2f + fx - iHalfWidth);
            object->flipsidesifneeded();
        } else {
            object->setYf((float)((tyTop << 5) + TILESIZE) + 0.2f + fy - iHalfHeight);
        }

        break;
    }

    //[X][X]
    //[ ][ ]
    case 3: {
        object->setYf((float)((tyTop << 5) + TILESIZE) + 0.2f + fy - iHalfHeight);
        break;
    }

    //[ ][ ]
    //[X][ ]
    case 4: {
        if (object->ix + (object->collisionWidth >> 1) > (txLeft << 5) + TILESIZE) {
            object->setXf((float)((txLeft << 5) + TILESIZE) + 0.2f + fx - iHalfWidth);
            object->flipsidesifneeded();
        } else {
            object->setYf((float)((tyBottom << 5) - object->collisionHeight) - 0.2f + fy - iHalfHeight);
        }

        break;
    }

    //[X][ ]
    //[X][ ]
    case 5: {
        object->setXf((float)((txLeft << 5) + TILESIZE) + 0.2f + fx - iHalfWidth);
        object->flipsidesifneeded();
        break;
    }

    //[ ][X]
    //[X][ ]
    case 6: {
        if (object->ix + (object->collisionWidth >> 1) > (txLeft << 5) + TILESIZE) {
            object->setYf((float)((tyTop << 5) + TILESIZE) + 0.2f + fy - iHalfHeight);
            object->setXf((float)((txLeft << 5) + TILESIZE) + 0.2f + fx - iHalfWidth);
            object->flipsidesifneeded();
        } else {
            object->setYf((float)((tyBottom << 5) - object->collisionHeight) - 0.2f + fy - iHalfHeight);
            object->setXf((float)((txRight << 5) - object->collisionWidth) - 0.2f + fx - iHalfWidth);
            object->flipsidesifneeded();
        }

        break;
    }

    //[X][X]
    //[X][ ]
    case 7: {
        object->setYf((float)((tyTop << 5) + TILESIZE) + 0.2f + fy - iHalfHeight);
        object->setXf((float)((txLeft << 5) + TILESIZE) + 0.2f + fx - iHalfWidth);
        object->flipsidesifneeded();
        break;
    }

    //[ ][ ]
    //[ ][X]
    case 8: {
        if (object->ix + (object->collisionWidth >> 1) < (txRight << 5)) {
            object->setXf((float)((txRight << 5) - object->collisionWidth) - 0.2f + fx - iHalfWidth);
            object->flipsidesifneeded();
        } else {
            object->setYf((float)((tyBottom << 5) - object->collisionHeight) - 0.2f + fy - iHalfHeight);
        }

        break;
    }

    //[X][ ]
    //[ ][X]
    case 9: {
        if (object->ix + (object->collisionWidth >> 1) > (txLeft << 5) + TILESIZE) {
            object->setYf((float)((tyBottom << 5) - object->collisionHeight) - 0.2f + fy - iHalfHeight);
            object->setXf((float)((txLeft << 5) + TILESIZE) + 0.2f + fx - iHalfWidth);
            object->flipsidesifneeded();
        } else {
            object->setYf((float)((tyTop << 5) + TILESIZE) + 0.2f + fy - iHalfHeight);
            object->setXf((float)((txRight << 5) - object->collisionWidth) - 0.2f + fx - iHalfWidth);
            object->flipsidesifneeded();
        }

        break;
    }

    //[ ][X]
    //[ ][X]
    case 10: {
        object->setXf((float)((txRight << 5) - object->collisionWidth) - 0.2f + fx - iHalfWidth);
        object->flipsidesifneeded();
        break;
    }

    //[X][X]
    //[ ][X]
    case 11: {
        object->setYf((float)((tyTop << 5) + TILESIZE) + 0.2f + fy - iHalfHeight);
        object->setXf((float)((txRight << 5) - object->collisionWidth) - 0.2f + fx - iHalfWidth);
        object->flipsidesifneeded();
        break;
    }

    //[ ][ ]
    //[X][X]
    case 12: {
        object->setYf((float)((tyBottom << 5) - object->collisionHeight) - 0.2f + fy - iHalfHeight);
        break;
    }

    //[X][ ]
    //[X][X]
    case 13: {
        object->setYf((float)((tyBottom << 5) - object->collisionHeight) - 0.2f + fy - iHalfHeight);
        object->setXf((float)((txLeft << 5) + TILESIZE) + 0.2f + fx - iHalfWidth);
        object->flipsidesifneeded();
        break;
    }

    //[ ][X]
    //[X][X]
    case 14: {
        object->setYf((float)((tyBottom << 5) - object->collisionHeight) - 0.2f + fy - iHalfHeight);
        object->setXf((float)((txRight << 5) - object->collisionWidth) - 0.2f + fx - iHalfWidth);
        object->flipsidesifneeded();
        break;
    }

    //If object is completely inside a block, default to moving it down
    //[X][X]
    //[X][X]
    case 15: {
        object->setYf((float)((tyBottom << 5) + TILESIZE) + 0.2f + fy - iHalfHeight);
        break;
    }

    default:
        break;
    }

    return true;
}

//Reports back if we had to overlap the edges to make the collision
short MovingPlatform::coldec_player(CPlayer * player)
{
    //Special cases to deal with players overlapping the right and left sides of the screen
    if (player->fx + PW < fx - iHalfWidth) {
        if (player->fx + App::screenWidth >= fx + iHalfWidth || player->fx + PW + App::screenWidth < fx - iHalfWidth || player->fPrecalculatedY >= fy + iHalfHeight || player->fPrecalculatedY + PH < fy - iHalfHeight)
            return collision_none;
        else
            return collision_overlap_right;
    } else if (fx + iHalfWidth < player->fx) {
        if (player->fx >= fx + iHalfWidth + App::screenWidth || player->fx + PW < fx - iHalfWidth + App::screenWidth || player->fPrecalculatedY >= fy + iHalfHeight || player->fPrecalculatedY + PH < fy - iHalfHeight)
            return collision_none;
        else
            return collision_overlap_left;
    } else { //Normal case where no overlap
        if (player->fx >= fx + iHalfWidth || fx - iHalfWidth > player->fx + PW || player->fPrecalculatedY >= fy + iHalfHeight || fy - iHalfHeight > player->fPrecalculatedY + PH) {
            //printf("No Platform Collision: %.5f >= %.5f || %.5f > %.5f || %.5f >= %.5f || %.5f > %.5f\n", player->fx, fx + iHalfWidth, fx - iHalfWidth, player->fx + PW, player->fPrecalculatedY, fy + iHalfHeight, fy - iHalfHeight, player->fPrecalculatedY + PH);
            return collision_none;
        } else
            return collision_normal;
    }
}

void MovingPlatform::GetTileTypesFromPlayer(CPlayer * player, int * lefttile, int * righttile)
{
    *lefttile = tile_flag_nonsolid;
    *righttile = tile_flag_nonsolid;

    float fRelativeY = player->fPrecalculatedY + PH - fy + iHalfHeight;

    if (fRelativeY < 0.0f || fRelativeY >= iHeight)
        return;

    short ty = (short)fRelativeY / TILESIZE;

    float fRelativeX1 = player->fx - fx + iHalfWidth;

    float fRelativeX2;
    if (player->fx + PW > App::screenWidth)
        fRelativeX2 = player->fx + PW - App::screenWidth - fx + iHalfWidth;
    else
        fRelativeX2 = player->fx + PW - fx + iHalfWidth;

    if (fRelativeX1 >= 0.0f && fRelativeX1 < iWidth)
        *lefttile = tileToFlags(tileTypeAt((short)fRelativeX1 / TILESIZE, ty));

    if (fRelativeX2 >= 0.0f && fRelativeX2 < iWidth)
        *righttile = tileToFlags(tileTypeAt((short)fRelativeX2 / TILESIZE, ty));
}

int MovingPlatform::GetTileTypeFromCoord(short x, short y)
{
    float fRelativeY = y - fy + iHalfHeight;

    if (fRelativeY < 0.0f || fRelativeY >= iHeight)
        return tile_flag_nonsolid;

    if (x >= App::screenWidth)
        x -= App::screenWidth;
    else if (x < 0)
        x += App::screenWidth;

    float fRelativeX = x - fx + iHalfWidth;

    if (fRelativeX < 0.0f || fRelativeX >= iWidth)
        return tile_flag_nonsolid;

    return tileToFlags(tileTypeAt((short)fRelativeX / TILESIZE, (short)fRelativeY / TILESIZE));
}

void MovingPlatform::collide(IO_MovingObject * object)
{
    float fColVelX = object->velx - fOldVelX;

    short coldec = coldec_object(object);
    if (coldec == collision_none) {
        if (object->platform == this)
            object->platform = NULL;

        return;
    }

    if (fDead)
        return;

    float fColVelY = object->vely - fOldVelY;

    if (object->platform) {
        fColVelY += object->platform->fOldVelY - GRAVITATION;

        if (object->velx < -0.6f || object->velx > 0.6f)
            fColVelX += object->platform->fOldVelX;

        /*
        if (object->platform != this)
        	fColVelX += object->platform->fOldVelX;
        else if (object->platform == this && object->velx != 0.0f)
        	fColVelX += object->platform->fOldVelX;
        	*/
    } else if (!object->inair) {
        fColVelY -= GRAVITATION;  //Ignore the gravity if the object is on the ground
    }

    float fRelativeY1;
    float fRelativeY2;

    if (object->platform != this) {
        fRelativeY1 = object->fy - fOldY + iHalfHeight;
        fRelativeY2 = fRelativeY1 + object->collisionHeight;
    } else {
        fRelativeY1 = object->fy - fy + iHalfHeight;
        fRelativeY2 = fRelativeY1 + object->collisionHeight;
    }

    if (fColVelX > 0.01f || object->iHorizontalPlatformCollision == 3) {
        float fRelativeX;
        if (coldec == collision_overlap_left)
            fRelativeX = object->fx + object->collisionWidth - fx + iHalfWidth - App::screenWidth;
        else if (coldec == collision_overlap_right)
            fRelativeX = object->fx + object->collisionWidth - fx + iHalfWidth + App::screenWidth;
        else
            fRelativeX = object->fx + object->collisionWidth - fx + iHalfWidth;

        if (fRelativeX >= 0.0f && fRelativeX < iWidth) {
            short tx = (short)fRelativeX / TILESIZE;

            int t1 = tile_flag_nonsolid;
            int t2 = tile_flag_nonsolid;

            if (fRelativeY1 >= 0.0f && fRelativeY1 < iHeight)
                t1 = tileToFlags(tileTypeAt(tx, (short)fRelativeY1 / TILESIZE));

            if (fRelativeY2 >= 0.0f && fRelativeY2 < iHeight)
                t2 = tileToFlags(tileTypeAt(tx, (short)fRelativeY2 / TILESIZE));

            if ((t1 & tile_flag_solid) || (t2 & tile_flag_solid)) {
                if (object->iHorizontalPlatformCollision == 3) {
                    object->KillObjectMapHazard();
                    return;
                } else {
                    object->setXf((float)((tx << 5) - object->collisionWidth) - 0.2f + fx - iHalfWidth);
                    object->flipsidesifneeded();

                    //test to see if we put the object on top of a background tile
                    short iTestBackgroundY = (short)object->fy / TILESIZE;
                    short iTestBackgroundY2 = ((short)object->fy + object->collisionHeight) / TILESIZE;

                    short iTestBackgroundX;
                    if (object->fx + object->collisionWidth >= App::screenWidth)
                        iTestBackgroundX = ((short)object->fx + object->collisionWidth - App::screenWidth) / TILESIZE;
                    else
                        iTestBackgroundX = ((short)object->fx + object->collisionWidth) / TILESIZE;

                    IO_Block * topblock = g_map->block(iTestBackgroundX, iTestBackgroundY);
                    IO_Block * bottomblock = g_map->block(iTestBackgroundX, iTestBackgroundY2);

                    if ((topblock && !topblock->isTransparent() && !topblock->isHidden()) ||
                            (bottomblock && !bottomblock->isTransparent() && !bottomblock->isHidden()) ||
                            (g_map->map(iTestBackgroundX, iTestBackgroundY) & tile_flag_solid) ||
                            (g_map->map(iTestBackgroundX, iTestBackgroundY2) & tile_flag_solid)) {
                        object->setXf((float)((iTestBackgroundX << 5) - object->collisionWidth) - 0.2f);
                        object->flipsidesifneeded();
                    }

                    if (fOldVelX < 0.0f)
                        object->fOldX = fx + 10.0f;  //Need to push out the old enough to collide correctly with flip blocks
                    else
                        object->fOldX = fx - 10.0f;  //Need to push out the old enough to collide correctly with flip blocks

                    object->iHorizontalPlatformCollision = 1;

                    if (object->velx > 0.0f)
                        object->velx = -object->velx;

                    removeifprojectile(object, true, true);

                    object->SideBounce(true);
                }
            }
        }
    } else if (fColVelX < -0.01f || object->iHorizontalPlatformCollision == 1) {
        float fRelativeX;
        if (coldec == collision_overlap_left)
            fRelativeX = object->fx - fx + iHalfWidth - App::screenWidth;
        else if (coldec == collision_overlap_right)
            fRelativeX = object->fx - fx + iHalfWidth + App::screenWidth;
        else
            fRelativeX = object->fx - fx + iHalfWidth;

        if (fRelativeX >= 0.0f && fRelativeX < iWidth) {
            short tx = (short)fRelativeX / TILESIZE;

            int t1 = tile_flag_nonsolid;
            int t2 = tile_flag_nonsolid;

            if (fRelativeY1 >= 0 && fRelativeY1 < iHeight)
                t1 = tileToFlags(tileTypeAt(tx, (short)fRelativeY1 / TILESIZE));

            if (fRelativeY2 >= 0 && fRelativeY2 < iHeight)
                t2 = tileToFlags(tileTypeAt(tx, (short)fRelativeY2 / TILESIZE));

            if ((t1 & tile_flag_solid) || (t2 & tile_flag_solid)) {
                if (object->iHorizontalPlatformCollision == 1) {
                    object->KillObjectMapHazard();
                    return;
                } else {
                    object->setXf((float)((tx << 5) + TILESIZE) + 0.2f + fx - iHalfWidth);
                    object->flipsidesifneeded();

                    //test to see if we put the object on top of a background tile
                    short iTestBackgroundY = (short)object->fy / TILESIZE;
                    short iTestBackgroundY2 = ((short)object->fy + object->collisionHeight) / TILESIZE;
                    short iTestBackgroundX = (short)object->fx / TILESIZE;

                    IO_Block * topblock = g_map->block(iTestBackgroundX, iTestBackgroundY);
                    IO_Block * bottomblock = g_map->block(iTestBackgroundX, iTestBackgroundY2);

                    if ((topblock && !topblock->isTransparent() && !topblock->isHidden()) ||
                            (bottomblock && !bottomblock->isTransparent() && !bottomblock->isHidden()) ||
                            (g_map->map(iTestBackgroundX, iTestBackgroundY) & tile_flag_solid) ||
                            (g_map->map(iTestBackgroundX, iTestBackgroundY2) & tile_flag_solid)) {
                        object->setXf((float)((iTestBackgroundX << 5) + TILESIZE) + 0.2f);
                        object->flipsidesifneeded();
                    }

                    if (fOldVelX < 0.0f)
                        object->fOldX = fx + 10.0f;  //Need to push out the old enough to collide correctly with flip blocks
                    else
                        object->fOldX = fx - 10.0f;  //Need to push out the old enough to collide correctly with flip blocks

                    object->iHorizontalPlatformCollision = 3;

                    if (object->velx < 0.0f)
                        object->velx = -object->velx;

                    removeifprojectile(object, true, true);

                    object->SideBounce(false);
                }
            }
        }
    }

    float fRelativeX1, fRelativeX2;

    coldec = coldec_object(object);
    if (coldec == collision_none) {
        if (object->platform == this)
            object->platform = NULL;

        return;
    } else if (coldec == collision_normal) {
        fRelativeX1 = object->fx - fx + iHalfWidth;
        fRelativeX2 = object->fx + object->collisionWidth - fx + iHalfWidth;
    } else if (coldec == collision_overlap_left) {
        fRelativeX1 = object->fx - fx + iHalfWidth - App::screenWidth;
        fRelativeX2 = object->fx + object->collisionWidth - fx + iHalfWidth - App::screenWidth;
    } else {
        fRelativeX1 = object->fx - fx + iHalfWidth + App::screenWidth;
        fRelativeX2 = object->fx + object->collisionWidth - fx + iHalfWidth + App::screenWidth;
    }

    if (fColVelY < 0.0f) {
        float fRelativeY;

        if (object->inair || object->platform == this)
            fRelativeY = object->fPrecalculatedY - fy + iHalfHeight;
        else
            fRelativeY = object->fy - fy + iHalfHeight;

        if (fRelativeY >= 0.0f && fRelativeY < iHeight) {
            short ty = (short)fRelativeY / TILESIZE;

            int t1 = tile_flag_nonsolid;
            int t2 = tile_flag_nonsolid;

            if (fRelativeX1 >= 0.0f && fRelativeX1 < iWidth)
                t1 = tileToFlags(tileTypeAt((short)fRelativeX1 / TILESIZE, ty));

            if (fRelativeX2 >= 0.0f && fRelativeX2 < iWidth)
                t2 = tileToFlags(tileTypeAt((short)fRelativeX2 / TILESIZE, ty));

            if ((t1 & tile_flag_solid) || (t2 & tile_flag_solid)) {
                if (object->iVerticalPlatformCollision == 2) {
                    object->KillObjectMapHazard();
                    return;
                } else {
                    object->fPrecalculatedY = (ty << 5) + TILESIZE + 0.2f + fy - iHalfHeight;
                    object->fOldY = object->fPrecalculatedY - fVelY - GRAVITATION;

                    if (object->vely < 0.0f && fVelY > 0.0f)
                        object->fPrecalculatedY += fVelY;

                    if (object->vely < 0.0f)
                        object->vely = CapFallingVelocity(-object->vely + fVelY);

                    object->iVerticalPlatformCollision = 0;
                }

                return;
            }
        }
    } else {
        float fRelativeY = object->fPrecalculatedY + object->collisionHeight - fy + iHalfHeight;;

        if (fRelativeY >= 0.0f && fRelativeY < iHeight) {
            short ty = (short)fRelativeY / TILESIZE;

            int t1 = tile_flag_nonsolid;
            int t2 = tile_flag_nonsolid;

            if (fRelativeX1 >= 0.0f && fRelativeX1 < iWidth)
                t1 = tileToFlags(tileTypeAt((short)fRelativeX1 / TILESIZE, ty));

            if (fRelativeX2 >= 0.0f && fRelativeX2 < iWidth)
                t2 = tileToFlags(tileTypeAt((short)fRelativeX2 / TILESIZE, ty));

            if (((t1 & tile_flag_solid_on_top) || (t2 & tile_flag_solid_on_top)) && object->fOldY + object->collisionHeight <= (ty << 5) + fOldY - iHalfHeight) {
                if (object->iVerticalPlatformCollision == 0) {
                    object->KillObjectMapHazard();
                    return;
                } else {
                    if (object->bounce >= 0.0f)
                        object->platform = this;

                    object->iVerticalPlatformCollision = 2;

                    object->fPrecalculatedY = (ty << 5) - object->collisionHeight - 0.2f + fy - iHalfHeight;
                    object->fOldY = object->fPrecalculatedY - fVelY;
                    object->vely = object->BottomBounce();
                    object->inair = false;
                    object->onice = false;
                }

                return;
            }

            bool fSuperDeathTileUnderObject = object->fObjectDiesOnSuperDeathTiles && (((t1 & tile_flag_super_death_top) && (t2 & tile_flag_super_death_top)) ||
                                              ((t1 & tile_flag_super_death_top) && !(t2 & tile_flag_solid)) ||
                                              (!(t1 & tile_flag_solid) && (t2 & tile_flag_super_death_top)));

            if (((t1 & tile_flag_solid) || (t2 & tile_flag_solid)) && !fSuperDeathTileUnderObject) {
                //on ground
                if (object->iVerticalPlatformCollision == 0) {
                    object->KillObjectMapHazard();
                    return;
                } else {
                    if (object->bounce >= 0.0f)
                        object->platform = this;

                    object->iVerticalPlatformCollision = 2;

                    object->fPrecalculatedY = (ty << 5) - object->collisionHeight - 0.2f + fy - iHalfHeight;
                    object->fOldY = object->fPrecalculatedY - fVelY;
                    object->vely = object->BottomBounce();
                    object->inair = false;

                    if ((t1 & tile_flag_ice && ((t2 & tile_flag_ice) || t2 == tile_flag_nonsolid || t2 == tile_flag_gap)) ||
                            (t2 & tile_flag_ice && ((t1 & tile_flag_ice) || t1 == tile_flag_nonsolid || t1 == tile_flag_gap)))
                        object->onice = true;
                    else
                        object->onice = false;

                    return;
                }
            } else if (fSuperDeathTileUnderObject) {
                object->KillObjectMapHazard();
                return;
            } else {
                if (object->platform == this)
                    object->platform = NULL;
            }
        }
    }
}

short MovingPlatform::coldec_object(IO_MovingObject * object)
{
    //Special cases to deal with players overlapping the right and left sides of the screen
    if (object->fx + object->collisionWidth < fx - iHalfWidth) {
        if (object->fx + App::screenWidth >= fx + iHalfWidth || object->fx + object->collisionWidth + App::screenWidth < fx - iHalfWidth || object->fPrecalculatedY >= fy + iHalfHeight || object->fPrecalculatedY + object->collisionHeight < fy - iHalfHeight)
            return collision_none;
        else
            return collision_overlap_right;
    } else if (fx + iHalfWidth < object->fx) {
        if (object->fx >= fx + iHalfWidth + App::screenWidth || object->fx + object->collisionWidth < fx - iHalfWidth + App::screenWidth || object->fPrecalculatedY >= fy + iHalfHeight || object->fPrecalculatedY + object->collisionHeight < fy - iHalfHeight)
            return collision_none;
        else
            return collision_overlap_left;
    } else { //Normal case where no overlap
        if (object->fx >= fx + iHalfWidth || fx - iHalfWidth > object->fx + object->collisionWidth || object->fPrecalculatedY >= fy + iHalfHeight || fy - iHalfHeight > object->fPrecalculatedY + object->collisionHeight)
            return collision_none;
        else
            return collision_normal;
    }
}


bool MovingPlatform::IsInNoSpawnZone(short iX, short iY, short w, short h)
{
    short iTop = (short)pPath->currentPos1().y - iHalfHeight;

    if (iY + h < iTop)
        return false;

    short iBottom = (short)pPath->currentPos1().y + iHalfHeight;

    if (iY >= iBottom)
        return false;

    short iLeft = (short)pPath->currentPos1().x - iHalfWidth;
    short iRight = (short)pPath->currentPos1().x + iHalfWidth;

    //Deal with screen side overlap
    if (iX + w < iLeft)
    	iX += App::screenWidth;
    else if (iX >= iRight)
    	iX -= App::screenWidth;

    if (iX + w < iLeft || iX >= iRight)
        return false;

    //If we have reached this point, the item is colliding with the bounding rect of the platform
    //but not necessarily colliding with the platform (example a U shaped platform has an open area in the center)

    short iRelativeX[2] = {static_cast<short>(iX - iLeft), static_cast<short>(iX + w - iLeft)};
    short iRelativeY[2] = {static_cast<short>(iY - iTop), static_cast<short>(iY + h - iTop)};

    for (short sX = 0; sX < 2; sX++) {
        for (short sY = 0; sY < 2; sY++) {
            if (iRelativeX[sX] >= 0 && iRelativeX[sX] < iWidth && iRelativeY[sY] >= 0 && iRelativeY[sY] < iHeight) {
                short tx = iRelativeX[sX] / TILESIZE;
                short ty = iRelativeY[sY] / TILESIZE;

                int t = tileToFlags(tileTypeAt(tx, ty));

                if (t & tile_flag_solid) {
                    return true;
                }
            }
        }
    }

    return false;
}
