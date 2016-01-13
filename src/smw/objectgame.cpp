#include "objectgame.h"

#include "eyecandy.h"
#include "GameMode.h"
#include "gamemodes.h"
#include "GameValues.h"
#include "map.h"
#include "movingplatform.h"
#include "ObjectContainer.h"
#include "objecthazard.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"

#include <cstdlib> // abs()
#include <cmath>

extern short iKingOfTheHillZoneLimits[4][4];
extern void PlayerKilledPlayer(short iKiller, CPlayer * killed, short deathstyle, short killstyle, bool fForce, bool fKillCarriedItem);
extern void PlayerKilledPlayer(CPlayer * killer, CPlayer * killed, short deathstyle, short killstyle, bool fForce, bool fKillCarriedItem);
extern bool SwapPlayers(short iUsingPlayerID);
extern short scorepowerupoffsets[3][3];

extern CPlayer * GetPlayerFromGlobalID(short iGlobalID);
extern void CheckSecret(short id);
extern SpotlightManager spotlightManager;
extern CObjectContainer objectcontainer[3];

extern CGM_Pipe_MiniGame * pipegamemode;

extern CPlayer* list_players[4];
extern short score_cnt;

extern CMap* g_map;
extern CEyecandyContainer eyecandy[3];

extern CGameValues game_values;
extern CResourceManager* rm;

void removeifprojectile(IO_MovingObject * object, bool playsound, bool forcedead)
{
    if (object->dead)
        return;

    MovingObjectType type = object->movingObjectType;
    if (type == movingobject_fireball || type == movingobject_hammer || type == movingobject_boomerang || type == movingobject_iceblast || type == movingobject_superfireball || type == movingobject_sledgehammer) {
        short iPlayerID = object->iPlayerID;
        bool fDie = true;

        if (type == movingobject_hammer && !game_values.hammerpower) {
            fDie = false;
        }

        if (fDie || forcedead) {
        	CPlayer * player = GetPlayerFromGlobalID(iPlayerID);

        	if (player != NULL)
        		player->decreaseProjectilesCount();

            object->dead = true;
            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, object->ix + (object->iw >> 1) - 16, object->iy + (object->ih >> 1) - 16, 3, 4));
        }

        if (playsound)
            ifSoundOnPlay(rm->sfx_hit);
    }
}

IO_MovingObject * createpowerup(short iType, short ix, short iy, bool side, bool spawn)
{
    MO_Powerup * powerup = NULL;
    CO_Shell * shell = NULL;
    PU_FeatherPowerup * feather = NULL;
    MO_Coin * coin = NULL;

    short iSpawnX = ix + 1;
    short iSpawnY = iy - 1;

    switch (iType) {
    case HEALTH_POWERUP: {
        powerup = new PU_ExtraHeartPowerup(&rm->spr_extraheartpowerup, iSpawnX, iSpawnY);
        break;
    }
    case TIME_POWERUP: {
        powerup = new PU_ExtraTimePowerup(&rm->spr_extratimepowerup, iSpawnX, iSpawnY);
        break;
    }
    case JAIL_KEY_POWERUP: {
        powerup = new PU_JailKeyPowerup(&rm->spr_jailkeypowerup, iSpawnX, iSpawnY);
        break;
    }
    case COIN_POWERUP: {
        short iRandCoin = RANDOM_INT(9);
        short iCoin = 2;

        if (iRandCoin == 8)
            iCoin = 3;
        else if (iRandCoin >= 6)
            iCoin = 1;
        else if (iRandCoin >= 3)
            iCoin = 0;

        static short iCoinValue[4] = {3, 5, 2, 10};
        static short iGreedValue[4] = {10, 15, 5, 20};

        powerup = new PU_CoinPowerup(&rm->spr_coin, iSpawnX, iSpawnY, iCoin, game_values.gamemode->gamemode == game_mode_greed ? iGreedValue[iCoin] : iCoinValue[iCoin]);
        break;
    }
    case MINIGAME_COIN: {
        coin = new MO_Coin(&rm->spr_coin, 0.0f, -VELJUMP / 2.0, iSpawnX, iSpawnY, 2, -1, 2, 0, false);
        break;
    }
    case SECRET1_POWERUP: {
        powerup = new PU_SecretPowerup(&rm->spr_secret1, iSpawnX, iSpawnY, 0);
        break;
    }
    case SECRET2_POWERUP: {
        powerup = new PU_SecretPowerup(&rm->spr_secret2, iSpawnX, iSpawnY, 1);
        break;
    }
    case SECRET3_POWERUP: {
        powerup = new PU_SecretPowerup(&rm->spr_secret3, iSpawnX, iSpawnY, 2);
        break;
    }
    case SECRET4_POWERUP: {
        powerup = new PU_SecretPowerup(&rm->spr_secret4, iSpawnX, iSpawnY, 3);
        break;
    }

    #pragma warning ("Please use enums here")

    case 0: {
        powerup = new PU_PoisonPowerup(&rm->spr_poisonpowerup, iSpawnX, iSpawnY, 1, side, 0, 30, 30, 1, 1);
        break;
    }
    case 1: {
        powerup = new PU_ExtraGuyPowerup(&rm->spr_1uppowerup, iSpawnX, iSpawnY, 1, side, 0, 30, 30, 1, 1, 1);
        break;
    }
    case 2: {
        powerup = new PU_ExtraGuyPowerup(&rm->spr_2uppowerup, iSpawnX, iSpawnY, 1, side, 0, 30, 30, 1, 1, 2);
        break;
    }
    case 3: {
        powerup = new PU_ExtraGuyPowerup(&rm->spr_3uppowerup, iSpawnX, iSpawnY, 1, side, 0, 30, 30, 1, 1, 3);
        break;
    }
    case 4: {
        powerup = new PU_ExtraGuyPowerup(&rm->spr_5uppowerup, iSpawnX, iSpawnY, 1, side, 0, 30, 30, 1, 1, 5);
        break;
    }
    case 5: {
        powerup = new PU_FirePowerup(&rm->spr_firepowerup, iSpawnX, iSpawnY, 1, side, 0, 30, 30, 1, 1);
        break;
    }
    case 6: {
        powerup = new PU_StarPowerup(&rm->spr_starpowerup, iSpawnX, iSpawnY, 4, side, 2, 30, 30, 1, 1);
        break;
    }
    case 7: {
        powerup = new PU_ClockPowerup(&rm->spr_clockpowerup, iSpawnX, iSpawnY, 1, side, 0, 30, 30, 1, 1);
        break;
    }
    case 8: {
        powerup = new PU_BobombPowerup(&rm->spr_bobombpowerup, iSpawnX, iSpawnY, 1, side, 0, 30, 30, 1, 1);
        break;
    }
    case 9: {
        powerup = new PU_PowPowerup(&rm->spr_powpowerup, iSpawnX, iSpawnY, 8, side, 8, 30, 30, 1, 1);
        break;
    }
    case 10: {
        powerup = new PU_BulletBillPowerup(&rm->spr_bulletbillpowerup, iSpawnX, iSpawnY, 1, side, 0, 30, 30, 1, 1);
        break;
    }
    case 11: {
        powerup = new PU_HammerPowerup(&rm->spr_hammerpowerup, iSpawnX, iSpawnY, 1, side, 0, 30, 30, 1, 1);
        break;
    }
    case 12: {
        shell = new CO_Shell(0, iSpawnX, iSpawnY, true, true, true, false);
        break;
    }
    case 13: {
        shell = new CO_Shell(1, iSpawnX, iSpawnY, false, true, true, false);
        break;
    }
    case 14: {
        shell = new CO_Shell(2, iSpawnX, iSpawnY, false, false, true, true);
        break;
    }
    case 15: {
        shell = new CO_Shell(3, iSpawnX, iSpawnY, false, true, false, false);
        break;
    }
    case 16: {
        powerup = new PU_ModPowerup(&rm->spr_modpowerup, iSpawnX, iSpawnY, 8, side, 8, 30, 30, 1, 1);
        break;
    }
    case 17: {
        feather = new PU_FeatherPowerup(&rm->spr_featherpowerup, iSpawnX, iSpawnY, 1, 0, 30, 30, 1, 1);
        break;
    }
    case 18: {
        powerup = new PU_MysteryMushroomPowerup(&rm->spr_mysterymushroompowerup, iSpawnX, iSpawnY, 1, side, 0, 30, 30, 1, 1);
        break;
    }
    case 19: {
        powerup = new PU_BoomerangPowerup(&rm->spr_boomerangpowerup, iSpawnX, iSpawnY, 1, side, 0, 30, 26, 1, 5);
        break;
    }
    case 20: {
        powerup = new PU_Tanooki(iSpawnX, iSpawnY);
        break;
    }
    case 21: {
        powerup = new PU_IceWandPowerup(&rm->spr_icewandpowerup, iSpawnX, iSpawnY, 1, 0, 30, 30, 1, 1);
        break;
    }
    case 22: {
        powerup = new PU_PodoboPowerup(&rm->spr_podobopowerup, iSpawnX, iSpawnY, 1, 0, 30, 30, 1, 1);
        break;
    }
    case 23: {
        powerup = new PU_BombPowerup(&rm->spr_bombpowerup, iSpawnX, iSpawnY, 1, 0, 30, 30, 1, 1);
        break;
    }
    case 24: {
        feather = new PU_LeafPowerup(&rm->spr_leafpowerup, iSpawnX, iSpawnY, 1, 0, 30, 30, 1, 1);
        break;
    }
    case 25: {
        powerup = new PU_PWingsPowerup(&rm->spr_pwingspowerup, iSpawnX, iSpawnY);
        break;
    }
    }

    if (coin) {
        ifSoundOnPlay(rm->sfx_coin);
        if (objectcontainer[1].add(coin))
            return coin;
    } else if (powerup) {
        if (objectcontainer[0].add(powerup)) {
            if (!spawn) {
                powerup->nospawn(iy);
                powerup->collision_detection_checksides();
            }

            return powerup;
        }
    } else if (shell) {
        if (objectcontainer[1].add(shell)) {
            if (!spawn) {
                shell->nospawn(iy, true);
                shell->collision_detection_checksides();
            }

            return shell;
        }
    } else if (feather) {
        if (objectcontainer[0].add(feather)) {
            if (!spawn)
                feather->nospawn(iy);

            return feather;
        }
    } else { //If no powerups were selected for this block, then fire out a podobo
        IO_MovingObject * podobo = new MO_Podobo(&rm->spr_podobo, ix + 2, iy, -(float(RANDOM_INT(5)) / 2.0f) - 6.0f, -1, -1, -1, true);
        objectcontainer[2].add(podobo);
        return podobo;
    }

    return NULL;
}

void CheckSecret(short id)
{
    if (id == 0 && !game_values.unlocksecretunlocked[0]) {
        short iCountTeams = 0;
        for (short iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
            if (game_values.unlocksecret1part1[iPlayer])
                iCountTeams++;
        }

        if (iCountTeams >= 2 && game_values.unlocksecret1part2 >= 8) {
            game_values.unlocksecretunlocked[0] = true;
            ifSoundOnPlay(rm->sfx_transform);

            IO_MovingObject * object = createpowerup(SECRET1_POWERUP, RANDOM_INT(smw->ScreenWidth), RANDOM_INT(smw->ScreenHeight), true, false);

            if (object)
                eyecandy[2].add(new EC_SingleAnimation(&rm->spr_poof, object->ix - 8, object->iy - 8, 4, 5));
        }
    } else if (id == 1 && !game_values.unlocksecretunlocked[1]) {
        if (game_values.unlocksecret2part1 && game_values.unlocksecret2part2 >= 3) {
            game_values.unlocksecretunlocked[1] = true;
            ifSoundOnPlay(rm->sfx_transform);

            IO_MovingObject * object = createpowerup(SECRET2_POWERUP, RANDOM_INT(smw->ScreenWidth), RANDOM_INT(smw->ScreenHeight), true, false);

            if (object)
                eyecandy[2].add(new EC_SingleAnimation(&rm->spr_poof, object->ix - 8, object->iy - 8, 4, 5));
        }
    } else if (id == 2 && !game_values.unlocksecretunlocked[2]) {
        for (short iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
            //number of songs on thriller + number of released albums (figure it out :))
            if (game_values.unlocksecret3part1[iPlayer] >= 9 && game_values.unlocksecret3part2[iPlayer] >= 13) {
                game_values.unlocksecretunlocked[2] = true;
                ifSoundOnPlay(rm->sfx_transform);

                IO_MovingObject * object = createpowerup(SECRET3_POWERUP, RANDOM_INT(smw->ScreenWidth), RANDOM_INT(smw->ScreenHeight), true, false);

                if (object)
                    eyecandy[2].add(new EC_SingleAnimation(&rm->spr_poof, object->ix - 8, object->iy - 8, 4, 5));
            }
        }
    } else if (id == 3 && !game_values.unlocksecretunlocked[3]) {
        game_values.unlocksecretunlocked[3] = true;
        ifSoundOnPlay(rm->sfx_transform);

        IO_MovingObject * object = createpowerup(SECRET4_POWERUP, RANDOM_INT(smw->ScreenWidth), RANDOM_INT(smw->ScreenHeight), true, false);

        if (object)
            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_poof, object->ix - 8, object->iy - 8, 4, 5));
    }
}


//------------------------------------------------------------------------------
// class powerup
//------------------------------------------------------------------------------
MO_Powerup::MO_Powerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
    IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    desty = fy - collisionHeight;
    movingObjectType = movingobject_powerup;
}

void MO_Powerup::draw()
{
    if (state == 0)
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, 0, iw, (short)(ih - fy + desty));
    else
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, 0, iw, ih);
}


void MO_Powerup::update()
{
    //Have the powerup grow out of the powerup block
    if (state == 0) {
        setYf(fy - 2.0f);

        if (fy <= desty) {
            fy = desty;
            state = 1;
            vely = 1.0f;
        }
    } else { //Then have it obey the physics
        fOldX = fx;
        fOldY = fy;

        collision_detection_map();
    }

    animate();
}

bool MO_Powerup::collide(CPlayer *)
{
    if (state > 0)
        dead = true;

    return false;
}

void MO_Powerup::nospawn(short y)
{
    state = 1;
    setYi(y);
    vely = -VELJUMP / 2.0;
}

//------------------------------------------------------------------------------
// tanooki suit
//------------------------------------------------------------------------------
PU_Tanooki::PU_Tanooki(short x, short y)
    : MO_Powerup(&rm->spr_tanooki, x, y, 1, 0, 30, 30, 1, 1)
{
}

bool PU_Tanooki :: collide (CPlayer * player)
{
    if (state > 0) {
        player->SetPowerup(9);
        dead = true;
    }

    return false;
}

//------------------------------------------------------------------------------
// pwings
//------------------------------------------------------------------------------
PU_PWingsPowerup::PU_PWingsPowerup(gfxSprite * nspr, short x, short y)
    : MO_Powerup(nspr, x, y, 1, 0, 30, 30, 1, 1)
{}

bool PU_PWingsPowerup::collide (CPlayer *player)
{
    player->SetPowerup(8);
    dead = true;
    return false;
}


//------------------------------------------------------------------------------
// class star powerup
//------------------------------------------------------------------------------
PU_StarPowerup::PU_StarPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
    MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    if (moveToRight)
        velx = 2.0f;
    else
        velx = -2.0f;

    bounce = -VELPOWERUPBOUNCE;
}

bool PU_StarPowerup::collide(CPlayer * player)
{
    if (state > 0) {
        dead = true;

        if (!player->isInvincible() && !player->shyguy) {
            player->makeinvincible();
        } else {
            player->SetStoredPowerup(6);
        }
    }

    return false;
}

//------------------------------------------------------------------------------
// class 1up powerup
//------------------------------------------------------------------------------
PU_ExtraGuyPowerup::PU_ExtraGuyPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short type) :
    MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    if (moveToRight)
        velx = 1.0f + (float)type;
    else
        velx = -1.0f - (float)type;

    iType = type;
}

bool PU_ExtraGuyPowerup::collide(CPlayer * player)
{
    if (state > 0) {
        game_values.gamemode->playerextraguy(*player, iType);
        ifSoundOnPlay(rm->sfx_extraguysound);

        eyecandy[2].add(new EC_FloatingObject(&rm->spr_extralife, player->ix + HALFPW - 19, player->iy - 16, 0.0f, -1.5f, 62, player->colorID * 38, (iType == 5 ? 3 : iType - 1) * 16, 38, 16));

        dead = true;
    }

    return false;
}

//------------------------------------------------------------------------------
// class poison powerup
//------------------------------------------------------------------------------
PU_PoisonPowerup::PU_PoisonPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
    MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    if (moveToRight)
        velx = 2.0f;
    else
        velx = -2.0f;

    movingObjectType = movingobject_poisonpowerup;
}

bool PU_PoisonPowerup::collide(CPlayer * player)
{
    if (state > 0) {
        if (player->isInvincible() || player->shyguy) {
            dead = true;
            return false;
        }

        if (player->isShielded())
            return false;

        dead = true;

        return player->KillPlayerMapHazard(false, kill_style_poisonmushroom, false) != player_kill_nonkill;
    }

    return false;
}

//------------------------------------------------------------------------------
// class mystery mushroom powerup
//------------------------------------------------------------------------------
PU_MysteryMushroomPowerup::PU_MysteryMushroomPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
    MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    if (moveToRight)
        velx = 2.0f;
    else
        velx = -2.0f;
}

bool PU_MysteryMushroomPowerup::collide(CPlayer * player)
{
    if (state > 0) {
        dead = true;

        if (!SwapPlayers(player->localID)) {
            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, player->ix + (HALFPW) - 16, player->iy + (HALFPH) - 16, 3, 8));
            ifSoundOnPlay(rm->sfx_spit);
        }
    }

    return false;
}

//------------------------------------------------------------------------------
// class fire powerup
//------------------------------------------------------------------------------
PU_FirePowerup::PU_FirePowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
    MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    velx = 0.0f;
}

bool PU_FirePowerup::collide(CPlayer * player)
{
    if (state > 0) {
        player->SetPowerup(1);
        dead = true;
    }

    return false;
}

//------------------------------------------------------------------------------
// class hammer powerup
//------------------------------------------------------------------------------
PU_HammerPowerup::PU_HammerPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
    MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    velx = 0.0f;
}

bool PU_HammerPowerup::collide(CPlayer * player)
{
    if (state > 0) {
        player->SetPowerup(2);
        dead = true;
    }

    return false;
}

//------------------------------------------------------------------------------
// class ice wand powerup
//------------------------------------------------------------------------------
PU_IceWandPowerup::PU_IceWandPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
    MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    velx = 0.0f;

    sparkleanimationtimer = 0;
    sparkledrawframe = 0;
}

void PU_IceWandPowerup::update()
{
    MO_Powerup::update();

    if (++sparkleanimationtimer >= 4) {
        sparkleanimationtimer = 0;
        sparkledrawframe += 32;
        if (sparkledrawframe >= smw->ScreenHeight)
            sparkledrawframe = 0;
    }
}

void PU_IceWandPowerup::draw()
{
    MO_Powerup::draw();

    //Draw sparkles
    if (state == 1)
        rm->spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, 32);
}

bool PU_IceWandPowerup::collide(CPlayer * player)
{
    if (state > 0) {
        player->SetPowerup(5);
        dead = true;
    }

    return false;
}

//------------------------------------------------------------------------------
// class podobo powerup
//------------------------------------------------------------------------------
PU_PodoboPowerup::PU_PodoboPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
    MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    velx = 0.0f;
}

bool PU_PodoboPowerup::collide(CPlayer * player)
{
    if (state > 0) {
        dead = true;
        player->SetStoredPowerup(22);
    }

    return false;
}

//------------------------------------------------------------------------------
// class bomb powerup
//------------------------------------------------------------------------------
PU_BombPowerup::PU_BombPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
    MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    velx = 0.0f;
}

bool PU_BombPowerup::collide(CPlayer * player)
{
    if (state > 0) {
        player->SetPowerup(6);
        dead = true;
    }

    return false;
}

//------------------------------------------------------------------------------
// secret powerup
//------------------------------------------------------------------------------
PU_SecretPowerup::PU_SecretPowerup(gfxSprite * nspr, short x, short y, short type)
    : MO_Powerup(nspr, x, y, 4, 8, 30, 30, 1, 1)
{
    itemtype = type;
    place();
}

void PU_SecretPowerup::update()
{
    MO_Powerup::update();

    if (++sparkleanimationtimer >= 4) {
        sparkleanimationtimer = 0;
        sparkledrawframe += 32;
        if (sparkledrawframe >= smw->ScreenHeight)
            sparkledrawframe = 0;
    }
}

void PU_SecretPowerup::draw()
{
    MO_Powerup::draw();

    //Draw sparkles
    rm->spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, 32);
}

bool PU_SecretPowerup::collide (CPlayer *player)
{
    if (itemtype == 0) {
        game_values.windaffectsplayers = true;

        for (short i = 0; i < 15; i++)
            eyecandy[2].add(new EC_Snow(&rm->spr_snow, (float)(RANDOM_INT(smw->ScreenWidth)), (float)RANDOM_INT(smw->ScreenHeight), RANDOM_INT(4) + 1));
    } else if (itemtype == 1) {
        game_values.spinscreen = true;
    } else if (itemtype == 2) {
        game_values.reversewalk = true;
    } else if (itemtype == 3) {
        game_values.spotlights = true;
    }

    ifSoundOnPlay(rm->sfx_pickup);

    dead = true;
    return false;
}

void PU_SecretPowerup::place()
{
    short iAttempts = 10;
    while (!g_map->findspawnpoint(5, &ix, &iy, collisionWidth, collisionHeight, false) && iAttempts-- > 0);

    fx = (float)ix;
    fy = (float)iy;
}


//------------------------------------------------------------------------------
// class treasure chest powerup
//------------------------------------------------------------------------------
PU_TreasureChestBonus::PU_TreasureChestBonus(gfxSprite *nspr, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iBonusItem) :
    MO_Powerup(nspr, 0, 0, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    velx = 0.0f;

    sparkleanimationtimer = 0;
    sparkledrawframe = 0;
    bounce = -VELPOWERUPBOUNCE * 2;
    numbounces = 5;
    state = 2;
    bonusitem = iBonusItem;

    short iAttempts = 10;
    while (!g_map->findspawnpoint(5, &ix, &iy, collisionWidth, collisionHeight, false) && iAttempts-- > 0);
    fx = (float)ix;
    fy = (float)iy;

    drawbonusitemx = 0;
    drawbonusitemy = 0;
    drawbonusitemtimer = 0;

    fObjectDiesOnSuperDeathTiles = false;
}

void PU_TreasureChestBonus::update()
{
    MO_Powerup::update();

    if (++sparkleanimationtimer >= 4) {
        sparkleanimationtimer = 0;
        sparkledrawframe += 32;
        if (sparkledrawframe >= smw->ScreenHeight)
            sparkledrawframe = 0;
    }

    //Draw rising powerup from chest
    if (state == 3) {
        drawbonusitemy -= 2;

        if (--drawbonusitemtimer <= 0)
            state = 4;
    } else if (state == 4) {
        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix, drawbonusitemy, 3, 8));
        dead = true;
    }
}

void PU_TreasureChestBonus::draw()
{
    if (state < 3) {
        MO_Powerup::draw();

        //Draw sparkles
        rm->spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, 32);
    } else {
        if (bonusitem >= NUM_POWERUPS)
            rm->spr_worlditems.draw(drawbonusitemx, drawbonusitemy, (bonusitem - NUM_POWERUPS) << 5, 0, 32, 32);
        else
            rm->spr_storedpoweruplarge.draw(drawbonusitemx, drawbonusitemy, bonusitem << 5, 0, 32, 32);
    }
}

bool PU_TreasureChestBonus::collide(CPlayer * player)
{
    if (state == 1) {
        ifSoundOnPlay(rm->sfx_treasurechest);
        //if (game_values.worldpowerupcount[player->teamID] < 32)
        //    game_values.worldpowerups[player->teamID][game_values.worldpowerupcount[player->teamID]++] = bonusitem;
        //else
        //	game_values.worldpowerups[player->teamID][31] = bonusitem;

        state = 3;

        drawbonusitemx = ix;
        drawbonusitemy = iy;
        drawbonusitemtimer = 60;

        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix, iy, 3, 8));

        game_values.noexit = false;
    }

    return false;
}

float PU_TreasureChestBonus::BottomBounce()
{
    if (state == 2) {
        if (--numbounces <= 0) {
            numbounces = 0;
            state = 1;
            bounce = GRAVITATION;
        } else {
            if (vely > 0.0f)
                bounce = -vely / 2.0f;
            else
                bounce /= 2.0f;
        }
    }

    return bounce;
}


//------------------------------------------------------------------------------
// class treasure chest powerup
//------------------------------------------------------------------------------
MO_BonusHouseChest::MO_BonusHouseChest(gfxSprite *nspr, short iX, short iY, short iBonusItem) :
    IO_MovingObject(nspr, iX, iY, 1, 0, 64, 64, 0, 0)
{
    iw = 64;
    ih = 64;

    state = 1;
    bonusitem = iBonusItem;

    drawbonusitemy = 0;
    drawbonusitemtimer = 0;

    movingObjectType = movingobject_treasurechest;

    fObjectDiesOnSuperDeathTiles = false;
    fObjectCollidesWithMap = false;
}

void MO_BonusHouseChest::update()
{
    //Draw rising powerup from chest
    if (state == 2) {
        drawbonusitemy -= 2;

        if (--drawbonusitemtimer <= 0) {
            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix + 16, drawbonusitemy, 3, 8));
            state = 3;
        }
    }
}

