#include "player.h"

#include "eyecandy.h"
#include "gamemodes.h"
#include "GameValues.h"
#include "map.h"
#include "movingplatform.h"
#include "net.h"
#include "object.h"
#include "ObjectContainer.h"
#include "objectgame.h"
#include "objecthazard.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"

#include <cassert>
#include <cmath>

extern bool SwapPlayers(short iUsingPlayerID);
extern void EnterBossMode(short bossType);
extern short g_iWinningPlayer;
extern short g_iSwirlSpawnLocations[4][2][25];
extern void CheckSecret(short id);
extern SpotlightManager spotlightManager;

extern CObjectContainer noncolcontainer;
extern CObjectContainer objectcontainer[3];

extern CMap* g_map;
extern CEyecandyContainer eyecandy[3];

extern CPlayer* list_players[4];
extern short list_players_cnt;

extern CGameValues game_values;
extern CResourceManager* rm;

extern short x_shake;
extern short y_shake;


void CScore::AdjustScore(short iValue)
{
    if (game_values.gamemode->gameover)
        return;

    score += iValue;

    if (score < 0)
        score = 0;

    SetDigitCounters();
}

void CMap::movingPlatformCollision(CPlayer * player)
{
    //Collide player with normal moving platforms
    for (short iPlatform = 0; iPlatform < iNumPlatforms; iPlatform++) {
        platforms[iPlatform]->collide(player);

        if (!player->isready())
            return;
    }

    //Collide player with temporary platforms (like falling donut blocks)
    std::list<MovingPlatform*>::iterator iterateAll = tempPlatforms.begin(), lim = tempPlatforms.end();
    while (iterateAll != lim) {
        (*iterateAll)->collide(player);

        if (!player->isready())
            return;

        iterateAll++;
    }
}

CPlayer * GetPlayerFromGlobalID(short iGlobalID)
{
    for (short i = 0; i < list_players_cnt; i++) {
        if (list_players[i]->globalID == iGlobalID)
            return list_players[i];
    }

    return NULL;
}

CPlayer::CPlayer(short iGlobalID, short iLocalID, short iTeamID, short iSubTeamID, short iColorID,
    gfxSprite * nsprites[PGFX_LAST], CScore *nscore, short * sRespawnCounter, CPlayerAI * ai)
    : shyguy(false)
    , globalID(iGlobalID)
    , teamID(iTeamID)
    , score(nscore)
    , localID(iLocalID)
    , subTeamID(iSubTeamID)
    , colorID(iColorID)
    , lockfall(false)
    , lockfire(false)
    , pPlayerAI(ai)
    , sprite_state(PGFX_JUMPING_R)
    , sprswitch(0)
    , projectiles(0)
    , spawnradius(0.0f)
    , spawnangle(0.0f)
    , respawncounter(sRespawnCounter)
    , powerupused(-1)
    , powerupradius(0.0f)
    , powerupangle(0.0f)
    , fAcceptingItem(false)
    , fPressedAcceptItem(false)
    , carriedItem(NULL)
    , ownerPlayerID(-1)
    , ownerColorOffsetX(0)
    , spawntext(20)  // set it to 20 so there is an immediate text spawned upon winning
    , iSuicideCreditPlayerID(-1)
    , iSuicideCreditTimer(0)
    , net_waitingForPowerupTrigger(false)
{
    //AI stuff
    if (pPlayerAI)
        pPlayerAI->SetPlayer(this);

    if (netplay.active) {
        if (iGlobalID == netplay.remotePlayerNumber) {
            playerKeys = &game_values.playerInput.outputControls[0];
            printf("[net] Player %d reads local input.\n", iGlobalID);
        } else {
            playerKeys = &netplay.netPlayerInput.outputControls[iGlobalID];
            printf("[net] Player %d reads network input.\n", iGlobalID);
        }
    }
    else {
        playerKeys = &game_values.playerInput.outputControls[globalID];
    }

    playerDevice = game_values.playerInput.inputControls[globalID]->iDevice;

    for (short i = 0; i < PGFX_LAST; i++)
        sprites[i] = nsprites[i];

    //Do this so we have a valid x,y to say the player is so other items that init with the player will get valid positions
    //The actual choosing of a spawning position happens later
    FindSpawnPoint();

    SetupNewPlayer();
    *respawncounter = 0;

    game_values.unlocksecret1part1[globalID] = false;
}

CPlayer::~CPlayer()
{
    if (pPlayerAI)
        delete pPlayerAI;
}

void CPlayer::Init()
{
    printf("CPlayer::init()\n");
    if (pPlayerAI)
        pPlayerAI->Init();
}

void CPlayer::updateFrozenStatus(int keymask)
{
    if (frozen) {
        if ((~(sprite_state & 0x1) && (keymask & 4)) || ((sprite_state & 0x1) && (keymask & 8)))
            frozentimer -= 5;

        if (--frozentimer <= 0) {
            frozentimer = 0;
            frozen = false;

            //Shield the player after becoming unfrozen to protect against being frozen again
            shield.turn_on();
            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix + HALFPW - 16, iy + HALFPH - 16, 3, 8));
        }
    }
}

void CPlayer::accelerate(float direction)
{
    // direction is
    //  1 on moving right
    // -1 on moving left
    assert(direction == 1.0 || direction == -1.0);

    if (onice)
        velx += VELMOVINGADDICE * direction;
    else
        velx += VELMOVINGADD * direction;

    float maxVel = 0.0f;
    if (!frozen) {
        if ((game_values.slowdownon != -1 && game_values.slowdownon != teamID) || jail.isActive())
            maxVel = VELSLOWMOVING;
        else if (playerKeys->game_turbo.fDown)
            maxVel = VELTURBOMOVING + (game_values.gamemode->tagged == this ? TAGGEDBOOST : 0.0f);
        else
            maxVel = VELMOVING + (game_values.gamemode->tagged == this ? TAGGEDBOOST : 0.0f);
    }
    assert(maxVel >= 0.0);

    if (std::abs(velx) > maxVel)
        velx = maxVel * direction;

    if (!inair) {
        //Make player hop or stick to ground in Kuribo's shoe
        if (kuriboshoe.is_on()) {
            //This makes the shoe stick to the ground (for sticky shoes)
            if (kuriboshoe.getType() == STICKY) {
                velx = 0.0f;
            } else {
                //only allow the player to jump in the air from kuribo's shoe if we aren't bouncing on a note block
                if (superjumptimer <= 0) {
                    Jump(direction, 1.0f, true);

                    superjumptype = 3;
                    superjumptimer = 16;
                }
            }
        }
        // If the player suddently moved to the other direction, play skid sound
        else if (direction > 0.0f ? velx < 0.0f : velx > 0.0f)
            game_values.playskidsound = true;

        //If rain candy is turned on
        if ((g_map->eyecandy[0] & 32 || g_map->eyecandy[1] & 32 || g_map->eyecandy[2] & 32) && fabs(velx) > VELMOVINGADD && ++rainsteptimer > 7) {
            rainsteptimer = 0;
            eyecandy[1].add(new EC_SingleAnimation(&rm->spr_frictionsmoke, ix, iy + PH - 14, 5, 3, 0, 16, 16, 16));
        }
    }
}

void CPlayer::accelerateRight()
{
    accelerate(1.0);
}

void CPlayer::accelerateLeft()
{
    accelerate(-1.0);
}

