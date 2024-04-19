#include "objectgame.h"

#include "eyecandy.h"
#include "Game.h"
#include "GameMode.h"
#include "GameValues.h"
#include "ObjectContainer.h"
#include "player.h"
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

extern CPlayer* GetPlayerFromGlobalID(short iGlobalID);

extern CObjectContainer objectcontainer[3];
extern CEyecandyContainer eyecandy[3];
extern CGameValues game_values;
extern CResourceManager* rm;


namespace {
IO_MovingObject* spawnRegularPowerup(PowerupType type, short spawnX, short spawnY, bool movesRight)
{
    switch (type) {
        case PowerupType::PoisonMushroom: return new PU_PoisonPowerup(&rm->spr_poisonpowerup, spawnX, spawnY, 1, movesRight, 0, 30, 30, 1, 1);
        case PowerupType::ExtraLife1: return new PU_ExtraGuyPowerup(&rm->spr_1uppowerup, spawnX, spawnY, 1, movesRight, 0, 30, 30, 1, 1, 1);
        case PowerupType::ExtraLife2: return new PU_ExtraGuyPowerup(&rm->spr_2uppowerup, spawnX, spawnY, 1, movesRight, 0, 30, 30, 1, 1, 2);
        case PowerupType::ExtraLife3: return new PU_ExtraGuyPowerup(&rm->spr_3uppowerup, spawnX, spawnY, 1, movesRight, 0, 30, 30, 1, 1, 3);
        case PowerupType::ExtraLife5: return new PU_ExtraGuyPowerup(&rm->spr_5uppowerup, spawnX, spawnY, 1, movesRight, 0, 30, 30, 1, 1, 5);
        case PowerupType::Fire: return new PU_FirePowerup(&rm->spr_firepowerup, spawnX, spawnY, 1, movesRight, 0, 30, 30, 1, 1);
        case PowerupType::Star: return new PU_StarPowerup(&rm->spr_starpowerup, spawnX, spawnY, 4, movesRight, 2, 30, 30, 1, 1);
        case PowerupType::Clock: return new PU_ClockPowerup(&rm->spr_clockpowerup, spawnX, spawnY, 1, movesRight, 0, 30, 30, 1, 1);
        case PowerupType::Bobomb: return new PU_BobombPowerup(&rm->spr_bobombpowerup, spawnX, spawnY, 1, movesRight, 0, 30, 30, 1, 1);
        case PowerupType::Pow: return new PU_PowPowerup(&rm->spr_powpowerup, spawnX, spawnY, 8, movesRight, 8, 30, 30, 1, 1);
        case PowerupType::BulletBill: return new PU_BulletBillPowerup(&rm->spr_bulletbillpowerup, spawnX, spawnY, 1, movesRight, 0, 30, 30, 1, 1);
        case PowerupType::Hammer: return new PU_HammerPowerup(&rm->spr_hammerpowerup, spawnX, spawnY, 1, movesRight, 0, 30, 30, 1, 1);
        case PowerupType::ShellGreen: return new CO_Shell(ShellType::Green, spawnX, spawnY, true, true, true, false);
        case PowerupType::ShellRed: return new CO_Shell(ShellType::Red, spawnX, spawnY, false, true, true, false);
        case PowerupType::ShellSpiny: return new CO_Shell(ShellType::Spiny, spawnX, spawnY, false, false, true, true);
        case PowerupType::ShellBuzzy: return new CO_Shell(ShellType::Buzzy, spawnX, spawnY, false, true, false, false);
        case PowerupType::Mod: return new PU_ModPowerup(&rm->spr_modpowerup, spawnX, spawnY, 8, movesRight, 8, 30, 30, 1, 1);
        case PowerupType::Feather: return new PU_FeatherPowerup(&rm->spr_featherpowerup, spawnX, spawnY, 1, 0, 30, 30, 1, 1);
        case PowerupType::MysteryMushroom: return new PU_MysteryMushroomPowerup(&rm->spr_mysterymushroompowerup, spawnX, spawnY, 1, movesRight, 0, 30, 30, 1, 1);
        case PowerupType::Boomerang: return new PU_BoomerangPowerup(&rm->spr_boomerangpowerup, spawnX, spawnY, 1, movesRight, 0, 30, 26, 1, 5);
        case PowerupType::Tanooki: return new PU_Tanooki(spawnX, spawnY);
        case PowerupType::IceWand: return new PU_IceWandPowerup(&rm->spr_icewandpowerup, spawnX, spawnY, 1, 0, 30, 30, 1, 1);
        case PowerupType::Podobo: return new PU_PodoboPowerup(&rm->spr_podobopowerup, spawnX, spawnY, 1, 0, 30, 30, 1, 1);
        case PowerupType::Bomb: return new PU_BombPowerup(&rm->spr_bombpowerup, spawnX, spawnY, 1, 0, 30, 30, 1, 1);
        case PowerupType::Leaf: return new PU_LeafPowerup(&rm->spr_leafpowerup, spawnX, spawnY, 1, 0, 30, 30, 1, 1);
        case PowerupType::PWings: return new PU_PWingsPowerup(&rm->spr_pwingspowerup, spawnX, spawnY);
    }
    return nullptr;
}

IO_MovingObject* spawnSpecialPowerup(short type, short spawnX, short spawnY)
{
    switch (type) {
        case HEALTH_POWERUP: return new PU_ExtraHeartPowerup(&rm->spr_extraheartpowerup, spawnX, spawnY);
        case TIME_POWERUP: return new PU_ExtraTimePowerup(&rm->spr_extratimepowerup, spawnX, spawnY);
        case JAIL_KEY_POWERUP: return new PU_JailKeyPowerup(&rm->spr_jailkeypowerup, spawnX, spawnY);
        case COIN_POWERUP: {
            short iRandCoin = RANDOM_INT(9);
            CoinColor color = CoinColor::Yellow;

            if (iRandCoin == 8)
                color = CoinColor::Blue;
            else if (iRandCoin >= 6)
                color = CoinColor::Green;
            else if (iRandCoin >= 3)
                color = CoinColor::Red;

            static short iCoinValue[4] = {3, 5, 2, 10};
            static short iGreedValue[4] = {10, 15, 5, 20};
            const size_t colorIdx = static_cast<size_t>(color);

            return new PU_CoinPowerup(&rm->spr_coin, spawnX, spawnY, color, game_values.gamemode->gamemode == game_mode_greed ? iGreedValue[colorIdx] : iCoinValue[colorIdx]);
        }
        case MINIGAME_COIN: return new MO_Coin(&rm->spr_coin, 0.0f, -VELJUMP / 2.0, spawnX, spawnY, 2, -1, 2, 0, false);
        case SECRET1_POWERUP: return new PU_SecretPowerup(&rm->spr_secret1, spawnX, spawnY, 0);
        case SECRET2_POWERUP: return new PU_SecretPowerup(&rm->spr_secret2, spawnX, spawnY, 1);
        case SECRET3_POWERUP: return new PU_SecretPowerup(&rm->spr_secret3, spawnX, spawnY, 2);
        case SECRET4_POWERUP: return new PU_SecretPowerup(&rm->spr_secret4, spawnX, spawnY, 3);
        default: return nullptr;
    }
}
} // namespace


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