void MO_BonusHouseChest::draw()
{
    if (state < 2)
        spr->draw(ix, iy, 0, 0, iw, ih);

    if (state >= 2)
        spr->draw(ix, iy, 128, 0, iw, ih);

    if (state == 2) {
        if (bonusitem >= NUM_POWERUPS + NUM_WORLD_POWERUPS) { //Score bonuses
            short iBonus = bonusitem - NUM_POWERUPS - NUM_WORLD_POWERUPS;
            short iBonusX = (iBonus % 10) << 5;
            short iBonusY = ((iBonus / 10) << 5) + 32;
            rm->spr_worlditems.draw(ix + 16, drawbonusitemy, iBonusX, iBonusY, 32, 32);
        } else if (bonusitem >= NUM_POWERUPS) //World Item
            rm->spr_worlditems.draw(ix + 16, drawbonusitemy, (bonusitem - NUM_POWERUPS) << 5, 0, 32, 32);
        else //Normal Powerup
            rm->spr_storedpoweruplarge.draw(ix + 16, drawbonusitemy, bonusitem << 5, 0, 32, 32);
    }

    if (state >= 2)
        spr->draw(ix, iy, 64, 0, iw, ih);
}

bool MO_BonusHouseChest::collide(CPlayer * player)
{
    if (state == 1 && !game_values.gamemode->gameover && player->playerKeys->game_turbo.fPressed) {
        if (bonusitem < NUM_POWERUPS + NUM_WORLD_POWERUPS) {
            if (game_values.worldpowerupcount[player->teamID] < 32)
                game_values.worldpowerups[player->teamID][game_values.worldpowerupcount[player->teamID]++] = bonusitem;
            else
                game_values.worldpowerups[player->teamID][31] = bonusitem;
        } else {
            short iBonus = bonusitem - NUM_POWERUPS - NUM_WORLD_POWERUPS;
            if (iBonus < 10)
                iBonus = iBonus + 1;
            else
                iBonus = 9 - iBonus;

            game_values.tournament_scores[player->teamID].total += iBonus;

            if (game_values.tournament_scores[player->teamID].total < 0)
                game_values.tournament_scores[player->teamID].total = 0;
        }

        ifSoundOnPlay(rm->sfx_treasurechest);
        state = 2;

        drawbonusitemy = iy + 32;
        drawbonusitemtimer = 75;

        game_values.forceexittimer = 180;
        game_values.gamemode->gameover = true;
        game_values.gamemode->winningteam = player->teamID;
    }

    return false;
}


//------------------------------------------------------------------------------
// class clock powerup
//------------------------------------------------------------------------------
PU_ClockPowerup::PU_ClockPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
    MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    velx = 0.0f;
}

bool PU_ClockPowerup::collide(CPlayer * player)
{
    if (state > 0) {
        player->SetStoredPowerup(7);
        dead = true;
    }

    return false;
}

//------------------------------------------------------------------------------
// class bobomb powerup
//------------------------------------------------------------------------------
PU_BobombPowerup::PU_BobombPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
    MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    velx = 0.0f;
}

bool PU_BobombPowerup::collide(CPlayer * player)
{
    if (state > 0) {
        player->SetPowerup(0);
        dead = true;
    }

    return false;
}

//------------------------------------------------------------------------------
// class POW powerup
//------------------------------------------------------------------------------
PU_PowPowerup::PU_PowPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
    MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    velx = 0.0f;
}

bool PU_PowPowerup::collide(CPlayer * player)
{
    if (state > 0) {
        dead = true;
        player->SetStoredPowerup(9);
    }

    return false;
}

//------------------------------------------------------------------------------
// class MOd powerup
//------------------------------------------------------------------------------
PU_ModPowerup::PU_ModPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
    MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    velx = 0.0f;
}

bool PU_ModPowerup::collide(CPlayer * player)
{
    if (state > 0) {
        dead = true;
        player->SetStoredPowerup(16);
    }

    return false;
}

//------------------------------------------------------------------------------
// class BulletBill powerup
//------------------------------------------------------------------------------
PU_BulletBillPowerup::PU_BulletBillPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
    MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    velx = 0.0f;
}

bool PU_BulletBillPowerup::collide(CPlayer * player)
{
    if (state > 0) {
        dead = true;
        player->SetStoredPowerup(10);
    }

    return false;
}

//------------------------------------------------------------------------------
// class feather powerup
//------------------------------------------------------------------------------
PU_FeatherPowerup::PU_FeatherPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
    IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    desty = fy - collisionHeight;
    movingObjectType = movingobject_powerup;

    iw = (short)nspr->getWidth() >> 1;

    velx = 0.0f;
    fFloatDirectionRight = true;
    dFloatAngle = HALF_PI;

    fObjectCollidesWithMap = false;
}

void PU_FeatherPowerup::draw()
{
    if (state == 0)
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, 0, iw, (short)(ih - fy + desty));
    else if (state == 1)
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, 0, iw, ih);
    else
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, (fFloatDirectionRight ? 0 : 32), 0, iw, ih);
}


void PU_FeatherPowerup::update()
{
    //Have the powerup grow out of the powerup block
    if (state == 0) {
        setYf(fy - 4.0f);

        if (fy <= desty) {
            state = 1;
        }
    } else if (state == 1) {
        fOldX = fx;
        fOldY = fy;

        setYf(fy - 4.0f);

        if (fy <= desty - 128.0f) {
            state = 2;
            dFloatCenterY = fy - 64.0f;
            dFloatCenterX = fx;
        }
    } else {
        if (!fFloatDirectionRight) {
            dFloatAngle += 0.035f;

            if (dFloatAngle >= THREE_QUARTER_PI) {
                dFloatAngle = THREE_QUARTER_PI;
                fFloatDirectionRight = true;
            }
        } else {
            dFloatAngle -= 0.035f;

            if (dFloatAngle <= QUARTER_PI) {
                dFloatAngle = QUARTER_PI;
                fFloatDirectionRight = false;
            }
        }

        dFloatCenterY += 1.0f;

        setXf(64.0f * cos(dFloatAngle) + dFloatCenterX);
        setYf(64.0f * sin(dFloatAngle) + dFloatCenterY);

        if (fy >= smw->ScreenHeight)
            dead = true;
    }
}

bool PU_FeatherPowerup::collide(CPlayer * player)
{
    if (state > 0) {
        player->SetPowerup(3);
        dead = true;
    }

    return false;
}

void PU_FeatherPowerup::nospawn(short y)
{
    state = 1;
    desty = y;
    setYi(y + TILESIZE - collisionHeight);
}

//------------------------------------------------------------------------------
// class leaf powerup
//------------------------------------------------------------------------------
PU_LeafPowerup::PU_LeafPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
    PU_FeatherPowerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{}

bool PU_LeafPowerup::collide(CPlayer * player)
{
    if (state > 0) {
        player->SetPowerup(7);
        dead = true;
    }

    return false;
}

//------------------------------------------------------------------------------
// class boomerang powerup
//------------------------------------------------------------------------------
PU_BoomerangPowerup::PU_BoomerangPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
    MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    velx = 0.0f;
}

bool PU_BoomerangPowerup::collide(CPlayer * player)
{
    if (state > 0) {
        player->SetPowerup(4);
        dead = true;
    }

    return false;
}


//------------------------------------------------------------------------------
// class special heart powerup for health mode
//------------------------------------------------------------------------------
PU_ExtraHeartPowerup::PU_ExtraHeartPowerup(gfxSprite *nspr, short x, short y) :
    MO_Powerup(nspr, x, y, 1, 0, 30, 30, 1, 1)
{
    velx = 0.0f;
}

bool PU_ExtraHeartPowerup::collide(CPlayer * player)
{
    if (state > 0) {
        if (game_values.gamemode->gamemode == game_mode_health) {
            if (player->Score().subscore[1] < game_values.gamemodesettings.health.maxlife)
                player->Score().subscore[1]++;

            if (player->Score().subscore[0] < game_values.gamemodesettings.health.maxlife)
                player->Score().subscore[0]++;
        }

        ifSoundOnPlay(rm->sfx_collectpowerup);
        dead = true;
    }

    return false;
}

//------------------------------------------------------------------------------
// class special extra time powerup for timed or star mode
//------------------------------------------------------------------------------
PU_ExtraTimePowerup::PU_ExtraTimePowerup(gfxSprite *nspr, short x, short y) :
    MO_Powerup(nspr, x, y, 1, 0, 30, 30, 1, 1)
{
    velx = 0.0f;
}

bool PU_ExtraTimePowerup::collide(CPlayer * player)
{
    if (state > 0) {
        if (game_values.gamemode->gamemode == game_mode_timelimit || game_values.gamemode->gamemode == game_mode_star) {
            CGM_TimeLimit * timelimitmode = (CGM_TimeLimit*)game_values.gamemode;
            timelimitmode->addtime(timelimitmode->goal / 5);
        }

        ifSoundOnPlay(rm->sfx_collectpowerup);
        dead = true;
    }

    return false;
}


//------------------------------------------------------------------------------
// class special extra coin powerup for coin or greed mode
//------------------------------------------------------------------------------
PU_CoinPowerup::PU_CoinPowerup(gfxSprite *nspr, short x, short y, short color, short value) :
    MO_Powerup(nspr, x, y, 4, 8, 30, 30, 1, 1)
{
    velx = 0.0f;
    iColorOffsetY = color << 5;
    iValue = value;

    sparkleanimationtimer = 0;
    sparkledrawframe = 0;
}

void PU_CoinPowerup::update()
{
    MO_Powerup::update();

    if (++sparkleanimationtimer >= 4) {
        sparkleanimationtimer = 0;
        sparkledrawframe += 32;
        if (sparkledrawframe >= smw->ScreenHeight)
            sparkledrawframe = 0;
    }
}

void PU_CoinPowerup::draw()
{
    if (state == 0) {
        short iHeight = (short)(32 - fy + desty);
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iColorOffsetY, 32, iHeight);
        rm->spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, iHeight);
    } else {
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iColorOffsetY, 32, 32);
        rm->spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, 32);
    }
}

bool PU_CoinPowerup::collide(CPlayer * player)
{
    if (state > 0) {
        if (game_values.gamemode->gamemode == game_mode_coins || game_values.gamemode->gamemode == game_mode_greed) {
            if (!game_values.gamemode->gameover) {
                player->Score().AdjustScore(iValue);
                game_values.gamemode->CheckWinner(player);
            }
        }

        ifSoundOnPlay(rm->sfx_coin);
        dead = true;
    }

    return false;
}


//------------------------------------------------------------------------------
// class special jail key powerup for jail mode
//------------------------------------------------------------------------------
PU_JailKeyPowerup::PU_JailKeyPowerup(gfxSprite *nspr, short x, short y) :
    MO_Powerup(nspr, x, y, 1, 0, 30, 30, 1, 1)
{
    velx = 0.0f;
}

bool PU_JailKeyPowerup::collide(CPlayer * player)
{
    if (state > 0) {
        dead = true;
        player->SetStoredPowerup(26);
    }

    return false;
}

//------------------------------------------------------------------------------
// class fireball
//------------------------------------------------------------------------------
MO_Fireball::MO_Fireball(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iGlobalID, short teamID, short iColorID) :
    IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, (short)nspr->getWidth() >> 2, (short)nspr->getHeight() >> 3, 0, 0)
{
    if (moveToRight)
        velx = 5.0f;
    else
        velx = -5.0f;

    //fireball sprites have both right and left sprites in them
    ih = ih >> 3;

    bounce = -FIREBALLBOUNCE;

    iPlayerID = iGlobalID;
    iTeamID = teamID;

    colorOffset = iColorID * 36;
    movingObjectType = movingobject_fireball;

    state = 1;

    ttl = game_values.fireballttl;

    sSpotlight = NULL;
}

void MO_Fireball::update()
{
    IO_MovingObject::update();

    if (--ttl <= 0) {
        removeifprojectile(this, true, true);
    } else if (game_values.spotlights) {
        if (!sSpotlight) {
            sSpotlight = spotlightManager.AddSpotlight(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1), 3);
        }

        if (sSpotlight) {
            sSpotlight->UpdatePosition(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1));
        }
    }
}

bool MO_Fireball::collide(CPlayer * player)
{
    if (iPlayerID != player->getGlobalID() && (game_values.teamcollision == 2|| iTeamID != player->getTeamID())) {
        if (!player->isShielded()) {
            removeifprojectile(this, false, false);

            if (!player->isInvincible() && !player->shyguy) {
                //Find the player that shot this fireball so we can attribute a kill
                PlayerKilledPlayer(iPlayerID, player, death_style_jump, kill_style_fireball, false, false);
                return true;
            }
        }
    }

    return false;
}

void MO_Fireball::draw()
{
    spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, (velx > 0 ? 0 : 18) + colorOffset, iw, ih);
}


//------------------------------------------------------------------------------
// class super fireball
//------------------------------------------------------------------------------
MO_SuperFireball::MO_SuperFireball(gfxSprite *nspr, short x, short y, short iNumSpr, float fVelyX, float fVelyY, short aniSpeed, short iGlobalID, short teamID, short iColorID) :
    IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, nspr->getWidth() / iNumSpr, nspr->getHeight() / 10, 0, 0)
{
    ih /= 10;

    iPlayerID = iGlobalID;
    iTeamID = teamID;
    colorOffset = (iColorID + 1) * 64;
    directionOffset = velx < 0.0f ? 0 : 32;
    movingObjectType = movingobject_superfireball;

    state = 1;

    velx = fVelyX;
    vely = fVelyY;
    ttl = RANDOM_INT(30) + 60;

    drawframe = 0;

    fObjectCollidesWithMap = false;
}

void MO_SuperFireball::update()
{
    animate();

    setXf(fx + velx);
    setYf(fy + vely);

    if (ix < 0)
        setXi(ix + smw->ScreenWidth);
    else if (ix > smw->ScreenWidth - 1)
        setXi(ix - smw->ScreenWidth);

	if (iy > smw->ScreenHeight|| iy < -ih || --ttl <= 0)
        removeifprojectile(this, false, true);
}

bool MO_SuperFireball::collide(CPlayer * player)
{
    if (!player->isShielded()) {
        dead = true;
        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix + (iw >> 1) - 16, iy + (ih >> 1) - 16, 3, 4));
        ifSoundOnPlay(rm->sfx_hit);

        if (!player->isInvincible() && !player->shyguy) {
            return player->KillPlayerMapHazard(false, kill_style_environment, false) != player_kill_nonkill;
        }
    }

    return false;
}

void MO_SuperFireball::draw()
{
    spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, colorOffset + directionOffset, iw, ih);
}

//------------------------------------------------------------------------------
// class hammer
//------------------------------------------------------------------------------
MO_Hammer::MO_Hammer(gfxSprite *nspr, short x, short y, short iNumSpr, float fVelyX, float fVelyY, short aniSpeed, short iGlobalID, short teamID, short iColorID, bool superHammer) :
    IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, (short)nspr->getWidth() / iNumSpr, (short)nspr->getHeight() >> 2, 0, 0)
{
    ih = ih >> 2;

    iPlayerID = iGlobalID;
    iTeamID = teamID;
	//RFC
    colorOffset = iColorID * 28;
    movingObjectType = movingobject_hammer;

    state = 1;

    velx = fVelyX;
    vely = fVelyY;
    ttl = game_values.hammerttl;

    fSuper = superHammer;

    if (velx > 0.0f)
        drawframe = 0;
    else
        drawframe = animationWidth - iw;

    fObjectCollidesWithMap = false;

    sSpotlight = NULL;
}

void MO_Hammer::update()
{
    if (++animationtimer == animationspeed) {
        animationtimer = 0;

        if (velx > 0) {
            drawframe += iw;
            if (drawframe >= animationWidth)
                drawframe = 0;
        } else {
            drawframe -= iw;
            if (drawframe < 0)
                drawframe = animationWidth - iw;
        }
    }

    setXf(fx + velx);
    setYf(fy + vely);

    if (!fSuper)
        vely += GRAVITATION;

    if (ix < 0)
        setXi(ix + smw->ScreenWidth);
    else if (ix > smw->ScreenWidth - 1)
        setXi(ix - smw->ScreenWidth);

    if (iy > smw->ScreenHeight || --ttl <= 0 || (fSuper && iy < -ih)) {
        removeifprojectile(this, false, true);
    } else if (game_values.spotlights) {
        if (!sSpotlight) {
            sSpotlight = spotlightManager.AddSpotlight(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1), 3);
        }

        if (sSpotlight) {
            sSpotlight->UpdatePosition(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1));
        }
    }

    //Detection collision with hammer breakable blocks
    IO_Block * blocks[4];
    GetCollisionBlocks(blocks);
    for (short iBlock = 0; iBlock < 4; iBlock++) {
        if (blocks[iBlock] && blocks[iBlock]->getBlockType() == block_weaponbreakable) {
            B_WeaponBreakableBlock * weaponbreakableblock = (B_WeaponBreakableBlock*)blocks[iBlock];
            if (weaponbreakableblock->iType == 5) {
                weaponbreakableblock->triggerBehavior(iPlayerID, iTeamID);
                removeifprojectile(this, false, false);
                return;
            }
        }
    }
}

bool MO_Hammer::collide(CPlayer * player)
{
    if (iPlayerID != player->globalID && (game_values.teamcollision == 2|| iTeamID != player->teamID)) {
        if (!player->isShielded()) {
            removeifprojectile(this, false, false);

            if (!player->isInvincible() && !player->shyguy) {
                //Find the player that shot this hammer so we can attribute a kill
                PlayerKilledPlayer(iPlayerID, player, death_style_jump, kill_style_hammer, false, false);
                return true;
            }
        }
    }

    return false;
}

void MO_Hammer::draw()
{
    spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, colorOffset, iw, ih);
}


//------------------------------------------------------------------------------
// class sledge hammer
//------------------------------------------------------------------------------
MO_SledgeHammer::MO_SledgeHammer(gfxSprite *nspr, short x, short y, short iNumSpr, float fVelyX, float fVelyY, short aniSpeed, short iGlobalID, short iTeamID, short iColorID, bool superHammer) :
    IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, (short)nspr->getWidth() / iNumSpr, (short)nspr->getHeight() / 5, 0, 0)
{
    ih = collisionHeight;

    playerID = iGlobalID;
    teamID = iTeamID;
    colorOffset = (iColorID + 1) * 32;
    movingObjectType = movingobject_sledgehammer;
    state = 1;
    velx = fVelyX;
    vely = fVelyY;

    fSuper = superHammer;

    if (velx > 0.0f)
        drawframe = 0;
    else
        drawframe = animationWidth - iw;
}

void MO_SledgeHammer::update()
{
    if (++animationtimer == animationspeed) {
        animationtimer = 0;

        if (velx > 0) {
            drawframe += iw;
            if (drawframe >= animationWidth)
                drawframe = 0;
        } else {
            drawframe -= iw;
            if (drawframe < 0)
                drawframe = animationWidth - iw;
        }
    }

    setXf(fx + velx);
    setYf(fy + vely);

    vely += GRAVITATION;

    if (ix < 0)
        setXi(ix + smw->ScreenWidth);
    else if (ix > smw->ScreenWidth - 1)
        setXi(ix - smw->ScreenWidth);

    if (iy >= smw->ScreenHeight)
        dead = true;
}

bool MO_SledgeHammer::collide(CPlayer * player)
{
    if (!player->isShielded()) {
        dead = true;
        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix + (iw >> 1) - 16, iy + (ih >> 1) - 16, 3, 4));
        ifSoundOnPlay(rm->sfx_hit);

        if (!player->isInvincible() && !player->shyguy) {
            return player->KillPlayerMapHazard(false, kill_style_environment, false) != player_kill_nonkill;
        }
    }

    return false;
}

void MO_SledgeHammer::explode()
{
    if (fSuper) {
        objectcontainer[2].add(new MO_Explosion(&rm->spr_explosion, ix + (iw >> 2) - 96, iy + (ih >> 2) - 64, 2, 4, -1, -1, kill_style_hammer));
        ifSoundOnPlay(rm->sfx_bobombsound);
    } else {
        short iCenterX = ix + (iw >> 1) - 14;
        short iCenterY = iy + (ih >> 1) - 14;
        short iColorID = colorOffset / 32 - 1;

        for (short iHammer = 0; iHammer < 3; iHammer++) {
            float dAngle = (float)(RANDOM_INT(628)) / 100.0f;
            float dVel = (float)(RANDOM_INT(5)) / 2.0f + 3.0f;
            float dVelX = dVel * cos(dAngle);
            float dVelY = dVel * sin(dAngle);
            objectcontainer[2].add(new MO_Hammer(&rm->spr_hammer, iCenterX, iCenterY, 6, dVelX, dVelY, 5, playerID, teamID, iColorID, true));
        }

    	CPlayer * player = GetPlayerFromGlobalID(playerID);

   		player->increaseProjectilesCount(3);

        ifSoundOnPlay(rm->sfx_cannon);
    }
}

void MO_SledgeHammer::draw()
{
    spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, colorOffset, iw, ih);
}


//------------------------------------------------------------------------------
// class Ice Blast
//------------------------------------------------------------------------------
MO_IceBlast::MO_IceBlast(gfxSprite *nspr, short x, short y, float fVelyX, short iGlobalID, short teamID, short iColorID) :
    IO_MovingObject(nspr, x, y, 4, 8, 32, 32, 0, 0, 0, (iColorID + 1) << 5, 32, 32)
{
    iPlayerID = iGlobalID;
    iTeamID = teamID;

    movingObjectType = movingobject_iceblast;

    state = 1;

    velx = fVelyX;
    vely = 0.0f;

    if (velx > 0.0f)
        drawframe = 0;
    else
        drawframe = animationWidth - iw;

    ttl = 120;

    fObjectCollidesWithMap = false;

    sSpotlight = NULL;
}

void MO_IceBlast::update()
{
    if (++animationtimer == animationspeed) {
        animationtimer = 0;

        if (velx > 0) {
            drawframe += iw;
            if (drawframe >= animationWidth)
                drawframe = 0;
        } else {
            drawframe -= iw;
            if (drawframe < 0)
                drawframe = animationWidth - iw;
        }
    }

    setXf(fx + velx);

    if (--ttl <= 0) {
        removeifprojectile(this, false, true);
    } else if (game_values.spotlights) {
        if (!sSpotlight) {
            sSpotlight = spotlightManager.AddSpotlight(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1), 3);
        }

        if (sSpotlight) {
            sSpotlight->UpdatePosition(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1));
        }
    }
}

bool MO_IceBlast::collide(CPlayer * player)
{
    if (iPlayerID != player->globalID && (game_values.teamcollision == 2 || iTeamID != player->teamID)) {
        if (!player->isShielded() && !player->isInvincible() && !player->shyguy) {
            player->makefrozen(game_values.wandfreezetime);
            removeifprojectile(this, false, true);
        }
    }

    return false;
}

void MO_IceBlast::draw()
{
    IO_MovingObject::draw();
}

//------------------------------------------------------------------------------
// class boomerang
//------------------------------------------------------------------------------
MO_Boomerang::MO_Boomerang(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iGlobalID, short teamID, short iColorID) :
    IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, (short)nspr->getWidth() / iNumSpr, (short)nspr->getHeight() >> 3, 0, 0)
{
    //boomerangs sprites have both right and left sprites in them
    ih = ih >> 3;

    iPlayerID = iGlobalID;
    iTeamID = teamID;
    colorOffset = iColorID * 64;
    movingObjectType = movingobject_boomerang;

    state = 1;

    fMoveToRight = moveToRight;

    if (moveToRight)
        velx = 5.0f;
    else
        velx = -5.0f;

    vely = 0.0f;

    fFlipped = false;

    //Don't let boomerang start off the screen or it won't rebound correctly
    if (moveToRight && fx + iw >= smw->ScreenWidth)
        setXf(fx - smw->ScreenWidth);
    else if (!moveToRight && fx < 0.0f)
        setXf(fx + smw->ScreenWidth);

    iStateTimer = 0;

    if (game_values.boomerangstyle == 3)
        iStyle = RANDOM_INT(3);
    else
        iStyle = game_values.boomerangstyle;

    fObjectCollidesWithMap = false;

    sSpotlight = NULL;
}

