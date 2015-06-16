#include "PlayerCardCollection.h"

#include "gamemodes.h"
#include "GameValues.h"
#include "player.h"

extern CGameValues game_values;

void PlayerCardCollection::reset() {
    timer = 0;
    index = 0;
}

void PlayerCardCollection::update(CPlayer& player, const uint8_t keymask)
{
    if (game_values.gamemode->gamemode == game_mode_collection) {
        if (index > 0) {
            if (++timer >= 32) {
                index = 0;
                timer = 0;
            }
        }

        if (keymask & 16) {
            index++;
        } else if (keymask & ~16) {
            index = 0;
            timer = 0;
        }

        if (index == 3 && timer < 32) {
            timer = 0;
            index = 0;

            ((CGM_Collection*)game_values.gamemode)->ReleaseCard(player);
        }
    }
}