void CPlayer::decreaseVelocity()
{
    //Stop ground velocity when wearing the sticky shoe
    if (!inair && kuriboshoe.getType() == STICKY) {
        velx = 0.0f;
        return;
    }

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

bool CPlayer::canSuperStomp() const {
    return inair && isready() && !superstomp.isInSuperStompState();
}

bool CPlayer::wantsToSuperStomp() const {
    return (playerKeys->game_down.fPressed && playerDevice == DEVICE_KEYBOARD)
        || (playerKeys->game_jump.fPressed && playerKeys->game_down.fDown);
}

bool CPlayer::highJumped() const {
    return (superjumptype != 3 || superjumptimer <= 0);
}

bool CPlayer::isInvincible() const {
    return invincibility.is_on();
}

bool CPlayer::isShielded() const {
    return shield.is_on();
}

bool CPlayer::IsInvincibleOnBottom() const {
    return isInvincible() || isShielded() || kuriboshoe.is_on();
}

bool CPlayer::IsSuperStomping() const {
    return superstomp.isStomping();
}

CScore& CPlayer::Score() {
    assert(score);
    return *score;
}

void CPlayer::SetCorpseType(short type) {
    diedas = type;
}

#if 0
void reference_code() {
    if (game_values.secrets)
    {
        for (short bossType = 0; bossType < 3; bossType++)
        {
            if (game_values.bosspeeking == bossType)
            {
                static const int boss_code[3][7] = { {2,1,4,8,16,16,32},
                                                     {1,1,4,8,2,2,32},
                                                     {4,8,4,8,16,16,0} };

                static const int boss_code_length[3] = {7, 7, 6};

                if (keymask & boss_code[bossType][boss_index[bossType]])
                    boss_index[bossType]++;
                else if (keymask & ~boss_code[bossType][boss_index[bossType]])
                    boss_index[bossType] = 0;

                if (boss_index[bossType] == boss_code_length[bossType])
                {
                    boss_index[bossType] = 0;
                    EnterBossMode(bossType);
                }
            }
            else
            {
                boss_index[bossType] = 0;
            }
        }

        if (konamiIndex < 11)
        {
            static const int konami_code[11] = {1,1,2,2,4,8,4,8,48,48,48};

            if (keymask & konami_code[konamiIndex])
                konamiIndex++;
            else if (keymask & ~konami_code[konamiIndex])
                konamiIndex = 0;

            extern int g_tanookiFlag;
            if (konamiIndex == 11)
            {
                ifSoundOnPlay(rm->sfx_transform);
                g_tanookiFlag++;
            }
        }

        //Super kick shells and BTBs
        if (keymask & 2)
            superKickIndex++;
        else if (keymask & ~2)
            superKickIndex = 0;

        int keymaskdown =
            (playerKeys->game_jump.fDown?1:0) |
            (playerKeys->game_down.fDown?2:0) |
            (playerKeys->game_left.fDown?4:0) |
            (playerKeys->game_right.fDown?8:0) |
            (playerKeys->game_turbo.fDown?16:0) |
            (playerKeys->game_powerup.fDown?32:0);

        //Invincible Dash
        if (invincible)
        {
            if (keymask & 4)
                dashLeftIndex++;
            else if (keymask & ~4)
                dashLeftIndex = 0;

            if (dashLeftIndex >= 3 && keymaskdown == 20)
            {
                dashLeftIndex = 0;
                dashLeft = true;
            }

            if (keymaskdown != 20)
                dashLeft = false;


            if (keymask & 8)
                dashRightIndex++;
            else if (keymask & ~8)
                dashRightIndex = 0;

            if (dashRightIndex >= 3 && keymaskdown == 24)
            {
                dashRightIndex = 0;
                dashRight = true;
            }

            if (keymaskdown != 24)
                dashRight = false;
        }
        else
        {
            dashLeft = false;
            dashRight = false;
        }

        //Keep this code -> good for items that destroy blocks on the map

        //Active Super POW destroys and triggers blocks
        if (super_pow && (game_values.screenshaketimer > 0 || powerupused == 9))
        {
            if (game_values.screenshaketimer > 0)
            {
                if (++super_pow_timer >= 60)
                {
                    game_values.screenshaketimer = 0;
                    super_pow_timer = 0;
                    super_pow = false;
                }

                if (keymask & 16)
                {
                    game_values.screenshaketimer += 10;

                    IO_Block * block = (IO_Block*)noncolcontainer.getRandomObject();

                    if (block)
                        block->triggerBehavior();
                }
            }
        }
        else
        {
            super_pow = false;
            super_pow_timer = 0;
        }

        //Active Super MOd Destroys and triggers blocks
        if (super_mod && (game_values.screenshaketimer > 0 || powerupused == 16))
        {
            if (game_values.screenshaketimer > 0)
            {
                if (++super_mod_timer >= 60)
                {
                    game_values.screenshaketimer = 0;
                    super_mod_timer = 0;
                    super_mod = false;
                }

                if (keymask & 16)
                {
                    game_values.screenshaketimer += 10;

                    IO_Block * block = (IO_Block*)noncolcontainer.getRandomObject();

                    if (block)
                        block->triggerBehavior();
                }
            }
        }
        else
        {
            super_mod = false;
            super_mod_timer = 0;
        }

        //Super Pow
        if (game_values.gamepowerups[globalID] == 9)
        {
            static const int super_pow_code[3] = {2, 2, 32};

            if (keymask & super_pow_code[super_pow_index])
                super_pow_index++;
            else if (keymask & ~super_pow_code[super_pow_index])
                super_pow_index = 0;

            if (super_pow_index >= 3)
            {
                super_pow_index = 0;
                ifSoundOnPlay(rm->sfx_transform);
                super_pow = true;
            }
        }

        //Super Mod
        if (game_values.gamepowerups[globalID] == 16)
        {
            static const int super_mod_code[3] = {1, 1, 32};

            if (keymask & super_mod_code[super_mod_index])
                super_mod_index++;
            else if (keymask & ~super_mod_code[super_mod_index])
                super_mod_index = 0;

            if (super_mod_index >= 3)
            {
                super_mod_index = 0;
                ifSoundOnPlay(rm->sfx_transform);
                super_mod = true;
            }
        }
        if (game_values.gamemode->gamemode == game_mode_stomp && !game_values.redkoopas)
        {
            if (redKoopaIndex < 7)
            {
                static const int red_koopa_code[7] = {2,2,4,2,8,2,1};

                if (keymask & red_koopa_code[redKoopaIndex])
                    redKoopaIndex++;
                else if (keymask & ~red_koopa_code[redKoopaIndex])
                    redKoopaIndex = 0;

                if (redKoopaIndex == 7)
                {
                    ifSoundOnPlay(rm->sfx_transform);
                    game_values.redkoopas = true;
                }
            }
        }

        if (!game_values.redthrowblocks)
        {
            if (redThrowBlockIndex < 8)
            {
                static const int red_throw_block_code[8] = {1,2,1,2,8,4,1,2};

                if (keymask & red_throw_block_code[redThrowBlockIndex])
                    redThrowBlockIndex++;
                else if (keymask & ~red_throw_block_code[redThrowBlockIndex])
                    redThrowBlockIndex = 0;

                if (redThrowBlockIndex == 8)
                {
                    ifSoundOnPlay(rm->sfx_transform);
                    game_values.redthrowblocks = true;

                    for (short iBlock = 0; iBlock < noncolcontainer.list_end; iBlock++)
                    {
                        if (((IO_Block*)noncolcontainer.list[iBlock])->getBlockType() == block_throw)
                        {
                            ((B_ThrowBlock*)noncolcontainer.list[iBlock])->SetType(true);
                        }
                    }
                }
            }
        }


        //Keep this code for later reference:
        //It is good code to swap out blocks on the fly
        if (!game_values.viewblocks)
        {
            if (viewBlockIndex < 7)
            {
                static const int view_block_code[7] = {8,2,4,2,1,1,1};

                if (keymask & view_block_code[viewBlockIndex])
                    viewBlockIndex++;
                else if (keymask & ~view_block_code[viewBlockIndex])
                    viewBlockIndex = 0;

                if (viewBlockIndex == 7)
                {
                    ifSoundOnPlay(rm->sfx_transform);
                    game_values.viewblocks = true;

                    for (short iBlock = 0; iBlock < noncolcontainer.list_end; iBlock++)
                    {
                        IO_Block * block = (IO_Block*)noncolcontainer.list[iBlock];
                        if (block->getBlockType() == block_powerup)
                        {
                            block->dead = true;
                            B_ViewBlock * viewBlock = new B_ViewBlock(&rm->spr_viewblock, block->col << 5, block->row << 5);

                            if (block->state != 0)
                            {
                                viewBlock->state = 3;
                                viewBlock->timer = ((B_PowerupBlock*)block)->timer;
                            }

                            g_map->blockdata[block->col][block->row] = viewBlock;
                            noncolcontainer.add(viewBlock);
                        }
                    }
                }
            }
        }

        if (secret_spring_index < 9)
        {
            static const int secret_spring_code[9] = {2,1,2,1,2,1,2,2,16};

            if (keymask & secret_spring_code[secret_spring_index])
                secret_spring_index++;
            else if (keymask & ~secret_spring_code[secret_spring_index])
                secret_spring_index = 0;

            if (secret_spring_index == 9)
            {
                ifSoundOnPlay(rm->sfx_transform);
                objectcontainer[1].add(new CO_Spring(&rm->spr_spring));
            }
        }

        if (secret_spike_index < 6)
        {
            static const int secret_spike_code[6] = {16,2,4,8,1,16};

            if (keymask & secret_spike_code[secret_spike_index])
                secret_spike_index++;
            else if (keymask & ~secret_spike_code[secret_spike_index])
                secret_spike_index = 0;

            if (secret_spike_index == 6)
            {
                ifSoundOnPlay(rm->sfx_transform);
                objectcontainer[1].add(new CO_Spike(&rm->spr_spike));
            }
        }
    }
}
#endif

void CPlayer::update_waitingForRespawn()
{
    assert(state == player_wait);

    //use 31 frames to do 0.5 second increments
    if (*respawncounter > 0 && ++waittimer >= 31) {
        waittimer = 0;
        (*respawncounter)--;
    }

    if (*respawncounter <= 0) {
        *respawncounter = 0;

        if (FindSpawnPoint()) {
            //Make sure spawn point isn't inside a tile
            collisions.checksides(*this);

            state = player_spawning;

            if (game_values.spawnstyle == 0) {
                eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix + HALFPW - 16, iy + HALFPH - 16, 3, 8));
            } else if (game_values.spawnstyle == 1) {
                eyecandy[0].add(new EC_Door(&rm->spr_spawndoor, sprites[sprite_state], ix + HALFPW - 16, iy + HALFPH - 16, 1, iSrcOffsetX, colorID));
            }
        }
    }
}

void CPlayer::update_respawning()
{
    assert(isspawning());

    if (game_values.spawnstyle == 0) {
        state = player_ready;
    } else if (game_values.spawnstyle == 1) {
        //Wait for door eyecandy to open to let mario out (20 frames for door to appear and 16 frames to open)
        if (++spawntimer > 36) {
            spawntimer = 0;
            state = player_ready;
        }
    } else if (game_values.spawnstyle == 2) {
        if (++spawntimer >= 50) {
            state = player_ready;
        } else if (spawntimer % 2) {
            short swirlindex = spawntimer >> 1;
            short ixoffset = ix - PWOFFSET;
            short iyoffset = iy - PHOFFSET;
            short iColorIdOffset = colorID << 5;

            for (short iSwirl = 0; iSwirl < 4; iSwirl++)
                eyecandy[2].add(new EC_SingleAnimation(&rm->spr_spawnsmoke, ixoffset + g_iSwirlSpawnLocations[iSwirl][0][swirlindex], iyoffset + g_iSwirlSpawnLocations[iSwirl][1][swirlindex], 4, 4, 0, iColorIdOffset, 32, 32));
        }
    }
}

void CPlayer::triggerPowerup()
{
    switch (powerupused) {
    case 1: {
        game_values.gamemode->playerextraguy(*this, 1);
        ifSoundOnPlay(rm->sfx_extraguysound);
        break;
    }
    case 2: {
        game_values.gamemode->playerextraguy(*this, 2);
        ifSoundOnPlay(rm->sfx_extraguysound);
        break;
    }
    case 3: {
        game_values.gamemode->playerextraguy(*this, 3);
        ifSoundOnPlay(rm->sfx_extraguysound);
        break;
    }
    case 4: {
        game_values.gamemode->playerextraguy(*this, 5);
        ifSoundOnPlay(rm->sfx_extraguysound);
        break;
    }
    case 5: {
        powerup = -1;
        SetPowerup(1);
        break;
    }
    case 6: {
        invincibility.turn_on(*this);
        break;
    }
    case 7: {
        turnslowdownon();
        outofarena.reset();
        break;
    }
    case 8: {
        powerup = -1;
        bobomb = false;
        SetPowerup(0);
        break;
    }
    case 9: {
        ifSoundOnPlay(rm->sfx_thunder);
        game_values.screenshaketimer = 20;
        game_values.screenshakeplayerid = globalID;
        game_values.screenshaketeamid = teamID;
        game_values.screenshakekillinair = false;
        game_values.screenshakekillscount = 0;
        break;
    }
    case 10: {
        game_values.bulletbilltimer[globalID] = 400;
        game_values.bulletbillspawntimer[globalID] = 0;
        break;
    }
    case 11: {
        powerup = -1;
        SetPowerup(2);
        break;
    }
    case 12: {
        CO_Shell * shell = new CO_Shell(0, 0, 0, true, true, true, false);
        if (objectcontainer[1].add(shell))
            shell->UsedAsStoredPowerup(this);
        break;
    }
    case 13: {
        CO_Shell * shell = new CO_Shell(1, 0, 0, false, true, true, false);
        if (objectcontainer[1].add(shell))
            shell->UsedAsStoredPowerup(this);
        break;
    }
    case 14: {
        CO_Shell * shell = new CO_Shell(2, 0, 0, false, false, true, true);
        if (objectcontainer[1].add(shell))
            shell->UsedAsStoredPowerup(this);
        break;
    }
    case 15: {
        CO_Shell * shell = new CO_Shell(3, 0, 0, false, true, false, false);
        if (objectcontainer[1].add(shell))
            shell->UsedAsStoredPowerup(this);
        break;
    }
    case 16: {
        ifSoundOnPlay(rm->sfx_thunder);
        game_values.screenshaketimer = 20;
        game_values.screenshakeplayerid = globalID;
        game_values.screenshaketeamid = teamID;
        game_values.screenshakekillinair = true;
        break;
    }
    case 17: {
        powerup = -1;
        SetPowerup(3);
        break;
    }
    case 18: {
        SwapPlayers(localID);
        break;
    }
    case 19: {
        powerup = -1;
        SetPowerup(4);
        break;
    }
    case 20: { //tanooki
        tanookisuit.onPickup();
        break;
    }
    case 21: { //ice wand
        powerup = -1;
        SetPowerup(5);
        break;
    }
    case 22: { //golden podobo
        short numPodobos = RANDOM_INT(6) + 10;
        for (short iPodobo = 0; iPodobo < numPodobos; iPodobo++) {
            objectcontainer[2].add(new MO_Podobo(&rm->spr_podobo, (short)RANDOM_INT(smw->ScreenWidth * 0.95f), smw->ScreenHeight, -(float(RANDOM_INT(9)) / 2.0f) - 9.0f, globalID, teamID, colorID, false));
        }
        ifSoundOnPlay(rm->sfx_thunder);
        break;
    }
    case 23: { //bombs
        powerup = -1;
        SetPowerup(6);
        break;
    }
    case 24: { //leaf
        powerup = -1;
        SetPowerup(7);
        break;
    }
    case 25: { //pwings
        powerup = -1;
        SetPowerup(8);
        break;
    }
    case 26: { //jail key
        jail.escape(*this);
        break;
    }
    }

    powerupused = -1;
}

void CPlayer::update_usePowerup()
{
    assert(powerupused > -1);

    powerupradius -= (float)game_values.storedpowerupdelay / 2.0f;
    powerupangle += 0.05f;

    if (netplay.active && net_waitingForPowerupTrigger)
        return;

    if (powerupradius < 0.0f)
        triggerPowerup();
}

void CPlayer::update_spriteColor()
{
    iSrcOffsetX = 0;

    bool fColorChosen = false;
    if (isInvincible()) {
        iSrcOffsetX = animationstate;
        fColorChosen = true;
    } else {
        if (game_values.gamemode->gamemode == game_mode_star) {
            CGM_Star * starmode = (CGM_Star*) game_values.gamemode;
            short starmodetype = starmode->getcurrentmodetype();
            if (starmodetype != 2 && starmode->isplayerstar(this)) {
                iSrcOffsetX = starmodetype ? 224 : 192;
                fColorChosen = true;
            }
        }
    }

    if (!fColorChosen) {
        if (game_values.gamemode->tagged == this)
            iSrcOffsetX = 160;
        else if (frozen)
            iSrcOffsetX = 256;
        else if (isShielded())
            iSrcOffsetX = 128;
    }
}