void MO_Boomerang::update()
{
    if (!rm->sfx_boomerang.isPlaying())
        ifSoundOnPlay(rm->sfx_boomerang);

    animate();

    //Detection collision with boomerang breakable blocks
    IO_Block * blocks[4];
    GetCollisionBlocks(blocks);
    for (short iBlock = 0; iBlock < 4; iBlock++) {
        if (blocks[iBlock] && blocks[iBlock]->getBlockType() == block_weaponbreakable) {
            B_WeaponBreakableBlock * weaponbreakableblock = (B_WeaponBreakableBlock*)blocks[iBlock];
            if (weaponbreakableblock->iType == 4) {
                weaponbreakableblock->triggerBehavior(iPlayerID, iTeamID);
                forcedead();
                return;
            }
        }
    }

    if (iStyle == 0) { //Flat style
        fOldX = fx;
        setXf(fx + velx);

        if (fMoveToRight && fx + iw >= smw->ScreenWidth && fOldX + iw < smw->ScreenWidth) {
            if (fFlipped) {
                forcedead();
                return;
            } else {
                setXf(smw->ScreenWidth - iw);
                fFlipped = true;
                fMoveToRight = false;
                velx = -velx;
            }
        } else if (!fMoveToRight && fx < 0.0f && fOldX >= 0.0f) {
            if (fFlipped) {
                forcedead();
                return;
            } else {
                setXf(0.0f);
                fFlipped = true;
                fMoveToRight = true;
                velx = -velx;
            }
        }
    } else if (iStyle == 1) {
        //Attempting to emulate the SMB3 boomerang behavior
        iStateTimer++;

        fOldX = fx;
        setXf(fx + velx);

        if (fx < 0.0f)
            setXf(fx + smw->ScreenWidth);
        else if (fx + iw >= smw->ScreenWidth)
            setXf(fx - smw->ScreenWidth);

        if (state == 1) {
            fOldY = fy;
            setYf(fy - 3.2f);

            if (iStateTimer >= 20) {
                iStateTimer = 0;
                state = 2;
            }
        } else if (state == 2) {
            if (iStateTimer >= 26) {
                iStateTimer = 0;
                state = 3;
            }
        } else if (state == 3) {
            fOldY = fy;
            setYf(fy + 1.0f);

            if (fMoveToRight) {
                //Add amount so that by time fy lowers by two tiles, we turn around the boomerang
                velx -= 0.15625f;

                if (velx <= -5.0f) {
                    velx = -5.0f;
                    state = 4;
                    fFlipped = true;
                }
            } else {
                velx += 0.15625f;

                if (velx >= 5.0f) {
                    velx = 5.0f;
                    state = 4;
                    fFlipped = true;
                }
            }

            iStateTimer = 0;
        } else if (state == 4) {
            if (iStateTimer >= 46) {
                if ((fMoveToRight && fx < 0.0f && fOldX >= 0.0f) ||
                        (!fMoveToRight && fx + iw >= smw->ScreenWidth && fOldX + iw < smw->ScreenWidth)) {
                    forcedead();
                    return;
                }
            }
        }
    } else if (iStyle == 2) { //Zelda style boomerang
        iStateTimer++;

        fOldX = fx;
        setXf(fx + velx);

        if (fx < 0.0f)
            setXf(fx + smw->ScreenWidth);
        else if (fx + iw >= smw->ScreenWidth)
            setXf(fx - smw->ScreenWidth);

        if (iStateTimer > game_values.boomeranglife) {
            forcedead();
            return;
        }

        if (state == 1) {
            if (iStateTimer >= 64) {
                state = 2;
                fFlipped = true;

                /*
                CPlayer * player = GetPlayerFromGlobalID(iPlayerID);

                //No wrap boomerang
                if (player)
                {
                    if ((player->ix < ix && velx > 0) || (player->ix > ix && velx < 0))
                		velx = -velx;
                }
                else
                {
                	velx = -velx;
                }
                */

                velx = -velx;  //Wrap Boomerang
            }
        } else if (state == 2) {
            fOldY = fy;
            setYf(fy + vely);

            //Follow the player zelda style
            CPlayer * player = GetPlayerFromGlobalID(iPlayerID);

            if (player) {
                bool fWrap = false;
                if (abs(player->ix - ix) > 320)
                    fWrap = true;

                if ((player->ix < ix && !fWrap) || (player->ix > ix && fWrap))  //Wrap Boomerang
                    //if (player->ix < ix)  //No Wrap Boomerang
                {
                    velx -= 0.2f;

                    if (velx < -5.0f)
                        velx = -5.0f;
                } else {
                    velx += 0.2f;

                    if (velx > 5.0f)
                        velx = 5.0f;
                }

                if (player->iy < iy) {
                    vely -= 0.2f;

                    if (vely < -3.0f)
                        vely = -3.0f;
                } else {
                    vely += 0.2f;

                    if (vely > 3.0f)
                        vely = 3.0f;
                }
            } else {
                //Remove boomerang if player was removed from game
                forcedead();
                return;

                /*
                //Die at nearest edge if player was removed from game
                if (velx > 0)
                	velx = 5.0f;
                else
                	velx = -5.0f;

                if ((fx < 0.0f && fOldX >= 0.0f) ||
                	(fx + iw >= smw->ScreenWidth && fOldX + iw < smw->ScreenWidth))
                {
                	forcedead();
                	return;
                }
                */
            }
        }
    }

    if (game_values.spotlights) {
        if (!sSpotlight) {
            sSpotlight = spotlightManager.AddSpotlight(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1), 3);
        }

        if (sSpotlight) {
            sSpotlight->UpdatePosition(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1));
        }
    }
}

//Call to kill boomerang when it is not caught by player
void MO_Boomerang::forcedead()
{
    removeifprojectile(this, false, true);
    ifsoundonstop(rm->sfx_boomerang);

    if (game_values.boomeranglimit == 0)
        return;

    //Penalize player if they did not catch it
    CPlayer * player = GetPlayerFromGlobalID(iPlayerID);

    if (player) {
        if (player->projectilelimit > 0)
            player->DecreaseProjectileLimit();
    }
}

bool MO_Boomerang::collide(CPlayer * player)
{
    if (iPlayerID != player->globalID && (game_values.teamcollision == 2|| iTeamID != player->teamID)) {
        if (!player->isShielded()) {
            removeifprojectile(this, false, false);

            if (!player->isInvincible() && !player->shyguy) {
                //Find the player that shot this boomerang so we can attribute a kill
                PlayerKilledPlayer(iPlayerID, player, death_style_jump, kill_style_boomerang, false, false);
                return true;
            }
        }
    } else if (iPlayerID == player->globalID && fFlipped) {
        removeifprojectile(this, false, true);
    }

    return false;
}

void MO_Boomerang::draw()
{
    spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, colorOffset + (fMoveToRight ? 0 : 32), iw, ih);
}

//------------------------------------------------------------------------------
// class bomb
//------------------------------------------------------------------------------
CO_Bomb::CO_Bomb(gfxSprite *nspr, short x, short y, float fVelX, float fVelY, short aniSpeed, short iGlobalID, short teamID, short iColorID, short timetolive) :
    MO_CarriedObject(nspr, x, y, 5, aniSpeed, 24, 24, 4, 13)
{
    iw = 28;
    ih = 38;

    iPlayerID = iGlobalID;
    iTeamID = teamID;
    iColorOffsetY = (iColorID + 1) * 38;

    movingObjectType = movingobject_bomb;
    state = 1;

    ttl = timetolive;

    velx = fVelX;
    vely = fVelY;

    iOwnerRightOffset = 14;
    iOwnerLeftOffset = -16;
    iOwnerUpOffset = 40;

    sSpotlight = NULL;
}

bool CO_Bomb::collide(CPlayer * player)
{
    if (state == 1 && owner == NULL) {
        if (player->AcceptItem(this)) {
            owner = player;

            velx = 0.0f;
            vely = 0.0f;

            if (iPlayerID == -1) {
                iPlayerID = owner->globalID;
                iTeamID = owner->teamID;
                iColorOffsetY = (owner->colorID + 1) * 38;
            }
        }
    }

    return false;
}

void CO_Bomb::update()
{
    if (--ttl <= 0)
        Die();

    if (dead)
        return;

    if (owner) {
        MoveToOwner();
    } else {
        applyfriction();

        //Collision detect map
        fOldX = fx;
        fOldY = fy;

        collision_detection_map();
    }

    animate();

    if (game_values.spotlights) {
        if (!sSpotlight) {
            sSpotlight = spotlightManager.AddSpotlight(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1), 3);
        }

        if (sSpotlight) {
            sSpotlight->UpdatePosition(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1));
        }
    }
}

void CO_Bomb::draw()
{
    if (owner && owner->iswarping())
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iColorOffsetY, iw, ih, owner->GetWarpState(), owner->GetWarpPlane());
    else
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iColorOffsetY, iw, ih);
}

void CO_Bomb::Die()
{
    if (owner) {
        owner->carriedItem = NULL;
        owner = NULL;
    }

	CPlayer * player = GetPlayerFromGlobalID(iPlayerID);

	if (player != NULL)
		player->decreaseProjectilesCount();

    dead = true;
    objectcontainer[2].add(new MO_Explosion(&rm->spr_explosion, ix + (iw >> 2) - 96, iy + (ih >> 2) - 64, 2, 4, iPlayerID, iTeamID, kill_style_bomb));
    ifSoundOnPlay(rm->sfx_bobombsound);
}

//------------------------------------------------------------------------------
// class coin (for coin mode)
//------------------------------------------------------------------------------
MO_Coin::MO_Coin(gfxSprite *nspr, float dvelx, float dvely, short ix, short iy, short color, short team, short type, short uncollectabletime, bool placecoin) :
    IO_MovingObject(nspr, ix, iy, 4, 8, 30, 30, 1, 1, 0, color << 5, 32, 32)
{
    state = 1;
    objectType = object_moving;
    movingObjectType = movingobject_coin;

    sparkleanimationtimer = 0;
    sparkledrawframe = 0;

    iType = type;
    iTeam = team;

    iUncollectableTime = uncollectabletime;
    velx = dvelx;
    vely = dvely;

    timer = 0;
    if (placecoin) {
        placeCoin();
    }

    if (iType == 0) {
        fObjectCollidesWithMap = false;
    } else {
        collision_detection_checksides();
    }
}

bool MO_Coin::collide(CPlayer * player)
{
    if (iUncollectableTime > 0 || (iType == 1 && (!game_values.gamemodesettings.greed.owncoins && iTeam == player->getTeamID())))
        return false;

    if (!game_values.gamemode->gameover) {
        if (iType == 2)
            player->Score().subscore[0]++;
        else
            player->Score().AdjustScore(1);

        game_values.gamemode->CheckWinner(player);
    }

    eyecandy[2].add(new EC_SingleAnimation(&rm->spr_coinsparkle, ix, iy, 7, 4));

    ifSoundOnPlay(rm->sfx_coin);

    if (iType == 0)
        placeCoin();
    else
        dead = true;

    return false;
}

void MO_Coin::update()
{
    if (iType != 1) {
        animate();

        if (++sparkleanimationtimer >= 4) {
            sparkleanimationtimer = 0;
            sparkledrawframe += 32;
            if (sparkledrawframe >= smw->ScreenHeight)
                sparkledrawframe = 0;
        }

        if (++timer > 1000)
            placeCoin();
    }

    if (iType != 0) {
        applyfriction();
        IO_MovingObject::update();

        iUncollectableTime--;

        if (iType == 1 && iUncollectableTime < -game_values.gamemodesettings.greed.coinlife) {
            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix, iy, 3, 8));
            dead = true;
        }
    }
}

void MO_Coin::draw()
{
    IO_MovingObject::draw();

    //Draw sparkles
    if (iType != 1)
        rm->spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, 32);
}

void MO_Coin::placeCoin()
{
    timer = 0;

    short x = 0, y = 0;
    short iAttempts = 32;
    while ((!g_map->findspawnpoint(5, &x, &y, collisionWidth, collisionHeight, false) ||
            objectcontainer[1].getClosestMovingObject(x, y, movingobject_coin) < 150.0f)
            && iAttempts-- > 0);

    setXi(x);
    setYi(y);
}


//------------------------------------------------------------------------------
// class thwomp (for thwomp mode)
//------------------------------------------------------------------------------
OMO_Thwomp::OMO_Thwomp(gfxSprite *nspr, short x, float nspeed) :
    IO_OverMapObject(nspr, x, (short)-nspr->getHeight(), 1, 0)
{
    objectType = object_thwomp;
    vely = nspeed;
}

void OMO_Thwomp::update()
{
    IO_OverMapObject::update();

	if (iy > smw->ScreenHeight - 1)
        dead = true;
}

bool OMO_Thwomp::collide(CPlayer * player)
{
    if (!player->isInvincible() && !player->isShielded() && (player->Score().score > 0 || game_values.gamemode->goal == -1))
        return player->KillPlayerMapHazard(false, kill_style_environment, false) != player_kill_nonkill;

    return false;
}


//------------------------------------------------------------------------------
// class podobo (for survival mode)
//------------------------------------------------------------------------------
MO_Podobo::MO_Podobo(gfxSprite *nspr, short x, short y, float dVelY, short playerid, short teamid, short colorid, bool isSpawned) :
    IO_MovingObject(nspr, x, y, 4, 6)
{
    fIsSpawned = isSpawned;
    iHiddenPlane = y;

    objectType = object_moving;
    movingObjectType = movingobject_podobo;
    vely = dVelY;

    ih = 32;
    collisionHeight = ih;

    iPlayerID = playerid;
    iTeamID = teamid;
    iColorOffsetY = (colorid + 1) * 64;

    fObjectCollidesWithMap = false;
}

void MO_Podobo::update()
{
    //Special slow podobo gravity
    vely += 0.2f;

    setXf(fx + velx);
    setYf(fy + vely);

    animate();

	if (iy > smw->ScreenHeight - 1 && vely > 0.0f)
        dead = true;
}

void MO_Podobo::draw()
{
    if (fIsSpawned && vely < 0.0f)
        spr->draw(ix, iy, drawframe, iColorOffsetY + (vely > 0.0f ? 32 : 0), iw, ih, 2, iHiddenPlane);
    else
        spr->draw(ix, iy, drawframe, iColorOffsetY + (vely > 0.0f ? 32 : 0), iw, ih);
}

bool MO_Podobo::collide(CPlayer * player)
{
    if (player->globalID != iPlayerID && (game_values.teamcollision == 2|| iTeamID != player->teamID) && !player->isInvincible() && !player->isShielded() && !player->shyguy) {
        //Find the player that made this explosion so we can attribute a kill
        PlayerKilledPlayer(iPlayerID, player, death_style_jump, kill_style_podobo, false, false);
        return true;
    }

    return false;
}

void MO_Podobo::collide(IO_MovingObject * object)
{
    if (iPlayerID == -1)
        return;

    MovingObjectType type = object->getMovingObjectType();

    if (type == movingobject_shell || type == movingobject_throwblock || type == movingobject_bulletbill) {
        //Same team bullet bills don't kill each other
        if (type == movingobject_bulletbill && ((MO_BulletBill*) object)->iTeamID == iTeamID)
            return;

        if (type == movingobject_shell)
            ((CO_Shell*)object)->Die();
        else if (type == movingobject_throwblock)
            ((CO_ThrowBlock*) object)->Die();
        else if (type == movingobject_bulletbill)
            ((MO_BulletBill*) object)->Die();

        ifSoundOnPlay(rm->sfx_kicksound);
    }
}

//------------------------------------------------------------------------------
// class Bowser Fire (for survival mode)
//------------------------------------------------------------------------------
OMO_BowserFire::OMO_BowserFire(gfxSprite *nspr, short x, short y, float dVelX, float dVelY, short id, short teamid, short colorid) :
    IO_OverMapObject(nspr, x, y, 3, 6)
{
    objectType = object_bowserfire;
    velx = dVelX;
    vely = dVelY;

    ih = 32;
    collisionHeight = ih;

    iPlayerID = id;
    iTeamID = teamid;
    iColorOffsetY = (colorid + 1) * 64;
}

void OMO_BowserFire::update()
{
    IO_OverMapObject::update();

    if ((velx < 0 && ix < -iw) || (velx > 0 && ix > smw->ScreenWidth)) {
    	CPlayer * player = GetPlayerFromGlobalID(iPlayerID);

    	if (player != NULL)
    		player->decreaseProjectilesCount();

        dead = true;
    }
}

void OMO_BowserFire::draw()
{
    spr->draw(ix, iy, drawframe, (velx > 0.0f ? 32 : 0) + iColorOffsetY, iw, ih);
}

bool OMO_BowserFire::collide(CPlayer * player)
{
    //if the fire is off the screen, don't wrap it to collide
    if ((ix < 0 && velx < 0.0f && player->ix > ix + iw && player->ix + PW < smw->ScreenWidth) ||
            (ix + iw >= smw->ScreenWidth && velx > 0.0f && player->ix + PW < ix && player->ix >= 0)) {
        return false;
    }

    if (player->globalID != iPlayerID && (game_values.teamcollision == 2|| iTeamID != player->teamID) && !player->isInvincible() && !player->isShielded()) {
        //Find the player that made this explosion so we can attribute a kill
        PlayerKilledPlayer(iPlayerID, player, death_style_jump, kill_style_fireball, false, false);
        return true;
    }

    return false;
}


//------------------------------------------------------------------------------
// class CarriedObject - all objects players can carry inheirit from this class
//------------------------------------------------------------------------------

MO_CarriedObject::MO_CarriedObject(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
    IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    init();
}

MO_CarriedObject::MO_CarriedObject(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth) :
    IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY, iAnimationOffsetX, iAnimationOffsetY, iAnimationHeight, iAnimationWidth)
{
    init();
}

MO_CarriedObject::~MO_CarriedObject()
{}

void MO_CarriedObject::init()
{
    owner = NULL;
    fSmoking = false;

    dKickX = 2.0f;
    dKickY = 4.0f;

    iOwnerRightOffset = HALFPW;
    iOwnerLeftOffset = HALFPW - 32;
    iOwnerUpOffset = 32;

    fCarriedByKuriboShoe = false;
}

void MO_CarriedObject::draw()
{
    if (owner && owner->iswarping())
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY, iw, ih, owner->GetWarpState(), owner->GetWarpPlane());
    else
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY, iw, ih);
}

void MO_CarriedObject::MoveToOwner()
{
    if (owner) {
        setXi(owner->ix + (owner->isFacingRight() ? iOwnerRightOffset : iOwnerLeftOffset));
        setYi(owner->iy + PH - iOwnerUpOffset + collisionOffsetY);
    }
}

void MO_CarriedObject::Drop()
{
    if (owner)
        owner->carriedItem = NULL;

    owner = NULL;

    collision_detection_checksides();
}

void MO_CarriedObject::Kick()
{
    if (owner) {
        velx = owner->velx + (owner->isFacingRight() ? dKickX : -dKickX);
        vely = -dKickY;
        ifSoundOnPlay(rm->sfx_kicksound);
    }

    Drop();
}

//------------------------------------------------------------------------------
// class egg (for egg mode)
//------------------------------------------------------------------------------
CO_Egg::CO_Egg(gfxSprite *nspr, short iColor) :
    MO_CarriedObject(nspr, 0, 0, 2, 16, 28, 30, 2, 1, 0, iColor << 5, 32, 32)
{
    state = 1;
    movingObjectType = movingobject_egg;

    owner_throw = NULL;
    owner_throw_timer = 0;

    sparkleanimationtimer = 0;
    sparkledrawframe = 0;

    color = iColor;

    egganimationrates[0] = 2;
    egganimationrates[1] = 4;
    egganimationrates[2] = 6;
    egganimationrates[3] = 8;
    egganimationrates[4] = 12;
    egganimationrates[5] = 16;

    iOwnerRightOffset = HALFPW;
    iOwnerLeftOffset = HALFPW - 28;
    iOwnerUpOffset = 32;

    fCarriedByKuriboShoe = true;

    placeEgg();
}

bool CO_Egg::collide(CPlayer * player)
{
    if (owner == NULL && player->isready()) {
        if (player->AcceptItem(this)) {
            owner = player;
            owner_throw = player;
        }
    }

    return false;
}

void CO_Egg::update()
{
    if (owner) {
        MoveToOwner();
        relocatetimer = 0;
        owner_throw = owner;
    } else if (++relocatetimer > 1500) {
        placeEgg();
        owner_throw = NULL;
    } else {
        if (owner_throw && --owner_throw_timer <= 0) {
            owner_throw_timer = 0;
            owner_throw = NULL;
        }

        applyfriction();

        //Collision detect map
        fOldX = fx;
        fOldY = fy;

        collision_detection_map();
    }

    if (++sparkleanimationtimer >= 4) {
        sparkleanimationtimer = 0;
        sparkledrawframe += 32;
        if (sparkledrawframe >= smw->ScreenHeight)
            sparkledrawframe = 0;
    }

    //Explode
    if (game_values.gamemodesettings.egg.explode > 0) {
        if (--explosiondrawtimer <= 0) {
            explosiondrawtimer = 62;
            if (--explosiondrawframe < 0) {
                objectcontainer[2].add(new MO_Explosion(&rm->spr_explosion, ix + (iw >> 1) - 96, iy + (ih >> 1) - 64, 2, 4, -1, -1, kill_style_bomb));
                placeEgg();

                ifSoundOnPlay(rm->sfx_bobombsound);
            } else {
                if (explosiondrawframe < 5)
                    animationspeed = egganimationrates[explosiondrawframe];
                else
                    animationspeed = egganimationrates[5];
            }
        }
    }

    animate();
}

void CO_Egg::draw()
{
    MO_CarriedObject::draw();

    //Display explosion timer
    if (game_values.gamemodesettings.egg.explode > 0 && explosiondrawframe < 5) {
        if (owner && owner->iswarping())
            rm->spr_eggnumbers.draw(ix - collisionOffsetX, iy - collisionOffsetY, explosiondrawframe << 5, color << 5, 32, 32, owner->GetWarpState(), owner->GetWarpPlane());
        else
            rm->spr_eggnumbers.draw(ix - collisionOffsetX, iy - collisionOffsetY, explosiondrawframe << 5, color << 5, 32, 32);
    }
}

void CO_Egg::placeEgg()
{
    relocatetimer = 0;
    if (game_values.gamemodesettings.egg.explode > 0) {
        explosiondrawframe = game_values.gamemodesettings.egg.explode - 1;
        explosiondrawtimer = 62;

        if (explosiondrawframe < 5)
            animationspeed = egganimationrates[explosiondrawframe];
        else
            animationspeed = egganimationrates[5];
    } else {
        animationspeed = egganimationrates[5];
    }

    short x = 0, y = 0;
    short iAttempts = 32;
    while ((!g_map->findspawnpoint(5, &x, &y, collisionWidth, collisionHeight, false) ||
            objectcontainer[1].getClosestMovingObject(x, y, movingobject_yoshi) < 250.0f)
            && iAttempts-- > 0);

    setXi(x);
    setYi(y);

    vely = GRAVITATION;
    velx = 0.0f;

    owner_throw = NULL;
    owner_throw_timer = 0;

    Drop();
}

void CO_Egg::Drop()
{
    MO_CarriedObject::Drop();
    owner_throw_timer = 62;
}

//------------------------------------------------------------------------------
// class star (for star mode)
//------------------------------------------------------------------------------
CO_Star::CO_Star(gfxSprite *nspr, short type, short id) :
    MO_CarriedObject(nspr, 0, 0, 8, 8, 30, 30, 1, 1)
{
    iID = id;

    state = 1;
    iw = 32;
    ih = 32;
    movingObjectType = movingobject_star;

    iType = type;

    iOffsetY = type == 1 ? 32 : 0;

    sparkleanimationtimer = 0;
    sparkledrawframe = 0;

    dKickX = 3.0f;
    dKickY = 6.0f;

    fCarriedByKuriboShoe = true;

    placeStar();

    iOwnerRightOffset = 14;
    iOwnerLeftOffset = -22;
    iOwnerUpOffset = 32;
}

bool CO_Star::collide(CPlayer * player)
{
    if (game_values.gamemode->gamemode != game_mode_star)
        return false;

    CGM_Star * starmode = (CGM_Star *)game_values.gamemode;

    timer = 0;
    if (owner == NULL && player->isready()) {
        if (player->throw_star == 0 && player->AcceptItem(this)) {
            owner = player;
        }
    }

    if ((iType == 0 && player->isInvincible()) || player->isShielded() || starmode->isplayerstar(player) || game_values.gamemode->gameover)
        return false;

    CPlayer * oldstar = starmode->swapplayer(iID, player);

    if (owner == oldstar) {
        oldstar->throw_star = 30;
        Kick();
    }

    return false;
}

void CO_Star::update()
{
    if (owner) {
        MoveToOwner();
        timer = 0;
    } else if (++timer > 300) {
        placeStar();
    } else {
        applyfriction();

        //Collision detect map
        IO_MovingObject::update();
    }

    if (++sparkleanimationtimer >= 4) {
        sparkleanimationtimer = 0;
        sparkledrawframe += 32;
        if (sparkledrawframe >= smw->ScreenHeight)
            sparkledrawframe = 0;
    }
}

void CO_Star::draw()
{
    if (owner) {
        if (owner->iswarping())
            spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, iOffsetY, iw, ih, owner->GetWarpState(), owner->GetWarpPlane());
        else
            spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, iOffsetY, iw, ih);
    } else {
        if (velx != 0.0f)
            spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, iOffsetY, iw, ih);  //keep the star still while it's moving
        else
            spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iOffsetY, iw, ih);
    }

    if (owner && owner->iswarping())
        rm->spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, iType ? 0 : 32, 32, 32, owner->GetWarpState(), owner->GetWarpPlane());
    else
        rm->spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, iType ? 0 : 32, 32, 32);
}

void CO_Star::placeStar()
{
    if (game_values.gamemode->gamemode != game_mode_star)
        return;

    CGM_Star * starmode = (CGM_Star*)game_values.gamemode;

    timer = 0;

    CPlayer * star = starmode->getstarplayer(iID);

    if (star) {
        setXf(star->fx + HALFPW - 16.0f);
        setYf(star->fy + HALFPH - 17.0f);

        velx = star->velx;
        vely = star->vely;
    }

    Drop();
}


//------------------------------------------------------------------------------
// class flag base (for CTF mode)
//------------------------------------------------------------------------------
MO_FlagBase::MO_FlagBase(gfxSprite *nspr, short iTeamID, short iColorID) :
    IO_MovingObject(nspr, 1280, 960, 5, 0)  //use 1280 and 960 so when placing base, it doesn't interfere (look in getClosestObject())
{
    state = 1;
    iw = 32;
    ih = 32;
    collisionWidth = 32;
    collisionHeight = 32;

    objectType = object_moving;
    movingObjectType = movingobject_flagbase;
    teamID = iTeamID;
    iGraphicOffsetX = iColorID * 48;

    angle = (float)(RANDOM_INT(1000) * 0.00628f);
    anglechange = (float)(RANDOM_INT(100) * 0.0002f);
    anglechangetimer = (short)(RANDOM_INT(50) + 100);

    velx = sin(angle);
    vely = cos(angle);

    homeflag = NULL;

    placeFlagBase(true);

    speed = (float)game_values.gamemodesettings.flag.speed / 4.0f;

    timer = 0;

    fObjectCollidesWithMap = false;
}

