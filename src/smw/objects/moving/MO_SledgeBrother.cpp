#include "MO_SledgeBrother.h"

#include "eyecandy.h"
#include "Game.h"
#include "GameMode.h"
#include "GameValues.h"
#include "ObjectContainer.h"
#include "player.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "gamemodes/MiniBoss.h"
#include "objects/carriable/CO_Bomb.h"
#include "objects/carriable/CO_ThrowBox.h"
#include "objects/moving/MO_Podobo.h"
#include "objects/moving/MO_SledgeHammer.h"
#include "objects/moving/MO_SuperFireball.h"
#include "objects/overmap/WO_Thwomp.h"

extern CPlayer* GetPlayerFromGlobalID(short iGlobalID);

extern CObjectContainer objectcontainer[3];
extern CEyecandyContainer eyecandy[3];
extern CGameValues game_values;
extern CResourceManager* rm;


namespace {
void pushBombs(CObjectContainer& container, short x, short y)
{
    for (const std::unique_ptr<CObject>& obj : container.list()) {
        auto* bomb = dynamic_cast<CO_Bomb*>(obj.get());
        if (!bomb || bomb->HasOwner())
            continue;

        int bombx = bomb->x() + (bomb->iw >> 1) - x;
        int bomby = bomb->y() + (bomb->ih >> 1) - y;

        int dist = bombx * bombx + bomby * bomby;

        if (dist < 10000) {
            if (bombx > 0)
                bomb->velx += ((float)(RANDOM_INT(30)) / 10.0f + 4.0f);
            else
                bomb->velx -= ((float)(RANDOM_INT(30)) / 10.0f + 4.0f);

            bomb->vely -= (float)(RANDOM_INT(30)) / 10.0f + 6.0f;
        }
    }
}
} // namespace

//------------------------------------------------------------------------------
// class sledge brother
//------------------------------------------------------------------------------
//{jump, throw, turn, wait, taunt} otherwise move
short g_iSledgeBrotherActions[3][5][5] = {
    { { 0, 50, 65, 75, 85 }, { 3, 53, 66, 76, 86 }, { 5, 55, 70, 78, 83 }, { 5, 70, 85, 85, 90 }, { 5, 75, 90, 90, 90 } },
    { { 0, 30, 60, 80, 85 }, { 5, 40, 65, 80, 85 }, { 5, 50, 65, 75, 85 }, { 10, 55, 65, 70, 90 }, { 10, 55, 65, 65, 90 } },
    { { 0, 50, 65, 75, 85 }, { 3, 53, 65, 75, 85 }, { 5, 60, 72, 80, 88 }, { 5, 65, 80, 85, 90 }, { 5, 75, 90, 90, 90 } }
};

short g_iSledgeBrotherNeedAction[3][5][6] = {
    { { 50, 5, 5, 10, 10, 8 }, { 30, 4, 5, 10, 10, 8 }, { 20, 4, 5, 12, 12, 8 }, { 12, 4, 5, 15, 15, 10 }, { 8, 4, 5, 15, 15, 12 } },
    { { 50, 5, 5, 10, 15, 8 }, { 30, 4, 5, 10, 12, 8 }, { 20, 4, 5, 12, 10, 8 }, { 15, 4, 5, 15, 8, 10 }, { 10, 4, 5, 15, 6, 12 } },
    { { 50, 5, 8, 10, 10, 8 }, { 30, 5, 8, 12, 12, 10 }, { 20, 4, 8, 15, 15, 15 }, { 15, 4, 8, 15, 15, 15 }, { 12, 3, 8, 15, 15, 15 } }
};

short g_iSledgeBrotherMaxAction[3][5][5] = {
    { { 1, 1, 1, 2, 2 }, { 1, 2, 1, 2, 2 }, { 1, 3, 1, 2, 2 }, { 1, 3, 1, 1, 1 }, { 1, 3, 1, 0, 0 } },
    { { 1, 1, 1, 2, 2 }, { 1, 2, 1, 2, 2 }, { 1, 3, 1, 2, 2 }, { 1, 3, 1, 1, 1 }, { 1, 3, 1, 0, 0 } },
    { { 1, 1, 1, 2, 2 }, { 1, 2, 1, 2, 2 }, { 1, 2, 1, 1, 1 }, { 1, 3, 1, 1, 1 }, { 1, 3, 1, 0, 0 } }
};