void CPlayer::tryFallingThroughPlatform(short movement_direction)
{
    //only if on the ground and the jump key was released somewhen after it was pressed the last time
    bool fFellThrough = false;
    if (playerKeys->game_down.fDown) {
        //Check to see what the player is standing on
        fPrecalculatedY = fy + vely;
        short under_tile_y = ((short)fPrecalculatedY + PH) / TILESIZE;

        short left_tile_x = ix / TILESIZE;
        if (left_tile_x < 0)
            left_tile_x += MAPWIDTH;
        else if (left_tile_x >= MAPWIDTH)
            left_tile_x -= MAPWIDTH;

        short right_tile_x;
        if (rightX() >= smw->ScreenWidth)
            right_tile_x = (rightX() - smw->ScreenWidth) / TILESIZE;
        else
            right_tile_x = rightX() / TILESIZE;

        int lefttile = g_map->map(left_tile_x, under_tile_y);
        int righttile = g_map->map(right_tile_x, under_tile_y);

        if (((lefttile & tile_flag_solid_on_top) && (righttile & tile_flag_solid_on_top || righttile == tile_flag_nonsolid || righttile == tile_flag_gap)) ||
                ((righttile & tile_flag_solid_on_top) && (lefttile & tile_flag_solid_on_top || lefttile == tile_flag_nonsolid || lefttile == tile_flag_gap))) {
            fFellThrough = true;
        }

        if (!fFellThrough && platform) {
            fPrecalculatedY += platform->fOldVelY;
            platform->GetTileTypesFromPlayer(this, &lefttile, &righttile);

            if (((lefttile & tile_flag_solid_on_top) && (righttile & tile_flag_solid_on_top || righttile == tile_flag_nonsolid || righttile == tile_flag_gap)) ||
                    ((righttile & tile_flag_solid_on_top) && (lefttile & tile_flag_solid_on_top || lefttile == tile_flag_nonsolid || lefttile == tile_flag_gap))) {
                fFellThrough = true;
            }
        }
    }

    if (fFellThrough) {
        lockfall = true;
        fallthrough = true;
    } else {
        Jump(movement_direction, 1.0f, false);
        ifSoundOnPlay(rm->sfx_jump);
    }

    lockjump = true;
}

void CPlayer::trySuperJumping(short movement_direction)
{
    assert(superjumptype > 0);

    if (superjumptype == 3) { //Kuribo's Shoe Jump
        Jump(movement_direction, 1.0f, false);
        ifSoundOnPlay(rm->sfx_jump);
    }
    if (superjumptype == 2) {
        vely = -VELSUPERJUMP;
        inair = true;
        ifSoundOnPlay(rm->sfx_superspring);
    } else if (superjumptype == 1) {
        vely = -VELTURBOJUMP;
        inair = true;
        ifSoundOnPlay(rm->sfx_springjump);
    }

    superjumptimer = 0;
    lockjump = true;
}

void CPlayer::tryCapeDoubleJump(short movement_direction)
{
    assert(powerup == 3);
    if (kuriboshoe.is_on())
        return;

    if (extrajumps < game_values.featherjumps) {
        if (game_values.featherlimit == 0 || projectilelimit > 0) {
            if (extrajumps < game_values.featherjumps) {
                Jump(movement_direction, 0.8f, false);
                ifSoundOnPlay(rm->sfx_capejump);
                lockjump = true;
            }

            extrajumps++;
        }

        if (game_values.featherlimit > 0)
            DecreaseProjectileLimit();
    }
}

void CPlayer::tryStartFlying()
{
    assert(powerup == 8);
    if (kuriboshoe.is_on() || flying || extrajumps != 0)
        return;

    flying = true;
    game_values.playflyingsound = true;

    lockjump = true;
    extrajumps++;
}

void CPlayer::tryShakingTail()
{
    if (kuriboshoe.is_on() || spin.isSpinInProgress())
        return;

    bool isGlidingChicken = game_values.gamemode->chicken == this
                            && game_values.gamemodesettings.chicken.glide
                            && powerup == -1;

    if (powerup == 7 || isGlidingChicken) {
        if (game_values.leaflimit == 0 || projectilelimit > 0)
            tail.shake(*this);
    }
}

void CPlayer::tryReleasingPowerup()
{
    if (tanookisuit.isStatue() || game_values.gamemode->gamemode == game_mode_bonus)
        return;

    // Don't allow usage of the poison powerup, it sticks with you and don't allow shyguys to use powerups
    if (game_values.gamepowerups[globalID] <= 0 || shyguy)
        return;

    powerupused = game_values.gamepowerups[globalID];
    game_values.gamepowerups[globalID] = -1;

    powerupradius = 100.0f;
    if (netplay.active)
        powerupangle = 0.0f;
    else
        powerupangle = (float)(RANDOM_INT(1000) * 0.00628f);

    ifSoundOnPlay(rm->sfx_storedpowerupsound);

    // only the locally controlled player should be able to send requests
    if (netplay.active && globalID == netplay.remotePlayerNumber) {
        if (netplay.theHostIsMe) {
            // this happens when the game host player triggers a powerup
            netplay.client.local_gamehost.sendPowerupStartByGH();

            // otherwise, when a remote client presses the trigger button,
            // it sends a powerup trigger request, so we don't have to handle
            // this on the host side here
        }
        else {
            net_waitingForPowerupTrigger = true;
            netplay.client.sendPowerupRequest();
        }
    }
}

void CPlayer::useSpecialPowerup()
{
    fAcceptingItem = carriedItem == NULL;

    if (!lockfire) {
        if (bobomb) { //If we're a bob-omb, explode
            action = player_action_bobomb;
        } else {
            if (powerup == 1 && projectiles < 2) {
                if (game_values.fireballlimit == 0 || projectilelimit > 0) {
                    action = player_action_fireball;
                }
            } else if (powerup == 2 && projectiles < 2 && hammertimer == 0) {
                if (game_values.hammerlimit == 0 || projectilelimit > 0) {
                    action = player_action_hammer;
                }
            } else if (powerup == 3 && !spin.isSpinInProgress() && !kuriboshoe.is_on()) {
                if (game_values.featherlimit == 0 || projectilelimit > 0) {
                    action = player_action_spincape;
                }
            } else if (powerup == 4 && projectiles < 1) { //only allow one boomerang
                if (game_values.boomeranglimit == 0 || projectilelimit > 0) {
                    action = player_action_boomerang;
                }
            } else if (powerup == 5 && projectiles < 1) {
                if (game_values.wandlimit == 0 || projectilelimit > 0) {
                    action = player_action_iceblast;
                }
            } else if (powerup == 6 && projectiles < 2 && hammertimer == 0) {
                if (game_values.bombslimit == 0 || projectilelimit > 0) {
                    action = player_action_bomb;
                }
            } else if (powerup == 7 && !spin.isSpinInProgress() && !kuriboshoe.is_on()) { //Racoon tail spin
                if (game_values.leaflimit == 0 || projectilelimit > 0) {
                    action = player_action_spintail;
                }
            }
        }

        lockfire = true;
    }
}

void CPlayer::releaseCarriedItem()
{
    lockfire = false;
    fAcceptingItem = false;

    if (carriedItem) {
        if (playerKeys->game_down.fDown)
            carriedItem->Drop();
        else {
            //Make sure the owner of the object we are kicking is this player
            carriedItem->owner = this;

            //Make sure the shell/block is out in front of player before kicking it
            if (carriedItem->getMovingObjectType() == movingobject_shell || carriedItem->getMovingObjectType() == movingobject_throwblock)
                carriedItem->MoveToOwner();

            carriedItem->Kick();
        }

        carriedItem = NULL;
    }
}

void CPlayer::updateFlyingStatus()
{
    if (flying) {
        //If they player was frozen while flying, cause them to stop flying
        //or if they have been flying for a while, stop them flying
        if (frozen || ++flyingtimer > 200) {
            flyingtimer = 0;
            flying = false;

            if (game_values.pwingslimit > 0)
                DecreaseProjectileLimit();
        } else { //otherwise allow them to rise and swoop while flying
            if (playerKeys->game_down.fDown && vely < 1.0f) {
                vely += 1.0f;
            } else if (!playerKeys->game_down.fDown && vely > -1.0f) {
                vely -= 1.5f;
                inair = true;
            }
        }
    }
}

void CPlayer::enableFreeFall()
{
    lockjump = false; //the jump key is not pressed: the player may jump again if he is on the ground

    if (vely < -VELSTOPJUMP)
        vely = -VELSTOPJUMP;

    if (flying) {
        flying = false;
        flyingtimer = 0;

        if (game_values.pwingslimit > 0)
            DecreaseProjectileLimit();
    }
}

void CPlayer::move()
{
    //Call the AI if cpu controlled
    if (state == player_ready) {
        if (pPlayerAI) {
            //Calculate movement every 4th frame (speed up optimization)
            if (game_values.cputurn == globalID) {
                cpu_think();

                if (playerKeys->game_jump.fDown || playerKeys->game_left.fDown || playerKeys->game_right.fDown)
                    suicidetimer.reset();
            }
        }
    }

    if (isInvincible())
        game_values.playinvinciblesound = true;

    if (flying)
        game_values.playflyingsound = true;

    int keymask =
        (playerKeys->game_jump.fPressed?1:0) |
        (playerKeys->game_down.fPressed?2:0) |
        (playerKeys->game_left.fPressed?4:0) |
        (playerKeys->game_right.fPressed?8:0) |
        (playerKeys->game_turbo.fPressed?16:0) |
        (playerKeys->game_powerup.fPressed?32:0);

    int keymaskdown =
        (playerKeys->game_jump.fDown?1:0) |
        (playerKeys->game_down.fDown?2:0) |
        (playerKeys->game_left.fDown?4:0) |
        (playerKeys->game_right.fDown?8:0) |
        (playerKeys->game_turbo.fDown?16:0) |
        (playerKeys->game_powerup.fDown?32:0);

    //If any key was pressed, reset the suicide timer
    if (keymask || (keymaskdown && (velx < -1.0f || velx > 1.0f)))
        suicidetimer.reset();

#if	0
    reference_code();
#endif

    secretcode.update(*this, keymask);
    kuriboshoe.update(*this, keymask);
    cardcollection.update(*this, keymask);
    tanookisuit.update(*this);
    superstomp.update_onGroundHit(*this);

    if (hammertimer > 0)
        hammertimer--;

    spin.update(*this);

    if (throw_star > 0)
        throw_star--;

    if (iSuicideCreditPlayerID >= 0) {
        if (--iSuicideCreditTimer <= 0) {
            iSuicideCreditTimer = 0;
            iSuicideCreditPlayerID = -1;
        }
    }

    update_spriteColor();

    if (!isready()) {
        if (state == player_wait) {
            update_waitingForRespawn();
            return;
        }

        if (isspawning())
            update_respawning();
        else if (iswarping())
            warpstatus.update(*this);
    }
    else if (powerupused > -1)
        update_usePowerup();

    invincibility.update(*this); // Animate invincibility

    updateFrozenStatus(keymask);

    //if player is warping or spawning don't pay attention to controls
    if (isready()) {
        //Super stomp
        superstomp.update(*this);

        //If player is shaking tail, slow decent
        tail.slowDescent(*this);

        shield.update();

        short movement_direction = 0;	//move left-right-no: -1.. left 0 no 1 ... right

        //Used for bouncing off of note blocks
        if (superjumptimer > 0)
            superjumptimer--;

        if (!frozen) {
            //Determine move direction pressed
            if (tanookisuit.notStatue() && !superstomp.isStomping()) {
                if (playerKeys->game_right.fDown)
                    movement_direction = 1;

                if (playerKeys->game_left.fDown)
                    movement_direction = -1;
            }

            //jump pressed?
            if (playerKeys->game_jump.fDown) {
                if (!lockjump && tanookisuit.notStatue() && !superstomp.isStomping()) {
                    if (!inair && superjumptimer == 0) {
                        tryFallingThroughPlatform(movement_direction);
                    } else if (superjumptimer > 0) {
                        trySuperJumping(movement_direction);
                    } else if (powerup == 3) {
                        tryCapeDoubleJump(movement_direction);
                    } else if (powerup == 8) { //Start pwings flight
                        tryStartFlying();
                    }
                    //This must come last or gliding chickens can't use powerups before this statement
                    else {
                        tryShakingTail();
                    }
                }
            } else
                enableFreeFall();

            if (playerKeys->game_down.fDown) {
                if (!lockfall && !inair && playerDevice == DEVICE_KEYBOARD) {
                    lockfall = true;
                    fallthrough = true;
                }
            } else {
                lockfall = false;
            }

            //POWERUP RELEASE
            if (playerKeys->game_powerup.fDown)
                tryReleasingPowerup();

            fPressedAcceptItem = playerKeys->game_turbo.fPressed;

            //Projectiles
            if (playerKeys->game_turbo.fDown) {
                useSpecialPowerup();
            } else
                releaseCarriedItem();
        }

        updateFlyingStatus();

        if (movement_direction == 1)
            accelerateRight();
        else if (movement_direction == -1)
            accelerateLeft();
        else
            decreaseVelocity();

        if (!inair) {
            if (game_values.unlocksecret3part2[globalID] > 0)
                game_values.unlocksecret3part2[globalID]--;
        }

        fOldX = fx;
        fOldY = fy;

        if (game_values.windaffectsplayers) {
            oldvelx = velx;
            float windx = game_values.gamewindx / (kuriboshoe.is_on() ? 3.0f : 1.5f);
            velx = CapSideVelocity(velx + windx);
        }

        collision_detection_map();

        //If the player died or entered a warp, don't reset his velocity
        if (game_values.windaffectsplayers && state == player_ready)
            velx = oldvelx;
    }

    //Player can be killed by map so only do this code if he is still living
    if (isready()) {
        //Deal with terminal burnup velocity
        burnup.update(*this);
        if (isdead())
            return;

        //Kill the player if he is standing still for too long
        suicidetimer.update(*this);
        if (isdead())
            return;

        //Deal with out of arena timer
        outofarena.update(*this);
        if (isdead())
            return;

        //Deal with release from jail timer
        jail.update(*this);
    }

    updateSprite();
}