bool MO_FlagBase::collide(CPlayer * player)
{
    if (teamID == player->teamID && player->carriedItem && player->carriedItem->getMovingObjectType() == movingobject_flag) {
        CO_Flag * flag = (CO_Flag*)player->carriedItem;
        scoreFlag(flag, player);
        timer = 0;
    }

    return false;
}

void MO_FlagBase::draw()
{
    spr->draw(ix - 8, iy - 8, iGraphicOffsetX, 0, 48, 48);
}


void MO_FlagBase::update()
{
    if (game_values.gamemodesettings.flag.speed > 0) {
        if (--anglechangetimer <= 0) {
            anglechange = (float)((RANDOM_INT(101) - 50) * 0.0002f);
            anglechangetimer = (short)(RANDOM_INT(50) + 100);
        }

        angle += anglechange;

        velx = speed * sin(angle);
        vely = speed * cos(angle);

        setXf(fx + velx);
        setYf(fy + vely);

        if (ix < 0) {
            velx = -velx;
            ix = 0;
            fx = (float)ix;

            angle = atan2(velx, vely);
		} else if (ix + collisionWidth >= smw->ScreenWidth) {
            velx = -velx;
			ix = smw->ScreenWidth - 1 - collisionWidth;
            fx = (float)ix;

            angle = atan2(velx, vely);
        }

        if (iy < 0) {
            vely = -vely;
            iy = 0;
            fy = (float)iy;

            angle = atan2(velx, vely);
		} else if (iy + collisionHeight >= smw->ScreenHeight) {
            vely = -vely;
			iy = smw->ScreenHeight - 1 - collisionHeight;
            fy = (float)iy;

            angle = atan2(velx, vely);
        }
    }

    if (game_values.gamemodesettings.flag.speed == 0 && timer++ > 1000)
        placeFlagBase(false);
}

void MO_FlagBase::placeFlagBase(bool fInit)
{
    timer = 0;
    short x = 0, y = 0;

    if (fInit && teamID < g_map->iNumFlagBases) {
        x = g_map->flagbaselocations[teamID].x;
        y = g_map->flagbaselocations[teamID].y;
    } else {
        short iAttempts = 32;
        while ((!g_map->findspawnpoint(5, &x, &y, collisionWidth, collisionHeight, false) ||
                objectcontainer[1].getClosestMovingObject(x, y, movingobject_flagbase) < 200.0f)
                && iAttempts-- > 0);
    }

    setXi(x);
    setYi(y);
}

void MO_FlagBase::collide(IO_MovingObject * object)
{
    if (object->getMovingObjectType() == movingobject_flag) {
        CO_Flag * flag = (CO_Flag*)object;
        CPlayer * player = flag->owner_throw;

        if (player) {
            if (teamID == player->teamID) {
                scoreFlag(flag, player);
            }
        }
    }
}

void MO_FlagBase::scoreFlag(CO_Flag * flag, CPlayer * player)
{
    if (flag->teamID == teamID) {
        flag->placeFlag();
        ifSoundOnPlay(rm->sfx_areatag);
    } else if (!game_values.gamemodesettings.flag.homescore || homeflag != NULL || game_values.gamemodesettings.flag.centerflag) {
        flag->placeFlag();
        if (!game_values.gamemode->gameover) {
            player->Score().AdjustScore(1);
            game_values.gamemode->CheckWinner(player);
        }

        ifSoundOnPlay(rm->sfx_racesound);

        if (game_values.gamemodesettings.flag.pointmove) {
            //Set the values way outside the map so it will place the base correctly
            ix = 1280;
            iy = 960;
            placeFlagBase(false);
        }
    }
}

//------------------------------------------------------------------------------
// class flag (for Capture the Flag mode)
//------------------------------------------------------------------------------
CO_Flag::CO_Flag(gfxSprite *nspr, MO_FlagBase * base, short iTeamID, short iColorID) :
    MO_CarriedObject(nspr, 0, 0, 4, 8, 30, 30, 1, 1, 0, iColorID << 6, 32, 32)
{
    state = 1;
    movingObjectType = movingobject_flag;
    flagbase = base;
    teamID = iTeamID;
    fLastFlagDirection = false;
    owner_throw = NULL;
    owner_throw_timer = 0;

    centerflag = teamID == -1;

    iOwnerRightOffset = HALFPW - 31;
    iOwnerLeftOffset = HALFPW + 1;
    iOwnerUpOffset = 38;

    fCarriedByKuriboShoe = true;

    placeFlag();
}

bool CO_Flag::collide(CPlayer * player)
{
    if (owner == NULL && player->isready() && (!fInBase || teamID != player->teamID)) {
        if (game_values.gamemodesettings.flag.touchreturn && teamID == player->teamID) {
            placeFlag();
            ifSoundOnPlay(rm->sfx_areatag);
        } else if (player->AcceptItem(this)) {
            owner = player;
            owner_throw = player;

            if (flagbase)
                flagbase->setFlag(NULL);
        }
    }

    return false;
}

void CO_Flag::update()
{
    if (owner) {
        MoveToOwner();
        timer = 0;
        fInBase = false;
        owner_throw = owner;
    } else if (fInBase) {
        if (flagbase) {
            setXf(flagbase->fx);
            setYf(flagbase->fy);
        }

        owner_throw = NULL;
    } else if (game_values.gamemodesettings.flag.autoreturn > 0 && ++timer > game_values.gamemodesettings.flag.autoreturn) {
        timer = 0;
        placeFlag();
        owner_throw = NULL;
    } else {
        if (owner_throw && --owner_throw_timer < 0) {
            owner_throw_timer = 0;
            owner_throw = NULL;
        }

        applyfriction();

        //Collision detect map
        fOldX = fx;
        fOldY = fy;

        collision_detection_map();
    }

    animate();

    if (centerflag) {
        if (animationtimer % 2 == 0) {
            animationOffsetY += 64;
            if (animationOffsetY > 192)
                animationOffsetY = 0;
        }
    }
}

void CO_Flag::draw()
{
    if (owner) {
        if (centerflag)
            animationOffsetY = owner->colorID << 6;

        if (owner->iswarping())
            spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY + (fLastFlagDirection ? 32 : 0), iw, ih, owner->GetWarpState(), owner->GetWarpPlane());
        else
            spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY + (fLastFlagDirection ? 32 : 0), iw, ih);
    } else {
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY + (fLastFlagDirection ? 32 : 0), iw, ih);
    }
}

void CO_Flag::MoveToOwner()
{
    MO_CarriedObject::MoveToOwner();

    if (owner)
        fLastFlagDirection = owner->isFacingRight();
}

void CO_Flag::placeFlag()
{
    if (centerflag) {
        Drop();
        fInBase = false;

        short iAttempts = 10;
        while (!g_map->findspawnpoint(5, &ix, &iy, collisionWidth, collisionHeight, false) && iAttempts-- > 0);
        fx = (float)ix;
        fy = (float)iy;

        velx = 0.0f;
        vely = 0.0f;
        fLastFlagDirection = false;
    } else if (flagbase) {
        Drop();
        fInBase = true;
        setXi(flagbase->ix);
        setYi(flagbase->iy);
        fLastFlagDirection = false;
        flagbase->setFlag(this);
    }

    owner_throw = NULL;
    owner_throw_timer = 0;

    timer = 0;
}

void CO_Flag::Drop()
{
    MO_CarriedObject::Drop();
    owner_throw_timer = 62;
}

//------------------------------------------------------------------------------
// class yoshi (for egg mode)
//------------------------------------------------------------------------------
MO_Yoshi::MO_Yoshi(gfxSprite *nspr, short iColor) :
    IO_MovingObject(nspr, 0, 0, 2, 8, 52, 56, 0, 0, 0, iColor * 56, 56, 52)
{
    objectType = object_moving;
    movingObjectType = movingobject_yoshi;
    state = 1;

    color = iColor;

    placeYoshi();

    fObjectCollidesWithMap = false;
}

bool MO_Yoshi::collide(CPlayer * player)
{
    if (player->carriedItem && player->carriedItem->getMovingObjectType() == movingobject_egg) {
        CO_Egg * egg = (CO_Egg*)player->carriedItem;

        if (egg->color == color) {
            if (!game_values.gamemode->gameover) {
                player->Score().AdjustScore(1);
                game_values.gamemode->CheckWinner(player);
            }

            placeYoshi();

            egg->placeEgg();

            ifSoundOnPlay(rm->sfx_yoshi);
        }
    }

    return false;
}

void MO_Yoshi::update()
{
    animate();

    if (++timer > 2000)
        placeYoshi();
}

void MO_Yoshi::placeYoshi()
{
    timer = 0;

    for (short tries = 0; tries < 64; tries++) {
		ix = (short)RANDOM_INT(smw->ScreenWidth - iw);
        iy = (short)RANDOM_INT(smw->ScreenHeight - ih - TILESIZE);	//don't spawn too low

        short ixl = ix / TILESIZE;
        short ixr = (ix + iw) / TILESIZE;
        short iyt = iy / TILESIZE;
        short iyb = (iy + ih) / TILESIZE;

        int upperLeft = g_map->map(ixl, iyt);
        int upperRight = g_map->map(ixr, iyt);
        int lowerLeft = g_map->map(ixl, iyb);
        int lowerRight = g_map->map(ixr, iyb);

        if ((upperLeft & tile_flag_solid) == 0 && (upperRight & tile_flag_solid) == 0 &&
                (lowerLeft & tile_flag_solid) == 0 && (lowerRight & tile_flag_solid) == 0 &&
                !g_map->block(ixl, iyt) && !g_map->block(ixr, iyt) && !g_map->block(ixl, iyb) && !g_map->block(ixr, iyb)) {
            //spawn on ground, but not on spikes
            short iDeathY = (iy+ih)/TILESIZE;
            short iDeathX1 = ix/TILESIZE;
            short iDeathX2 = (ix+iw)/TILESIZE;

            while (iDeathY < MAPHEIGHT) {
                int ttLeftTile = g_map->map(iDeathX1, iDeathY);
                int ttRightTile = g_map->map(iDeathX2, iDeathY);

                if (((ttLeftTile & tile_flag_solid || ttLeftTile & tile_flag_solid_on_top) && (ttLeftTile & tile_flag_death_on_top) == 0) ||
                        ((ttRightTile & tile_flag_solid || ttRightTile & tile_flag_solid_on_top) && (ttRightTile & tile_flag_death_on_top) == 0) ||
                        g_map->block(iDeathX1, iDeathY) || g_map->block(iDeathX2, iDeathY)) {
                    short top = ((iDeathY << 5) - ih) / TILESIZE;

                    if (g_map->spawn(1, iDeathX1, top) && g_map->spawn(1, iDeathX2, top) &&
                            g_map->spawn(1, iDeathX1, iDeathY - 1) && g_map->spawn(1, iDeathX2, iDeathY - 1)) {
                        setXi(ix);
                        setYi((iDeathY << 5) - ih);
                        return;
                    }

                    break;
                } else if (ttLeftTile & tile_flag_death_on_top || ttRightTile & tile_flag_death_on_top) {
                    break;
                }

                iDeathY++;
            }
        }
    }

    ix = 320;
    iy = 240;
}

void MO_Yoshi::collide(IO_MovingObject * object)
{
    if (object->getMovingObjectType() == movingobject_egg) {
        CO_Egg * egg = (CO_Egg*)object;

        if (egg->color == color && egg->owner_throw) {
            CPlayer * player = egg->owner_throw;

            if (!game_values.gamemode->gameover) {
                player->Score().AdjustScore(1);
                game_values.gamemode->CheckWinner(player);
            }

            placeYoshi();
            egg->placeEgg();

            ifSoundOnPlay(rm->sfx_yoshi);
        }
    }
}

//------------------------------------------------------------------------------
// class area (for Domination mode)
//------------------------------------------------------------------------------
OMO_Area::OMO_Area(gfxSprite *nspr, short iNumAreas) :
    IO_OverMapObject(nspr, 1280, 960, 5, 0)
{
    iw = (short)spr->getWidth() / 5;
    collisionWidth = iw;

    objectType = object_area;
    iPlayerID = -1;
    iTeamID = -1;

    colorID = -1;
    scoretimer = 0;

    state = 1;

    frame = 0;
    relocatetimer = 0;
    numareas = iNumAreas;
    totalTouchingPlayers = 0;
    touchingPlayer = NULL;

    placeArea();
}

bool OMO_Area::collide(CPlayer * player)
{
    if (player->tanookisuit.notStatue() && !player->isdead()) {
        totalTouchingPlayers++;

        if (totalTouchingPlayers == 1) {
            touchingPlayer = player;
        } else {
            touchingPlayer = NULL;
            reset();
        }
    }

    return false;
}

void OMO_Area::draw()
{
    spr->draw(ix, iy, frame, 0, iw, ih);
}

void OMO_Area::update()
{
    if (touchingPlayer)
        setOwner(touchingPlayer);

    if (iPlayerID != -1 && !game_values.gamemode->gameover) {
        if (++scoretimer >= (game_values.pointspeed << 1)) {
            scoretimer = 0;
            list_players[iPlayerID]->Score().AdjustScore(1);
            game_values.gamemode->CheckWinner(list_players[iPlayerID]);
        }
    }

    if (game_values.gamemodesettings.domination.relocationfrequency > 0) {
        if (++relocatetimer >= game_values.gamemodesettings.domination.relocationfrequency) {
            relocatetimer = 0;
            placeArea();
        }
    }

    totalTouchingPlayers = 0;
    touchingPlayer = NULL;
}

void OMO_Area::placeArea()
{
    short x = 0, y = 0;
    short iAttempts = 32;
    while ((!g_map->findspawnpoint(5, &x, &y, collisionWidth, collisionHeight, false) ||
            objectcontainer[0].getClosestObject(x, y, object_area) <= (200.0f - ((numareas - 3) * 25.0f)))
            && iAttempts-- > 0);

    setXi(x);
    setYi(y);
}

void OMO_Area::reset()
{
    iPlayerID = -1;
    iTeamID = -1;

    colorID = -1;
    scoretimer = 0;
    frame = 0;
}

void OMO_Area::setOwner(CPlayer * player)
{
    if (colorID != player->colorID) {
        iPlayerID = player->localID;
        iTeamID = player->teamID;
        colorID = player->colorID;

        frame = (colorID + 1) * iw;
        ifSoundOnPlay(rm->sfx_areatag);
    }
}

//------------------------------------------------------------------------------
// class KingOfTheHillArea (for King of the Hill mode)
//------------------------------------------------------------------------------
OMO_KingOfTheHillZone::OMO_KingOfTheHillZone(gfxSprite *nspr) :
    IO_OverMapObject(nspr, 0, 0, 5, 0)
{
    size = game_values.gamemodesettings.kingofthehill.areasize;

    if (size < 2)
        game_values.gamemodesettings.kingofthehill.areasize = size = 2;

    iw = TILESIZE * size;
    collisionWidth = iw;
    ih = TILESIZE * size;
    collisionHeight = ih;

    objectType = object_kingofthehill_area;
    colorID = -1;
    scoretimer = 0;

    state = 1;

    frame = 0;
    relocatetimer = 0;
    iPlayerID = -1;

    for (short iPlayer = 0; iPlayer < 4; iPlayer++) {
        playersTouching[iPlayer] = NULL;
        playersTouchingCount[iPlayer] = 0;
    }

    totalTouchingPlayers = 0;

    multiplier = 1;
    multipliertimer = 0;

    placeArea();
}

bool OMO_KingOfTheHillZone::collide(CPlayer * player)
{
    if (!player->IsTanookiStatue()) {
        playersTouching[player->getTeamID()] = player;
        playersTouchingCount[player->getTeamID()]++;
        totalTouchingPlayers++;
    }
    return false;
}

void OMO_KingOfTheHillZone::draw()
{
    for (short iRow = 0; iRow < size; iRow++) {
        short iYPiece = TILESIZE;
        if (iRow == 0)
            iYPiece = 0;
        if (iRow == size - 1)
            iYPiece = TILESIZE * 2;

        for (short iCol = 0; iCol < size; iCol++) {
            short iXPiece = TILESIZE;
            if (iCol == 0)
                iXPiece = 0;
            if (iCol == size - 1)
                iXPiece = TILESIZE * 2;

            short iColX = ix + (iCol << 5);
            short iRowX = iy + (iRow << 5);

            if (multiplier > 1)
                rm->spr_awardkillsinrow.draw(iColX + 8, iRowX + 8, (multiplier - 1) << 4, colorID << 4, 16, 16);

            spr->draw(iColX, iRowX, iXPiece + frame, iYPiece, TILESIZE, TILESIZE);
        }
    }
}

void OMO_KingOfTheHillZone::update()
{
    short iMax = 0;
    short iMaxTeam = -1;

    for (short iTeam = 0; iTeam < 4; iTeam++) {
        if (playersTouchingCount[iTeam] > iMax) {
            iMax = playersTouchingCount[iTeam];
            iMaxTeam = iTeam;
        }
    }

    if ((iMax << 1) > totalTouchingPlayers) { //If the max touching player team is greater than the rest of the touching players
        colorID = playersTouching[iMaxTeam]->getColorID();
        iPlayerID = playersTouching[iMaxTeam]->localID;
        frame = ((colorID + 1) << 5) * 3;
    } else {
        colorID = -1;
        iPlayerID = -1;
        frame = 0;
    }

    if (iPlayerID != -1 && !game_values.gamemode->gameover) {
        scoretimer += (iMax << 1) - totalTouchingPlayers;  //Speed of point accumulation is proportional to how many players are in zone

        if (scoretimer >= game_values.pointspeed) {
            scoretimer = 0;
            list_players[iPlayerID]->Score().AdjustScore(multiplier);
            game_values.gamemode->CheckWinner(list_players[iPlayerID]);

            if (game_values.gamemodesettings.kingofthehill.maxmultiplier > 1 && ++multipliertimer >= 5) {
                multipliertimer = 0;

                if (multiplier < game_values.gamemodesettings.kingofthehill.maxmultiplier)
                    multiplier++;
            }
        }
    } else {
        multiplier = 1;
        multipliertimer = 0;
    }

    if (game_values.gamemodesettings.kingofthehill.relocationfrequency > 0) {
        if (++relocatetimer >= game_values.gamemodesettings.kingofthehill.relocationfrequency) {
            relocatetimer = 0;
            placeArea();
        }
    }

    for (short iPlayer = 0; iPlayer < 4; iPlayer++) {
        playersTouching[iPlayer] = NULL;
        playersTouchingCount[iPlayer] = 0;
    }

    totalTouchingPlayers = 0;
}

void OMO_KingOfTheHillZone::placeArea()
{
    relocatetimer = 0;
    colorID = -1;
    iPlayerID = -1;
    frame = 0;

    multiplier = 1;
    multipliertimer = 0;

    short x;
    short y;

    for (short iLoop = 0; iLoop < 64; iLoop++) {
        x = (short)RANDOM_INT(MAPWIDTH - size + 1);
        y = (short)RANDOM_INT(MAPHEIGHT - size);

        //First move the zone down so it is sitting on atleast 1 solid tile
        short iFindY = y + size;
        short iOldFindY = iFindY;
        bool fTryAgain = false;
        bool fDone = false;

        while (!fDone) {
            for (short iCol = 0; iCol < size; iCol++) {
                int type = g_map->map(x + iCol, iFindY);
                if (((type & tile_flag_solid_on_top || type & tile_flag_solid) && (type & tile_flag_death_on_top) == 0) || g_map->block(x + iCol, iFindY)) {
                    fDone = true;
                    break;
                }
            }

            if (fDone)
                break;

            if (++iFindY >= MAPHEIGHT)
                iFindY = size;

            if (iFindY == iOldFindY) { //If we didn't find solid ground in that loop, look for a new place for the zone
                fTryAgain = true;
                break;
            }
        }

        if (fTryAgain)
            continue;

        y = iFindY - size;

        //Now verify that the area is not completely covered with solid tiles
        short iCountSolidTiles = 0;
        for (short iRow = 0; iRow < size; iRow++) {
            for (short iCol = 0; iCol < size; iCol++) {
                //If there is a solid tile inside the zone
                if ((g_map->map(x + iCol, y + iRow) & tile_flag_solid) || !g_map->spawn(1, x + iCol, y + iRow) || g_map->block(x + iCol, y + iRow)) {
                    iCountSolidTiles++;

                    //Be more picky in the first few loops, but allow solid tiles to be in
                    if ((iLoop < 16 && iCountSolidTiles > iKingOfTheHillZoneLimits[size - 2][0]) || (iLoop < 32 && iCountSolidTiles > iKingOfTheHillZoneLimits[size - 2][1]) ||
                            (iLoop < 48 && iCountSolidTiles > iKingOfTheHillZoneLimits[size - 2][2]) || (iLoop < 63 && iCountSolidTiles > iKingOfTheHillZoneLimits[size - 2][3])) {
                        fTryAgain = true;
                        break;
                    }
                }
            }

            if (fTryAgain)
                break;
        }

        if (fTryAgain)
            continue;

        //Verify zone is not in a platform
        if (g_map->IsInPlatformNoSpawnZone(x << 5, y << 5, size << 5, size << 5))
            continue;

        break;
    }

    ix = x << 5;
    iy = y << 5;
}

void OMO_KingOfTheHillZone::reset()
{
    iPlayerID = -1;
    colorID = -1;
    scoretimer = 0;
    frame = 0;
}

//------------------------------------------------------------------------------
// class race goal (for Race mode)
//------------------------------------------------------------------------------
short flagpositions[3][4][2] = { { {18, 20}, {34, 20}, {0, 0}, {0, 0} },
    { {18, 20}, {34, 20}, {26, 36}, {0, 0} },
    { {18, 20}, {34, 20}, {18, 36}, {34, 36} }
};

OMO_RaceGoal::OMO_RaceGoal(gfxSprite *nspr, short id) :
    IO_OverMapObject(nspr, 0, 0, 2, 8)
{
    iw = (short)spr->getWidth() >> 1;
    ih = (short)spr->getHeight() >> 1;
    collisionWidth = 36;
    collisionHeight = 36;
    collisionOffsetX = 16;
    collisionOffsetY = 18;

    objectType = object_race_goal;
    state = 1;

    goalID = id;

    for (short k = 0; k < 4; k++)
        tagged[k] = -1;

    angle = (float)(RANDOM_INT(1000) * 0.00628f);
    anglechange = (float)(RANDOM_INT(100) * 0.0002f);
    anglechangetimer = (short)(RANDOM_INT(50) + 100);

    velx = sin(angle);
    vely = cos(angle);

    placeRaceGoal();

    speed = (float)game_values.gamemodesettings.race.speed / 4.0f;
    quantity = game_values.gamemodesettings.race.quantity;
    isfinishline = goalID == quantity - 1;
}

bool OMO_RaceGoal::collide(CPlayer * player)
{
    if (game_values.gamemode->getgamemode() == game_mode_race && player->tanookisuit.notStatue()) {
        CGM_Race * gamemode = (CGM_Race*)game_values.gamemode;

        if (tagged[player->teamID] != player->colorID && gamemode->getNextGoal(player->teamID) >= goalID) {
            tagged[player->teamID] = player->colorID;

            if (isfinishline)
                ifSoundOnPlay(rm->sfx_racesound);
            else
                ifSoundOnPlay(rm->sfx_areatag);
        }

        if (gamemode->getNextGoal(player->teamID) == goalID)
            gamemode->setNextGoal(player->teamID);
    }
    return false;
}

void OMO_RaceGoal::draw()
{
    if (isfinishline) {
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, 54, iw, ih);
    } else {
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, 0, iw, ih);

        for (short k = 0; k < score_cnt; k++) {
            if (tagged[k] > -1)
                rm->spr_bonus.draw(ix - collisionOffsetX + flagpositions[score_cnt - 2][k][0], iy - collisionOffsetY + flagpositions[score_cnt - 2][k][1], 0, tagged[k] * 16, 16, 16);
        }
    }

    rm->spr_racetext.draw(ix - collisionOffsetX + 26, iy - collisionOffsetY, (goalID + 1) * 16, 0, 16, 16);
}

void OMO_RaceGoal::update()
{
    if (--anglechangetimer <= 0) {
        anglechange = (float)((RANDOM_INT(101) - 50) * 0.0002f);
        anglechangetimer = (short)(RANDOM_INT(50) + 100);
    }

    angle += anglechange;

    velx = speed * sin(angle);
    vely = speed * cos(angle);

    IO_OverMapObject::update();

    ix = (short)fx;
    iy = (short)fy;

    if (ix < 0) {
        velx = -velx;
        ix = 0;
        fx = (float)ix;

        angle = atan2(velx, vely);
    } else if (ix + collisionWidth >= smw->ScreenWidth) {
        velx = -velx;
        ix = smw->ScreenWidth - 1 - collisionWidth;
        fx = (float)ix;

        angle = atan2(velx, vely);
    }

    if (iy < 0) {
        vely = -vely;
        iy = 0;
        fy = (float)iy;

        angle = atan2(velx, vely);
	} else if (iy + collisionHeight >= smw->ScreenHeight) {
        vely = -vely;
		iy = smw->ScreenHeight - 1 - collisionHeight;
        fy = (float)iy;

        angle = atan2(velx, vely);
    }
}

