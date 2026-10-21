#include "WorldPlayer.h"

#include "FileList.h"
#include "GameValues.h"
#include "ResourceManager.h"

extern CGameValues game_values;
extern CResourceManager* rm;

extern SkinList *skinlist;

WorldPlayer::WorldPlayer()
    : WorldPlayer(0, 0)
{}

WorldPlayer::WorldPlayer(short iCol, short iRow)
    : WorldMovingObject()
{
    WorldMovingObject::Init(iCol, iRow, 0, 0, 32);
}

void WorldPlayer::Draw(short iMapOffsetX, short iMapOffsetY) const
{
    rm->spr_player[iDrawSprite][iAnimationFrame + iDrawDirection].draw(pos.x + iMapOffsetX, pos.y + iMapOffsetY, {0, 0, 32, 32});
}

void WorldPlayer::SetSprite(short iPlayer)
{
    while (!rm->LoadMenuSkin(iPlayer, game_values.skinids[iPlayer], game_values.colorids[iPlayer], true)) {
        if (++game_values.skinids[iPlayer] >= static_cast<short>(skinlist->count()))
            game_values.skinids[iPlayer] = 0;
    }

    iDrawSprite = iPlayer;
}