void CPlayer::CommitAction()
{
    if (player_action_bobomb == action) {
        bobomb = false;
        objectcontainer[2].add(new MO_Explosion(&rm->spr_explosion, ix + HALFPW - 96, iy + HALFPH - 64, 2, 4, globalID, teamID, kill_style_bobomb));
        ifSoundOnPlay(rm->sfx_bobombsound);
    } else if (player_action_fireball == action) {
        objectcontainer[0].add(new MO_Fireball(&rm->spr_fireball, ix + 6, iy, 4, isFacingRight(), 5, globalID, teamID, colorID));
        ifSoundOnPlay(rm->sfx_fireball);

        projectiles++;

        if (game_values.fireballlimit > 0)
            DecreaseProjectileLimit();
    } else if (player_action_hammer == action) {
        if (isFacingRight())
            objectcontainer[2].add(new MO_Hammer(&rm->spr_hammer, ix + 8, iy, 6, (game_values.reversewalk ? -velx : velx) + 2.0f, -HAMMERTHROW, 5, globalID, teamID, colorID, false));
        else
            objectcontainer[2].add(new MO_Hammer(&rm->spr_hammer, ix - 14, iy, 6, (game_values.reversewalk ? -velx : velx) - 2.0f, -HAMMERTHROW, 5, globalID, teamID, colorID, false));

        projectiles++;

        hammertimer = game_values.hammerdelay;
        ifSoundOnPlay(rm->sfx_fireball);

        if (game_values.hammerlimit > 0)
            DecreaseProjectileLimit();
    } else if (player_action_boomerang == action) {
        objectcontainer[2].add(new MO_Boomerang(&rm->spr_boomerang, ix, iy + HALFPH - 16, 4, isFacingRight(), 5, globalID, teamID, colorID));
        projectiles++;

        if (game_values.boomeranglimit > 0)
            DecreaseProjectileLimit();
    } else if (player_action_iceblast == action) {
        if (isFacingRight())
            objectcontainer[2].add(new MO_IceBlast(&rm->spr_iceblast, ix + HALFPW - 2, iy + HALFPH - 16, 5.0f, globalID, teamID, colorID));
        else
            objectcontainer[2].add(new MO_IceBlast(&rm->spr_iceblast, ix + HALFPW - 30, iy + HALFPH - 16, -5.0f, globalID, teamID, colorID));

        projectiles++;

        ifSoundOnPlay(rm->sfx_wand);

        if (game_values.wandlimit > 0)
            DecreaseProjectileLimit();
    } else if (player_action_bomb == action) {
        CO_Bomb * bomb = new CO_Bomb(&rm->spr_bomb, ix + HALFPW - 14, iy - 8, isFacingRight() ? 3.0f : -3.0f, -3.0f, 4, globalID, teamID, colorID, RANDOM_INT(120) + 120);

        if (AcceptItem(bomb)) {
            bomb->owner = this;
            bomb->MoveToOwner();
        }

        objectcontainer[1].add(bomb);
        projectiles++;

        hammertimer = 90;

        ifSoundOnPlay(rm->sfx_fireball);

        if (game_values.bombslimit > 0)
            DecreaseProjectileLimit();
    } else if (player_action_spincape == action) {
        cape.spin(*this);
    } else if (player_action_spintail == action) {
        tail.spin(*this);
    }

    if (action == player_action_fireball) {
        game_values.unlocksecret3part1[globalID]++;
        CheckSecret(2);
    } else if (action != player_action_none) {
        game_values.unlocksecret3part1[globalID] = 0;
    }

    action = player_action_none;
}

bool shouldUpdateSprite()
{
    if (netplay.active)
        return true;

    return !game_values.pausegame && !game_values.exitinggame && !game_values.swapplayers;
}

void CPlayer::updateSprite()
{
    int iReverseSprite = game_values.reversewalk ? 1 : 0;

    //Use correct sprite (and animate)
    if (shouldUpdateSprite()) {
        //if player is warping from below, set them in the air
        if (iswarping()) {
            if (state == player_exiting_warp_down || state == player_entering_warp_up)
                inair = true;
            else
                inair = false;
        }

        //lockjump is true when we are in the air (even if we fell of an edge)
        if (spin.isSpinInProgress()) {
            sprite_state = spin.toSpriteID();
        } else if (state == player_spawning) {
            if (!(sprite_state & 0x1))
                sprite_state = PGFX_JUMPING_R + iReverseSprite;
            else
                sprite_state = PGFX_JUMPING_L - iReverseSprite;
        } else if (inair) {
            frictionslidetimer = 0;
            rainsteptimer = 0;

            if (isFacingRight())
                sprite_state = PGFX_JUMPING_R;
            else
                sprite_state = PGFX_JUMPING_L;
        } else {
            if (velx > 0.0f) {
                if (playerKeys->game_left.fDown && !playerKeys->game_right.fDown && state == player_ready) {
                    sprite_state = PGFX_STOPPING_R + iReverseSprite;

                    if (++frictionslidetimer > 3) {
                        frictionslidetimer = 0;
                        eyecandy[1].add(new EC_SingleAnimation(&rm->spr_frictionsmoke, ix, iy + PH - 12, 4, 4, 0, 0, 16, 16));
                    }
                } else {
                    if (onice && !playerKeys->game_right.fDown && !playerKeys->game_left.fDown)
                        sprite_state = PGFX_STANDING_R + iReverseSprite;
                    else {
                        if (--sprswitch < 1) {
                            if (game_values.reversewalk) {
                                if (sprite_state == PGFX_STANDING_L)
                                    sprite_state = PGFX_RUNNING_L;
                                else
                                    sprite_state = PGFX_STANDING_L;
                            } else {
                                if (sprite_state == PGFX_STANDING_R)
                                    sprite_state = PGFX_RUNNING_R;
                                else
                                    sprite_state = PGFX_STANDING_R;
                            }

                            sprswitch = 4;
                        } else {
                            //If animation timer hasn't fired, make sure we're facing the correct direction

                            if (game_values.reversewalk) {
                                if (!(sprite_state & 0x1))
                                    sprite_state = PGFX_STANDING_L;
                            } else {
                                if (sprite_state & 0x1)
                                    sprite_state = PGFX_STANDING_R;
                            }
                        }
                    }
                }
            } else if (velx < 0.0f) {
                if (playerKeys->game_right.fDown && !playerKeys->game_left.fDown && state == player_ready) {
                    sprite_state = PGFX_STOPPING_L - iReverseSprite;

                    if (++frictionslidetimer > 3) {
                        frictionslidetimer = 0;
                        eyecandy[1].add(new EC_SingleAnimation(&rm->spr_frictionsmoke, ix + PW - 16, iy + PH - 12, 4, 4, 0, 0, 16, 16));
                    }
                } else {
                    if (onice && !playerKeys->game_right.fDown && !playerKeys->game_left.fDown)
                        sprite_state = PGFX_STANDING_L - iReverseSprite;
                    else {
                        if (--sprswitch < 1) {
                            if (game_values.reversewalk) {
                                if (sprite_state == PGFX_STANDING_R)
                                    sprite_state = PGFX_RUNNING_R;
                                else
                                    sprite_state = PGFX_STANDING_R;
                            } else {
                                if (sprite_state == PGFX_STANDING_L)
                                    sprite_state = PGFX_RUNNING_L;
                                else
                                    sprite_state = PGFX_STANDING_L;
                            }

                            sprswitch = 4;
                        } else {
                            //If animation timer hasn't fired, make sure we're facing the correct direction
                            if (game_values.reversewalk) {
                                if (sprite_state & 0x1)
                                    sprite_state = PGFX_STANDING_R;
                            } else {
                                if (!(sprite_state & 0x1))
                                    sprite_state = PGFX_STANDING_L;
                            }
                        }
                    }

                }
            } else {
                //standing
                if (playerKeys->game_left.fDown)
                    sprite_state = PGFX_STANDING_L - iReverseSprite;
                else if (playerKeys->game_right.fDown)
                    sprite_state = PGFX_STANDING_R + iReverseSprite;
                else {
                    if (sprite_state & 0x1)
                        sprite_state = PGFX_STANDING_L;
                    else
                        sprite_state = PGFX_STANDING_R;
                }
            }
        }
    } else if (game_values.swapplayers) {
        short iSpriteDirection = 0;

        if (fNewSwapX < fOldSwapX)
            iSpriteDirection = 1;

        if (--sprswitch < 1) {
            if (sprite_state == PGFX_STANDING_R + iSpriteDirection)
                sprite_state = PGFX_RUNNING_R + iSpriteDirection;
            else
                sprite_state = PGFX_STANDING_R + iSpriteDirection;

            sprswitch = 4;
        }
    }
}

void CPlayer::Jump(short iMove, float jumpModifier, bool fKuriboBounce)
{
    if (fKuriboBounce)
        vely = -VELKURIBOBOUNCE;
    else if ((game_values.slowdownon != -1 && game_values.slowdownon != teamID) || jail.isActive())
        vely = -VELSLOWJUMP * jumpModifier;
    else if (ABS(velx) > VELMOVING && iMove != 0 && playerKeys->game_turbo.fDown)
        vely = -VELTURBOJUMP * jumpModifier;
    else
        vely = -VELJUMP * jumpModifier;

    inair = true;

    //Need to help the player off the platform otherwise it will collide with them again
    if (platform) {
        //if (platform->fVelY < 0.0f)
        //	setYf(fy + platform->fVelY);

        platform = NULL;
    }

    //printf("Player Jumped!\n");
}

void CPlayer::cpu_think()
{
    pPlayerAI->Think(playerKeys);
}

