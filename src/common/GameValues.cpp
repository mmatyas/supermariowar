#include "GameValues.h"

#include "FileIO.h"
#include "FileList.h"
#include "GameMode.h"
#include "GlobalConstants.h"
#include "MapList.h" // req. only by WriteConfig
#include "sfx.h"

#include <stdio.h>

// TODO: Refactor externs

extern CGameMode * gamemodes[GAMEMODE_LAST];

extern FiltersList *filterslist;
extern MapList *maplist;
extern SkinList *skinlist;
extern AnnouncerList *announcerlist;
extern MusicList *musiclist;
extern WorldMusicList *worldmusiclist;
extern GraphicsList *menugraphicspacklist;
extern GraphicsList *worldgraphicspacklist;
extern GraphicsList *gamegraphicspacklist;
extern SoundsList *soundpacklist;

extern short joystickcount;
extern short g_iCurrentPowerupPresets[NUM_POWERUP_PRESETS][NUM_POWERUPS];

extern int g_iVersion[];
extern bool VersionIsEqual(int iVersion[], short iMajor, short iMinor, short iMicro, short iBuild);
extern bool VersionIsEqualOrBefore(int iVersion[], short iMajor, short iMinor, short iMicro, short iBuild);
extern bool VersionIsEqualOrAfter(int iVersion[], short iMajor, short iMinor, short iMicro, short iBuild);