short g_iSledgeBrotherWaitTime[3][5][2] = {
    { { 30, 50 }, { 25, 45 }, { 20, 40 }, { 15, 30 }, { 10, 20 } },
    { { 30, 50 }, { 25, 45 }, { 20, 40 }, { 15, 30 }, { 10, 20 } },
    { { 30, 50 }, { 25, 45 }, { 20, 40 }, { 15, 30 }, { 10, 20 } }
};

MO_SledgeBrother::MO_SledgeBrother(gfxSprite* nspr, short platformY, Boss type)
    : IO_MovingObject(nspr, Vec2s::zero(), 8, 0, 32, 56, 8, 8)
{
    iType = static_cast<short>(type);  // FIXME
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
                // Shake screen and kill players
                ifSoundOnPlay(rm->sfx_thunder);
                game_values.flags.screenshaketimer = 20;
                game_values.flags.screenshakeplayerid = -1;
                game_values.flags.screenshaketeamid = -1;
                game_values.flags.screenshakekillinair = false;
                game_values.flags.screenshakekillscount = 0;
            } else if (iType == 1) {
                // Spawn thwomps
                ifSoundOnPlay(rm->sfx_thunder);

                short numThwomps = RANDOM_INT(5) + 6;

                for (short iThwomp = 0; iThwomp < numThwomps; iThwomp++) {
                    objectcontainer[2].add(new OMO_Thwomp(&rm->spr_thwomp, (short)(RANDOM_INT(591)), 2.0f + (float)(RANDOM_INT(20)) / 10.0f));
                }

                // short numBombs = RANDOM_INT(5) + 6;

                // for (short iBomb = 0; iBomb < numBombs; iBomb++)
                //{
                //	short iRandomX = RANDOM_INT(612);
                //	short iRandomY = RANDOM_INT(442);
                //	eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, iRandomX - 2, iRandomY + 3, 3, 8));
                //	objectcontainer[2].add(new CO_Bomb(&rm->spr_bomb, iRandomX, iRandomY, 0.0f, 0.0f, 4, -1, -1, -1, RANDOM_INT(30) + 30));
                // }
            } else if (iType == 2) {
                // Spawn lots of podobos
                ifSoundOnPlay(rm->sfx_thunder);

                short numPodobos = RANDOM_INT(5) + 8;

                for (short iPodobo = 0; iPodobo < numPodobos; iPodobo++) {
                    objectcontainer[2].add(new MO_Podobo(&rm->spr_podobo, {(short)(RANDOM_INT(608)), App::screenHeight}, -(float(RANDOM_INT(9)) / 2.0f) - 9.0f, -1, -1, -1, false));
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
        // move towards destination
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
        // If we are done taunting, reset arm/legs back to normal state
        if (--taunt_timer <= 0) {
            iActionState = 0;
            arm_offset_x = 0;
            leg_offset_x = 0;
            arm_movement_timer = 0;
            leg_movement_timer = 0;
        } else {  // otherwise move them around
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

    // Force an action if it has been too long since the last action of that type
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
    // then do action based on probability
    else if ((last_action != 0 || last_action_count < g_iSledgeBrotherMaxAction[iType][game_values.gamemodesettings.boss.difficulty][0]) && randaction < g_iSledgeBrotherActions[iType][game_values.gamemodesettings.boss.difficulty][0])
        jump();
    else if ((last_action != 1 || last_action_count < g_iSledgeBrotherMaxAction[iType][game_values.gamemodesettings.boss.difficulty][1]) && randaction < g_iSledgeBrotherActions[iType][game_values.gamemodesettings.boss.difficulty][1])
        throwprojectile();
    else if ((last_action != 2 || last_action_count < g_iSledgeBrotherMaxAction[iType][game_values.gamemodesettings.boss.difficulty][2]) && randaction < g_iSledgeBrotherActions[iType][game_values.gamemodesettings.boss.difficulty][2])
        turn();
    else if ((last_action != 2 || last_action_count < g_iSledgeBrotherMaxAction[iType][game_values.gamemodesettings.boss.difficulty][3]) && randaction < g_iSledgeBrotherActions[iType][game_values.gamemodesettings.boss.difficulty][3])
        wait(g_iSledgeBrotherWaitTime[iType][game_values.gamemodesettings.boss.difficulty][0], g_iSledgeBrotherWaitTime[iType][game_values.gamemodesettings.boss.difficulty][1]);
    else if ((last_action != 2 || last_action_count < g_iSledgeBrotherMaxAction[iType][game_values.gamemodesettings.boss.difficulty][4]) && randaction < g_iSledgeBrotherActions[iType][game_values.gamemodesettings.boss.difficulty][4])
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
        objectcontainer[2].add(new MO_SledgeHammer(&rm->spr_sledgehammer, {(face_right ? ix + 32 : ix) - collisionOffsetX, iy}, 8, {fHammerVelX, -HAMMERTHROW}, 5, -1, -1, -1, false));
    } else if (iType == 1) {
        float fBombVelX = ((float)(RANDOM_INT(5) + 12)) / 2.0f - (face_right ? 0.0f : 14.0f);
        float fBombVelY = -(float)(RANDOM_INT(13)) / 2.0f - 6.0f;
        objectcontainer[2].add(new CO_Bomb(&rm->spr_bomb, {face_right ? ix + iw - 32 : ix - 20, iy}, {fBombVelX, fBombVelY}, 4, -1, -1, -1, RANDOM_INT(60) + 120));
    } else if (iType == 2) {
        float fFireVelX = ((float)(RANDOM_INT(9) + 6)) / 2.0f - (face_right ? 0.0f : 10.0f);
        float fFireVelY = (float)(RANDOM_INT(17)) / 2.0f - 4.0f;
        objectcontainer[2].add(new MO_SuperFireball(&rm->spr_superfireball, {face_right ? ix + iw - 32 : ix - 16, iy}, 4, {fFireVelX, fFireVelY}, 4, -1, -1, -1));
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

    // If this is a bomb brother, push bombs away when taunting
    if (iType == 1) {
        pushBombs(objectcontainer[2], ix + 32, iy + 32);
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

bool MO_SledgeBrother::collide(CPlayer* player)
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

bool MO_SledgeBrother::hit(CPlayer* player)
{
    if (player->isShielded())
        return false;

    return player->KillPlayerMapHazard(false, KillStyle::Environment, false) != PlayerKillType::NonKill;
}

void MO_SledgeBrother::collide(IO_MovingObject* object)
{
    if (object->isDead())
        return;

    MovingObjectType type = object->getMovingObjectType();

    // Ignore hammers and fireballs thrown from sledge brother
    if (type == movingobject_sledgehammer || type == movingobject_superfireball) {
        if (object->iPlayerID == -1)
            return;
    }

    removeifprojectile(object, false, false);

    // These types of attacks damage the boss
    bool fDamageWeapon = type == movingobject_shell || type == movingobject_throwblock || type == movingobject_throwbox || type == movingobject_explosion;

    // These don't damage him but still collide
    bool fNoDamageWeapon = type == movingobject_fireball || type == movingobject_bulletbill || type == movingobject_hammer || type == movingobject_boomerang || type == movingobject_attackzone;

    if (fDamageWeapon || fNoDamageWeapon) {
        // If it is a shell but it is sitting, don't collide
        if (type == movingobject_shell && object->GetState() == 2)
            return;

        // If it is a throw box but is sitting, dont' collide
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

    // Find the player that shot this projectile so we can attribute a kill
    CPlayer* killer = GetPlayerFromGlobalID(playerID);

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