void CPlayer::die(short deathStyle, bool fTeamRemoved, bool fKillCarriedItem)
{
    //Only show the death gfx if the player is alive when he died
    //If he is spawning or already dead, then don't show anything
    if (state >= player_dead) {
        short iDeathSprite = deathStyle == death_style_jump ? PGFX_DEADFLYING : PGFX_DEAD;

        gfxSprite * corpseSprite = sprites[iDeathSprite];

        //If the player was a bobomb or chicken, make sure their death sprite matches
        if (diedas == 1 || game_values.gamemode->chicken == this)
            corpseSprite = rm->spr_chocobo[colorID][iDeathSprite];
        else if (diedas == 2 || bobomb)
            corpseSprite = rm->spr_bobomb[colorID][iDeathSprite];
        else if (diedas == 3 || shyguy)
            corpseSprite = rm->spr_shyguy[colorID][iDeathSprite];

        //Add eyecandy for the dead player
        if (deathStyle == death_style_shatter || frozen) {
            eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, ix + HALFPW - 16, iy + HALFPH - 16, -1.5f, -7.0f, 4, 2, 0, 0, 16, 16));
            eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, ix + HALFPW, iy + HALFPH - 16, 1.5f, -7.0f, 4, 2, 0, 0, 16, 16));
            eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, ix + HALFPW - 16, iy + HALFPH, -1.5f, -4.0f, 4, 2, 0, 0, 16, 16));
            eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, ix + HALFPW, iy + HALFPH, 1.5f, -4.0f, 4, 2, 0, 0, 16, 16));

            game_values.unlocksecret2part2++;
        } else if (deathStyle == death_style_jump) {
            eyecandy[2].add(new EC_FallingObject(corpseSprite, ix + HALFPW - 16, iy + PH - 32, 0.0f, -VELTURBOJUMP, 1, 0, iSrcOffsetX, 0, 32, 32));
        } else if (deathStyle == death_style_squish) {
            eyecandy[1].add(new EC_Corpse(corpseSprite, (float)(ix - PWOFFSET), (float)(iy+PH-32), iSrcOffsetX));
        }
    }

    //Drop any item the dead player was carrying
    if (carriedItem) {
        if (fKillCarriedItem)
            carriedItem->KillObjectMapHazard();
        else
            carriedItem->Drop();

        carriedItem = NULL;
    }

    //Drop a shoe item if the player died in one
    if (kuriboshoe.is_on()) {
        CO_KuriboShoe * shoe = new CO_KuriboShoe(&rm->spr_kuriboshoe, ix - PWOFFSET, iy - PHOFFSET, kuriboshoe.getType() == STICKY);
        shoe->collision_detection_checksides();

        objectcontainer[1].add(shoe);
    }

    //If the game mode didn't say to remove the team from the game, then respawn the player
    if (!fTeamRemoved) {
        if (game_values.screencrunch && !game_values.spinscreen)
            y_shake += CRUNCHAMOUNT;

        if (y_shake > CRUNCHMAX)
            y_shake = CRUNCHMAX;

        SetupNewPlayer();
    }
}

// Called by CO_KuriboShoe
void CPlayer::SetKuriboShoe(KuriboShoeType type)
{
    assert(type > 0);
    kuriboshoe.setType(type);

    if (carriedItem && !carriedItem->IsCarriedByKuriboShoe()) {
        carriedItem->Drop();
        carriedItem = NULL;
    }

    //Clear out powerup states that the player might be in the middle of
    ClearPowerupStates();
}

void CPlayer::SetupNewPlayer()
{
    pScoreboardSprite = sprites;
    action = player_action_none;

    velx = 0.0f;
    oldvelx = velx;

    if (game_values.spawnstyle == 1)
        vely = 0.0f;
    else
        vely = -(VELJUMP / 2);	//make the player jump up on respawn

    fOldX = fx;
    fOldY = fy;

    inair = true;
    onice = false;
    lockjump = true;
    superjumptimer = 0;
    superjumptype = 0;
    projectilelimit = 0;
    hammertimer = 0;

    frozen = false;
    frozentimer = 0;

    animationstate = 0;
    animationtimer = 0;

    killsinrow = 0;
    killsinrowinair = 0;
    extrajumps = 0;

    StripPowerups();
    ClearPowerupStates();

    sSpotlight = NULL;

    cape.reset();

    frictionslidetimer = 0;
    bobombsmoketimer = 0;
    rainsteptimer = 0;

    spawntimer = 0;
    waittimer = 0;
    *respawncounter = game_values.respawn;
    state = player_wait;

    fallthrough = false;
    diedas = 0;
    iSrcOffsetX = 0;

    outofarena.reset();

    platform = NULL;
    iHorizontalPlatformCollision = -1;
    iVerticalPlatformCollision = -1;
    iPlatformCollisionPlayerId = -1;

    shield.reset();
    kuriboshoe.reset();
    secretcode.reset();
    cardcollection.reset();
    superstomp.reset();
    suicidetimer.reset();

    throw_star = 0;

    game_values.unlocksecret3part1[globalID] = 0;
    game_values.unlocksecret3part2[globalID] = 0;
}

void CPlayer::StripPowerups()
{
    bobomb = false;
    powerup	= -1;

    tanookisuit.reset();
    invincibility.reset();

    powerupused = -1;
}

bool CPlayer::FindSpawnPoint()
{
    //Spawn the player at his flagbase if it is CTF
    //if (game_values.gamemode->gamemode == game_mode_ctf && teamID < g_map->iNumFlagBases)
    //{
    //	ix = g_map->flagbaselocations[teamID].x + 16 - HALFPW;
    //	iy = g_map->flagbaselocations[teamID].y + 16 - HALFPH;
    //}
    //else
    //{
    bool fRet = g_map->findspawnpoint(teamID + 1, &ix, &iy, PW, PH, false);
    //}

    fx = (float)ix;
    fy = (float)iy;

    return fRet;
}

void CPlayer::spawnText(const char * szText)
{
    if (++spawntext >= 20) {
        eyecandy[2].add(new EC_GravText(&rm->game_font_large, ix + HALFPW, iy, szText, -VELJUMP));
        spawntext = 0;	//spawn text every 20 frames
    }
}

bool CPlayer::bouncejump()
{
    superstomp.reset();

    if (playerKeys->game_jump.fDown) {
        lockjump = true;
        vely = -VELJUMP;
        return true;
    } else {
        vely = -VELJUMP / 2;	//jump a little off of collision object
        return false;
    }
}

void CPlayer::AddKillerAward(CPlayer* killed, killstyle style) {
    awardeffects.addKillerAward(*this, killed, style);
}

void CPlayer::DeathAwards() {
    awardeffects.addDeathAward(*this);
}

void CPlayer::AddKillsInRowInAirAward() {
    awardeffects.addKillsInRowInAirAward(*this);
}

short PlayerKilledPlayer(short iKiller, CPlayer * killed, short deathstyle, killstyle style, bool fForce, bool fKillCarriedItem)
{
    CPlayer * killer = GetPlayerFromGlobalID(iKiller);

    if (killer && killer->globalID != killed->globalID) {
        return killer->KilledPlayer(killed, deathstyle, style, fForce, fKillCarriedItem);
    } else {
        killed->DeathAwards();

        short iKillType = game_values.gamemode->playerkilledself(*killed, style);

        if (player_kill_normal == iKillType || (fForce && player_kill_nonkill == iKillType))
            killed->die(death_style_jump, false, fKillCarriedItem);

        if (player_kill_nonkill != iKillType) {
            if (deathstyle == death_style_shatter)
                ifSoundOnPlay(rm->sfx_breakblock);
            else
                ifSoundOnPlay(rm->sfx_deathsound);
        }

        return iKillType;
    }
}

short CPlayer::KilledPlayer(CPlayer * killed, short deathstyle, killstyle style, bool fForce, bool fKillCarriedItem)
{
    CPlayer * killer = this;

    //If this player is already dead, then don't kill him again
    if (killed->state != player_ready)
        return player_kill_none;

    bool fSoundPlayed = false;
    if (game_values.gamemode->chicken == killer && style != kill_style_pow) {
        ifSoundOnPlay(rm->sfx_chicken);
        fSoundPlayed = true;
    }

    if (killed->frozen)
        deathstyle = death_style_shatter;

    if (killer->teamID != killed->teamID)
        killer->AddKillerAward(killed, style);

    if (game_values.awardstyle != award_style_none)
        killed->DeathAwards();

    //now kill the player (don't call this function earlier because we need the old position, etc.
    short iKillType = game_values.gamemode->playerkilledplayer(*killer, *killed, style);

    if (player_kill_nonkill != iKillType || fForce) {
        if (killed->bobomb) {
            killed->diedas = 2;
            killer->SetPowerup(0);
        }

        if (deathstyle == death_style_jump)
            ifSoundOnPlay(rm->sfx_deathsound);
        else if (deathstyle == death_style_squish)
            ifSoundOnPlay(rm->sfx_mip);
        else if (deathstyle == death_style_shatter)
            ifSoundOnPlay(rm->sfx_breakblock);
    }

    if (player_kill_normal == iKillType || (fForce && player_kill_nonkill == iKillType))
        killed->die(deathstyle, false, fKillCarriedItem);

    return iKillType;
}


void CPlayer::TransferTag(CPlayer* o2)
{
    CPlayer * o1 = this;

    if (game_values.gamemode->gamemode != game_mode_tag)
        return;

    if (!o1->isready() || !o2->isready())
        return;

    if (game_values.gamemode->tagged == o1 && !o2->isShielded() && !o2->isInvincible()) {
        game_values.gamemode->tagged = o2;
        o1->shield.turn_on();
        eyecandy[2].add(new EC_GravText(&rm->game_font_large, game_values.gamemode->tagged->ix + HALFPW, game_values.gamemode->tagged->iy + PH, "Tagged!", -VELJUMP*1.5));
        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, game_values.gamemode->tagged->ix + HALFPW - 16, game_values.gamemode->tagged->iy + HALFPH - 16, 3, 8));
        ifSoundOnPlay(rm->sfx_transform);
    } else if (game_values.gamemode->tagged == o2 && !o1->isShielded() && !o1->isInvincible()) {
        game_values.gamemode->tagged = o1;
        o2->shield.turn_on();
        eyecandy[2].add(new EC_GravText(&rm->game_font_large, game_values.gamemode->tagged->ix + HALFPW, game_values.gamemode->tagged->iy + PH, "Tagged!", -VELJUMP*1.5));
        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, game_values.gamemode->tagged->ix + HALFPW - 16, game_values.gamemode->tagged->iy + HALFPH - 16, 3, 8));
        ifSoundOnPlay(rm->sfx_transform);
    }
}

void CPlayer::TransferShyGuy(CPlayer* o2)
{
    CPlayer * o1 = this;

    //Don't shyguy tag if this isn't shyguy tag mode or if tag transfers is set to kills only
    if (game_values.gamemode->gamemode != game_mode_shyguytag || game_values.gamemodesettings.shyguytag.tagtransfer == 1)
        return;

    if (!o1->isready() || !o2->isready())
        return;

    CGM_ShyGuyTag * sgt = (CGM_ShyGuyTag*)game_values.gamemode;

    if (o1->shyguy && !o2->shyguy && !o2->isShielded() && !o2->isInvincible()) {
        sgt->SetShyGuy(o2->getTeamID());
    } else if (o2->shyguy && !o1->shyguy && !o1->isShielded() && !o1->isInvincible()) {
        sgt->SetShyGuy(o1->getTeamID());
    }
}

void CPlayer::BounceAssistPlayer(CPlayer* o2)
{
    CPlayer * o1 = this;

    if (o1->state == player_ready && o1->fOldY + PH <= o2->fOldY && o1->iy + PH >= o2->iy && o1->playerKeys->game_jump.fDown) {
        o1->setYi(o2->iy - PH);		//set new position to top of other player
        o1->collisions.checktop(*o1);
        o1->platform = NULL;
        o1->vely = -VELSUPERJUMP;

        o1->superstomp.reset();

        ifSoundOnPlay(rm->sfx_superspring);
    }
}

void CPlayer::draw_spotlight()
{
    if (game_values.spotlights && state != player_dead) {
        if (!sSpotlight) {
            sSpotlight = spotlightManager.AddSpotlight(ix + HALFPW, iy + HALFPH, 7);
        }

        if (sSpotlight) {
            sSpotlight->UpdatePosition(ix + HALFPW, iy + HALFPH);
        }
    }
}

void CPlayer::draw_powerupRing()
{
    if (powerupused > -1) {
        short numeyecandy = 8;
        float addangle = TWO_PI / numeyecandy;
        float displayangle = powerupangle;

        for (short k = 0; k < numeyecandy; k++) {
            short powerupX = ix + HALFPW - 8 + (short)(powerupradius * cos(displayangle));
            short powerupY = iy + HALFPH - 8 + (short)(powerupradius * sin(displayangle));

            displayangle += addangle;

            if (iswarping())
                rm->spr_storedpowerupsmall.draw(powerupX, powerupY, powerupused * 16, 0, 16, 16, (short)state %4, GetWarpPlane());
            else
                rm->spr_storedpowerupsmall.draw(powerupX, powerupY, powerupused * 16, 0, 16, 16);
        }
    }
}