//[Keyboard/Joystick][Game/Menu][NumPlayers][NumKeys]  left, right, jump, down, turbo, powerup, start, cancel
SDL_KEYTYPE controlkeys[2][2][4][NUM_KEYS] = { { { {SDLK_LEFT, SDLK_RIGHT, SDLK_UP, SDLK_DOWN, SDLK_RCTRL, SDLK_RSHIFT, SDLK_RETURN, SDLK_ESCAPE},
            {SDLK_a, SDLK_d, SDLK_w, SDLK_s, SDLK_e, SDLK_q, SDLK_UNKNOWN, SDLK_UNKNOWN},
            {SDLK_g, SDLK_j, SDLK_y, SDLK_h, SDLK_u, SDLK_t, SDLK_UNKNOWN, SDLK_UNKNOWN},
            {SDLK_l, SDLK_QUOTE, SDLK_p, SDLK_SEMICOLON, SDLK_LEFTBRACKET, SDLK_o, SDLK_UNKNOWN, SDLK_UNKNOWN}
        },

        //up, down, left, right, select, cancel, random, fast scroll
        {   {SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT, SDLK_RETURN, SDLK_ESCAPE, SDLK_SPACE,  SDLK_LSHIFT},
            {SDLK_w, SDLK_s, SDLK_a, SDLK_d, SDLK_e, SDLK_q, SDLK_UNKNOWN, SDLK_UNKNOWN},
            {SDLK_y, SDLK_h, SDLK_g, SDLK_j, SDLK_u, SDLK_t, SDLK_UNKNOWN, SDLK_UNKNOWN},
            {SDLK_p, SDLK_SEMICOLON, SDLK_l, SDLK_QUOTE, SDLK_LEFTBRACKET, SDLK_o, SDLK_UNKNOWN, SDLK_UNKNOWN}
        }
    },
#ifdef _XBOX
    //left, right, jump, down, turbo, powerup, start, cancel
    {   {   {JOY_HAT_LEFT, JOY_HAT_RIGHT, 12, JOY_HAT_DOWN, 14, 15, 20, 21},
            {JOY_HAT_LEFT, JOY_HAT_RIGHT, 12, JOY_HAT_DOWN, 14, 15, 20, 21},
            {JOY_HAT_LEFT, JOY_HAT_RIGHT, 12, JOY_HAT_DOWN, 14, 15, 20, 21},
            {JOY_HAT_LEFT, JOY_HAT_RIGHT, 12, JOY_HAT_DOWN, 14, 15, 20, 21}
        },

        //up, down, left, right, select, cancel, random, fast scroll
        {   {JOY_HAT_UP, JOY_HAT_DOWN, JOY_HAT_LEFT, JOY_HAT_RIGHT, 12, 21, 14, 15},
            {JOY_HAT_UP, JOY_HAT_DOWN, JOY_HAT_LEFT, JOY_HAT_RIGHT, 12, 21, 14, 15},
            {JOY_HAT_UP, JOY_HAT_DOWN, JOY_HAT_LEFT, JOY_HAT_RIGHT, 12, 21, 14, 15},
            {JOY_HAT_UP, JOY_HAT_DOWN, JOY_HAT_LEFT, JOY_HAT_RIGHT, 12, 21, 14, 15}
        }
    }
};
#else
    //left, right, jump, down, turbo, powerup, start, cancel;
    { { {JOY_STICK_1_LEFT, JOY_STICK_1_RIGHT, JOY_BUTTON_START, JOY_STICK_1_DOWN, JOY_BUTTON_START + 1, JOY_BUTTON_START + 2, JOY_BUTTON_START + 3, JOY_BUTTON_START + 4},
            {JOY_STICK_1_LEFT, JOY_STICK_1_RIGHT, JOY_BUTTON_START, JOY_STICK_1_DOWN, JOY_BUTTON_START + 1, JOY_BUTTON_START + 2, JOY_BUTTON_START + 3, JOY_BUTTON_START + 4},
            {JOY_STICK_1_LEFT, JOY_STICK_1_RIGHT, JOY_BUTTON_START, JOY_STICK_1_DOWN, JOY_BUTTON_START + 1, JOY_BUTTON_START + 2, JOY_BUTTON_START + 3, JOY_BUTTON_START + 4},
            {JOY_STICK_1_LEFT, JOY_STICK_1_RIGHT, JOY_BUTTON_START, JOY_STICK_1_DOWN, JOY_BUTTON_START + 1, JOY_BUTTON_START + 2, JOY_BUTTON_START + 3, JOY_BUTTON_START + 4}
        },

        //up, down, left, right, select, cancel, random, fast scroll
        {   {JOY_STICK_1_UP, JOY_STICK_1_DOWN, JOY_STICK_1_LEFT, JOY_STICK_1_RIGHT, JOY_BUTTON_START, JOY_BUTTON_START + 1, JOY_BUTTON_START + 2, JOY_BUTTON_START + 3},
            {JOY_STICK_1_UP, JOY_STICK_1_DOWN, JOY_STICK_1_LEFT, JOY_STICK_1_RIGHT, JOY_BUTTON_START, JOY_BUTTON_START + 1, JOY_BUTTON_START + 2, JOY_BUTTON_START + 3},
            {JOY_STICK_1_UP, JOY_STICK_1_DOWN, JOY_STICK_1_LEFT, JOY_STICK_1_RIGHT, JOY_BUTTON_START, JOY_BUTTON_START + 1, JOY_BUTTON_START + 2, JOY_BUTTON_START + 3},
            {JOY_STICK_1_UP, JOY_STICK_1_DOWN, JOY_STICK_1_LEFT, JOY_STICK_1_RIGHT, JOY_BUTTON_START, JOY_BUTTON_START + 1, JOY_BUTTON_START + 2, JOY_BUTTON_START + 3}
        }
    }
};
#endif

void fread_or_exception(void * ptr, size_t size, size_t count, FILE * stream)
{
    if (fread(ptr, size, count, stream) != count)
        throw "File read error";
}

void fwrite_or_exception(const void * ptr, size_t size, size_t count, FILE * stream)
{
    if (fwrite(ptr, size, count, stream) != count)
        throw "File write error";
}