void OMO_RaceGoal::placeRaceGoal()
{
    short x = 0, y = 0;

    if (goalID < g_map->iNumRaceGoals) {
        x = g_map->racegoallocations[goalID].x;
        y = g_map->racegoallocations[goalID].y;
    } else {
        short tries = 0;
        do {
            if (++tries > 32)
                break;

            x = (short)RANDOM_INT(smw->ScreenWidth - collisionWidth);
            y = (short)RANDOM_INT(smw->ScreenHeight - collisionHeight);
        } while (objectcontainer[2].getClosestObject(x, y, object_race_goal) <= 250.0f - (quantity * 25.0f));
    }

    setXi(x);
    setYi(y);
}

//------------------------------------------------------------------------------
// class frenzycard (for fire frenzy mode)
//------------------------------------------------------------------------------
MO_FrenzyCard::MO_FrenzyCard(gfxSprite *nspr, short iType) :
    IO_MovingObject(nspr, 0, 0, 12, 8, -1, -1, -1, -1, 0, iType * 32, 32, 32)
{
    state = 1;
    objectType = object_frenzycard;
    type = iType;

    if (type == NUMFRENZYCARDS - 1)
        type = RANDOM_INT (NUMFRENZYCARDS - 1);

    sparkleanimationtimer = 0;
    sparkledrawframe = 0;

    placeCard();

    fObjectCollidesWithMap = false;
}

bool MO_FrenzyCard::collide(CPlayer * player)
{
    if (type < 14 || type > 17 || game_values.gamemodesettings.frenzy.storedshells) {
        player->SetPowerup(type);
        game_values.gamemode->frenzyowner = player;
    } else {
        switch (type) {
        case 14: {
            CO_Shell * shell = new CO_Shell(0, 0, 0, true, true, true, false);
            if (objectcontainer[1].add(shell))
                shell->UsedAsStoredPowerup(player);
            break;
        }
        case 15: {
            CO_Shell * shell = new CO_Shell(1, 0, 0, false, true, true, false);
            if (objectcontainer[1].add(shell))
                shell->UsedAsStoredPowerup(player);
            break;
        }
        case 16: {
            CO_Shell * shell = new CO_Shell(2, 0, 0, false, false, true, true);
            if (objectcontainer[1].add(shell))
                shell->UsedAsStoredPowerup(player);
            break;
        }
        case 17: {
            CO_Shell * shell = new CO_Shell(3, 0, 0, false, true, false, false);
            if (objectcontainer[1].add(shell))
                shell->UsedAsStoredPowerup(player);
            break;
        }
        }
    }

    dead = true;
    return false;
}

void MO_FrenzyCard::update()
{
    animate();

    if (++sparkleanimationtimer >= 4) {
        sparkleanimationtimer = 0;
        sparkledrawframe += 32;
        if (sparkledrawframe >= smw->ScreenHeight)
            sparkledrawframe = 0;
    }

    if (++timer > 1500)
        placeCard();
}

void MO_FrenzyCard::draw()
{
    IO_MovingObject::draw();

    //Draw sparkles
    rm->spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, 32);
}

void MO_FrenzyCard::placeCard()
{
    timer = 0;

    short x = 0, y = 0;
    short iAttempts = 32;
    while ((!g_map->findspawnpoint(5, &x, &y, collisionWidth, collisionHeight, false) ||
            objectcontainer[1].getClosestObject(x, y, object_frenzycard) <= 150.0f)
            && iAttempts-- > 0);

    setXi(x);
    setYi(y);
}

//------------------------------------------------------------------------------
// class collection card (for card collection mode)
//------------------------------------------------------------------------------
MO_CollectionCard::MO_CollectionCard(gfxSprite *nspr, short iType, short iValue, short iUncollectableTime, float dvelx, float dvely, short iX, short iY) :
    IO_MovingObject(nspr, iX, iY, 6, 8, -1, -1, -1, -1, 0, 0, 32, 32)
{
    state = 1;
    objectType = object_moving;
    movingObjectType = movingobject_collectioncard;

    sparkleanimationtimer = 0;
    sparkledrawframe = 0;

    type = iType;
    value = iValue;

    uncollectabletime = iUncollectableTime;
    velx = dvelx;
    vely = dvely;

    if (iType == 0) {
        placeCard();
        fObjectCollidesWithMap = false;
    } else {
        collision_detection_checksides();
        animationOffsetY = (value + 1) << 5;
    }
}

bool MO_CollectionCard::collide(CPlayer * player)
{
    //If it is not collectable, return
    if ((type == 1 && uncollectabletime > 0) || state != 1)
        return false;

    ifSoundOnPlay(rm->sfx_areatag);

    //Add this card to the team's score
    if (player->Score().subscore[0] < 3) {
        player->Score().subscore[1] |= value << (player->Score().subscore[0] << 1);
        player->Score().subscore[0]++;
    } else {
        player->Score().subscore[1] &= ~48; //Clear previous card in 3rd slot
        player->Score().subscore[1] |= value << 4; //Set card to newly collected one in 3rd slot
    }

    player->Score().subscore[2] = 0;

    if (type == 1) {
        dead = true;
    } else {
        state = 2;
        animationspeed = 4;
        animationtimer = 0;
        animationOffsetY = animationOffsetY = (value + 1) << 5; // FIXME
        drawframe = 96;
    }

    timer = 0;

    return false;
}

void MO_CollectionCard::update()
{
    if (type == 1 || state < 3)
        animate();

    //Handle flipping over a card to reveal it's value
    if (state == 2 && drawframe == 0) {
        state = 3;
        timer = 0;
    } else if (state == 3) {
        if (++timer > 200) {
            dead = true;
            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix, iy, 3, 8));
        }
    }


    if (++sparkleanimationtimer >= 4) {
        sparkleanimationtimer = 0;
        sparkledrawframe += 32;
        if (sparkledrawframe >= smw->ScreenHeight)
            sparkledrawframe = 0;
    }

    if (type == 0) {
        if (++timer > 1500)
            placeCard();
    } else {
        applyfriction();
        IO_MovingObject::update();

        if (--uncollectabletime < -game_values.gamemodesettings.collection.cardlife) {
            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix, iy, 3, 8));
            dead = true;
        }
    }
}

void MO_CollectionCard::draw()
{
    IO_MovingObject::draw();

    //Draw sparkles
    rm->spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, 32);
}

void MO_CollectionCard::placeCard()
{
    timer = 0;

    short x = 0, y = 0;
    short iAttempts = 32;
    while ((!g_map->findspawnpoint(5, &x, &y, collisionWidth, collisionHeight, false) ||
            objectcontainer[1].getClosestMovingObject(x, y, movingobject_collectioncard) <= 150.0f)
            && iAttempts-- > 0);

    setXi(x);
    setYi(y);
}


//------------------------------------------------------------------------------
// class walking enemy (base class for goomba and koopa)
//------------------------------------------------------------------------------
MO_WalkingEnemy::MO_WalkingEnemy(gfxSprite *nspr, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth, bool moveToRight, bool killOnWeakWeapon, bool bouncing, bool fallOffLedges) :
    IO_MovingObject(nspr, 0, 0, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY, iAnimationOffsetX, iAnimationOffsetY, iAnimationHeight, iAnimationWidth)
{
    if (moveToRight)
        velx = 1.0f;
    else
        velx = -1.0f;

    movingObjectType = movingobject_none;

    fBouncing = bouncing;
    if (fBouncing)
        bounce = -VELENEMYBOUNCE;
    else
        bounce = GRAVITATION;

    spawnradius = 100.0f;
    spawnangle = (float)(RANDOM_INT(1000) * 0.00628f);
    inair = true;

    iSpawnIconOffset = 0;

    burnuptimer = 0;

    fKillOnWeakWeapon = killOnWeakWeapon;

    frozen = false;
    frozentimer = 0;
    frozenvelocity = velx;
    frozenanimationspeed = aniSpeed;

    fFallOffLedges = fallOffLedges;

    place();
}

void MO_WalkingEnemy::draw()
{
    if (state == 0) {
        short numeyecandy = 8;
        float addangle = TWO_PI / numeyecandy;
        float displayangle = spawnangle;

        for (short k = 0; k < numeyecandy; k++) {
            short spawnX = ix + (collisionWidth >> 1) - 8 + (short)(spawnradius * cos(displayangle));
            short spawnY = iy + (collisionHeight >> 1) - 8 + (short)(spawnradius * sin(displayangle));

            displayangle += addangle;

            rm->spr_awardsouls.draw(spawnX, spawnY, iSpawnIconOffset, 0, 16, 16);
        }
    } else {
        IO_MovingObject::draw();

        if (frozen) {
            rm->spr_iceblock.draw(ix - collisionOffsetX + iw - 32, iy - collisionOffsetY + ih - 32, 0, 0, 32, 32);
        }
    }
}

void MO_WalkingEnemy::update()
{
    if (frozen) {
        if (--frozentimer <= 0) {
            frozentimer = 0;
            frozen = false;

            velx = frozenvelocity;
            animationspeed = frozenanimationspeed;

            if (fBouncing)
                bounce = -VELENEMYBOUNCE;

            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix - collisionOffsetX + iw - 32, iy - collisionOffsetY + ih - 32, 3, 8));
        }
    }

    if (state == 0) {
        spawnradius -= 2.0f;
        spawnangle += 0.05f;

        if (spawnradius < 10.0f)
            state = 1;
    } else {
        IO_MovingObject::update();
    }

    //Deal with terminal burnup velocity
    if (vely >= MAXVELY) {
        if (++burnuptimer > 20) {
            if (burnuptimer > 80)
                KillObjectMapHazard();
            else
                eyecandy[0].add(new EC_SingleAnimation(&rm->spr_burnup, ix + (collisionWidth >> 1) - 16, iy + (collisionHeight >> 1) - 16, 5, 4));
        }
    } else {
        burnuptimer = 0;
    }

    //If this enemy doesn't fall off of ledges, then take a look at the area in front of them
    //to determine if they need to turn around
    if (!inair && !fFallOffLedges) {
        short probeCenterX = ix + (collisionWidth >> 1);
        short probeFrontX = ix + (velx > 0.0f ? collisionWidth + 1 : -1);
        short probeY = iy + collisionHeight + 5;

        if (platform) {
            int iFrontTileType = platform->GetTileTypeFromCoord(probeFrontX, probeY);
            int iCenterTileType = platform->GetTileTypeFromCoord(probeCenterX, probeY);

            bool fFrontGap = iFrontTileType == tile_flag_nonsolid || iFrontTileType == tile_flag_super_death_top;
            bool fCenterGap = iCenterTileType == tile_flag_nonsolid || iCenterTileType == tile_flag_super_death_top;

            //If there is a hole or the type will kill the enemy, then turn around
            if (fFrontGap && fCenterGap) {
                velx = -velx;
            }
        } else {
            if (probeFrontX >= smw->ScreenWidth) {
                probeFrontX -= smw->ScreenWidth;
            } else if (probeFrontX < 0) {
                probeFrontX += smw->ScreenWidth;
            }

            if (probeCenterX >= smw->ScreenWidth) {
                probeCenterX -= smw->ScreenWidth;
            } else if (probeCenterX < 0) {
                probeCenterX += smw->ScreenWidth;
            }

            if (probeFrontX >= 0 && probeFrontX < smw->ScreenWidth && probeCenterX >= 0 && probeCenterX < smw->ScreenWidth && probeY >= 0 && probeY < smw->ScreenHeight) {
                probeFrontX /= TILESIZE;
                probeCenterX /= TILESIZE;
                probeY /= TILESIZE;

                IO_Block * frontBlock = g_map->block(probeFrontX, probeY);
                IO_Block * centerBlock = g_map->block(probeCenterX, probeY);

                bool fFoundFrontBlock = frontBlock && !frontBlock->isTransparent() && !frontBlock->isHidden();
                bool fFoundCenterBlock = centerBlock && !centerBlock->isTransparent() && !centerBlock->isHidden();

                if (!fFoundFrontBlock && !fFoundCenterBlock) {
                    int frontTile = g_map->map(probeFrontX, probeY);
                    int centerTile = g_map->map(probeCenterX, probeY);

                    bool fFrontGap = (frontTile & tile_flag_super_death_top) || (!(frontTile & tile_flag_solid) && !(frontTile & tile_flag_solid_on_top));
                    bool fCenterGap = (centerTile & tile_flag_super_death_top) || (!(centerTile & tile_flag_solid) && !(centerTile & tile_flag_solid_on_top));

                    if (fFrontGap && fCenterGap) {
                        velx = -velx;
                    }
                }
            }
        }
    }
}


bool MO_WalkingEnemy::collide(CPlayer * player)
{
    if (state == 0)
        return false;

    if (player->isInvincible() || frozen) {
        player->AddKillerAward(NULL, killStyle);

        if (game_values.gamemode->gamemode == game_mode_stomp && !game_values.gamemode->gameover)
            player->Score().AdjustScore(1);

        if (frozen) {
            ShatterDie();
        } else {
            ifSoundOnPlay(rm->sfx_kicksound);
            Die();
        }
    } else {
        if (player->fOldY + PH <= fOldY && player->iy + PH >= iy)
            return hittop(player);
        else
            return hitother(player);
    }

    return false;
}

bool MO_WalkingEnemy::hitother(CPlayer * player)
{
    if (player->isShielded())
        return false;

    return player->KillPlayerMapHazard(false, kill_style_environment, false) != player_kill_nonkill;
}

void MO_WalkingEnemy::collide(IO_MovingObject * object)
{
    if (state == 0)
        return;

    if (!object->isDead()) {
        removeifprojectile(object, false, false);

        MovingObjectType type = object->getMovingObjectType();

        if (((type == movingobject_fireball || type == movingobject_hammer || type == movingobject_boomerang) && (fKillOnWeakWeapon || frozen)) || type == movingobject_shell || type == movingobject_throwblock || type == movingobject_throwbox || type == movingobject_bulletbill || type == movingobject_podobo || type == movingobject_attackzone || type == movingobject_explosion || type == movingobject_sledgehammer) {
            //Don't kill enemies with non-moving shells
            if (type == movingobject_shell && object->state == 2)
                return;

            //Don't kill enemies with slow or non-moving boxes
            if (type == movingobject_throwbox && !((CO_ThrowBox*)object)->HasKillVelocity())
                return;

            if (game_values.gamemode->gamemode == game_mode_stomp && !game_values.gamemode->gameover) {
                //Find the player that shot this fireball so we can attribute a kill
                CPlayer * killer = GetPlayerFromGlobalID(object->iPlayerID);

                if (killer) {
                    killer->AddKillerAward(NULL, killStyle);
                    killer->Score().AdjustScore(1);

                    if (type == movingobject_shell)
                        ((CO_Shell*)object)->AddMovingKill(killer);
                }
            }

            if (frozen) {
                ShatterDie();
            } else {
                ifSoundOnPlay(rm->sfx_kicksound);

                if (type == movingobject_attackzone)
                    DieAndDropShell(true, false);
                else
                    Die();
            }

            if (type == movingobject_shell || type == movingobject_throwblock) {
                object->CheckAndDie();
            } else if (type == movingobject_bulletbill || type == movingobject_attackzone || type == movingobject_throwbox) {
                object->Die();
            }
        } else if (type == movingobject_iceblast) {
            frozenvelocity = velx;
            velx = 0.0f;
            animationspeed = 0;

            bounce = GRAVITATION;

            frozen = true;
            frozentimer = 300;

            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix - collisionOffsetX + iw - 32, iy - collisionOffsetY + ih - 32, 3, 8));
        }
    }
}


void MO_WalkingEnemy::place()
{
    short iAttempts = 10;
    while (!g_map->findspawnpoint(5, &ix, &iy, collisionWidth, collisionHeight, false) && iAttempts-- > 0);
    fx = (float)ix;
    fy = (float)iy;
}

void MO_WalkingEnemy::ShatterDie()
{
    ifSoundOnPlay(rm->sfx_breakblock);
    dead = true;

    short iBrokenIceX = ix - collisionOffsetX + iw - 32, iBrokenIceY = iy - collisionOffsetY + ih - 32;
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, iBrokenIceX, iBrokenIceY, -1.5f, -7.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, iBrokenIceX + 16, iBrokenIceY, 1.5f, -7.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, iBrokenIceX, iBrokenIceY + 16, -1.5f, -4.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, iBrokenIceX + 16, iBrokenIceY + 16, 1.5f, -4.0f, 4, 2, 0, 0, 16, 16));

    game_values.unlocksecret2part2++;
}


//------------------------------------------------------------------------------
// class goomba
//------------------------------------------------------------------------------
MO_Goomba::MO_Goomba(gfxSprite *nspr, bool moveToRight, bool fBouncing) :
    MO_WalkingEnemy(nspr, 2, 8, 30, 20, 1, 11, 0, moveToRight ? 0 : 32, 32, 32, moveToRight, true, fBouncing, true)
{
    movingObjectType = movingobject_goomba;
    iSpawnIconOffset = 64;
    killStyle = kill_style_goomba;

    if (fBouncing) {
        iw = 40;
        ih = 48;

        fOldY = fy - ih;

        collisionOffsetX = 5;
        collisionOffsetY = 27;

        animationOffsetY = moveToRight ? 0 : ih;
    }
}

void MO_Goomba::draw()
{
    //if frozen, just draw shell, not entire koopa
    if (frozen) {
        rm->spr_goomba.draw(ix - collisionOffsetX + iw - 32, iy - collisionOffsetY + ih - 32, 0, 0, 32, 32);
        rm->spr_iceblock.draw(ix - collisionOffsetX + iw - 32, iy - collisionOffsetY + ih - 32, 0, 0, 32, 32);
    } else {
        MO_WalkingEnemy::draw();
    }
}

void MO_Goomba::update()
{
    if (velx < 0.0f)
        animationOffsetY = ih;
    else
        animationOffsetY = 0;

    MO_WalkingEnemy::update();
}

bool MO_Goomba::hittop(CPlayer * player)
{
    player->setYi(iy - PH - 1);
    player->bouncejump();
    player->collisions.checktop(*player);
    player->platform = NULL;

    if (game_values.gamemode->gamemode == game_mode_stomp && !game_values.gamemode->gameover)
        player->Score().AdjustScore(1);

    if (fBouncing) {
        fBouncing = false;
        bounce = GRAVITATION;

        if (vely < GRAVITATION)
            vely = GRAVITATION;

        iw = 32;
        ih = 32;

        collisionOffsetX = 1;
        collisionOffsetY = 11;

        animationWidth = 64;
        drawframe = 0;

        animationOffsetY = velx > 0.0f ? 0 : ih;
        spr = &rm->spr_goomba;
    } else {
        dead = true;

        player->AddKillerAward(NULL, killStyle);

        eyecandy[0].add(new EC_Corpse(&rm->spr_goombadead, (float)(ix - collisionOffsetX), (float)(iy + collisionHeight - 32), 0));
    }

    ifSoundOnPlay(rm->sfx_mip);

    return false;
}


void MO_Goomba::Die()
{
    if (frozen) {
        ShatterDie();
        return;
    }

    dead = true;
    eyecandy[2].add(new EC_FallingObject(&rm->spr_goombadeadflying, ix, iy, 0.0f, -VELJUMP / 2.0f, 1, 0, 0, 0, 0, 0));
}

//------------------------------------------------------------------------------
// class koopa
//------------------------------------------------------------------------------
MO_Koopa::MO_Koopa(gfxSprite *nspr, bool moveToRight, bool red, bool fBouncing, bool bFallOffLedges) :
    MO_WalkingEnemy(nspr, 2, 8, 30, 28, 1, 25, 0, moveToRight ? 0 : 54, 54, 32, moveToRight, true, fBouncing, bFallOffLedges)
{
    fRed = red;
    movingObjectType = movingobject_koopa;
    iSpawnIconOffset = fRed ? 144 : 112;
    killStyle = kill_style_koopa;
}

void MO_Koopa::draw()
{
    //if frozen, just draw shell, not entire koopa
    if (frozen) {
        rm->spr_shell.draw(ix - collisionOffsetX + iw - 32, iy - collisionOffsetY + ih - 32, 0, fRed ? 32 : 0, 32, 32);
        rm->spr_iceblock.draw(ix - collisionOffsetX + iw - 32, iy - collisionOffsetY + ih - 32, 0, 0, 32, 32);
    } else {
        MO_WalkingEnemy::draw();
    }
}

void MO_Koopa::update()
{
    if (velx < 0.0f)
        animationOffsetY = 54;
    else
        animationOffsetY = 0;

    MO_WalkingEnemy::update();
}

bool MO_Koopa::hittop(CPlayer * player)
{
    player->setYi(iy - PH - 1);
    player->bouncejump();
    player->collisions.checktop(*player);
    player->platform = NULL;

    if (game_values.gamemode->gamemode == game_mode_stomp && !game_values.gamemode->gameover)
        player->Score().AdjustScore(1);

    if (fBouncing) {
        fBouncing = false;
        bounce = GRAVITATION;

        if (vely < GRAVITATION)
            vely = GRAVITATION;

        spr = fRed ? &rm->spr_redkoopa : &rm->spr_koopa;
    } else {
        dead = true;

        player->AddKillerAward(NULL, kill_style_koopa);

        DropShell(false, false);
    }

    ifSoundOnPlay(rm->sfx_mip);

    return false;
}

void MO_Koopa::Die()
{
    if (frozen) {
        ShatterDie();
        return;
    }

    dead = true;
    eyecandy[2].add(new EC_FallingObject(&rm->spr_shelldead, ix, iy, 0.0f, -VELJUMP / 2.0f, 1, 0, fRed ? 32 : 0, 0, 32, 32));
}

void MO_Koopa::DropShell(bool fBounce, bool fFlip)
{
    //Give the shell a state 2 so it is already spawned but sitting
    CO_Shell * shell;

    if (fRed)
        shell = new CO_Shell(1, ix - 1, iy + 8, false, true, true, false);
    else
        shell = new CO_Shell(0, ix - 1, iy + 8, true, true, true, false);

    shell->nospawn(iy + 8, fBounce);

    if (fFlip)
        shell->Flip();

    objectcontainer[1].add(shell);
}

//------------------------------------------------------------------------------
// class buzzy beetle
//------------------------------------------------------------------------------
MO_BuzzyBeetle::MO_BuzzyBeetle(gfxSprite *nspr, bool moveToRight) :
    MO_WalkingEnemy(nspr, 2, 8, 30, 20, 1, 11, 0, moveToRight ? 0 : 32, 32, 32, moveToRight, false, false, true)
{
    movingObjectType = movingobject_buzzybeetle;
    iSpawnIconOffset = 160;
    killStyle = kill_style_buzzybeetle;
}

void MO_BuzzyBeetle::update()
{
    if (velx < 0.0f)
        animationOffsetY = 32;
    else
        animationOffsetY = 0;

    MO_WalkingEnemy::update();
}

bool MO_BuzzyBeetle::hittop(CPlayer * player)
{
    player->setYi(iy - PH - 1);
    player->bouncejump();
    player->collisions.checktop(*player);
    player->platform = NULL;
    dead = true;

    player->AddKillerAward(NULL, kill_style_buzzybeetle);

    if (game_values.gamemode->gamemode == game_mode_stomp && !game_values.gamemode->gameover)
        player->Score().AdjustScore(1);

    ifSoundOnPlay(rm->sfx_mip);

    DropShell(false, false);

    return false;
}

void MO_BuzzyBeetle::Die()
{
    if (frozen) {
        ShatterDie();
        return;
    }

    dead = true;
    eyecandy[2].add(new EC_FallingObject(&rm->spr_shelldead, ix, iy, 0.0f, -VELJUMP / 2.0f, 1, 0, 96, 0, 32, 32));
}

void MO_BuzzyBeetle::DropShell(bool fBounce, bool fFlip)
{
    //Give the shell a state 2 so it is already spawned but sitting
    CO_Shell * shell = new CO_Shell(3, ix - 1, iy, false, true, false, false);
    shell->nospawn(iy, fBounce);

    if (fFlip)
        shell->Flip();

    objectcontainer[1].add(shell);
}

//------------------------------------------------------------------------------
// class spiny
//------------------------------------------------------------------------------
MO_Spiny::MO_Spiny(gfxSprite *nspr, bool moveToRight) :
    MO_WalkingEnemy(nspr, 2, 8, 30, 20, 1, 11, 0, moveToRight ? 0 : 32, 32, 32, moveToRight, true, false, true)
{
    movingObjectType = movingobject_spiny;
    iSpawnIconOffset = 176;
    killStyle = kill_style_spiny;
}

void MO_Spiny::update()
{
    if (velx < 0.0f)
        animationOffsetY = 32;
    else
        animationOffsetY = 0;

    MO_WalkingEnemy::update();
}

bool MO_Spiny::hittop(CPlayer * player)
{
    //Kill player here
    if (player->isready() && !player->isShielded() && !player->isInvincible() && !player->kuriboshoe.is_on())
        return player->KillPlayerMapHazard(false, kill_style_environment, false) != player_kill_nonkill;

    if (player->kuriboshoe.is_on()) {
        player->setYi(iy - PH - 1);
        player->bouncejump();
        player->collisions.checktop(*player);
        player->platform = NULL;

        dead = true;

        player->AddKillerAward(NULL, kill_style_spiny);

        if (game_values.gamemode->gamemode == game_mode_stomp && !game_values.gamemode->gameover)
            player->Score().AdjustScore(1);

        DropShell(false, false);

        ifSoundOnPlay(rm->sfx_mip);
    }

    return false;
}

void MO_Spiny::Die()
{
    if (frozen) {
        ShatterDie();
        return;
    }

    dead = true;
    eyecandy[2].add(new EC_FallingObject(&rm->spr_shelldead, ix, iy, 0.0f, -VELJUMP / 2.0f, 1, 0, 64, 0, 32, 32));
}