void CPlayer::draw_winnerCrown()
{
    if (game_values.showwinningcrown && g_iWinningPlayer == teamID) {
        if (iswarping())
            rm->spr_crown.draw(ix + HALFPW - (isFacingRight() ? 4 : 10), iy - 10 - (kuriboshoe.is_on() ? 16 : 0), 0, 0, 14, 14, (short)state % 4, GetWarpPlane());
        else
            rm->spr_crown.draw(ix + HALFPW - (isFacingRight() ? 4 : 10), iy - 10 - (kuriboshoe.is_on() ? 16 : 0));
    }
}

void CPlayer::draw()
{
    //Don't draw a player that is waiting to respawn
    if (state == player_wait)
        return;

    draw_spotlight();

    if (state == player_spawning)
        return;

    //Draw player
    pScoreboardSprite = sprites;

    if (tanookisuit.isStatue()) {
        //Make sure the scoreboard still accurately represents the player
        if (bobomb)
            pScoreboardSprite = rm->spr_bobomb[colorID];
        else if (game_values.gamemode->chicken == this)
            pScoreboardSprite = rm->spr_chocobo[colorID];
        else if (shyguy)
            pScoreboardSprite = rm->spr_shyguy[colorID];

        tanookisuit.drawStatue(*this);

        return;
    } else if (bobomb) { //draw him as bob-omb
        pScoreboardSprite = rm->spr_bobomb[colorID];

        //Add smoke to the top of the bomb
        if (++bobombsmoketimer > 2 && (velx != 0.0f || vely != GRAVITATION) && state == player_ready) {
            bobombsmoketimer = 0;
            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_bobombsmoke, ix + HALFPH - 8, iy - PHOFFSET - 8, 4, 4));
        }
    } else if (game_values.gamemode->chicken == this) { //draw him as chicken
        pScoreboardSprite = rm->spr_chocobo[colorID];
    } else if (shyguy) { //draw him as chicken
        pScoreboardSprite = rm->spr_shyguy[colorID];
        rm->spr_ownedtags.draw(ix - PWOFFSET - 8, iy - PHOFFSET - 8, ownerColorOffsetX, 0, 48, 48);
    }

    if (ownerPlayerID > -1) {
        if (iswarping())
            rm->spr_ownedtags.draw(ix - PWOFFSET - 8, iy - PHOFFSET - 8, ownerColorOffsetX, 0, 48, 48, (short)state % 4, GetWarpPlane());
        else
            rm->spr_ownedtags.draw(ix - PWOFFSET - 8, iy - PHOFFSET - 8, ownerColorOffsetX, 0, 48, 48);
    }

    //Don't allow cape, tail, wings to be used with shoe
    if (!kuriboshoe.is_on()) {
        if (powerup == 3)
            cape.draw(*this);
        else if (powerup == 8)
            wings.draw(*this);
        //This has to come last otherwise chickens with glide option won't be able to use cape or wings
        else if (powerup == 7 || (powerup == -1 && game_values.gamemode->chicken == this && game_values.gamemodesettings.chicken.glide))
            tail.draw(*this);
    }

    //Draw the player sprite above the shoe
    short iPlayerKuriboOffsetY = 0;
    if (kuriboshoe.is_on())
        iPlayerKuriboOffsetY = 16;

    //Don't draw the player if he is frozen in a shoe
    if (!frozen || !kuriboshoe.is_on()) {
        if (iswarping())
            pScoreboardSprite[sprite_state]->draw(ix - PWOFFSET, iy - PHOFFSET - iPlayerKuriboOffsetY, iSrcOffsetX, 0, 32, 32, (short)state % 4, GetWarpPlane());
        else
            pScoreboardSprite[sprite_state]->draw(ix - PWOFFSET, iy - PHOFFSET - iPlayerKuriboOffsetY, iSrcOffsetX, 0, 32, 32);
    }

    //Draw Kuribo's Shoe
    kuriboshoe.draw(*this);

    //Draw the crown on the player
    draw_winnerCrown();

    if (state < player_ready)
        return;

    if (frozen) {
        if (iswarping())
            rm->spr_iceblock.draw(ix - PWOFFSET, iy - PHOFFSET, 0, 0, 32, 32, (short)state % 4, GetWarpPlane());
        else
            rm->spr_iceblock.draw(ix - PWOFFSET, iy - PHOFFSET, 0, 0, 32, 32);
    }

    jail.draw(*this);
    suicidetimer.draw(*this);

    //Draw the Ring awards
    if (game_values.awardstyle == award_style_halo && killsinrow >= MINAWARDSNEEDED)
        awardeffects.drawRingAward(*this);

    //Draw the powerup ring when a powerup is being used
    draw_powerupRing();
}

void CPlayer::drawOutOfScreenIndicators()
{
    outofarena.draw(*this);
}

void CPlayer::updateswap()
{
    if (state != player_ready)
        return;

    if (game_values.swapstyle == 1) {
        setXf(game_values.swapplayersblink ? fOldSwapX : fNewSwapX);
        setYf(game_values.swapplayersblink ? fOldSwapY : fNewSwapY);
    } else {
        setXf(((fNewSwapX - fOldSwapX) * game_values.swapplayersposition) + fOldSwapX);
        setYf(((fNewSwapY - fOldSwapY) * game_values.swapplayersposition) + fOldSwapY);
    }

    if (carriedItem)
        carriedItem->MoveToOwner();
}

void CPlayer::drawswap()
{
    if (state != player_ready) {
        draw();
        return;
    }

    if (game_values.swapstyle != 1)
        updateSprite();

    draw();

    if (carriedItem)
        carriedItem->draw();
}

void CPlayer::mapcolldet_handlePlatformVelocity(float &fPlatformVelX, float &fPlatformVelY)
{
    if (platform) {
        fPlatformVelX = platform->fVelX;
        setXf(fx + fPlatformVelX);
        flipsidesifneeded();

        fPlatformVelY = platform->fVelY;

        if (platform->fOldVelY < 0.0f)
            fy += platform->fOldVelY;

        fPrecalculatedY += platform->fOldVelY;
    }
}

bool CPlayer::mapcolldet_handleOutOfScreen()
{
    if (fPrecalculatedY + PH < 0.0f) {
        // on top outside of the screen
        setYf(fPrecalculatedY);
        vely = CapFallingVelocity(GRAVITATION + vely);

        if (!platform) {
            inair = true;
            onice = false;
            superjumptimer = 0;
            fallthrough = false;
        }

        return true;
    } else if (fPrecalculatedY + PH >= 480) {
        //on ground outside of the screen?
        setYi(-PH);
        fOldY = (float)(-PH - 1);
        fallthrough = false;
        onice = false;

        //Clear the platform if the player wrapped to the top of the screen

        if (platform) {
            vely = platform->fVelY;
            platform = NULL;
        }

        return true;
    }

    return false;
}

void CPlayer::mapcolldet_moveHorizontally(short direction)
{
    assert(direction == 1 || direction == 3);
    short counter_direction = (direction == 1) ? 3 : 1;

    //Could be optimized with bit shift >> 5
    short ty = (short)fy / TILESIZE;
    short ty2 = ((short)fy + PH) / TILESIZE;
    short tx = -1;

    //printf("Before X - ix: %d\tiy: %d\toldx: %.2f\toldy: %.2f\tty: %d\tty2: %d\ttxl: %d\ttxr: %d\tfx: %.2f\tfy: %.2f\tvelx: %.2f\tvely: %.2f\n", ix, iy, fOldX, fOldY, ty, ty2, ix/TILESIZE, (ix+PW)/TILESIZE, fx, fy, velx, vely);

    bool isMoveKeyDown = false;
    if (direction == 1)
        isMoveKeyDown = playerKeys->game_left.fDown;
    else
        isMoveKeyDown = playerKeys->game_right.fDown;

    if (direction == 1) {
        //moving left
        tx = (short)fx / TILESIZE;
    }
    else {
        //moving right
        if (fx + PW >= smw->ScreenWidth) {
            tx = (short)(fx + PW - smw->ScreenWidth) / TILESIZE;
            fOldX -= smw->ScreenWidth;
        } else
            tx = ((short)fx + PW) / TILESIZE;
    }

    //Just in case tx out of bounds and flipsidesifneeded wasn't called
    if (tx < 0)
        tx += 20;
    else if (tx > 19)
        tx -= 20;

    IO_Block * topblock = g_map->block(tx, ty);
    IO_Block * bottomblock = g_map->block(tx, ty2);

    int toptile = g_map->map(tx, ty);
    int bottomtile = g_map->map(tx, ty2);

    bool deathTileBehind;
    bool superDeathTileBehind;
    if (direction == 1) {
        deathTileBehind = ((toptile & tile_flag_death_on_right) && (bottomtile & tile_flag_death_on_right)) ||
                          ((toptile & tile_flag_death_on_right) && !(bottomtile & tile_flag_solid)) ||
                          (!(toptile & tile_flag_solid) && (bottomtile & tile_flag_death_on_right));

        superDeathTileBehind = ((toptile & tile_flag_super_or_player_death_right) && (bottomtile & tile_flag_super_or_player_death_right)) ||
                               ((toptile & tile_flag_super_or_player_death_right) && !(bottomtile & tile_flag_solid)) ||
                               (!(toptile & tile_flag_solid) && (bottomtile & tile_flag_super_or_player_death_right));
    }
    else {
        deathTileBehind = ((toptile & tile_flag_death_on_left) && (bottomtile & tile_flag_death_on_left)) ||
                          ((toptile & tile_flag_death_on_left) && !(bottomtile & tile_flag_solid)) ||
                          (!(toptile & tile_flag_solid) && (bottomtile & tile_flag_death_on_left));

        superDeathTileBehind = ((toptile & tile_flag_super_or_player_death_left) && (bottomtile & tile_flag_super_or_player_death_left)) ||
                               ((toptile & tile_flag_super_or_player_death_left) && !(bottomtile & tile_flag_solid)) ||
                               (!(toptile & tile_flag_solid) && (bottomtile & tile_flag_super_or_player_death_left));
    }

    bool fTopBlockSolid = topblock && !topblock->isTransparent() && !topblock->isHidden();
    bool fBottomBlockSolid = bottomblock && !bottomblock->isTransparent() && !bottomblock->isHidden();

    //first check to see if player hit a warp
    if (isMoveKeyDown && !frozen && g_map->checkforwarp(tx, ty, ty2, direction)) {
        if (direction == 1)
            setXf((float)((tx << 5) + TILESIZE) + 0.2f); // move to the edge of the tile
        else
            setXf((float)((tx << 5) - PW) - 0.2f);       // move to the edge of the tile (tile on the right -> mind the player width)

        warpstatus.enterWarp(*this, g_map->warp(tx, ty2));

        if (iy - PHOFFSET < (ty << 5))
            setYi((ty << 5) + PHOFFSET);
        else if (iy + PH > (ty2 << 5) + TILESIZE - 3)
            setYi((ty2 << 5) + TILESIZE - PH - 3);

        return;
    } else if (fTopBlockSolid || fBottomBlockSolid) {
        if (iHorizontalPlatformCollision == direction) {
            KillPlayerMapHazard(true, kill_style_environment, true, iPlatformCollisionPlayerId);
            return;
        }

        if (netplay.active && netplay.theHostIsMe)
            netplay.client.local_gamehost.prepareMapCollisionEvent(*this);

        bool collisionresult = true;

        if (fTopBlockSolid) { // collide with top block
            collisionresult &= topblock->collide(this, counter_direction,
                netplay.active ? netplay.theHostIsMe || netplay.allowMapCollisionEvent : true);
            flipsidesifneeded();
        }

        if (fBottomBlockSolid) { // then bottom
            collisionresult &= bottomblock->collide(this, counter_direction,
                netplay.active ? netplay.theHostIsMe || netplay.allowMapCollisionEvent : true);
            flipsidesifneeded();
        }

        if (netplay.active && netplay.theHostIsMe && collisionresult)
            netplay.client.local_gamehost.sendMapCollisionEvent();
    } else if (superDeathTileBehind || (deathTileBehind && !isInvincible() && !isShielded() && !shyguy)) {
        if (player_kill_nonkill != KillPlayerMapHazard(superDeathTileBehind, kill_style_environment, false))
            return;
    }
    //collision on the side.
    else if ((toptile & tile_flag_solid) || (bottomtile & tile_flag_solid)) { //collide with solid, ice, and death and all sides death
        if (iHorizontalPlatformCollision == direction) {
            KillPlayerMapHazard(true, kill_style_environment, true, iPlatformCollisionPlayerId);
            return;
        }

        if (direction == 1)
            setXf((float)((tx << 5) + TILESIZE) + 0.2f); // move to the edge of the tile
        else
            setXf((float)((tx << 5) - PW) - 0.2f);       // move to the edge of the tile (tile on the right -> mind the player width)

        fOldX = fx;

        if (fabs(velx) > 0.0f)
            velx = 0.0f;

        if (fabs(oldvelx) > 0.0f)
            oldvelx = 0.0f;

        flipsidesifneeded();
    }
}