void CGameValues::init()
{
    //set standard game values
    playercontrol[0]  = 1;
    playercontrol[1]  = 1;
    showfps       = false;
    frameadvance    = false;
    autokill      = false;
    framelimiter    = WAITTIME;
    sound       = true;
    music       = true;
    gamestate     = GS_SPLASH;
#ifdef _DEBUG
    fullscreen      = false;
#else
    fullscreen      = false;
#endif

    screenResizeX   = 20.0f;
    screenResizeY   = 20.0f;
    screenResizeW   = -40.0f;
    screenResizeH   = -40.0f;

    flickerfilter   = 5;  //Full flicker filter by default
    hardwarefilter    = 2;  //Bilinear by default
    softfilter      = 0;  //No soft filter by default
    aspectratio10x11  = false;  //No 10x11 aspect ratio by default

#ifdef _XBOX
    SDL_XBOX_SetScreenPosition(screenResizeX, screenResizeY);
    SDL_XBOX_SetScreenStretch(screenResizeW, screenResizeH);
#endif

    pausegame     = false;
    exitinggame     = false;
    exityes       = false;
    awardstyle      = award_style_fireworks;
    spawnstyle      = 2;
    tournamentgames   = 2;
    tournamentwinner  = -1;
    selectedminigame  = 0;
    matchtype     = MATCH_TYPE_SINGLE_GAME;
    tourindex     = 0;
    tourstopcurrent   = 0;
    tourstoptotal   = 0;
    worldindex      = 0;
    slowdownon      = -1;
    slowdowncounter   = 0;
    teamcollision   = 0;
    screencrunch    = true;
    screenshaketimer  = 0;
    screenshakeplayerid = -1;
    screenshaketeamid = -1;
    toplayer      = true;
    loadedannouncer   = -1;
    loadedmusic     = -1;
    scoreboardstyle     = 0;
    teamcolors          = true;
    cputurn       = -1;
    shieldtime      = 62;
    shieldstyle     = 2;
    musicvolume     = 128;
    soundvolume     = 128;
    respawn       = 2;
    itemrespawntime   = 1860;  //default item respawn is 30 seconds (30 * 62 fps)
    hiddenblockrespawn  = 1860;  //default item respawn is 30 seconds
    outofboundstime   = 5;
    warplockstyle   = 1;  // Lock Warp Exit Only
    warplocktime    = 186;  // 3 seconds
#ifdef _DEBUG
    suicidetime     = 0;  // Turn off suicide kills for debug
#else
    suicidetime     = 310;  // 5 seconds
#endif
    cpudifficulty   = 2;
    fireballttl     = 310;  // 5 seconds
    shellttl      = 496;  // 8 seconds
    blueblockttl    = 310;  // 5 seconds
    redblockttl     = 310;  // 5 seconds
    grayblockttl    = 310;  // 5 seconds
    hammerdelay     = 25; // 0.4 second
    hammerttl     = 49; // 0.8 second
    hammerpower     = true; //hammers die on first hit
    fireballlimit   = 0;  //Unlimited
    hammerlimit     = 0;  //Unlimited
    boomerangstyle    = 1;  //SMB3 style
    boomeranglife   = 248;  // 4 seconds of zelda boomerang
    boomeranglimit    = 0;  //Unlimited
    featherjumps    = 1;  //Allow one extra cape jump
    featherlimit    = 0;  //Unlimited
    leaflimit     = 0;  //Unlimited
    pwingslimit     = 0;  //Unlimited
    tanookilimit    = 0;  //Unlimited
    bombslimit      = 0;  //Unlimited
    wandfreezetime    = 310;  //5 seconds of freeze time
    wandlimit     = 0;  //Unlimited
    storedpowerupdelay  = 4;
    bonuswheel      = 1;
    keeppowerup     = false;
    showwinningcrown  = false;
    startgamecountdown  = true;
    startmodedisplay  = true;
    deadteamnotice    = true;
    playnextmusic   = false;
    pointspeed      = 20;
    swapstyle     = 1;  //Blink then swap
    worldpointsbonus  = -1; //no world multiplier until player uses item to boost it
    singleplayermode  = -1;
    worldskipscoreboard = false;
    overridepowerupsettings = 0;
    minigameunlocked  = false;
    poweruppreset   = 0;
    tournamentcontrolstyle = 0;


    pfFilters     = new bool[NUM_AUTO_FILTERS + filterslist->GetCount()];
    piFilterIcons   = new short[NUM_AUTO_FILTERS + filterslist->GetCount()];
    fNeedWriteFilters = false;

    for (short iFilter = 0; iFilter < NUM_AUTO_FILTERS + filterslist->GetCount(); iFilter++) {
        pfFilters[iFilter] = false;
        piFilterIcons[iFilter] = 0;
    }

    //networktype   = 0;
    //networkhost   = false;
    //gamehost      = false;

    for (short iPlayer = 0; iPlayer < 4; iPlayer++) {
        storedpowerups[iPlayer] = -1;
        gamepowerups[iPlayer] = -1;
        teamids[iPlayer][0] = iPlayer;
        teamcounts[iPlayer] = 1;
        skinids[iPlayer] = 0;
        colorids[iPlayer] = iPlayer;
        randomskin[iPlayer] = false;

        //Setup the default key/button input configurations
        for (short iInputType = 0; iInputType < 2; iInputType++) { //for keyboard/joystick
            inputConfiguration[iPlayer][iInputType].iDevice = iInputType - 1;

            for (short iInputState = 0; iInputState < 2; iInputState++) { //for game/menu
                for (short iKey = 0; iKey < NUM_KEYS; iKey++) {
                    inputConfiguration[iPlayer][iInputType].inputGameControls[iInputState].keys[iKey] = controlkeys[iInputType][iInputState][iPlayer][iKey];
                }
            }
        }

        //Set the players input to the default configuration (will be overwritten by options.bin settings)
#ifdef _XBOX
        inputConfiguration[iPlayer][1].iDevice = iPlayer;
        playerInput.inputControls[iPlayer] = &inputConfiguration[iPlayer][1];
#else
        playerInput.inputControls[iPlayer] = &inputConfiguration[iPlayer][0];
#endif
    }
}

