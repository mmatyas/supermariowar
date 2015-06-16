#include "PlayerAwardEffects.h"

#include "eyecandy.h"
#include "GameValues.h"
#include "player.h"
#include "ResourceManager.h"

#include <cassert>
#include <cmath>

extern CGameValues game_values;
extern CResourceManager* rm;

extern CEyecandyContainer eyecandy[3];

extern void CheckSecret(short id);

struct STextAward {
    const char      *name;
    gfxFont         *font;

    STextAward(const char *nname, gfxFont *nfont) {
        name = nname;
        font = nfont;
    }
};

#define PAWARD_LAST     9
const STextAward awardtexts[PAWARD_LAST] = {
    STextAward("Double Kill", &rm->game_font_small),
    STextAward("Triple Kill", &rm->game_font_small),
    STextAward("Killing Spree",  &rm->game_font_small),
    STextAward("Killing Spree x 2", &rm->game_font_small),
    STextAward("Killing Spree x 3", &rm->game_font_small),
    STextAward("Dominating", &rm->game_font_large),
    STextAward("Dominating x 2", &rm->game_font_large),
    STextAward("Dominating x 3", &rm->game_font_large),
    STextAward("Unstoppable!", &rm->game_font_large)
};

void PlayerAwardEffects::drawRingAward(CPlayer& player)
{
    awardangle += 0.02f;

    if (awardangle > TWO_PI)
        awardangle -= TWO_PI;

    short numawards = (player.killsinrow > MAXAWARDS ? MAXAWARDS : player.killsinrow);
    float addangle = TWO_PI / (float)numawards;

    short xoffset = player.centerX() - 8;
    short yoffset = player.centerY() - 8;

    for (short k = 0; k < numawards; k++) {
        float angle = (float)k * addangle + awardangle;
        short awardx = xoffset + (short)(30.0f * cos(angle));
        short awardy = yoffset + (short)(30.0f * sin(angle));

        if (player.iswarping())
            rm->spr_award.draw(awardx, awardy, awards[k] * 16, 0, 16, 16, (short)player.state % 4, player.GetWarpPlane());
        else
            rm->spr_award.draw(awardx, awardy, awards[k] * 16, 0, 16, 16);
    }
}

void PlayerAwardEffects::addExploding(CPlayer& player)
{
    if (player.killsinrow < MINAWARDSNEEDED)
        return;

    ifSoundOnPlay(rm->sfx_cannon);

    short numawards = (player.killsinrow > MAXAWARDS ? MAXAWARDS : player.killsinrow);
    float addangle = TWO_PI / (float)numawards;

    for (short k = 0; k < numawards; k++) {
        float angle = (float)k * addangle + awardangle;
        float cosangle = cos(angle);
        float sinangle = sin(angle);

        short awardx = player.centerX() - 8 + (short)(30.0f * cosangle);
        short awardy = player.centerY() - 8 + (short)(30.0f * sinangle);

        float awardvelx = 7.0f * cosangle;
        float awardvely = 7.0f * sinangle;

        eyecandy[2].add(new EC_ExplodingAward(&rm->spr_awardsolid, awardx, awardy, awardvelx, awardvely, 30, awards[k]));
    }
}

void PlayerAwardEffects::addSwirling(CPlayer& player)
{
    if (player.killsinrow < MINAWARDSNEEDED)
        return;

    ifSoundOnPlay(rm->sfx_cannon);

    short numawards = (player.killsinrow > MAXAWARDS ? MAXAWARDS : player.killsinrow);
    float addangle = TWO_PI / (float)numawards;

    for (short k = 0; k < numawards; k++) {
        float angle = (float)k * addangle + awardangle;

        if (numawards == MAXAWARDS)
            eyecandy[2].add(new EC_SwirlingAward(&rm->spr_awardkillsinrow, player.centerX() - 8, player.centerY() - 8, angle, 30.0f, 0.05f, 60, 10, player.getColorID(), 16, 16, 4, 4));
        else
            eyecandy[2].add(new EC_SwirlingAward(&rm->spr_awardkillsinrow, player.centerX() - 8, player.centerY() - 8, angle, 30.0f, 0.05f, 60, numawards - 1, player.getColorID(), 16, 16));
    }
}

void PlayerAwardEffects::addRocket(CPlayer& player)
{
    if (player.killsinrow < MINAWARDSNEEDED)
        return;

    ifSoundOnPlay(rm->sfx_cannon);

    short numawards = (player.killsinrow > MAXAWARDS ? MAXAWARDS : player.killsinrow);

    float addangle = QUARTER_PI / 10.0f;
    float startangle = -HALF_PI - ((addangle / 2) * (float)(numawards - 1));

    for (short k = 0; k < numawards; k++) {
        float angle = (float)k * addangle + startangle;
        float awardvelx = 9.0f * cos(angle);
        float awardvely = 9.0f * sin(angle);

        if (numawards == MAXAWARDS)
            eyecandy[2].add(new EC_RocketAward(&rm->spr_awardkillsinrow, player.centerX() - 8, player.centerY() - 8, awardvelx, awardvely, 80, 10, player.getColorID(), 16, 16, 4, 4));
        else
            eyecandy[2].add(new EC_RocketAward(&rm->spr_awardkillsinrow, player.centerX() - 8, player.centerY() - 8, awardvelx, awardvely, 80, numawards - 1, player.getColorID(), 16, 16));
    }
}