void MO_Spiny::DropShell(bool fBounce, bool fFlip)
{
    //Give the shell a state 2 so it is already spawned but sitting
    CO_Shell * shell = new CO_Shell(2, ix - 1, iy, false, true, false, false);
    shell->nospawn(iy, fBounce);

    if (fFlip)
        shell->Flip();

    objectcontainer[1].add(shell);
}

//------------------------------------------------------------------------------
// class cheep cheep
//------------------------------------------------------------------------------
MO_CheepCheep::MO_CheepCheep(gfxSprite *nspr) :
    IO_MovingObject(nspr, 0, smw->ScreenHeight, 2, 8, 30, 28, 1, 3)
{
    ih = 32;
    setXi((short)(RANDOM_INT(608)));

    velx = 0.0f;
    while (velx == 0.0f)
        velx = float(RANDOM_INT(19) - 9) / 2.0f;

    //Cheep cheep up velocity is between 9.0 and 13.0 in 0.5 increments
    vely = -(float(RANDOM_INT(11)) / 2.0f) - 9.0f;

    movingObjectType = movingobject_cheepcheep;
    state = 1;

    iColorOffsetY = (short)(RANDOM_INT(3)) * 64;

    if (velx > 0.0f)
        iColorOffsetY += 32;

    fObjectCollidesWithMap = false;
    frozen = false;
}

void MO_CheepCheep::update()
{
    fOldX = fx;
    fOldY = fy;

    setXf(fx + velx);
    setYf(fy + vely);

    //Cheep cheep gravitation
    vely += 0.2f;

    animate();

    //Remove if cheep cheep has fallen below bottom of screen
    if (vely > 0.0f && iy > smw->ScreenHeight)
        dead = true;
}

void MO_CheepCheep::draw()
{
    spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iColorOffsetY, iw, ih);

    if (frozen) {
        rm->spr_iceblock.draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, 0, 32, 32);
    }
}

bool MO_CheepCheep::collide(CPlayer * player)
{
    if (player->isInvincible() || frozen) {
        player->AddKillerAward(NULL, kill_style_cheepcheep);

        if (game_values.gamemode->gamemode == game_mode_stomp && !game_values.gamemode->gameover)
            player->Score().AdjustScore(1);

        if (frozen) {
            ShatterDie();
        } else {
            ifSoundOnPlay(rm->sfx_kicksound);
            Die();
        }
    } else {
        if (player->fOldY + PH <= fOldY && player->iy + PH >= iy)
            return hittop(player);
        else
            return hitother(player);
    }

    return false;
}


bool MO_CheepCheep::hittop(CPlayer * player)
{
    player->setYi(iy - PH - 1);
    player->bouncejump();
    player->collisions.checktop(*player);
    player->platform = NULL;

    player->AddKillerAward(NULL, kill_style_cheepcheep);

    if (game_values.gamemode->gamemode == game_mode_stomp && !game_values.gamemode->gameover)
        player->Score().AdjustScore(1);

    ifSoundOnPlay(rm->sfx_mip);

    Die();

    return false;
}

bool MO_CheepCheep::hitother(CPlayer * player)
{
    if (player->isShielded())
        return false;

    return player->KillPlayerMapHazard(false, kill_style_environment, false) != player_kill_nonkill;
}

void MO_CheepCheep::collide(IO_MovingObject * object)
{
    if (!object->isDead()) {
        removeifprojectile(object, false, false);

        MovingObjectType type = object->getMovingObjectType();

        if (type == movingobject_fireball || type == movingobject_hammer || type == movingobject_boomerang || type == movingobject_shell || type == movingobject_throwblock || type == movingobject_throwbox || type == movingobject_bulletbill || type == movingobject_podobo || type == movingobject_attackzone || type == movingobject_explosion || type == movingobject_sledgehammer) {
            //Don't kill goombas with non-moving shells
            if (type == movingobject_shell && object->state == 2)
                return;

            if (type == movingobject_throwbox && !((CO_ThrowBox*)object)->HasKillVelocity())
                return;

            if (game_values.gamemode->gamemode == game_mode_stomp && !game_values.gamemode->gameover) {
                //Find the player that shot this projectile so we can attribute a kill
                CPlayer * killer = GetPlayerFromGlobalID(object->iPlayerID);

                if (killer) {
                    killer->AddKillerAward(NULL, kill_style_cheepcheep);
                    killer->Score().AdjustScore(1);

                    if (type == movingobject_shell)
                        ((CO_Shell*)object)->AddMovingKill(killer);
                }
            }

            if (frozen) {
                ShatterDie();
            } else {
                ifSoundOnPlay(rm->sfx_kicksound);
                Die();
            }

            if (type == movingobject_shell || type == movingobject_throwblock) {
                object->CheckAndDie();
            } else if (type == movingobject_bulletbill || type == movingobject_attackzone || type == movingobject_throwbox) {
                object->Die();
            }
        } else if (type == movingobject_iceblast) {
            animationspeed = 0;
            frozen = true;

            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix - collisionOffsetX, iy - collisionOffsetY, 3, 8));
        }
    }
}

void MO_CheepCheep::Die()
{
    if (frozen) {
        ShatterDie();
        return;
    }

    dead = true;
    eyecandy[2].add(new EC_FallingObject(&rm->spr_cheepcheepdead, ix, iy, 0.0f, -VELJUMP / 2.0f, 1, 0, 0, iColorOffsetY, 32, 32));
}

void MO_CheepCheep::ShatterDie()
{
    ifSoundOnPlay(rm->sfx_breakblock);
    dead = true;

    short iBrokenIceX = ix - collisionOffsetX, iBrokenIceY = iy - collisionOffsetY;
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, iBrokenIceX, iBrokenIceY, -1.5f, -7.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, iBrokenIceX + 16, iBrokenIceY, 1.5f, -7.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, iBrokenIceX, iBrokenIceY + 16, -1.5f, -4.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, iBrokenIceX + 16, iBrokenIceY + 16, 1.5f, -4.0f, 4, 2, 0, 0, 16, 16));

    game_values.unlocksecret2part2++;
}

///////////////////////DEBUG!  REMOVE THIS WHEN DONE/////////////////////////////
#ifdef _DEBUG
extern bool fDebugShowBossSettings;
#endif
///////////////////////DEBUG!  REMOVE THIS WHEN DONE/////////////////////////////

//------------------------------------------------------------------------------
// class sledge brother
//------------------------------------------------------------------------------
//{jump, throw, turn, wait, taunt} otherwise move
short g_iSledgeBrotherActions[3][5][5] = {
    {{0,50,65,75,85},{3,53,66,76,86},{5,55,70,78,83},{5,70,85,85,90},{5,75,90,90,90}},
    {{0,30,60,80,85},{5,40,65,80,85},{5,50,65,75,85},{10,55,65,70,90},{10,55,65,65,90}},
    {{0,50,65,75,85},{3,53,65,75,85},{5,60,72,80,88},{5,65,80,85,90},{5,75,90,90,90}}
};

short g_iSledgeBrotherNeedAction[3][5][6] = {
    {{50,5,5,10,10,8},{30,4,5,10,10,8},{20,4,5,12,12,8},{12,4,5,15,15,10},{8,4,5,15,15,12}},
    {{50,5,5,10,15,8},{30,4,5,10,12,8},{20,4,5,12,10,8},{15,4,5,15,8,10},{10,4,5,15,6,12}},
    {{50,5,8,10,10,8},{30,5,8,12,12,10},{20,4,8,15,15,15},{15,4,8,15,15,15},{12,3,8,15,15,15}}
};

short g_iSledgeBrotherMaxAction[3][5][5] = {
    {{1,1,1,2,2},{1,2,1,2,2},{1,3,1,2,2},{1,3,1,1,1},{1,3,1,0,0}},
    {{1,1,1,2,2},{1,2,1,2,2},{1,3,1,2,2},{1,3,1,1,1},{1,3,1,0,0}},
    {{1,1,1,2,2},{1,2,1,2,2},{1,2,1,1,1},{1,3,1,1,1},{1,3,1,0,0}}
};

short g_iSledgeBrotherWaitTime[3][5][2] = {
    {{30,50},{25,45},{20,40},{15,30},{10,20}},
    {{30,50},{25,45},{20,40},{15,30},{10,20}},
    {{30,50},{25,45},{20,40},{15,30},{10,20}}
};

MO_SledgeBrother::MO_SledgeBrother(gfxSprite *nspr, short platformY, short type) :
    IO_MovingObject(nspr, 0, 0, 8, 0, 32, 56, 8, 8)
{
    iType = type;
    state = 1;
    iActionState = 0;
    location = 2;

    ih = spr->getHeight() / 3;

    movingObjectType = movingobject_sledgebrother;

    inair = true;
    throwing_timer = 0;

    hit_timer = 0;
    hit_movement_timer = 0;
    hit_offset_y = iType * 64;

    leg_offset_x = 0;
    leg_movement_timer = 0;

    arm_offset_x = 0;
    arm_movement_timer = 0;

    taunt_timer = 0;

    wait_timer = 0;

    hit_points = game_values.gamemodesettings.boss.hitpoints;
    face_right = false;

    vely = 0.0f;
    velx = 0.0f;

    iPlatformY = platformY - collisionHeight;
    setYi(iPlatformY);

    for (short iLocation = 0; iLocation < 5; iLocation++)
        iDestLocationX[iLocation] = 84 * iLocation + 128;

    iDestX = iDestLocationX[location];
    setXi(iDestX);

    for (short iAction = 0; iAction < 6; iAction++)
        need_action[iAction] = 0;

    last_action = -1;
    last_action_count = 0;
}

void MO_SledgeBrother::draw()
{
    spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, leg_offset_x + arm_offset_x + (face_right ? 0 : 192), hit_offset_y, iw, ih);

    if (hit_timer != 0) {
        for (short iHeart = 0; iHeart < hit_points; iHeart++)
            rm->spr_scorehearts.draw(ix - collisionOffsetX + iHeart * 8, iy - collisionOffsetY - 18, 0, 0, 16, 16);
    }
}

void MO_SledgeBrother::update()
{
    if (iActionState == 0) {
        if (hit_timer <= 0) {
            randomaction();
        }
    } else if (iActionState == 1) {
        if (--wait_timer <= 0)
            iActionState = 0;
    } else if (iActionState == 2) {
        setYf(fy + vely);
        vely += GRAVITATION;

        if (iy >= iPlatformY) {
            iActionState = 0;

            if (iType == 0) {
                //Shake screen and kill players
                ifSoundOnPlay(rm->sfx_thunder);
                game_values.screenshaketimer = 20;
                game_values.screenshakeplayerid = -1;
                game_values.screenshaketeamid = -1;
                game_values.screenshakekillinair = false;
                game_values.screenshakekillscount = 0;
            } else if (iType == 1) {
                //Spawn thwomps
                ifSoundOnPlay(rm->sfx_thunder);

                short numThwomps = RANDOM_INT(5) + 6;

                for (short iThwomp = 0; iThwomp < numThwomps; iThwomp++) {
                    objectcontainer[2].add(new OMO_Thwomp(&rm->spr_thwomp, (short)(RANDOM_INT(591)), 2.0f + (float)(RANDOM_INT(20))/10.0f));
                }

                //short numBombs = RANDOM_INT(5) + 6;

                //for (short iBomb = 0; iBomb < numBombs; iBomb++)
                //{
                //	short iRandomX = RANDOM_INT(612);
                //	short iRandomY = RANDOM_INT(442);
                //	eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, iRandomX - 2, iRandomY + 3, 3, 8));
                //	objectcontainer[2].add(new CO_Bomb(&rm->spr_bomb, iRandomX, iRandomY, 0.0f, 0.0f, 4, -1, -1, -1, RANDOM_INT(30) + 30));
                //}
            } else if (iType == 2) {
                //Spawn lots of podobos
                ifSoundOnPlay(rm->sfx_thunder);

                short numPodobos = RANDOM_INT(5) + 8;

                for (short iPodobo = 0; iPodobo < numPodobos; iPodobo++) {
                    objectcontainer[2].add(new MO_Podobo(&rm->spr_podobo, (short)(RANDOM_INT(608)), smw->ScreenHeight, -(float(RANDOM_INT(9)) / 2.0f) - 9.0f, -1, -1, -1, false));
                }
            }
        }
    } else if (iActionState == 3) {
        if (--throwing_timer <= 0) {
            iActionState = 0;
            throwing_timer = 0;
            arm_offset_x = 0;
        }
    } else if (iActionState == 4) {
        //move towards destination
        if (ix < iDestX) {
            ix += game_values.gamemodesettings.boss.difficulty >= 3 ? 2 : 1;

            if (ix >= iDestX) {
                ix = iDestX;
                iActionState = 0;
                leg_offset_x = 0;
                leg_movement_timer = 0;
            }
        } else if (ix > iDestX) {
            ix -= game_values.gamemodesettings.boss.difficulty >= 3 ? 2 : 1;

            if (ix <= iDestX) {
                ix = iDestX;
                iActionState = 0;
                leg_offset_x = 0;
                leg_movement_timer = 0;
            }
        }

        if (iActionState != 0 && ++leg_movement_timer == 8) {
            leg_movement_timer = 0;

            if (leg_offset_x == 0)
                leg_offset_x = 48;
            else
                leg_offset_x = 0;
        }
    } else if (iActionState == 5) {
        //If we are done taunting, reset arm/legs back to normal state
        if (--taunt_timer <= 0) {
            iActionState = 0;
            arm_offset_x = 0;
            leg_offset_x = 0;
            arm_movement_timer = 0;
            leg_movement_timer = 0;
        } else { //otherwise move them around
            if (++arm_movement_timer == 8) {
                arm_movement_timer = 0;

                if (arm_offset_x == 0)
                    arm_offset_x = 96;
                else
                    arm_offset_x = 0;
            }

            if (++leg_movement_timer == 6) {
                leg_movement_timer = 0;

                if (leg_offset_x == 0)
                    leg_offset_x = 48;
                else
                    leg_offset_x = 0;
            }
        }
    }

    if (hit_timer > 0) {
        if (--hit_timer <= 0) {
            hit_offset_y = iType * 64;
            hit_timer = 0;
        } else {
            if (++hit_movement_timer == 2) {
                hit_movement_timer = 0;

                hit_offset_y += 64;

                if (hit_offset_y >= 192)
                    hit_offset_y = 0;
            }
        }
    }
}

void MO_SledgeBrother::randomaction()
{
    int randaction = RANDOM_INT(100);

    //Force an action if it has been too long since the last action of that type
    if (need_action[0] > g_iSledgeBrotherNeedAction[iType][game_values.gamemodesettings.boss.difficulty][0])
        jump();
    else if (need_action[1] > g_iSledgeBrotherNeedAction[iType][game_values.gamemodesettings.boss.difficulty][1])
        throwprojectile();
    else if (need_action[2] > g_iSledgeBrotherNeedAction[iType][game_values.gamemodesettings.boss.difficulty][2])
        turn();
    else if (need_action[3] > g_iSledgeBrotherNeedAction[iType][game_values.gamemodesettings.boss.difficulty][3])
        wait(g_iSledgeBrotherWaitTime[iType][game_values.gamemodesettings.boss.difficulty][0], g_iSledgeBrotherWaitTime[iType][game_values.gamemodesettings.boss.difficulty][1]);
    else if (need_action[4] > g_iSledgeBrotherNeedAction[iType][game_values.gamemodesettings.boss.difficulty][4])
        taunt();
    else if (need_action[5] > g_iSledgeBrotherNeedAction[iType][game_values.gamemodesettings.boss.difficulty][5])
        move();
    //then do action based on probability
    else if ((last_action != 0 || last_action_count < g_iSledgeBrotherMaxAction[iType][game_values.gamemodesettings.boss.difficulty][0]) &&
            randaction < g_iSledgeBrotherActions[iType][game_values.gamemodesettings.boss.difficulty][0])
        jump();
    else if ((last_action != 1 || last_action_count < g_iSledgeBrotherMaxAction[iType][game_values.gamemodesettings.boss.difficulty][1]) &&
            randaction < g_iSledgeBrotherActions[iType][game_values.gamemodesettings.boss.difficulty][1])
        throwprojectile();
    else if ((last_action != 2 || last_action_count < g_iSledgeBrotherMaxAction[iType][game_values.gamemodesettings.boss.difficulty][2]) &&
            randaction < g_iSledgeBrotherActions[iType][game_values.gamemodesettings.boss.difficulty][2])
        turn();
    else if ((last_action != 2 || last_action_count < g_iSledgeBrotherMaxAction[iType][game_values.gamemodesettings.boss.difficulty][3]) &&
            randaction < g_iSledgeBrotherActions[iType][game_values.gamemodesettings.boss.difficulty][3])
        wait(g_iSledgeBrotherWaitTime[iType][game_values.gamemodesettings.boss.difficulty][0], g_iSledgeBrotherWaitTime[iType][game_values.gamemodesettings.boss.difficulty][1]);
    else if ((last_action != 2 || last_action_count < g_iSledgeBrotherMaxAction[iType][game_values.gamemodesettings.boss.difficulty][4]) &&
            randaction < g_iSledgeBrotherActions[iType][game_values.gamemodesettings.boss.difficulty][4])
        taunt();
    else
        move();

    for (short iAction = 0; iAction < 6; iAction++)
        need_action[iAction]++;
}

void MO_SledgeBrother::move()
{
    bool moveright = true;
    if (location == 0)
        moveright = true;
    else if (location == 4)
        moveright = false;
    else
        moveright = RANDOM_INT(2) == 0;

    SetLastAction(3);

    if (moveright)
        location++;
    else
        location--;

    iDestX = iDestLocationX[location];
    iActionState = 4;

    if (iDestX > ix)
        face_right = true;
    else
        face_right = false;

    need_action[5] = 0;
}

void MO_SledgeBrother::throwprojectile()
{
    SetLastAction(1);

    throwing_timer = 20;
    iActionState = 3;
    arm_offset_x = 96;

    if (iType == 0) {
        float fHammerVelX = ((float)(RANDOM_INT(9) + 2)) / 2.0f - (face_right ? 0.0f : 6.0f);
        objectcontainer[2].add(new MO_SledgeHammer(&rm->spr_sledgehammer, (face_right ? ix + 32 : ix) - collisionOffsetX, iy, 8, fHammerVelX, -HAMMERTHROW, 5, -1, -1, -1, false));
    } else if (iType == 1) {
        float fBombVelX = ((float)(RANDOM_INT(5) + 12)) / 2.0f - (face_right ? 0.0f : 14.0f);
        float fBombVelY = -(float)(RANDOM_INT(13)) / 2.0f - 6.0f;
        objectcontainer[2].add(new CO_Bomb(&rm->spr_bomb, face_right ? ix + iw - 32 : ix - 20, iy, fBombVelX, fBombVelY, 4, -1, -1, -1, RANDOM_INT(60) + 120));
    } else if (iType == 2) {
        float fFireVelX = ((float)(RANDOM_INT(9) + 6)) / 2.0f - (face_right ? 0.0f : 10.0f);
        float fFireVelY = (float)(RANDOM_INT(17)) / 2.0f - 4.0f;
        objectcontainer[2].add(new MO_SuperFireball(&rm->spr_superfireball, face_right ? ix + iw - 32 : ix - 16, iy, 4, fFireVelX, fFireVelY, 4, -1, -1, -1));
    }

    need_action[1] = 0;
}

void MO_SledgeBrother::taunt()
{
    SetLastAction(2);

    ifSoundOnPlayLoop(rm->sfx_boomerang, 3);
    taunt_timer = 60;
    iActionState = 5;

    need_action[4] = 0;

    //If this is a bomb brother, push bombs away when taunting
    if (iType == 1) {
        objectcontainer[2].pushBombs(ix + 32, iy + 32);
    }
}

void MO_SledgeBrother::turn()
{
    face_right = !face_right;
    wait(g_iSledgeBrotherWaitTime[iType][game_values.gamemodesettings.boss.difficulty][0] >> 1, g_iSledgeBrotherWaitTime[iType][game_values.gamemodesettings.boss.difficulty][1] >> 1);

    need_action[2] = 0;
}

void MO_SledgeBrother::jump()
{
    SetLastAction(0);

    vely = -VELJUMP;
    iActionState = 2;

    need_action[0] = 0;
}

void MO_SledgeBrother::wait(short min, short max)
{
    SetLastAction(2);

    wait_timer = RandomNumberGenerator::generator().getInteger(min, max);
    iActionState = 1;

    need_action[3] = 0;
}

bool MO_SledgeBrother::collide(CPlayer * player)
{
    if (iActionState == 0)
        return false;

    if (player->isInvincible()) {
        ifSoundOnPlay(rm->sfx_kicksound);
        Die();

        if (game_values.gamemode->gamemode == game_mode_boss_minigame) {
            ((CGM_Boss_MiniGame*)game_values.gamemode)->SetWinner(player);
        }
    } else {
        return hit(player);
    }

    return false;
}

bool MO_SledgeBrother::hit(CPlayer * player)
{
    if (player->isShielded())
        return false;

    return player->KillPlayerMapHazard(false, kill_style_environment, false) != player_kill_nonkill;
}

void MO_SledgeBrother::collide(IO_MovingObject * object)
{
    if (object->isDead())
        return;

    MovingObjectType type = object->getMovingObjectType();

    //Ignore hammers and fireballs thrown from sledge brother
    if (type == movingobject_sledgehammer || type == movingobject_superfireball) {
        if (object->iPlayerID == -1)
            return;
    }

    removeifprojectile(object, false, false);

    //These types of attacks damage the boss
    bool fDamageWeapon = type == movingobject_shell || type == movingobject_throwblock || type == movingobject_throwbox || type == movingobject_explosion;

    //These don't damage him but still collide
    bool fNoDamageWeapon = type == movingobject_fireball || type == movingobject_bulletbill || type == movingobject_hammer || type == movingobject_boomerang || type == movingobject_attackzone;

    if (fDamageWeapon || fNoDamageWeapon) {
        //If it is a shell but it is sitting, don't collide
        if (type == movingobject_shell && object->GetState() == 2)
            return;

        //If it is a throw box but is sitting, dont' collide
        if (type == movingobject_throwbox && !((CO_ThrowBox*)object)->HasKillVelocity())
            return;

        if (fDamageWeapon) {
            Damage(object->iPlayerID);

            if (type == movingobject_shell || type == movingobject_throwblock || type == movingobject_throwbox || type == movingobject_attackzone) {
                ifSoundOnPlay(rm->sfx_kicksound);
                object->Die();
            }
        } else {
            ifSoundOnPlay(rm->sfx_hit);

            if (type == movingobject_attackzone || type == movingobject_bulletbill)
                object->Die();
        }
    }
}

void MO_SledgeBrother::Die()
{
    dead = true;
    eyecandy[2].add(new EC_FallingObject(&rm->spr_sledgebrothersdead, ix, iy, 0.0f, -VELJUMP / 2.0f, 1, 0, 0, iType * 64, iw, ih));
}

void MO_SledgeBrother::Damage(short playerID)
{
    if (hit_timer != 0 || playerID == -1)
        return;

    //Find the player that shot this projectile so we can attribute a kill
    CPlayer * killer = GetPlayerFromGlobalID(playerID);

    if (killer) {
        if (--hit_points <= 0) {
            Die();

            if (game_values.gamemode->gamemode == game_mode_boss_minigame) {
                ((CGM_Boss_MiniGame*)game_values.gamemode)->SetWinner(killer);
            }
        } else {
            hit_timer = 60;
            ifSoundOnPlay(rm->sfx_stun);
        }
    }
}

void MO_SledgeBrother::SetLastAction(short type)
{
    if (last_action != type) {
        last_action_count = 1;
        last_action = type;
    } else {
        last_action_count++;
    }
}

//------------------------------------------------------------------------------
// class shell projectile
//------------------------------------------------------------------------------
//state 0: Shell is being spawned
//state 1: Shell is moving
//state 2: Shell is waiting to be picked up
//state 3: Shell is being held
CO_Shell::CO_Shell(short type, short x, short y, bool dieOnMovingPlayerCollision, bool dieOnHoldingPlayerCollision, bool dieOnFire, bool killBouncePlayer) :
    MO_CarriedObject(&rm->spr_shell, x, y, 4, 4, 30, 20, 1, 11, 0, type * 32, 32, 32)
{
    iShellType = type;

    state = 0;

    movingObjectType = movingobject_shell;

    iPlayerID = -1;
    iTeamID = -1;

    iIgnoreBounceTimer = 0;
    iBounceCounter = 0;

    fDieOnMovingPlayerCollision = dieOnMovingPlayerCollision;
    fDieOnHoldingPlayerCollision = dieOnHoldingPlayerCollision;
    fDieOnFire = dieOnFire;
    fKillBouncePlayer = killBouncePlayer;

    iDeathTime = 0;

    iDestY = iy - collisionHeight;
    fy = iDestY + 32.0f;
    iColorOffsetY = type * 32;

    iKillCounter = 0;
    iNoOwnerKillTime = 0;

    fFlipped = false;
    iFlippedOffset = 0;

    iOwnerRightOffset = 14;
    iOwnerLeftOffset = -22;
    iOwnerUpOffset = 32;

    frozen = false;
    frozentimer = 0;
    frozenvelocity = 0.0f;
    frozenanimationspeed = 4;

    sSpotlight = NULL;
}