void CGameValues::ReadBinaryConfig() {
    //Read saved settings from disk
    FILE * fp = OpenFile("options.bin", "rb");

    if (fp) {
      try
      {
        int version[4];
        fread_or_exception(version, sizeof(int), 4, fp);

        if (VersionIsEqual(g_iVersion, version[0], version[1], version[2], version[3])) {
#ifdef _XBOX
            fread_or_exception(&flickerfilter, sizeof(short), 1, fp);
            fread_or_exception(&hardwarefilter, sizeof(short), 1, fp);
            fread_or_exception(&softfilter, sizeof(short), 1, fp);
            fread_or_exception(&aspectratio10x11, sizeof(bool), 1, fp);

            fread_or_exception(&screenResizeX, sizeof(float), 1, fp);
            fread_or_exception(&screenResizeY, sizeof(float), 1, fp);
            fread_or_exception(&screenResizeW, sizeof(float), 1, fp);
            fread_or_exception(&screenResizeH, sizeof(float), 1, fp);

            SDL_XBOX_SetScreenPosition(screenResizeX, screenResizeY);
            SDL_XBOX_SetScreenStretch(screenResizeW, screenResizeH);
#endif

            unsigned char abyte[35];
            fread_or_exception(abyte, sizeof(unsigned char), 35, fp);
            spawnstyle = (short) abyte[0];
            awardstyle = (short) abyte[1];
            teamcollision = (short)abyte[3];
            screencrunch = ((short)abyte[4] > 0 ? true : false);
            toplayer = ((short)abyte[5] > 0 ? true : false);
            scoreboardstyle = (short)abyte[6];
            teamcolors = ((short)abyte[7] > 0 ? true : false);
            sound = ((short)abyte[8] > 0 ? true : false);
            music = ((short)abyte[9] > 0 ? true : false);
            musicvolume = (short)abyte[10];
            soundvolume = (short)abyte[11];
            respawn = (short)abyte[12];
            outofboundstime = (short)abyte[15];
            cpudifficulty = (short)abyte[16];
            framelimiter = (short)abyte[19];
            bonuswheel = (short)abyte[20];
            keeppowerup = ((short)abyte[21] > 0 ? true : false);
            showwinningcrown = ((short)abyte[22] > 0 ? true : false);
            playnextmusic = ((short)abyte[23] > 0 ? true : false);
            pointspeed = (short)abyte[24];
            swapstyle = (short)abyte[25];
            overridepowerupsettings = (short)abyte[28];
            minigameunlocked = ((short)abyte[29] > 0 ? true : false);
            startgamecountdown = ((short)abyte[30] > 0 ? true : false);
            deadteamnotice = ((short)abyte[31] > 0 ? true : false);
            tournamentcontrolstyle = (short)abyte[33];
            startmodedisplay = ((short)abyte[34] > 0 ? true : false);

            fread_or_exception(&shieldtime, sizeof(short), 1, fp);
            fread_or_exception(&shieldstyle, sizeof(short), 1, fp);
            fread_or_exception(&itemrespawntime, sizeof(short), 1, fp);
            fread_or_exception(&hiddenblockrespawn, sizeof(short), 1, fp);
            fread_or_exception(&fireballttl, sizeof(short), 1, fp);
            fread_or_exception(&fireballlimit, sizeof(short), 1, fp);
            fread_or_exception(&hammerdelay, sizeof(short), 1, fp);
            fread_or_exception(&hammerttl, sizeof(short), 1, fp);
            fread_or_exception(&hammerpower, sizeof(bool), 1, fp);
            fread_or_exception(&hammerlimit, sizeof(short), 1, fp);
            fread_or_exception(&boomerangstyle, sizeof(short), 1, fp);
            fread_or_exception(&boomeranglife, sizeof(short), 1, fp);
            fread_or_exception(&boomeranglimit, sizeof(short), 1, fp);
            fread_or_exception(&featherjumps, sizeof(short), 1, fp);
            fread_or_exception(&featherlimit, sizeof(short), 1, fp);
            fread_or_exception(&leaflimit, sizeof(short), 1, fp);
            fread_or_exception(&pwingslimit, sizeof(short), 1, fp);
            fread_or_exception(&tanookilimit, sizeof(short), 1, fp);
            fread_or_exception(&bombslimit, sizeof(short), 1, fp);
            fread_or_exception(&wandfreezetime, sizeof(short), 1, fp);
            fread_or_exception(&wandlimit, sizeof(short), 1, fp);
            fread_or_exception(&shellttl, sizeof(short), 1, fp);
            fread_or_exception(&blueblockttl, sizeof(short), 1, fp);
            fread_or_exception(&redblockttl, sizeof(short), 1, fp);
            fread_or_exception(&grayblockttl, sizeof(short), 1, fp);
            fread_or_exception(&storedpowerupdelay, sizeof(short), 1, fp);
            fread_or_exception(&warplockstyle, sizeof(short), 1, fp);
            fread_or_exception(&warplocktime, sizeof(short), 1, fp);
            fread_or_exception(&suicidetime, sizeof(short), 1, fp);

            fread_or_exception(&poweruppreset, sizeof(short), 1, fp);
            fread_or_exception(&g_iCurrentPowerupPresets, sizeof(short), NUM_POWERUP_PRESETS * NUM_POWERUPS, fp);

            //TODO: Need to test what happens when you unplug some controllers from the xbox
            //and then start up (device index will probably point to a gamepad that isn't in the list)
            //and this will cause a crash
            fread_or_exception(inputConfiguration, sizeof(CInputPlayerControl), 8, fp);

            //setup player input controls for game
            for (short iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
                short iDevice;
                fread_or_exception(&iDevice, sizeof(short), 1, fp);

#ifdef _XBOX
                playerInput.inputControls[iPlayer] = &inputConfiguration[iPlayer][1]; //Always use gamepads as input devices on xbox
#else
                if (iDevice >= joystickcount)
                    iDevice = DEVICE_KEYBOARD;

                playerInput.inputControls[iPlayer] = &inputConfiguration[iPlayer][iDevice == DEVICE_KEYBOARD ? 0 : 1];
#endif
            }

#ifndef _XBOX
            fread_or_exception(&fullscreen, sizeof(bool), 1, fp);
#endif

            for (short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++)
                fread_or_exception(&(gamemodes[iGameMode]->goal), sizeof(short), 1, fp);

            fread_or_exception(&gamemodemenusettings, sizeof(GameModeSettings), 1, fp);

            fread_or_exception(&teamcounts, sizeof(short), 4, fp);
            fread_or_exception(&teamids, sizeof(short), 12, fp);
            fread_or_exception(&skinids, sizeof(short), 4, fp);
            fread_or_exception(&randomskin, sizeof(bool), 4, fp);
            fread_or_exception(&playercontrol, sizeof(short), 4, fp);

            //Load skin/team settings
            for (short iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
                if (skinids[iPlayer] >= skinlist->GetCount() || skinids[iPlayer] < 0)
                    skinids[iPlayer] = 0;
            }

            announcerlist->SetCurrent((short) abyte[2]);
            musiclist->SetCurrent((short) abyte[13]);
            worldmusiclist->SetCurrent((short) abyte[14]);
            menugraphicspacklist->SetCurrent((short) abyte[17]);
            worldgraphicspacklist->SetCurrent((short) abyte[32]);
            gamegraphicspacklist->SetCurrent((short) abyte[26]);
            soundpacklist->	SetCurrent((short) abyte[18]);

            sfx_setmusicvolume(musicvolume);
            sfx_setsoundvolume(soundvolume);
        } else {
            printf("Old options.bin detected.  Skipped reading it.\n");
        }
      }
      catch (char* errormsg)
      {
        perror(errormsg);
      }

      fclose(fp);
    }

}