void PlayerAwardEffects::addDeathAward(CPlayer& player)
{
    if (game_values.awardstyle == award_style_halo)
        addExploding(player);
    else if (game_values.awardstyle == award_style_souls && player.killsinrow >= MINAWARDSNEEDED)
        eyecandy[2].add(new EC_SoulsAward(&rm->spr_awardsouls,
            &rm->spr_awardsoulspawn,
            player.centerX(), player.centerY(),
            60, 9.0f,
            player.killsinrow, awards));

    player.killsinrow = 0;
    player.killsinrowinair = 0;
}

void PlayerAwardEffects::addKillerAward(CPlayer& killer, CPlayer* killed, killstyle style)
{
    killer.killsinrow++;

    /*
    if (killer.killsinrow >= 10 && game_values.secrets)
    {
        if (RANDOM_INT(5) == 0)
        {
            if (!game_values.gamemode->gameover && game_values.bosspeeking == -1)
            {
                eyecandy[2].add(new EC_BossPeeker(&rm->spr_sledgebrothers, RANDOM_INT(90) + 90, 2));

                rm->backgroundmusic[0].stop();
                ifsoundonstop(rm->sfx_invinciblemusic);
                ifsoundonstop(rm->sfx_timewarning);
            }
        }
    }
    */

    if (killer.inair && (style == kill_style_stomp || style == kill_style_goomba || style == kill_style_koopa || style == kill_style_cheepcheep || style == kill_style_bulletbill || style == kill_style_feather))
        killer.killsinrowinair++;

    //Play announcer
    bool fSoundPlayed = false;
    if (killer.killsinrowinair > 1) {
        if (ifsoundonandreadyplay(rm->sfx_announcer[9]) > -1)
            fSoundPlayed = true;
    }

    if (killed && killed->killsinrow >= 2 && !fSoundPlayed) {
        if (ifsoundonandreadyplay(rm->sfx_announcer[10]) > -1)
            fSoundPlayed = true;
    }

    short awardIndex = 0;
    if (killer.killsinrow >= 2) {
        awardIndex = (killer.killsinrow - 2) >= PAWARD_LAST ? PAWARD_LAST -1 : (killer.killsinrow - 2);

        if (!fSoundPlayed) {
            if (ifsoundonandreadyplay(rm->sfx_announcer[awardIndex]) > -1)
                fSoundPlayed = true;
        }

        if (killer.killsinrow >= 5) {
            game_values.unlocksecret2part1 = true;
            CheckSecret(1);
        }
    }

    //Add eyecandy
    if (game_values.awardstyle != award_style_none) {
        if (game_values.awardstyle == award_style_halo)
            killer.awardeffects.awards[(killer.killsinrow - 1) % MAXAWARDS] = (short)style;
        else if (game_values.awardstyle == award_style_souls) {
            if (killed)
                killer.awardeffects.awards[(killer.killsinrow - 1) % MAXAWARDS] = killed->getColorID();
            else if (style == kill_style_goomba)
                killer.awardeffects.awards[(killer.killsinrow - 1) % MAXAWARDS] = 4; //soul id for goomba
            else if (style == kill_style_bulletbill)
                killer.awardeffects.awards[(killer.killsinrow - 1) % MAXAWARDS] = 5; //soul id for bullet bill
            else if (style == kill_style_cheepcheep)
                killer.awardeffects.awards[(killer.killsinrow - 1) % MAXAWARDS] = 6; //soul id for cheep cheep
            else if (style == kill_style_koopa)
                killer.awardeffects.awards[(killer.killsinrow - 1) % MAXAWARDS] = 7; //soul id for koopa
            else
                killer.awardeffects.awards[(killer.killsinrow - 1) % MAXAWARDS] = 8; //soul id for ?

        } else if (game_values.awardstyle == award_style_swirl)
            killer.awardeffects.addSwirling(killer);
        else if (game_values.awardstyle == award_style_fireworks)
            killer.awardeffects.addRocket(killer);

        if (killer.killsinrowinair > 1)
            killer.awardeffects.addKillsInRowInAirAward(killer);

        //if we have enough kills in a row -> spawn an award

        if (game_values.awardstyle == award_style_text) {
            if (killer.killsinrow >= 2) {
                char    text[128];              //text to show
                sprintf(text, "%d - %s", killer.killsinrow, awardtexts[awardIndex].name);

                //now add the eyecandy
                eyecandy[2].add(new EC_GravText(awardtexts[awardIndex].font, killer.centerX(), killer.bottomY(), text, -VELJUMP));
            }

            //if we stopped the other players run show another award
            if (killed && killed->killsinrow >= 2) {
                short a = (killed->killsinrow - 2) >= PAWARD_LAST ? PAWARD_LAST -1 : (killed->killsinrow - 2);
                char text[128];
                sprintf(text, "%s Stopped!",  awardtexts[a].name);

                eyecandy[2].add(new EC_GravText(awardtexts[a].font, killed->centerX(), killed->bottomY(), text, -VELJUMP*1.3f));
            }
        }
    }
}

void PlayerAwardEffects::addKillsInRowInAirAward(CPlayer& player)
{
    float angle = 0.0f;
    for (short k = 0; k < 15; k++) {
        float vel = 7.0f + ((k % 2) * 5.0f);
        float awardvelx = vel * cos(angle);
        float awardvely = vel * sin(angle);

        eyecandy[2].add(new EC_FallingObject(&rm->spr_bonus,
            player.centerX() - 8,
            player.centerY() - 8,
            awardvelx, awardvely,
            4, 2, 0,
            player.getColorID() * 16, 16, 16));
        angle -= (float)PI / 14;
    }

    //Track to unlock secret
    game_values.unlocksecret1part1[player.getGlobalID()] = true;
    CheckSecret(0);
}
