#include "PlayerJail.h"

#include "eyecandy.h"
#include "player.h"
#include "GameValues.h"
#include "ResourceManager.h"

#include <cassert>

extern CEyecandyContainer eyecandy[3];
extern CGameValues game_values;
extern CResourceManager* rm;

PlayerJail::PlayerJail()
    : owner_teamID(-1)
    , color(0)
    , timer(0)
{}

bool PlayerJail::isActive() const
{
    return timer > 0;
}

void PlayerJail::setColor(int8_t color)
{
    assert(color < 4 && color >= -1);
    this->color = color;
}

int8_t PlayerJail::getColor() const
{
    return color;
}

void PlayerJail::lockInBy(CPlayer& inflictor)
{
    timer = game_values.gamemodesettings.jail.timetofree;
    owner_teamID = inflictor.teamID;

    if (game_values.gamemodesettings.jail.style == 1)
        color = inflictor.colorID;
    else
        color = -1;
}

void PlayerJail::escape(CPlayer& player)
{
    if (isActive()) {
        timer = 0;
        owner_teamID = -1;

        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_poof,
            player.centerX() - 24,
            player.centerY() - 24,
            4, 5));
        ifSoundOnPlay(rm->sfx_transform);
    } else {
        ifSoundOnPlay(rm->sfx_hit);
    }
}

void PlayerJail::free_by_teammate(CPlayer& player)
{
    if (isActive()) {
        timer = 0;
        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion,
            player.centerX() - 16,
            player.centerY() - 16,
            3, 8));
        ifSoundOnPlay(rm->sfx_transform);
    }
}

void PlayerJail::update(CPlayer& player)
{
    if (isActive() && game_values.gamemodesettings.jail.timetofree > 1) {
        if (--timer <= 0) {
            timer = 0;
            owner_teamID = -1;
            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion,
                player.centerX() - 16,
                player.centerY() - 16,
                3, 8));
            ifSoundOnPlay(rm->sfx_transform);
        }
    }
}

void PlayerJail::draw(CPlayer& player)
{
    if (isActive()) {
        if (player.iswarping())
            rm->spr_jail.draw(
                player.leftX() - PWOFFSET - 6,
                player.topY() - PHOFFSET - 6,
                (color + 1) * 44,
                0, 44, 44,
                (short)player.state % 4, player.GetWarpPlane());
        else
            rm->spr_jail.draw(
                player.leftX() - PWOFFSET - 6,
                player.topY() - PHOFFSET - 6,
                (color + 1) * 44,
                0, 44, 44);
    }
}