void CGameValues::WriteConfig()
{
    FILE * fp = OpenFile("options.bin", "wb");

    if (fp != NULL) {
        fwrite_or_exception(g_iVersion, sizeof(int), 4, fp);

#ifdef _XBOX

        fwrite_or_exception(&flickerfilter, sizeof(short), 1, fp);
        fwrite_or_exception(&hardwarefilter, sizeof(short), 1, fp);
        fwrite_or_exception(&softfilter, sizeof(short), 1, fp);
        fwrite_or_exception(&aspectratio10x11, sizeof(bool), 1, fp);

        fwrite_or_exception(&screenResizeX, sizeof(float), 1, fp);
        fwrite_or_exception(&screenResizeY, sizeof(float), 1, fp);
        fwrite_or_exception(&screenResizeW, sizeof(float), 1, fp);
        fwrite_or_exception(&screenResizeH, sizeof(float), 1, fp);
#endif

        unsigned char abyte[35];
        abyte[0] = (unsigned char) spawnstyle;
        abyte[1] = (unsigned char) awardstyle;
        abyte[2] = (unsigned char) announcerlist->GetCurrentIndex();
        abyte[3] = (unsigned char) teamcollision;
        abyte[4] = (unsigned char) screencrunch;
        abyte[5] = (unsigned char) toplayer;
        abyte[6] = (unsigned char) scoreboardstyle;
        abyte[7] = (unsigned char) teamcolors;
        abyte[8] = (unsigned char) sound;
        abyte[9] = (unsigned char) music;
        abyte[10] = (unsigned char) musicvolume;
        abyte[11] = (unsigned char) soundvolume;
        abyte[12] = (unsigned char) respawn;
        abyte[13] = (unsigned char) musiclist->GetCurrentIndex();
        abyte[14] = (unsigned char) worldmusiclist->GetCurrentIndex();
        abyte[15] = (unsigned char) outofboundstime;
        abyte[16] = (unsigned char) cpudifficulty;
        abyte[17] = (unsigned char) menugraphicspacklist->GetCurrentIndex();
        abyte[18] = (unsigned char) soundpacklist->GetCurrentIndex();
        abyte[19] = (unsigned char) framelimiter;
        abyte[20] = (unsigned char) bonuswheel;
        abyte[21] = (unsigned char) keeppowerup;
        abyte[22] = (unsigned char) showwinningcrown;
        abyte[23] = (unsigned char) playnextmusic;
        abyte[24] = (unsigned char) pointspeed;
        abyte[25] = (unsigned char) swapstyle;
        abyte[26] = (unsigned char) gamegraphicspacklist->GetCurrentIndex();
        abyte[28] = (unsigned char) overridepowerupsettings;
        abyte[29] = (unsigned char) minigameunlocked;
        abyte[30] = (unsigned char) startgamecountdown;
        abyte[31] = (unsigned char) deadteamnotice;
        abyte[32] = (unsigned char) worldgraphicspacklist->GetCurrentIndex();
        abyte[33] = (unsigned char) tournamentcontrolstyle;
        abyte[34] = (unsigned char) startmodedisplay;
        fwrite_or_exception(abyte, sizeof(unsigned char), 35, fp);

        fwrite_or_exception(&shieldtime, sizeof(short), 1, fp);
        fwrite_or_exception(&shieldstyle, sizeof(short), 1, fp);
        fwrite_or_exception(&itemrespawntime, sizeof(short), 1, fp);
        fwrite_or_exception(&hiddenblockrespawn, sizeof(short), 1, fp);
        fwrite_or_exception(&fireballttl, sizeof(short), 1, fp);
        fwrite_or_exception(&fireballlimit, sizeof(short), 1, fp);
        fwrite_or_exception(&hammerdelay, sizeof(short), 1, fp);
        fwrite_or_exception(&hammerttl, sizeof(short), 1, fp);
        fwrite_or_exception(&hammerpower, sizeof(bool), 1, fp);
        fwrite_or_exception(&hammerlimit, sizeof(short), 1, fp);
        fwrite_or_exception(&boomerangstyle, sizeof(short), 1, fp);
        fwrite_or_exception(&boomeranglife, sizeof(short), 1, fp);
        fwrite_or_exception(&boomeranglimit, sizeof(short), 1, fp);
        fwrite_or_exception(&featherjumps, sizeof(short), 1, fp);
        fwrite_or_exception(&featherlimit, sizeof(short), 1, fp);
        fwrite_or_exception(&leaflimit, sizeof(short), 1, fp);
        fwrite_or_exception(&pwingslimit, sizeof(short), 1, fp);
        fwrite_or_exception(&tanookilimit, sizeof(short), 1, fp);
        fwrite_or_exception(&bombslimit, sizeof(short), 1, fp);
        fwrite_or_exception(&wandfreezetime, sizeof(short), 1, fp);
        fwrite_or_exception(&wandlimit, sizeof(short), 1, fp);
        fwrite_or_exception(&shellttl, sizeof(short), 1, fp);
        fwrite_or_exception(&blueblockttl, sizeof(short), 1, fp);
        fwrite_or_exception(&redblockttl, sizeof(short), 1, fp);
        fwrite_or_exception(&grayblockttl, sizeof(short), 1, fp);
        fwrite_or_exception(&storedpowerupdelay, sizeof(short), 1, fp);
        fwrite_or_exception(&warplockstyle, sizeof(short), 1, fp);
        fwrite_or_exception(&warplocktime, sizeof(short), 1, fp);
        fwrite_or_exception(&suicidetime, sizeof(short), 1, fp);

        fwrite_or_exception(&poweruppreset, sizeof(short), 1, fp);
        fwrite_or_exception(&g_iCurrentPowerupPresets, sizeof(short), NUM_POWERUP_PRESETS * NUM_POWERUPS, fp);

        fwrite_or_exception(inputConfiguration, sizeof(CInputPlayerControl), 8, fp);

        for (int iPlayer = 0; iPlayer < 4; iPlayer++) {
            fwrite_or_exception(&playerInput.inputControls[iPlayer]->iDevice, sizeof(short), 1, fp);
        }

#ifndef _XBOX
        fwrite_or_exception(&fullscreen, sizeof(bool), 1, fp);
#endif
        //Write out game mode goals
        for (short k = 0; k < GAMEMODE_LAST; k++)
            fwrite_or_exception(&gamemodes[k]->goal, sizeof(short), 1, fp);

        fwrite_or_exception(&gamemodemenusettings, sizeof(GameModeSettings), 1, fp);

        // FIXME
        //fwrite_or_exception(&miTeamSelect->iTeamCounts, sizeof(short), 4, fp);
        //fwrite_or_exception(&miTeamSelect->iTeamIDs, sizeof(short), 12, fp);

        fwrite_or_exception(&skinids, sizeof(short), 4, fp);
        fwrite_or_exception(&randomskin, sizeof(bool), 4, fp);
        fwrite_or_exception(&playercontrol, sizeof(short), 4, fp);

        fclose(fp);
    }

    maplist->WriteFilters();
    maplist->WriteMapSummaryCache();
}