void CPlayer::mapcolldet_moveUpward(short txl, short txc, short txr,
    short alignedBlockX, short unAlignedBlockX, float unAlignedBlockFX)
{
    // moving up
    fallthrough = false;

    short ty = (short)(fPrecalculatedY) / TILESIZE;

    IO_Block * leftblock = g_map->block(txl, ty);
    IO_Block * centerblock = g_map->block(txc, ty);
    IO_Block * rightblock = g_map->block(txr, ty);

    if (playerKeys->game_jump.fDown && !frozen && g_map->checkforwarp(alignedBlockX, unAlignedBlockX, ty, 2)) {
        setYf((float)((ty << 5) + TILESIZE) + 0.2f);
        warpstatus.enterWarp(*this, g_map->warp(unAlignedBlockX, ty));

        if (ix - PWOFFSET < (txl << 5) + 1)
            setXi((txl << 5) + PHOFFSET + 1);
        else if (ix + PW + PWOFFSET > (txr << 5) + TILESIZE)
            setXi((txr << 5) + TILESIZE - PW - PWOFFSET);

        return;
    }

    if (centerblock && !centerblock->isTransparent()) {
        if (netplay.active && netplay.theHostIsMe)
            netplay.client.local_gamehost.prepareMapCollisionEvent(*this);

        if (!centerblock->collide(this, 0,
            netplay.active ? netplay.theHostIsMe || netplay.allowMapCollisionEvent : true)) {
            if (iVerticalPlatformCollision == 2 && !centerblock->isHidden())
                KillPlayerMapHazard(true, kill_style_environment, true, iPlatformCollisionPlayerId);

            if (netplay.active && netplay.theHostIsMe)
                netplay.client.local_gamehost.sendMapCollisionEvent();

            return;
        }
    }

    //Player hit a solid, ice or death on top
    //or if the player is invincible and hits death or death on bottom

    //There is a known issue where where if a death on bottom tile and a super death on bottom tile
    //are next to each other and the player hits from below and aligns with the super death on bottom
    //the player will then shift over and fully hit the super death on bottom and die even if shielded
    //or invincible.

    int alignedTileType = g_map->map(alignedBlockX, ty);
    if ((alignedTileType & tile_flag_solid) && !(alignedTileType & tile_flag_super_or_player_death_bottom) &&
            (!(alignedTileType & tile_flag_death_on_bottom) || isInvincible() || isShielded() || shyguy)) {
        setYf((float)((ty << 5) + TILESIZE) + 0.2f);
        fOldY = fy - 1.0f;

        if (vely < 0.0f)
            vely = -vely * BOUNCESTRENGTH;

        if (iVerticalPlatformCollision == 2)
            KillPlayerMapHazard(true, kill_style_environment, true, iPlatformCollisionPlayerId);

        return;
    }

    if (leftblock && !leftblock->isTransparent()) { //then left
        bool useBehavior = alignedBlockX == txl || rightblock == NULL || rightblock->isTransparent() || rightblock->isHidden();

        if (netplay.active && netplay.theHostIsMe)
            netplay.client.local_gamehost.prepareMapCollisionEvent(*this);

        if (!leftblock->collide(this, 0,
            netplay.active ? (netplay.theHostIsMe || netplay.allowMapCollisionEvent) && useBehavior : useBehavior)) {
            if (iVerticalPlatformCollision == 2 && !leftblock->isHidden())
                KillPlayerMapHazard(true, kill_style_environment, true, iPlatformCollisionPlayerId);

            if (netplay.active && netplay.theHostIsMe)
                netplay.client.local_gamehost.sendMapCollisionEvent();

            return;
        }
    }

    if (rightblock && !rightblock->isTransparent()) { //then right
        bool useBehavior = alignedBlockX == txr || leftblock == NULL || leftblock->isTransparent() || leftblock->isHidden();

        if (netplay.active && netplay.theHostIsMe)
            netplay.client.local_gamehost.prepareMapCollisionEvent(*this);

        if (!rightblock->collide(this, 0,
            netplay.active ? (netplay.theHostIsMe || netplay.allowMapCollisionEvent) && useBehavior : useBehavior)) {
            if (iVerticalPlatformCollision == 2 && !rightblock->isHidden())
                KillPlayerMapHazard(true, kill_style_environment, true, iPlatformCollisionPlayerId);

            if (netplay.active && netplay.theHostIsMe)
                netplay.client.local_gamehost.sendMapCollisionEvent();

            return;
        }
    }

    //Player squeezed around the block, ice or death on top
    //or if the player is invincible and hits death or death on bottom
    int unalignedTileType = g_map->map(unAlignedBlockX, ty);
    if ((unalignedTileType & tile_flag_solid) && !(unalignedTileType & tile_flag_super_or_player_death_bottom) &&
            (!(unalignedTileType & tile_flag_death_on_bottom) || isInvincible() || isShielded() || shyguy)) {
        setXf(unAlignedBlockFX);
        fOldX = fx;

        setYf(fPrecalculatedY);
        vely += GRAVITATION;
    } else if ((alignedTileType & tile_flag_player_or_death_on_bottom) || (unalignedTileType & tile_flag_player_or_death_on_bottom)) {
        bool fRespawnPlayer = ((alignedTileType & tile_flag_super_or_player_death_bottom) && (unalignedTileType & tile_flag_super_or_player_death_bottom)) ||
                              ((alignedTileType & tile_flag_super_or_player_death_bottom) &&  !(unalignedTileType & tile_flag_solid)) ||
                              ((alignedTileType & tile_flag_solid) && !(unalignedTileType & tile_flag_super_or_player_death_bottom));

        if (player_kill_nonkill != KillPlayerMapHazard(fRespawnPlayer, kill_style_environment, false))
            return;
    } else {
        setYf(fPrecalculatedY);
        vely += GRAVITATION;
    }

    if (!platform) {
        inair = true;
        onice = false;
    }
}

void CPlayer::mapcolldet_moveDownward(short txl, short txc, short txr,
    short alignedBlockX, short unAlignedBlockX, float unAlignedBlockFX)
{
    // moving down / on ground
    short ty = ((short)fPrecalculatedY + PH) / TILESIZE;

    if (playerKeys->game_down.fDown && !frozen && g_map->checkforwarp(txl, txr, ty, 0)) {
        setYf((float)((ty << 5) - PH) - 0.2f);
        warpstatus.enterWarp(*this, g_map->warp(txr,ty));

        fallthrough = false;
        platform = NULL;

        if (ix - PWOFFSET < (txl << 5) + 1)
            setXi((txl << 5) + PHOFFSET + 1);
        else if (ix + PW + PWOFFSET > (txr << 5) + TILESIZE)
            setXi((txr << 5) + TILESIZE - PW - PWOFFSET);

        return;
    }

    IO_Block * leftblock = g_map->block(txl, ty);
    IO_Block * rightblock = g_map->block(txr, ty);

    bool fLeftBlockSolid = leftblock && !leftblock->isTransparent() && !leftblock->isHidden();
    bool fRightBlockSolid = rightblock && !rightblock->isTransparent() && !rightblock->isHidden();

    if (fLeftBlockSolid || fRightBlockSolid) {
        bool collisionresult = true;
        if (netplay.active && netplay.theHostIsMe)
            netplay.client.local_gamehost.prepareMapCollisionEvent(*this);

        if (fLeftBlockSolid) { //collide with left block
            bool useBehavior = alignedBlockX == txl || rightblock == NULL || rightblock->isTransparent() || rightblock->isHidden();
            collisionresult &= leftblock->collide(this, 2,
                netplay.active ? (netplay.theHostIsMe || netplay.allowMapCollisionEvent) && useBehavior : useBehavior);

            if (netplay.active && netplay.theHostIsMe && collisionresult)
                netplay.client.local_gamehost.sendMapCollisionEvent();

            //If player was bumped and killed then return
            if (state != player_ready)
                return;
        }

        if (fRightBlockSolid) { //then right
            bool useBehavior = alignedBlockX == txr || leftblock == NULL || leftblock->isTransparent() || leftblock->isHidden();
            collisionresult &= rightblock->collide(this, 2,
                netplay.active ? (netplay.theHostIsMe || netplay.allowMapCollisionEvent) && useBehavior : useBehavior);

            if (netplay.active && netplay.theHostIsMe && collisionresult)
                netplay.client.local_gamehost.sendMapCollisionEvent();

            //If player was bumped and killed then return
            if (state != player_ready)
                return;
        }

        if (!collisionresult) {
            platform = NULL;
            onice = false;

            if (iVerticalPlatformCollision == 0)
                KillPlayerMapHazard(true, kill_style_environment, true, iPlatformCollisionPlayerId);

            return;
        }
    }

    int lefttile = g_map->map(txl, ty);
    int righttile = g_map->map(txr, ty);

    bool fGapSupport = (velx >= VELTURBOMOVING || velx <= -VELTURBOMOVING) && (lefttile == tile_flag_gap || righttile == tile_flag_gap);

    bool fSolidTileUnderPlayer = (lefttile & tile_flag_solid)  || (righttile & tile_flag_solid);

    if ((lefttile & tile_flag_solid_on_top || righttile & tile_flag_solid_on_top || fGapSupport) && fOldY + PH <= (ty << 5)) {
        //on ground
        //Deal with player down jumping through solid on top tiles

        if (!platform)
            onice = false;

        if (fallthrough && !fSolidTileUnderPlayer) {
            setYf((float)((ty << 5) - PH) + 0.2f);

            if (!platform) {
                inair = true;
            }
        } else {
            //we were above the tile in the previous frame
            setYf((float)((ty << 5) - PH) - 0.2f);
            vely = GRAVITATION;

            if (!platform) {
                int alignedtile = g_map->map(alignedBlockX, ty);

                if (alignedtile & tile_flag_ice || ((alignedtile == tile_flag_nonsolid || alignedtile == tile_flag_gap) && g_map->map(unAlignedBlockX, ty) & tile_flag_ice))
                    onice = true;
                else
                    onice = false;

                inair = false;
                extrajumps = 0;
                killsinrowinair = 0;
            }
        }

        fOldY = fy - GRAVITATION;

        if (!platform)
            fallthrough = false;

        platform = NULL;

        if (iVerticalPlatformCollision == 0)
            KillPlayerMapHazard(true, kill_style_environment, true, iPlatformCollisionPlayerId);

        return;
    }

    bool fDeathTileUnderPlayer = ((lefttile & tile_flag_death_on_top) && (righttile & tile_flag_death_on_top)) ||
                                 ((lefttile & tile_flag_death_on_top) && !(righttile & tile_flag_solid)) ||
                                 (!(lefttile & tile_flag_solid) && (righttile & tile_flag_death_on_top));

    bool fSuperDeathTileUnderPlayer = ((lefttile & tile_flag_super_or_player_death_top) && (righttile & tile_flag_super_or_player_death_top)) ||
                                      ((lefttile & tile_flag_super_or_player_death_top) && !(righttile & tile_flag_solid)) ||
                                      (!(lefttile & tile_flag_solid) && (righttile & tile_flag_super_or_player_death_top));

    if (fSolidTileUnderPlayer && !fSuperDeathTileUnderPlayer &&
            (!fDeathTileUnderPlayer || IsInvincibleOnBottom() || shyguy) ) {
        //on ground

        setYf((float)((ty << 5) - PH) - 0.2f);
        vely = GRAVITATION;				//1 so we test against the ground again int the next frame (0 would test against the ground in the next+1 frame)

        if (!platform) {
            int alignedtile = g_map->map(alignedBlockX, ty);

            if (alignedtile & tile_flag_ice || ((alignedtile == tile_flag_nonsolid || alignedtile == tile_flag_gap) && g_map->map(unAlignedBlockX, ty) & tile_flag_ice))
                onice = true;
            else
                onice = false;

            inair = false;
            extrajumps = 0;
            killsinrowinair = 0;
        }

        platform = NULL;

        if (iVerticalPlatformCollision == 0) {
            KillPlayerMapHazard(true, kill_style_environment, true, iPlatformCollisionPlayerId);
            return;
        }
    } else if (fDeathTileUnderPlayer || fSuperDeathTileUnderPlayer) {
        if (player_kill_nonkill != KillPlayerMapHazard(fSuperDeathTileUnderPlayer, kill_style_environment, false))
            return;
    } else {
        //falling (in air)
        setYf(fPrecalculatedY);
        vely = CapFallingVelocity(GRAVITATION + vely);

        if (!platform) {
            //If we're not hopping with kuribo's shoe, then zero out our super jump timer
            if (superjumptype != 3)
                superjumptimer = 0;

            inair = true;
        }
    }
}