bool CO_Shell::collide(CPlayer * player)
{
    if (player->isInvincible() || player->shyguy || frozen) {
        if (frozen) {
            ShatterDie();
            return false;
        } else {
            if (state == 0 || state == 2) { //If sitting or spawning then just die
                Die();
                return false;
            } else if (state == 3) { //if held, but not by us then die
                if (owner != player) {
                    Die();
                    return false;
                }
            } else if (state == 1) { //If moving, see if it is actually hitting us before we kill it
                short flipx = 0;

                if (player->ix + PW < 320 && ix > 320)
                    flipx = smw->ScreenWidth;
                else if (ix + iw < 320 && player->ix > 320)
                    flipx = -smw->ScreenWidth;

                if ((player->ix + HALFPW + flipx >= ix + (iw >> 1) && velx > 0.0f) || (player->ix + HALFPW + flipx < ix + (iw >> 1) && velx < 0.0f)) {
                    Die();
                    return false;
                }
            }
        }
    }

    if (player->tanookisuit.notStatue()) {
        if (player->fOldY + PH <= iy && player->iy + PH >= iy)
            return HitTop(player);
        else
            return HitOther(player);
    }

    return false;
}

bool CO_Shell::HitTop(CPlayer * player)
{
    if (player->isInvincible() || player->kuriboshoe.is_on() || player->shyguy) {
        Die();
        fSmoking = false;
        return false;
    }

    if (fKillBouncePlayer && !fFlipped) {
        KillPlayer(player);
    } else if (state == 2) { //Sitting
        owner = player;
        Kick();
        fSmoking = false;
        if (player->ix + HALFPW < ix + (iw >> 1))
            velx = 5.0f;
        else
            velx = -5.0f;

        iIgnoreBounceTimer = 10;
    } else if (state == 1 && iIgnoreBounceTimer == 0) { //Moving
        Stop();

        player->setYi(iy - PH - 1);
        player->bouncejump();
        player->collisions.checktop(*player);
        player->platform = NULL;
    } else if (state == 3) { //Holding
        if (player != owner && (game_values.teamcollision == 2|| player->teamID != owner->teamID)) {
            if (owner)
                owner->carriedItem = NULL;

            Kick();
            fSmoking = false;

            player->setYi(iy - PH - 1);
            player->bouncejump();
            player->collisions.checktop(*player);
            player->platform = NULL;
        }
    }

    return false;
}

bool CO_Shell::HitOther(CPlayer * player)
{
    if (state == 2) { //Sitting
        if (owner == NULL && player->isready()) {
            if (player->AcceptItem(this)) {
                owner = player;
                iPlayerID = owner->globalID;
                iTeamID = owner->teamID;
                state = 3;
            } else {
                short flipx = 0;

                if (player->ix + PW < 320 && ix > 320)
                    flipx = smw->ScreenWidth;
                else if (ix + iw < 320 && player->ix > 320)
                    flipx = -smw->ScreenWidth;

                owner = player;
                Kick();
                if (player->ix + HALFPW + flipx < ix + (iw >> 1))
                    velx = 5.0f;
                else
                    velx = -5.0f;
            }
        }
    } else if (state == 1) { //Moving
        short flipx = 0;

        if (player->ix + PW < 320 && ix > 320)
            flipx = smw->ScreenWidth;
        else if (ix + iw < 320 && player->ix > 320)
            flipx = -smw->ScreenWidth;

        if (iNoOwnerKillTime == 0 || player->globalID != iPlayerID || (player->ix + HALFPW + flipx >= ix + (iw >> 1) && velx > 0.0f) || (player->ix + HALFPW + flipx < ix + (iw >> 1) && velx < 0.0f))
            return KillPlayer(player);
    } else if (state == 3) { //Holding
        if (player != owner && (game_values.teamcollision == 2 || player->teamID != owner->teamID)) {
            iPlayerID = owner->globalID;
            iTeamID = owner->teamID;
            return KillPlayer(player);
        }
    }

    return false;
}

void CO_Shell::UsedAsStoredPowerup(CPlayer * player)
{
    owner = player;
    MoveToOwner();

    if (player->AcceptItem(this))
        state = 3;
    else
        Kick();
}

bool CO_Shell::KillPlayer(CPlayer * player)
{
    if (player->isShielded() || player->isInvincible() || player->shyguy)
        return false;

    CheckAndDie();

    //Find the player that shot this shell so we can attribute a kill
    PlayerKilledPlayer(iPlayerID, player, death_style_jump, kill_style_shell, false, false);

    CPlayer * killer = GetPlayerFromGlobalID(iPlayerID);
    if (killer && iPlayerID != player->globalID) {
        AddMovingKill(killer);
    }

    return true;
}

void CO_Shell::AddMovingKill(CPlayer * killer)
{
    if (state == 1 && game_values.awardstyle != award_style_none) { //If the shell is moving, the keep track of how many people we kill in a row with it
        if (++iKillCounter > 1)
            killer->AddKillsInRowInAirAward();
    }
}

void CO_Shell::collide(IO_MovingObject * object)
{
    if (object->isDead())
        return;

    //Don't allow shells to die if they are warping
    if (owner && owner->iswarping())
        return;

    removeifprojectile(object, false, false);

    MovingObjectType type = object->getMovingObjectType();

    if (type == movingobject_shell) {
        CO_Shell * shell = (CO_Shell*)object;

        //Green shells should die on collision, other shells should not,
        //except if they also hit a non dead on collision shell

        if (frozen || shell->frozen) {
            Die();
            shell->Die();
        }
        if (shell->fSmoking && !fSmoking) {
            Die();
        } else if (!shell->fSmoking && fSmoking) {
            shell->Die();
        } else {
            if (fDieOnMovingPlayerCollision || state == 2 || (!shell->fDieOnMovingPlayerCollision && shell->state != 2))
                Die();

            if (shell->fDieOnMovingPlayerCollision || shell->state == 2 || (!fDieOnMovingPlayerCollision && state != 2))
                shell->Die();
        }
    } else if (type == movingobject_throwblock) {
        CO_ThrowBlock * block = (CO_ThrowBlock*)object;

        Die();
        block->Die();
    } else if (type == movingobject_throwbox) {
        CO_ThrowBox * box = (CO_ThrowBox*)object;

        Die();
        box->Die();
    } else if (type == movingobject_explosion || type == movingobject_sledgehammer || type == movingobject_superfireball) {
        Die();
    } else if (type == movingobject_fireball || type == movingobject_hammer || type == movingobject_boomerang) {
        if (fDieOnFire)
            Die();
    } else if (type == movingobject_iceblast) {
        frozenvelocity = velx;
        velx = 0.0f;
        animationspeed = 0;

        frozen = true;
        frozentimer = 300;

        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix - collisionOffsetX, iy - collisionOffsetY, 3, 8));
    }
}

void CO_Shell::update()
{
    if (iNoOwnerKillTime > 0)
        iNoOwnerKillTime--;

    if (frozen) {
        if (--frozentimer <= 0) {
            frozentimer = 0;
            frozen = false;

            velx = frozenvelocity;
            animationspeed = frozenanimationspeed;

            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix - collisionOffsetX, iy - collisionOffsetY, 3, 8));
        }
    } else {
        if (state == 1) {
            if (game_values.shellttl > 0 && ++iDeathTime >= game_values.shellttl) {
                eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix, iy, 3, 8));
                dead = true;
                ifSoundOnPlay(rm->sfx_kicksound);

                if (owner) {
                    owner->carriedItem = NULL;
                    owner = NULL;
                }

                return;
            }
        } else {
            iDeathTime = 0;
        }
    }

    //Have the powerup grow out of the powerup block
    if (state == 0) {
        setYf(fy - 2.0f);

        if (fy <= iDestY) {
            state = 2;
            vely = GRAVITATION;
            setYf(iDestY);
        }

        return;
    }

    if (iIgnoreBounceTimer > 0)
        iIgnoreBounceTimer--;

    if (iBounceCounter > 0)
        iBounceCounter--;

    if (owner) {
        MoveToOwner();
        inair = true;
    } else {
        IO_MovingObject::update();
    }

    if (game_values.spotlights) {
        if (state == 1) {
            if (!sSpotlight) {
                sSpotlight = spotlightManager.AddSpotlight(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1), 3);
            }

            if (sSpotlight) {
                sSpotlight->UpdatePosition(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1));
            }
        } else {
            sSpotlight = NULL;
        }
    }
}

void CO_Shell::draw()
{
    if (state == 0) {
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, iColorOffsetY, iw, (short)(ih - fy + iDestY));
    } else if (owner) {
        if (owner->iswarping())
            spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, iColorOffsetY + iFlippedOffset, iw, ih, owner->GetWarpState(), owner->GetWarpPlane());
        else
            spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, iColorOffsetY + iFlippedOffset, iw, ih);
    } else {
        if (state == 2)
            spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, iColorOffsetY + iFlippedOffset, iw, ih);
        else if (state == 1)
            spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iColorOffsetY + iFlippedOffset, iw, ih);
    }

    if (frozen) {
        rm->spr_iceblock.draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, 0, 32, 32);
    } else if (fSmoking) {
        eyecandy[0].add(new EC_SingleAnimation(&rm->spr_burnup, ix - collisionOffsetX + (iw >> 1) - 16, iy - collisionOffsetY + (ih >> 1) - 16, 5, 3));
    }
}

void CO_Shell::Drop()
{
    if (owner) {
        owner->carriedItem = NULL;
        setXi(owner->ix + (owner->isFacingRight() ? PW + 1: -31));
    }

    if (collision_detection_checksides()) {
        //Move back to where it was before checking sides, then kill it
        setXi(owner->ix + (owner->isFacingRight() ? PW + 1: -31));
        setYi(owner->iy + PH - 32 + collisionOffsetY);
        Die();
    } else {
        owner = NULL;
        state = 2;
    }
}

void CO_Shell::Kick()
{
    /*
    if (superkick)
    {
    	vel = 10.0f;
    	fSmoking = true;
    	ifSoundOnPlay(rm->sfx_cannon);
    }
    */

    float fVel, fPlayerBonusVel = owner->velx / 2.0f;
    if (owner->isFacingRight()) {
        fVel = 5.0f;
        if (fPlayerBonusVel > 0.0f)
            fVel += fPlayerBonusVel;

        //if (fVel >= 7.5f)
        //	fSmoking = true;
    } else {
        fVel = -5.0f;
        if (fPlayerBonusVel < 0.0f)
            fVel += fPlayerBonusVel;

        //if (fVel <= -7.5f)
        //	fSmoking = true;
    }

    velx = fVel;
    vely = 0.0f;

    iPlayerID = owner->globalID;
    iTeamID = owner->teamID;

    owner = NULL;
    iNoOwnerKillTime = 30;

    state = 1;

    if (collision_detection_checksides())
        Die();
    else
        ifSoundOnPlay(rm->sfx_kicksound);
}

void CO_Shell::CheckAndDie()
{
    if ((fDieOnMovingPlayerCollision && state == 1) || ((fDieOnHoldingPlayerCollision || fFlipped)&& state == 3))
        Die();
    else if (!fDieOnHoldingPlayerCollision && state == 3 && (RANDOM_INT(5)) == 0)
        Die();
}

void CO_Shell::Die()
{
    if (frozen) {
        ShatterDie();
        return;
    }

    eyecandy[2].add(new EC_FallingObject(&rm->spr_shelldead, ix, iy, -velx / 4.0f, -VELJUMP / 2.0f, 1, 0, iShellType * 32, 0, 32, 32));
    dead = true;
    ifSoundOnPlay(rm->sfx_kicksound);
    iKillCounter = 0;

    if (owner) {
        owner->carriedItem = NULL;
        owner = NULL;
    }
}

void CO_Shell::ShatterDie()
{
    dead = true;
    ifSoundOnPlay(rm->sfx_breakblock);
    iKillCounter = 0;

    if (owner) {
        owner->carriedItem = NULL;
        owner = NULL;
    }

    short iBrokenIceX = ix - collisionOffsetX, iBrokenIceY = iy - collisionOffsetY;
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, iBrokenIceX, iBrokenIceY, -1.5f, -7.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, iBrokenIceX + 16, iBrokenIceY, 1.5f, -7.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, iBrokenIceX, iBrokenIceY + 16, -1.5f, -4.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, iBrokenIceX + 16, iBrokenIceY + 16, 1.5f, -4.0f, 4, 2, 0, 0, 16, 16));
}

void CO_Shell::SideBounce(bool fRightSide)
{
    if (state == 1) {
        if (iBounceCounter == 0) {
            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_shellbounce, ix + (velx > 0 ? 0 : collisionWidth) - 21, iy + (collisionHeight >> 1) - 20, 4, 4));
            ifSoundOnPlay(rm->sfx_bump);

            iBounceCounter = 7; //Allow bounce stars to show on each bounce on a 2x wide pit
        }
    }
}

void CO_Shell::Flip()
{
    if (frozen) {
        ShatterDie();
        return;
    }

    if (owner) {
        Die();
        return;
    }

    if (!fFlipped) {
        fFlipped = true;
        iFlippedOffset = 128;
    }

    Stop();
    vely = -VELJUMP / 2.0;
}

void CO_Shell::Stop()
{
    owner = NULL;
    velx = 0.0f;
    state = 2;
    fSmoking = false;
    ifSoundOnPlay(rm->sfx_kicksound);
    iKillCounter = 0;
}

void CO_Shell::nospawn(short y, bool fBounce)
{
    state = 2;
    setYi(y);

    if (fBounce)
        vely = -VELJUMP / 2.0;
}

//------------------------------------------------------------------------------
// class throwable block projectile
//------------------------------------------------------------------------------
//State 1: Moving
//State 2: Holding
CO_ThrowBlock::CO_ThrowBlock(gfxSprite * nspr, short x, short y, short type) :
    MO_CarriedObject(nspr, x, y, 4, 2, 30, 30, 1, 1)
{
    state = 2;
    ih = 32;
    movingObjectType = movingobject_throwblock;
    iPlayerID = -1;
    iTeamID = -1;

    fDieOnBounce = type != 2;
    fDieOnPlayerCollision = type == 0;

    iType = type;

    iDeathTime = 0;
    iBounceCounter = 0;
    iNoOwnerKillTime = 0;

    iOwnerRightOffset = 14;
    iOwnerLeftOffset = -22;
    iOwnerUpOffset = 32;

    frozen = false;
    frozentimer = 0;
    frozenvelocity = 0.0f;
    frozenanimationspeed = 2;

    sSpotlight = NULL;
}

bool CO_ThrowBlock::collide(CPlayer * player)
{
    if (frozen) {
        ShatterDie();
        return false;
    }

    if (player->fOldY + PH <= iy && player->iy + PH >= iy)
        return HitTop(player);
    else
        return HitOther(player);
}

bool CO_ThrowBlock::HitTop(CPlayer * player)
{
    if (player->isInvincible() || player->shyguy) {
        Die();
    } else {
        if (state == 1) { //moving
            return KillPlayer(player);
        } else if (state == 2) { //Holding
            if (player != owner) {
                if (owner)
                    owner->carriedItem = NULL;

                Kick();

                player->setYi(iy - PH - 1);
                player->bouncejump();
                player->collisions.checktop(*player);
                player->platform = NULL;
            }
        }
    }

    return false;
}

bool CO_ThrowBlock::HitOther(CPlayer * player)
{
    if (state == 1) { //Moving
        short flipx = 0;

        if (player->ix + PW < 320 && ix > 320)
            flipx = smw->ScreenWidth;
        else if (ix + iw < 320 && player->ix > 320)
            flipx = -smw->ScreenWidth;

        if (iNoOwnerKillTime == 0 || player->globalID != iPlayerID || (player->ix + flipx > ix + (iw >> 1) && velx > 0.0f) || (player->ix + flipx <= ix - (iw >> 1) && velx < 0.0f)) {
            return KillPlayer(player);
        }
    } else if (state == 2) { //Holding
        if (player != owner) {
            iPlayerID = owner->globalID;
            iTeamID = owner->teamID;
            return KillPlayer(player);
        }
    }

    return false;
}

bool CO_ThrowBlock::KillPlayer(CPlayer * player)
{
    if (player->isInvincible() || player->shyguy) {
        Die();
        return false;
    }

    if (player->isShielded())
        return false;

    CheckAndDie();

    //Find the player that shot this shell so we can attribute a kill
    PlayerKilledPlayer(iPlayerID, player, death_style_jump, kill_style_throwblock, false, false);
    return true;
}

void CO_ThrowBlock::collide(IO_MovingObject * object)
{
    if (object->isDead())
        return;

    removeifprojectile(object, false, false);

    MovingObjectType type = object->getMovingObjectType();

    if (type == movingobject_throwblock) {
        CO_ThrowBlock * block = (CO_ThrowBlock*)object;

        Die();
        block->Die();
    } else if (type == movingobject_throwbox) {
        CO_ThrowBox * box = (CO_ThrowBox*)object;

        Die();
        box->Die();
    } else if (type == movingobject_explosion) {
        Die();
    } else if (type == movingobject_iceblast) {
        frozenvelocity = velx;
        velx = 0.0f;
        animationspeed = 0;

        frozen = true;
        frozentimer = 300;

        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix - collisionOffsetX, iy - collisionOffsetY, 3, 8));
    }
}

void CO_ThrowBlock::update()
{
    if (iNoOwnerKillTime > 0)
        iNoOwnerKillTime--;

    if (frozen) {
        if (--frozentimer <= 0) {
            frozentimer = 0;
            frozen = false;

            velx = frozenvelocity;
            animationspeed = frozenanimationspeed;

            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix - collisionOffsetX, iy - collisionOffsetY, 3, 8));
        }
    } else if (game_values.blueblockttl > 0 && ++iDeathTime >= game_values.blueblockttl) {
        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix, iy, 3, 8));
        dead = true;

        if (owner) {
            owner->carriedItem = NULL;
            owner = NULL;
        }

        return;
    }

    if (owner) {
        MoveToOwner();
        inair = true;
    } else {
        if (iBounceCounter > 0)
            iBounceCounter--;

        fOldX = fx;
        fOldY = fy;

        collision_detection_map();
    }

    animate();

    if (game_values.spotlights && state == 1) {
        if (!sSpotlight) {
            sSpotlight = spotlightManager.AddSpotlight(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1), 3);
        }

        if (sSpotlight) {
            sSpotlight->UpdatePosition(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1));
        }
    }
}

void CO_ThrowBlock::draw()
{
    if (owner && owner->iswarping())
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iType << 5, iw, ih, owner->GetWarpState(), owner->GetWarpPlane());
    else
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iType << 5, iw, ih);

    if (frozen) {
        rm->spr_iceblock.draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, 0, 32, 32);
    } else if (fSmoking) {
        eyecandy[0].add(new EC_SingleAnimation(&rm->spr_burnup, ix - collisionOffsetX + (iw >> 1) - 16, iy - collisionOffsetY + (ih >> 1) - 16, 5, 3));
    }
}

void CO_ThrowBlock::Drop()
{
    Kick();
}

void CO_ThrowBlock::Kick()
{
    /*
    if (superkick)
    {
    	vel = 12.0f;
    	fSmoking = true;
    	ifSoundOnPlay(rm->sfx_cannon);
    }
    */

    iDeathTime = 0;

    float fVel, fPlayerBonusVel = owner->velx / 2.0f;
    if (owner->isFacingRight()) {
        fVel = 6.5f;
        if (fPlayerBonusVel > 0.0f)
            fVel += fPlayerBonusVel;

        //if (fVel >= 9.0f)
        //	fSmoking = true;
    } else {
        fVel = -6.5f;
        if (fPlayerBonusVel < 0.0f)
            fVel += fPlayerBonusVel;

        //if (fVel <= -9.0f)
        //	fSmoking = true;
    }

    velx = fVel;
    vely = 0.0f;

    iPlayerID = owner->globalID;
    iTeamID = owner->teamID;

    owner = NULL;
    iNoOwnerKillTime = 30;

    state = 1;

    if (collision_detection_checksides())
        Die();
    else
        ifSoundOnPlay(rm->sfx_kicksound);
}

void CO_ThrowBlock::CheckAndDie()
{
    if (fDieOnPlayerCollision)
        Die();
}

void CO_ThrowBlock::Die()
{
    if (frozen) {
        ShatterDie();
        return;
    }

    if (dead)
        return;

    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokenblueblock, ix, iy, -1.5f, -7.0f, 6, 2, 0, iType << 4, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokenblueblock, ix + 16, iy, 1.5f, -7.0f, 6, 2, 0, iType << 4, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokenblueblock, ix, iy + 16, -1.5f, -4.0f, 6, 2, 0, iType << 4, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokenblueblock, ix + 16, iy + 16, 1.5f, -4.0f, 6, 2, 0, iType << 4, 16, 16));

    DieHelper();
}

void CO_ThrowBlock::ShatterDie()
{
    if (dead)
        return;

    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, ix, iy, -1.5f, -7.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, ix + 16, iy, 1.5f, -7.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, ix, iy + 16, -1.5f, -4.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, ix + 16, iy + 16, 1.5f, -4.0f, 4, 2, 0, 0, 16, 16));

    game_values.unlocksecret2part2++;

    DieHelper();
}

void CO_ThrowBlock::DieHelper()
{
    dead = true;
    ifSoundOnPlay(rm->sfx_breakblock);

    if (owner) {
        owner->carriedItem = NULL;
        owner = NULL;
    }
}

void CO_ThrowBlock::SideBounce(bool fRightSide)
{
    if (fDieOnBounce) {
        Die();
    } else if (state == 1) {
        if (iBounceCounter == 0) {
            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_shellbounce, ix + (velx > 0 ? 0 : collisionWidth) - 21, iy + (collisionHeight >> 1) - 20, 4, 4));
            ifSoundOnPlay(rm->sfx_bump);

            iBounceCounter = 7; //Allow bounce stars to show on each bounce on a 2x wide pit
        }
    }
}

//------------------------------------------------------------------------------
// class throwable box - can be used as a shield, thrown at a player, or holds items
//------------------------------------------------------------------------------
CO_ThrowBox::CO_ThrowBox(gfxSprite * nspr, short x, short y, short item) :
    MO_CarriedObject(nspr, x, y, 4, 8, 30, 30, 1, 1)
{
    state = 1;
    ih = 32;
    iw = 32;

    movingObjectType = movingobject_throwbox;

    iPlayerID = -1;
    iTeamID = -1;

    iItem = item;

    iOwnerRightOffset = 14;
    iOwnerLeftOffset = -22;
    iOwnerUpOffset = 32;

    frozen = false;
    frozentimer = 0;
    frozenanimationspeed = 8;

    sSpotlight = NULL;
}

bool CO_ThrowBox::collide(CPlayer * player)
{
    //Kill the player if it is moving

    if (frozen) {
        ShatterDie();
        return false;
    }

    if (HasKillVelocity()) {
        short flipx = 0;

        if (player->ix + PW < 320 && ix > 320)
            flipx = smw->ScreenWidth;
        else if (ix + iw < 320 && player->ix > 320)
            flipx = -smw->ScreenWidth;

        if (player->globalID != iPlayerID) {
            return KillPlayer(player);
        }
    }
    /*
    //Kill player when another player is holding the box
    else
    {
        if (owner && player != owner && (game_values.teamcollision == 2 || player->teamID != owner->teamID))
    	{
    		iPlayerID = owner->globalID;
    		iTeamID = owner->teamID;
    		return KillPlayer(player);
    	}
    }*/

    //Otherwise allow them to pick this box up
    if (owner == NULL && player->isready()) {
        if (player->AcceptItem(this)) {
            owner = player;
        }
    }

    return false;
}


bool CO_ThrowBox::KillPlayer(CPlayer * player)
{
    if (player->isInvincible() || player->shyguy) {
        Die();
        return false;
    }

    if (player->isShielded())
        return false;

    Die();

    //Find the player that shot this shell so we can attribute a kill
    PlayerKilledPlayer(iPlayerID, player, death_style_jump, kill_style_throwblock, false, false);
    return true;
}

void CO_ThrowBox::collide(IO_MovingObject * object)
{
    if (object->isDead())
        return;

    removeifprojectile(object, false, false);

    MovingObjectType type = object->getMovingObjectType();

    if (type == movingobject_throwbox) {
        CO_ThrowBox * box = (CO_ThrowBox*)object;
        if (frozen || box->frozen || HasKillVelocity() || box->HasKillVelocity()) {
            Die();
            box->Die();
        }
    } else if (type == movingobject_explosion || type == movingobject_fireball || type == movingobject_hammer || type == movingobject_boomerang || type == movingobject_superfireball || type == movingobject_sledgehammer) {
        Die();
    } else if (type == movingobject_iceblast) {
        velx = 0.0f;
        animationspeed = 0;

        frozen = true;
        frozentimer = 300;

        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix - collisionOffsetX, iy - collisionOffsetY, 3, 8));
    }
}

void CO_ThrowBox::update()
{
    if (frozen) {
        if (--frozentimer <= 0) {
            frozentimer = 0;
            frozen = false;

            animationspeed = frozenanimationspeed;

            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix - collisionOffsetX, iy - collisionOffsetY, 3, 8));
        }
    }

    fOldX = fx;
    fOldY = fy;

    if (owner) {
        MoveToOwner();
        inair = true;
    } else {
        applyfriction();
        collision_detection_map();
    }

    animate();

    if (game_values.spotlights && HasKillVelocity()) {
        if (!sSpotlight) {
            sSpotlight = spotlightManager.AddSpotlight(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1), 3);
        }

        if (sSpotlight) {
            sSpotlight->UpdatePosition(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1));
        }
    }
}

void CO_ThrowBox::draw()
{
    if (owner && owner->iswarping())
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, 0, iw, ih, owner->GetWarpState(), owner->GetWarpPlane());
    else
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, 0, iw, ih);

    if (frozen) {
        rm->spr_iceblock.draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, 0, 32, 32);
    }
}

void CO_ThrowBox::Drop()
{
    if (owner) {
        iPlayerID = owner->globalID;
        iTeamID = owner->teamID;
    }

    if (collision_detection_checksides())
        Die();
    else
        MO_CarriedObject::Drop();
}

