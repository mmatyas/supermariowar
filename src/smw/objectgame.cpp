#include "objectgame.h"

#include "eyecandy.h"
#include "GameMode.h"
#include "GameValues.h"
#include "map.h"
#include "movingplatform.h"
#include "ObjectContainer.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "objects/moving/MO_Coin.h"
#include "objects/moving/MO_Podobo.h"
#include "objects/carriable/CO_Shell.h"
#include "objects/powerup/PU_BobombPowerup.h"
#include "objects/powerup/PU_BombPowerup.h"
#include "objects/powerup/PU_BoomerangPowerup.h"
#include "objects/powerup/PU_BulletBillPowerup.h"
#include "objects/powerup/PU_ClockPowerup.h"
#include "objects/powerup/PU_CoinPowerup.h"
#include "objects/powerup/PU_ExtraGuyPowerup.h"
#include "objects/powerup/PU_ExtraHeartPowerup.h"
#include "objects/powerup/PU_ExtraTimePowerup.h"
#include "objects/powerup/PU_FeatherPowerup.h"
#include "objects/powerup/PU_FirePowerup.h"
#include "objects/powerup/PU_HammerPowerup.h"
#include "objects/powerup/PU_IceWandPowerup.h"
#include "objects/powerup/PU_JailKeyPowerup.h"
#include "objects/powerup/PU_LeafPowerup.h"
#include "objects/powerup/PU_ModPowerup.h"
#include "objects/powerup/PU_MysteryMushroomPowerup.h"
#include "objects/powerup/PU_PodoboPowerup.h"
#include "objects/powerup/PU_PoisonPowerup.h"
#include "objects/powerup/PU_PowPowerup.h"
#include "objects/powerup/PU_PWingsPowerup.h"
#include "objects/powerup/PU_SecretPowerup.h"
#include "objects/powerup/PU_StarPowerup.h"
#include "objects/powerup/PU_Tanooki.h"
#include "objects/powerup/PU_TreasureChestBonus.h"
#include "objects/MysteryMushroomTempPlayer.h"

extern CPlayer* GetPlayerFromGlobalID(short iGlobalID);

extern CObjectContainer objectcontainer[3];
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
    if (!game_values.secretsenabled)
        return;
    if (id == 0 && !game_values.unlocksecretunlocked[0]) {
        short iCountTeams = 0;
        for (short iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
            if (game_values.unlocksecret1part1[iPlayer])
                iCountTeams++;
        }

        if (iCountTeams >= 2 && game_values.unlocksecret1part2 >= 8) {
            game_values.unlocksecretunlocked[0] = true;
            ifSoundOnPlay(rm->sfx_transform);

            IO_MovingObject * object = createpowerup(SECRET1_POWERUP, RANDOM_INT(App::screenWidth), RANDOM_INT(App::screenHeight), true, false);

            if (object)
                eyecandy[2].add(new EC_SingleAnimation(&rm->spr_poof, object->ix - 8, object->iy - 8, 4, 5));
        }
    } else if (id == 1 && !game_values.unlocksecretunlocked[1]) {
        if (game_values.unlocksecret2part1 && game_values.unlocksecret2part2 >= 3) {
            game_values.unlocksecretunlocked[1] = true;
            ifSoundOnPlay(rm->sfx_transform);

            IO_MovingObject * object = createpowerup(SECRET2_POWERUP, RANDOM_INT(App::screenWidth), RANDOM_INT(App::screenHeight), true, false);

            if (object)
                eyecandy[2].add(new EC_SingleAnimation(&rm->spr_poof, object->ix - 8, object->iy - 8, 4, 5));
        }
    } else if (id == 2 && !game_values.unlocksecretunlocked[2]) {
        for (short iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
            //number of songs on thriller + number of released albums (figure it out :))
            if (game_values.unlocksecret3part1[iPlayer] >= 9 && game_values.unlocksecret3part2[iPlayer] >= 13) {
                game_values.unlocksecretunlocked[2] = true;
                ifSoundOnPlay(rm->sfx_transform);

                IO_MovingObject * object = createpowerup(SECRET3_POWERUP, RANDOM_INT(App::screenWidth), RANDOM_INT(App::screenHeight), true, false);

                if (object)
                    eyecandy[2].add(new EC_SingleAnimation(&rm->spr_poof, object->ix - 8, object->iy - 8, 4, 5));
            }
        }
    } else if (id == 3 && !game_values.unlocksecretunlocked[3]) {
        game_values.unlocksecretunlocked[3] = true;
        ifSoundOnPlay(rm->sfx_transform);

        IO_MovingObject * object = createpowerup(SECRET4_POWERUP, RANDOM_INT(App::screenWidth), RANDOM_INT(App::screenHeight), true, false);

        if (object)
            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_poof, object->ix - 8, object->iy - 8, 4, 5));
    }
}