void CPlayer::collision_detection_map()
{
    setXf(fx + velx);
    flipsidesifneeded();

    fPrecalculatedY = fy + vely;  //Fixes weird float rounding error.  Must be computed here before casting to int.  Otherwise, this will miss the bottom collision, but then hit the side collision and the player can slide out of 1x1 spaces.

    float fPlatformVelX = 0.0f;
    float fPlatformVelY = 0.0f;

    float fTempY = fy;

    mapcolldet_handlePlatformVelocity(fPlatformVelX, fPlatformVelY);

    iHorizontalPlatformCollision = -1;
    iVerticalPlatformCollision = -1;
    iPlatformCollisionPlayerId = -1;

    g_map->movingPlatformCollision(this);

    if (state != player_ready)
        return;

    fy = fTempY;

    if (mapcolldet_handleOutOfScreen())
        return;

    //-----------------------------------------------------------------
    //  x axis (--)
    //-----------------------------------------------------------------
    if (fy + PH >= 0.0f) {
        if (velx + fPlatformVelX > 0.01f || iHorizontalPlatformCollision == 3)
            mapcolldet_moveHorizontally(3);
        else if (velx + fPlatformVelX < -0.01f || iHorizontalPlatformCollision == 1)
            mapcolldet_moveHorizontally(1);

        if (isdead())
            return;
    }

    //-----------------------------------------------------------------
    //  then y axis (|)
    //-----------------------------------------------------------------

    short txl = 0, txr = 0, txc = 0;
    short iPlayerL = ix, iPlayerC = ix + HALFPW, iPlayerR = ix + PW;

    if (iPlayerL < 0)
        iPlayerL += smw->ScreenWidth;
    else if (iPlayerL >= smw->ScreenWidth)
        iPlayerL -= smw->ScreenWidth;

    if (iPlayerC >= smw->ScreenWidth)
        iPlayerC -= smw->ScreenWidth;

    if (iPlayerR >= smw->ScreenWidth)
        iPlayerR -= smw->ScreenWidth;

    txl = iPlayerL / TILESIZE;
    txc = iPlayerC / TILESIZE;
    txr = iPlayerR / TILESIZE;

    //What block is the player aligned to (this will be the block that has the action on it)
    short alignedBlockX = 0;
    short unAlignedBlockX = 0;
    float unAlignedBlockFX = 0;

    //printf("Before Y - ix: %d\tiy: %d\toldx: %.2f\toldy: %d\tty: %.2f\tty2: %d\ttxl: %d\ttxr: %d\tfx: %.2f\tfy: %.2f\tvelx: %.2f\tvely: %.2f\n", ix, iy, fOldX, fOldY, (int)(fPrecalculatedY) / TILESIZE, ((int)(fPrecalculatedY) + PH) / TILESIZE, txl, txr, fx, fy, velx, vely);

    short overlaptxl = (txl << 5) + TILESIZE + 1;

    if (ix + HALFPW < overlaptxl) {
        alignedBlockX = txl;
        unAlignedBlockX = txr;
        unAlignedBlockFX = (float)((txr << 5) - PW) - 0.2f;
    } else {
        alignedBlockX = txr;
        unAlignedBlockX = txl;
        unAlignedBlockFX = (float)((txl << 5) + TILESIZE) + 0.2f;
    }

    float fMovingUp = vely;
    if (platform)
        fMovingUp = vely + fPlatformVelY - GRAVITATION;

    if (fMovingUp < -0.01f) {
        //moving up
        mapcolldet_moveUpward(txl, txc, txr, alignedBlockX, unAlignedBlockX, unAlignedBlockFX);
    } else {
        //moving down / on ground
        mapcolldet_moveDownward(txl, txc, txr, alignedBlockX, unAlignedBlockX, unAlignedBlockFX);
    }

    if (!platform) {
        fallthrough = false;

        if (inair)
            onice = false;
    }

    //printf("After Y - ix: %d\tiy: %d\toldx: %.2f\toldy: %.2f\tty: %d\tty2: %d\ttxl: %d\ttxr: %d\tfx: %.2f\tfy: %.2f\tvelx: %.2f\tvely: %.2f\n\n", ix, iy, fOldX, fOldY, ty, ty, txl, txr, fx, fy, velx, vely);
}

//iPlayerIdCredit is passed in if this platform was triggered by another player and crushed this player (e.g. donut block)
short CPlayer::KillPlayerMapHazard(bool fForce, killstyle style, bool fKillCarriedItem, short iPlayerIdCredit)
{
    if (iPlayerIdCredit >= 0 || iSuicideCreditPlayerID >= 0) {
        return PlayerKilledPlayer(iPlayerIdCredit >= 0 ? iPlayerIdCredit : iSuicideCreditPlayerID, this, death_style_jump, kill_style_push, fForce, fKillCarriedItem);
    } else {
        DeathAwards();

        short iKillType = game_values.gamemode->playerkilledself(*this, style);
        if (player_kill_normal == iKillType || (player_kill_nonkill == iKillType && fForce))
            die(death_style_jump, false, fKillCarriedItem);

        ifSoundOnPlay(rm->sfx_deathsound);

        return iKillType;
    }
}

void CPlayer::collidesWith(CPlayer* other)
{
    collisions.handle_p2p(this, other);
}

bool CPlayer::collidesWith(CObject* object)
{
    return collisions.handle_p2o(this, object);
}

void CPlayer::flipsidesifneeded()
{
    //Use ix here to avoid rounding issues (can crash if tile_x_right evals to over the right side of screen)
    if (ix < 0 || fx < 0.0f) {
        //This avoids rounding errors
        setXf(fx + 640);
        fOldX += 640;
        //printf("Flipped Left\n");
    } else if (ix >= 640 || fx >= 640) {
        setXf(fx - 640);
        fOldX -= 640;
        //printf("Flipped Right\n");
    }
}

void CPlayer::makeinvincible()
{
    invincibility.turn_on(*this);
}

void CPlayer::makefrozen(short iTime)
{
    if (!frozen) {
        frozen = true;
        frozentimer = iTime;

        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix + HALFPW - 16, iy + HALFPH - 16, 3, 8));
    }
}

void CPlayer::turnslowdownon()
{
    game_values.slowdownon = teamID;
    game_values.slowdowncounter = 0;
}

//Returns true if player facing right, false if left
bool CPlayer::isFacingRight() const
{
    bool fLeft = game_values.reversewalk;
    bool fRight = !fLeft;

    if (game_values.swapplayers && game_values.swapstyle == 0) {
        if (fNewSwapX < fOldSwapX)
            return fLeft;
        else
            return fRight;
    }

    if (state == player_ready) {
        if (playerKeys->game_left.fDown && playerKeys->game_right.fDown && velx != 0.0f) {
            if (velx > 0.0f)
                return fRight;
            else
                return fLeft;
        } else {
            if (playerKeys->game_left.fDown)
                return fLeft;
            else if (playerKeys->game_right.fDown)
                return fRight;
        }
    }

    if (sprite_state == PGFX_STOPPING_R)
        return false;
    else if (sprite_state == PGFX_STOPPING_L)
        return true;
    else if (!(sprite_state & 0x1))
        return true;

    return false;
}

bool CPlayer::AcceptItem(MO_CarriedObject * item)
{
    if (fAcceptingItem && tanookisuit.notStatue() && (!kuriboshoe.is_on() || item->IsCarriedByKuriboShoe())) {
        action = player_action_none;

        carriedItem = item;
        item->owner = this;

        fAcceptingItem = false;
        return true;
    }

    return false;
}

void CPlayer::SetPowerup(short iPowerup)
{
    if (shyguy) {
        ifSoundOnPlay(rm->sfx_stun);
        return;
    }

    if (iPowerup == 0) {
        if (bobomb)
            SetStoredPowerup(8);
        else {
            ifSoundOnPlay(rm->sfx_transform);
            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_poof, ix + HALFPW - 24, iy + HALFPH - 24, 4, 5));
            bobomb = true;
        }
    } else if (iPowerup == 9) {
        if (tanookisuit.isOn())
            SetStoredPowerup(20);
        else
            tanookisuit.onPickup();
    } else if (iPowerup >= 10) {
        if (iPowerup == 10)
            SetStoredPowerup(9);   //POW
        else if (iPowerup == 11)
            SetStoredPowerup(16);  //MOd
        else if (iPowerup == 12)
            SetStoredPowerup(10);  //Bullet Bill
        else if (iPowerup == 13)
            SetStoredPowerup(22);  //Podobo
        else if (iPowerup >= 14)
            SetStoredPowerup(iPowerup - 2); //Storing shells
    } else {
        if (iPowerup == 3 || iPowerup == 7 || iPowerup == 8) {
            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix + HALFPW - 16, iy + HALFPH - 16, 3, 8));
        }

        if (powerup != iPowerup) {
            if (iPowerup == 3)
                ifSoundOnPlay(rm->sfx_collectfeather);
            else
                ifSoundOnPlay(rm->sfx_collectpowerup);
        }

        ClearPowerupStates();

        if (powerup == 1)
            SetStoredPowerup(5);
        else if (powerup == 2)
            SetStoredPowerup(11);
        else if (powerup == 3)
            SetStoredPowerup(17);
        else if (powerup == 4)
            SetStoredPowerup(19);
        else if (powerup == 5)
            SetStoredPowerup(21);
        else if (powerup == 6)
            SetStoredPowerup(23);
        else if (powerup == 7)
            SetStoredPowerup(24);
        else if (powerup == 8)
            SetStoredPowerup(25);

        powerup = iPowerup;
        projectilelimit = 0;

        flying = false;

        if (powerup == 1) {
            if (game_values.fireballlimit > 0)
                projectilelimit = game_values.fireballlimit;
        } else if (powerup == 2) {
            if (game_values.hammerlimit > 0)
                projectilelimit = game_values.hammerlimit;
        } else if (powerup == 3) {
            if (game_values.featherlimit > 0)
                projectilelimit = game_values.featherlimit;
        } else if (powerup == 4) {
            if (game_values.boomeranglimit > 0)
                projectilelimit = game_values.boomeranglimit;
        } else if (powerup == 5) {
            if (game_values.wandlimit > 0)
                projectilelimit = game_values.wandlimit;
        } else if (powerup == 6) {
            if (game_values.bombslimit > 0)
                projectilelimit = game_values.bombslimit;
        } else if (powerup == 7) {
            if (game_values.leaflimit > 0)
                projectilelimit = game_values.leaflimit;
        } else if (powerup == 8) {
            if (game_values.pwingslimit > 0)
                projectilelimit = game_values.pwingslimit;
        }
    }

    //Minor fix for becoming caped to draw animation correctly
    if (iPowerup == 3)
        cape.restart_animation();
}

void CPlayer::SetStoredPowerup(short iPowerup)
{
    //If the player as the poison mushroom or the game is over, don't store the powerup
    if (game_values.gamepowerups[globalID] == 0 || game_values.gamemode->gameover) {
        ifSoundOnPlay(rm->sfx_stun);
        return;
    }

    game_values.gamepowerups[globalID] = iPowerup;
    ifSoundOnPlay(rm->sfx_storepowerup);
}

void CPlayer::ClearPowerupStates()
{
    tail.reset();
    spin.reset();
    wings.reset();

    flying = false;
    flyingtimer = 0;
}

void CPlayer::DecreaseProjectileLimit()
{
    if (--projectilelimit <= 0) {
        projectilelimit = 0;
        powerup = -1;
        ifSoundOnPlay(rm->sfx_powerdown);
    }
}