void CO_ThrowBox::Kick()
{
    if (owner) {
        iPlayerID = owner->globalID;
        iTeamID = owner->teamID;
    }

    if (collision_detection_checksides())
        Die();
    else
        MO_CarriedObject::Kick();
}

void CO_ThrowBox::Die()
{
    if (frozen) {
        ShatterDie();
        return;
    }

    if (dead)
        return;

    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokenyellowblock, ix, iy, -2.2f, -10.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokenyellowblock, ix + 16, iy, 2.2f, -10.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokenyellowblock, ix, iy + 16, -2.2f, -5.5f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokenyellowblock, ix + 16, iy + 16, 2.2f, -5.5f, 4, 2, 0, 0, 16, 16));

    DieHelper();
}

void CO_ThrowBox::ShatterDie()
{
    if (dead)
        return;

    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, ix, iy, -1.5f, -7.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, ix + 16, iy, 1.5f, -7.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, ix, iy + 16, -1.5f, -4.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, ix + 16, iy + 16, 1.5f, -4.0f, 4, 2, 0, 0, 16, 16));

    game_values.unlocksecret2part2++;

    DieHelper();
}

void CO_ThrowBox::DieHelper()
{
    dead = true;
    ifSoundOnPlay(rm->sfx_breakblock);

    if (owner) {
        owner->carriedItem = NULL;
        owner = NULL;
    }

    //Check to see if we should spawn an item here
    if (iItem != NO_POWERUP) {
        createpowerup(iItem, ix, iy, velx < 0.0f, false);
    }
}

void CO_ThrowBox::SideBounce(bool fRightSide)
{
    if (dead)
        return;

    if (HasKillVelocity()) {
        if (frozen)
            ShatterDie();
        else
            Die();
    }
}

float CO_ThrowBox::BottomBounce()
{
    if (dead)
        return bounce;

    if (HasKillVelocity()) {
        if (frozen)
            ShatterDie();
        else
            Die();
    }

    return bounce;
}

bool CO_ThrowBox::HasKillVelocity()
{
    return velx < -0.01f || velx > 0.01f || vely < -0.01f || vely > 2.0f;
}

//------------------------------------------------------------------------------
// class spring
//------------------------------------------------------------------------------
CO_Spring::CO_Spring(gfxSprite *nspr, short iX, short iY, bool fsuper) :
    MO_CarriedObject(nspr, iX, iY, 4, 4, 30, 31, 1, 0)
{
    fSuper = fsuper;
    iOffsetY = fSuper ? 32 : 0;

    state = 1;
    movingObjectType = movingobject_carried;

    iOwnerRightOffset = 14;
    iOwnerLeftOffset = -22;
    iOwnerUpOffset = 32;
}

bool CO_Spring::collide(CPlayer * player)
{
    if (owner == NULL) {
        if (player->fOldY + PH <= fOldY && player->iy + PH >= iy)
            hittop(player);
        else if (state == 1)
            hitother(player);
    }

    return false;
}

void CO_Spring::hittop(CPlayer * player)
{
    state = 2;
    drawframe += iw;

    player->setYi(iy - PH - 1);
    player->collisions.checktop(*player);
    player->platform = NULL;
    player->inair = false;
    player->fallthrough = false;
    player->killsinrowinair = 0;
    player->extrajumps = 0;

    player->superjumptimer = 4;
    player->superjumptype = fSuper ? 2 : 1;
    player->vely = -VELNOTEBLOCKREPEL;

    ifSoundOnPlay(rm->sfx_bump);
}

void CO_Spring::hitother(CPlayer * player)
{
    if (owner == NULL && player->isready()) {
        if (player->AcceptItem(this)) {
            owner = player;
        }
    }
}

void CO_Spring::update()
{
    if (owner) {
        MoveToOwner();
    } else {
        applyfriction();

        //Collision detect map
        fOldX = fx;
        fOldY = fy;

        collision_detection_map();
    }

    if (state == 2) {
        if (++animationtimer == animationspeed) {
            animationtimer = 0;

            drawframe += iw;
            if (drawframe >= animationWidth) {
                drawframe = 0;
                state = 1;
            }
        }
    }
}

void CO_Spring::draw()
{
    if (owner) {
        if (owner->iswarping())
            spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, animationOffsetX, iOffsetY, 32, 32, owner->GetWarpState(), owner->GetWarpPlane());
        else
            spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, animationOffsetX, iOffsetY, 32, 32);
    } else {
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, animationOffsetX + drawframe, iOffsetY, 32, 32);
    }
}

void CO_Spring::place()
{
    short iAttempts = 10;
    while (!g_map->findspawnpoint(5, &ix, &iy, collisionWidth, collisionHeight, false) && iAttempts-- > 0);
    fx = (float)ix;
    fy = (float)iy;

    Drop();
}


//------------------------------------------------------------------------------
// class spike
//------------------------------------------------------------------------------
CO_Spike::CO_Spike(gfxSprite *nspr, short iX, short iY) :
    CO_Spring(nspr, iX, iY, false)
{
    iw = 32;
    ih = 32;

    movingObjectType = movingobject_carried;
}

void CO_Spike::hittop(CPlayer * player)
{
    if (player->isready() && !player->isShielded() && !player->isInvincible() && !player->kuriboshoe.is_on() && !player->shyguy)
        player->KillPlayerMapHazard(false, kill_style_environment, false);
}


//------------------------------------------------------------------------------
// class kuribo's shoe
//------------------------------------------------------------------------------
CO_KuriboShoe::CO_KuriboShoe(gfxSprite *nspr, short iX, short iY, bool sticky) :
    CO_Spring(nspr, iX, iY + 15, false)
{
    iw = 32;
    ih = 32;

    collisionOffsetY = 15;
    collisionHeight = 16;

    animationOffsetX = sticky ? 64 : 0;

    movingObjectType = movingobject_carried;

    fSticky = sticky;
}

void CO_KuriboShoe::hittop(CPlayer * player)
{
    if (!player->kuriboshoe.is_on() && player->tanookisuit.notStatue()) {
        dead = true;
        player->SetKuriboShoe(fSticky ? STICKY : NORMAL);
        ifSoundOnPlay(rm->sfx_transform);
        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, player->ix + HALFPW - 16, player->iy + HALFPH - 16, 3, 8));
    }
}


//------------------------------------------------------------------------------
// attack zone(invisible area that kills objects and players)
//------------------------------------------------------------------------------
MO_AttackZone::MO_AttackZone(short playerId, short teamId, short x, short y, short w, short h, short time, killstyle style, bool dieoncollision) :
    IO_MovingObject(NULL, x, y, 1, 0, w, h, 0, 0)
{
    iPlayerID = playerId;
    iTeamID = teamId;
    iStyle = style;

    objectType = object_moving;
    movingObjectType = movingobject_attackzone;

    iTimer = time;
    fDieOnCollision = dieoncollision;

    state = 1;

    fObjectCollidesWithMap = false;
}

bool MO_AttackZone::collide(CPlayer * player)
{
    if (player->isShielded() || player->isInvincible() || player->shyguy || dead)
        return false;

    if (game_values.teamcollision != 2 && player->teamID == iTeamID)
        return false;

    CPlayer * killer = GetPlayerFromGlobalID(iPlayerID);

    if (killer && killer->globalID == player->globalID)
        return false;

    PlayerKilledPlayer(iPlayerID, player, death_style_jump, iStyle, false, false);

    Die();

    return true;
}

/*void MO_AttackZone::draw()
{
	if (!dead)
	{
		SDL_Rect r = {ix, iy, collisionWidth, collisionHeight};
		SDL_FillRect(blitdest, &r, 0xf000);
	}
}*/

void MO_AttackZone::update()
{
    if (--iTimer <= 0)
        dead = true;
}

void MO_AttackZone::collide(IO_MovingObject * object)
{
    if (dead)
        return;

    MovingObjectType type = object->getMovingObjectType();

    if (type == movingobject_shell || type == movingobject_throwblock || type == movingobject_throwbox) {
        if (type == movingobject_shell) {
            CO_Shell * shell = (CO_Shell*)object;
            if (shell->frozen)
                shell->ShatterDie();
            else
                shell->Die();
        } else if (type == movingobject_throwblock) {
            CO_ThrowBlock * block = (CO_ThrowBlock*) object;

            if (block->frozen)
                block->ShatterDie();
            else if (!block->owner || block->owner->globalID != iPlayerID)
                block->Die();
        } else if (type == movingobject_throwbox) {
            CO_ThrowBox * box = (CO_ThrowBox*) object;

            if (box->frozen)
                box->ShatterDie();
            else if (!box->owner || box->owner->globalID != iPlayerID)
                box->Die();
        }

        ifSoundOnPlay(rm->sfx_kicksound);
    }
}

void MO_AttackZone::Die()
{
    if (fDieOnCollision)
        dead = true;
}


//------------------------------------------------------------------------------
// class spin death (spinning cape or tail)
//------------------------------------------------------------------------------
MO_SpinAttack::MO_SpinAttack(short playerId, short teamId, killstyle style, bool direction, short offsety) :
    MO_AttackZone(playerId, teamId, 0, 0, 24, 12, 16, style, true)
{
    fDirection = direction;
    iOffsetY = offsety;

    state = 0;
    objectType = object_moving;
}

bool MO_SpinAttack::collide(CPlayer * player)
{
    if (iTimer > 11)
        return false;

    return MO_AttackZone::collide(player);
}

/*
void MO_SpinAttack::draw()
{
	if (iTimer <= 11 && !dead)
	{
		SDL_Rect r = {ix, iy, collisionWidth, collisionHeight};
		SDL_FillRect(blitdest, &r, 0xff00);
	}
}
*/

void MO_SpinAttack::update()
{
    MO_AttackZone::update();

    if (iTimer <= 11)
        state = 1;

    CPlayer * owner = GetPlayerFromGlobalID(iPlayerID);

    if (owner) {
        //Move to the owner
        setXi(owner->ix - PWOFFSET + (fDirection ? 24 : -16));
        setYi(owner->iy + PH - iOffsetY);

        if (iTimer < 5 || iy + collisionHeight < 0)
            return;

        //Check block collisions
        short iTop = iy / TILESIZE;
        short iBottom = (iy + collisionHeight) / TILESIZE;

        short iLeft;
        if (ix < 0)
            iLeft = (ix + smw->ScreenWidth) / TILESIZE;
        else
            iLeft = ix / TILESIZE;

        short iRight = (ix + collisionWidth) / TILESIZE;

        if (iLeft < 0)
            iLeft += 20;

        if (iLeft >= 20)
            iLeft -= 20;

        if (iRight < 0)
            iRight += 20;

        if (iRight >= 20)
            iRight -= 20;

        IO_Block * topleftblock = NULL;
        IO_Block * toprightblock = NULL;
        IO_Block * bottomleftblock = NULL;
        IO_Block * bottomrightblock = NULL;

        if (iTop >= 0 && iTop < 15) {
            topleftblock = g_map->block(iLeft, iTop);
            toprightblock = g_map->block(iRight, iTop);
        }

        if (iBottom >= 0 && iBottom < 15) {
            bottomleftblock = g_map->block(iLeft, iBottom);
            bottomrightblock = g_map->block(iRight, iBottom);
        }

        bool fHitBlock = false;
        if (topleftblock && !topleftblock->isTransparent() && !topleftblock->isHidden())
            fHitBlock = topleftblock->collide(this, 3);

        if (!fHitBlock && toprightblock && !toprightblock->isTransparent() && !toprightblock->isHidden())
            fHitBlock = toprightblock->collide(this, 1);

        if (!fHitBlock && bottomleftblock && !bottomleftblock->isTransparent() && !bottomleftblock->isHidden())
            fHitBlock = bottomleftblock->collide(this, 3);

        if (!fHitBlock && bottomrightblock && !bottomrightblock->isTransparent() && !bottomrightblock->isHidden())
            fHitBlock = bottomrightblock->collide(this, 1);

        if (fHitBlock)
            dead = true;
    } else {
        dead = true;
    }
}

void MO_SpinAttack::collide(IO_MovingObject * object)
{
    if (dead || iTimer > 11)
        return;

    MovingObjectType type = object->getMovingObjectType();

    if (type == movingobject_shell || type == movingobject_throwblock || type == movingobject_throwbox) {
        if (type == movingobject_shell) {
            CO_Shell * shell = (CO_Shell*)object;

            if (shell->frozen) {
                shell->ShatterDie();
            } else {
                shell->Flip();
                ifSoundOnPlay(rm->sfx_kicksound);
            }

            Die();
        } else if (type == movingobject_throwblock) {
            CO_ThrowBlock * block = (CO_ThrowBlock*) object;

            if (block->frozen) {
                block->ShatterDie();
            } else if (!block->owner || block->owner->globalID != iPlayerID) {
                block->Die();
                ifSoundOnPlay(rm->sfx_kicksound);
                Die();
            }
        } else if (type == movingobject_throwbox) {
            CO_ThrowBox * box = (CO_ThrowBox*) object;

            if (box->frozen) {
                box->ShatterDie();
            } else if (!box->owner || box->owner->globalID != iPlayerID) {
                box->Die();
                ifSoundOnPlay(rm->sfx_kicksound);
                Die();
            }
        }
    }
}


//------------------------------------------------------------------------------
// class pipe coin (for coin pipe minigame)
//------------------------------------------------------------------------------
OMO_PipeCoin::OMO_PipeCoin(gfxSprite *nspr, float dvelx, float dvely, short iX, short iY, short teamid, short colorid, short uncollectabletime) :
    IO_OverMapObject(nspr, iX, iY, 4, 8, 30, 30, 1, 1, 0, colorid << 5, 32, 32)
{
    iTeamID = teamid;
    iColorID = colorid;
    state = 1;
    objectType = object_pipe_coin;

    sparkleanimationtimer = 0;
    sparkledrawframe = 0;

    velx = dvelx;

    if (pipegamemode->IsSlowdown())
        vely = dvely / 2.0f;
    else
        vely = dvely;

    iUncollectableTime = uncollectabletime;
}

bool OMO_PipeCoin::collide(CPlayer * player)
{
    if (iUncollectableTime > 0)
        return false;

    if (!game_values.gamemode->gameover) {
        if (iTeamID != -1) {
            if (player->teamID == iTeamID) {
                player->Score().AdjustScore(1);
                ifSoundOnPlay(rm->sfx_coin);
            }
        } else {
            if (iColorID == 2) {
                player->Score().AdjustScore(1);
                ifSoundOnPlay(rm->sfx_coin);
            } else if (iColorID == 0) {
                player->Score().AdjustScore(-1);
                ifSoundOnPlay(rm->sfx_stun);
            } else if (iColorID == 1) {
                player->Score().AdjustScore(5);
                ifSoundOnPlay(rm->sfx_extraguysound);
            }
        }

        game_values.gamemode->CheckWinner(player);
    }

    eyecandy[2].add(new EC_SingleAnimation(&rm->spr_coinsparkle, ix, iy, 7, 4));

    dead = true;
    return false;
}

void OMO_PipeCoin::update()
{
    setXf(fx + velx);
    setYf(fy + vely);

    if (iTeamID == -1)
        animate();

    if (iy >= smw->ScreenHeight)
        dead = true;

    if (pipegamemode->IsSlowdown())
        vely += GRAVITATION / 1.5f;
    else
        vely += GRAVITATION;

    if (++sparkleanimationtimer >= 4) {
        sparkleanimationtimer = 0;
        sparkledrawframe += 32;
        if (sparkledrawframe >= smw->ScreenHeight)
            sparkledrawframe = 0;
    }

    if (iUncollectableTime > 0)
        --iUncollectableTime;
}

void OMO_PipeCoin::draw()
{
    if (iUncollectableTime > 0) {
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY, iw, ih, 2, 256);

        //Draw sparkles
        if (iTeamID == -1)
            rm->spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, 32, 2, 256);

    } else {
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY, iw, ih);

        //Draw sparkles
        if (iTeamID == -1)
            rm->spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, 32);
    }
}

//------------------------------------------------------------------------------
// class pipe powerup (for coin pipe minigame)
//------------------------------------------------------------------------------
OMO_PipeBonus::OMO_PipeBonus(gfxSprite *nspr, float dvelx, float dvely, short iX, short iY, short type, short duration, short uncollectabletime) :
    IO_OverMapObject(nspr, iX, iY, 4, 8, 30, 30, 1, 1, 0, type << 5, 32, 32)
{
    iType = type;
    iDuration = duration;
    state = 1;
    objectType = object_pipe_bonus;

    velx = dvelx;

    if (pipegamemode->IsSlowdown())
        vely = dvely / 1.5f;
    else
        vely = dvely;

    iUncollectableTime = uncollectabletime;
}

bool OMO_PipeBonus::collide(CPlayer * player)
{
    if (iUncollectableTime > 0)
        return false;

    //fireball
    if (iType == 5) {
        if (!player->isShielded()) {
            dead = true;
            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix - 1, iy - 1, 3, 8));

            if (!player->isInvincible()) {
                return player->KillPlayerMapHazard(false, kill_style_environment, false) != player_kill_nonkill;
            }
        }

        return false;
    } else {
        if (!game_values.gamemode->gameover)
            pipegamemode->SetBonus(iType + 1, iDuration, player->getTeamID());
    }

    dead = true;
    return false;
}

void OMO_PipeBonus::update()
{
    IO_OverMapObject::update();

    if (iy >= smw->ScreenHeight)
        dead = true;

    if (pipegamemode->IsSlowdown())
        vely += GRAVITATION / 2.0f;
    else
        vely += GRAVITATION;

    if (iUncollectableTime > 0)
        --iUncollectableTime;
}

void OMO_PipeBonus::draw()
{
    if (iUncollectableTime > 0)
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY, iw, ih, 2, 256);
    else
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY, iw, ih);
}

//------------------------------------------------------------------------------
// class Phanto (for phanto mode)
//------------------------------------------------------------------------------
OMO_Phanto::OMO_Phanto(gfxSprite *nspr, short x, short y, float dVelX, float dVelY, short type) :
    IO_OverMapObject(nspr, x, y, 1, 0, 30, 32, 1, 0, type << 5, 0, 32, 32)
{
    objectType = object_phanto;
    velx = dVelX;
    vely = dVelY;

    iType = type;

    dMaxSpeedY = (float)game_values.gamemodesettings.chase.phantospeed / 2.0f;
    dMaxSpeedX = dMaxSpeedY + 1.0f;
    dSpeedRatio = (float)game_values.gamemodesettings.chase.phantospeed / 6.0f;

    iSpeedTimer = 0;
    dReactionSpeed = 0.2f;
}

void OMO_Phanto::update()
{
    setXf(fx + velx);
    setYf(fy + vely);

    if (fx < 0.0f)
        setXf(fx + smw->ScreenWidth);
    else if (fx + iw >= smw->ScreenWidth)
        setXf(fx - smw->ScreenWidth);

    if (++iSpeedTimer > 62) {
        iSpeedTimer = 0;
        dReactionSpeed = (0.05f + (float)(RANDOM_INT(20)) / 100.0f) * dSpeedRatio;
    }

    //Chase player or move off screen if there is no player holding the key
    if (game_values.gamemode->gamemode == game_mode_chase) {
        CGM_Chase * chasemode = (CGM_Chase*)game_values.gamemode;

        CPlayer * player = chasemode->GetKeyHolder();

        if (!game_values.gamemode->gameover && player) { //Chase the player
            bool fWrap = false;
            if (abs(player->ix - ix) > 320)
                fWrap = true;

            //Chase wrapped around edge of screen
            if ((player->ix < ix && !fWrap) || (player->ix > ix && fWrap)) {
                velx -= dReactionSpeed;

                if (velx < -dMaxSpeedX)
                    velx = -dMaxSpeedX;
            } else {
                velx += dReactionSpeed;

                if (velx > dMaxSpeedX)
                    velx = dMaxSpeedX;
            }

            if (player->iy < iy) {
                vely -= dReactionSpeed;

                if (vely < -dMaxSpeedY)
                    vely = -dMaxSpeedY;
            } else {
                vely += dReactionSpeed;

                if (vely > dMaxSpeedY)
                    vely = dMaxSpeedY;
            }
        } else { //Wander off screen
            if (iy > 240) {
                vely += dReactionSpeed;

                if (vely > dMaxSpeedY)
                    vely = dMaxSpeedY;
            } else {
                vely -= dReactionSpeed;

                if (vely < -dMaxSpeedY)
                    vely = -dMaxSpeedY;
            }

			if (iy >= smw->ScreenHeight || iy + ih < -CRUNCHMAX) {
                vely = 0.0f;
                velx = 0.0f;

                //Randomly position phanto off screen
                setXi(RANDOM_INT(smw->ScreenWidth));
				setYi(RANDOM_BOOL() ? -ih - CRUNCHMAX: smw->ScreenHeight);
            }
        }
    }
}

bool OMO_Phanto::collide(CPlayer * player)
{
    if (iy <= -ih || iy >= smw->ScreenHeight)
        return false;

    //If the player is holding the key, kill him
    if (!player->isInvincible() && !player->isShielded()) {
        if (iType == 2) {
            player->KillPlayerMapHazard(false, kill_style_phanto, false);
            return true;
        } else {
            if (game_values.gamemode->gamemode == game_mode_chase) {
                CGM_Chase * chasemode = (CGM_Chase*)game_values.gamemode;
                CPlayer * keyholder = chasemode->GetKeyHolder();

                if (keyholder == player) {
                    player->KillPlayerMapHazard(false, kill_style_phanto, false);

                    if (!game_values.gamemode->gameover && iType == 1) {
                        player->Score().AdjustScore(-10);
                        ifSoundOnPlay(rm->sfx_stun);
                    }

                    return true;
                }
            }
        }
    }

    return false;
}


//------------------------------------------------------------------------------
// class phanto key (for chase mode)
//------------------------------------------------------------------------------
CO_PhantoKey::CO_PhantoKey(gfxSprite *nspr) :
    MO_CarriedObject(nspr, 0, 0, 1, 0, 30, 30, 1, 1, 0, 0, 32, 32)
{
    state = 1;
    movingObjectType = movingobject_phantokey;

    iOwnerRightOffset = 12;
    iOwnerLeftOffset = -20;
    iOwnerUpOffset = 32;

    sparkleanimationtimer = 0;
    sparkledrawframe = 0;

    fCarriedByKuriboShoe = true;

    placeKey();
}

bool CO_PhantoKey::collide(CPlayer * player)
{
    if (owner == NULL && player->isready()) {
        if (player->AcceptItem(this)) {
            owner = player;
        }
    }

    return false;
}

void CO_PhantoKey::update()
{
    if (owner) {
        MoveToOwner();
        relocatetimer = 0;
    } else if (++relocatetimer > 1000) {
        placeKey();
    } else {
        applyfriction();

        //Collision detect map
        fOldX = fx;
        fOldY = fy;

        collision_detection_map();
    }

    if (++sparkleanimationtimer >= 4) {
        sparkleanimationtimer = 0;
        sparkledrawframe += 32;
        if (sparkledrawframe >= smw->ScreenHeight)
            sparkledrawframe = 0;
    }
}

void CO_PhantoKey::draw()
{
    MO_CarriedObject::draw();

    rm->spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, 32);
}

void CO_PhantoKey::placeKey()
{
    relocatetimer = 0;

    short x = 0, y = 0;
    short iAttempts = 10;
    while (!g_map->findspawnpoint(5, &x, &y, collisionWidth, collisionHeight, false) && iAttempts-- > 0);

    setXi(x);
    setYi(y);

    vely = GRAVITATION;
    velx = 0.0f;

    Drop();
}


void MysteryMushroomTempPlayer::SetPlayer(CPlayer * player, short iPowerup)
{
    fx = player->fx;
    fy = player->fy;

    fOldX = player->fOldX;
    fOldY = player->fOldY;

    velx = player->velx;
    vely = player->vely;

    //bobomb = player->bobomb;
    //powerup = player->powerup;

    burnupstarttimer = player->burnup.starttimer;
    burnuptimer = player->burnup.timer;

    inair = player->inair;
    onice = player->onice;
    //invincible = player->invincible;
    //invincibletimer = player->invincibletimer;

    platform = player->platform;
    //iCapeFrameX = player->iCapeFrameX;
    //iCapeFrameY = player->iCapeFrameY;
    //iCapeTimer = player->iCapeTimer;
    //iCapeYOffset = player->iCapeYOffset;

    gamepowerup = iPowerup;

    iOldPowerupX = player->Score().x + scorepowerupoffsets[game_values.teamcounts[player->teamID] - 1][player->subTeamID];
    iOldPowerupY = player->Score().y + 25;
}

void MysteryMushroomTempPlayer::GetPlayer(CPlayer * player, short * iPowerup)
{
    player->fNewSwapX = fx;
    player->fNewSwapY = fy;

    player->iOldPowerupX = iOldPowerupX;
    player->iOldPowerupY = iOldPowerupY;

    player->fOldX = fOldX;
    player->fOldY = fOldY;

    player->velx = velx;
    player->vely = vely;

    //player->bobomb = bobomb;
    //player->powerup = powerup;

    player->burnup.starttimer = burnupstarttimer;
    player->burnup.timer = burnuptimer;

    player->inair = inair;
    player->onice = onice;
    //player->invincible = invincible;
    //player->invincibletimer = invincibletimer;

    player->platform = platform;
    //player->iCapeFrameX = iCapeFrameX;
    //player->iCapeFrameY	= iCapeFrameY;
    //player->iCapeTimer = iCapeTimer;
    //player->iCapeYOffset = iCapeYOffset;

    *iPowerup = gamepowerup;

    if (player->carriedItem)
        player->carriedItem->MoveToOwner();
}