IO_MovingObject* createpowerup(short iType, short ix, short iy, bool side, bool spawn)
{
    MO_Powerup* powerup = nullptr;
    CO_Shell* shell = nullptr;
    PU_FeatherPowerup* feather = nullptr;
    MO_Coin* coin = nullptr;

    const short spawnX = ix + 1;
    const short spawnY = iy - 1;

    if (iType < 0) {
        IO_MovingObject* object = spawnSpecialPowerup(iType, spawnX, spawnY);
        if (iType == MINIGAME_COIN) {
            coin = static_cast<MO_Coin*>(object);
        } else {
            powerup = static_cast<MO_Powerup*>(object);
        }
    }
    else if (iType < NUM_POWERUPS) {
        const auto powerupType = static_cast<PowerupType>(iType);
        IO_MovingObject* object = spawnRegularPowerup(powerupType, spawnX, spawnY, side);

        switch (powerupType) {
            case PowerupType::ShellGreen:
            case PowerupType::ShellRed:
            case PowerupType::ShellSpiny:
            case PowerupType::ShellBuzzy:
                shell = static_cast<CO_Shell*>(object);
                break;
            case PowerupType::Feather:
            case PowerupType::Leaf:
                feather = static_cast<PU_FeatherPowerup*>(object);
                break;
            default:
                powerup = static_cast<MO_Powerup*>(object);
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
    } else {
        //If no powerups were selected for this block, then fire out a podobo
        IO_MovingObject * podobo = new MO_Podobo(&rm->spr_podobo, ix + 2, iy, -(float(RANDOM_INT(5)) / 2.0f) - 6.0f, -1, -1, -1, true);
        objectcontainer[2].add(podobo);
        return podobo;
    }

    return nullptr;
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
