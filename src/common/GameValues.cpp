#include "GameValues.h"

#include "FileIO.h"
#include "FileList.h"
#include "GameMode.h"
#include "GlobalConstants.h"
#include "MapList.h" // req. only by WriteConfig
#include "sfx.h"

#include <stdio.h>
#include <stdexcept>

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

extern int32_t g_iVersion[];
extern bool VersionIsEqual(int32_t iVersion[], short iMajor, short iMinor, short iMicro, short iBuild);


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

void CGameValues::resetGameplaySettings()
{
    pausegame = false;
    exitinggame = false;
    exityes = false;
    teamdeadcounter = 0;
    screenshaketimer = 0;
    slowdownon = -1;
    slowdowncounter = 0;
    showscoreboard = false;
    scorepercentmove = 0.0f;
    playskidsound = false;
    playinvinciblesound = false;
    playflyingsound = false;
    swapplayers = false;
    swapplayersposition = 0.0f;
    swapplayersblink = false;
    swapplayersblinkcount = 0;
    screenfade = 255;
    screenfadespeed = -8;
    noexit = false;
    noexittimer = 0;
    forceexittimer = 0;
    gamewindx = 0.0f;
    gamewindy = 0.0f;

    windaffectsplayers = false;
    spinscreen = false;
    reversewalk = false;
    spotlights = false;

    //Initialize game mode
    gamemode->init();
}

void CGameValues::resetSecretCounters()
{
    //Reset Secret Counters
    for (short iPlayer = 0; iPlayer < 4; iPlayer++)
        unlocksecret1part1[iPlayer] = false;

    unlocksecret1part2 = 0;
    unlocksecret2part1 = false;
    unlocksecret2part2 = 0;
    unlocksecret3part1[0] = 0;
    unlocksecret3part1[1] = 0;
    unlocksecret3part1[2] = 0;
    unlocksecret3part1[3] = 0;
    unlocksecret3part2[0] = 0;
    unlocksecret3part2[1] = 0;
    unlocksecret3part2[2] = 0;
    unlocksecret3part2[3] = 0;
    unlocksecretunlocked[0] = false;
    unlocksecretunlocked[1] = false;
    unlocksecretunlocked[2] = false;
    unlocksecretunlocked[3] = false;
}

// Read saved settings from disk
void CGameValues::ReadBinaryConfig() {
    try {
        std::string options_path(GetHomeDirectory() + "options.bin");
        BinaryFile options(options_path.c_str(), "rb");
        if (!options.is_open())
            throw std::runtime_error("Could not open " + options_path);

        int32_t version[4];
        options.read_i32_array(version, 4);

        if (!VersionIsEqual(g_iVersion, version[0], version[1], version[2], version[3])) {
            printf("Old options.bin detected. Skipped reading it.\n");
            return;
        }

#ifdef _XBOX
        flickerfilter = options.read_i16();
        hardwarefilter = options.read_i16();
        softfilter = options.read_i16();
        aspectratio10x11 = options.read_bool();

        screenResizeX = options.read_float();
        screenResizeY = options.read_float();
        screenResizeW = options.read_float();
        screenResizeH = options.read_float();

        SDL_XBOX_SetScreenPosition(screenResizeX, screenResizeY);
        SDL_XBOX_SetScreenStretch(screenResizeW, screenResizeH);
#endif

        spawnstyle = options.read_u8();
        awardstyle = options.read_u8();
        teamcollision = options.read_u8();
        screencrunch = options.read_u8();
        toplayer = options.read_u8();
        scoreboardstyle = options.read_u8();
        teamcolors = options.read_u8();
        sound = options.read_u8();
        music = options.read_u8();
        musicvolume = options.read_u8();
        soundvolume = options.read_u8();
        respawn = options.read_u8();
        outofboundstime = options.read_u8();
        cpudifficulty = options.read_u8();
        framelimiter = options.read_u8();
        bonuswheel = options.read_u8();
        keeppowerup = options.read_u8();
        showwinningcrown = options.read_u8();
        playnextmusic = options.read_u8();
        pointspeed = options.read_u8();
        swapstyle = options.read_u8();
        overridepowerupsettings = options.read_u8();
        minigameunlocked = options.read_u8();
        startgamecountdown = options.read_u8();
        deadteamnotice = options.read_u8();
        tournamentcontrolstyle = options.read_u8();
        startmodedisplay = options.read_u8();

        shieldtime = options.read_i16();
        shieldstyle = options.read_i16();
        itemrespawntime = options.read_i16();
        hiddenblockrespawn = options.read_i16();
        fireballttl = options.read_i16();
        fireballlimit = options.read_i16();
        hammerdelay = options.read_i16();
        hammerttl = options.read_i16();
        hammerpower = options.read_i16();
        hammerlimit = options.read_i16();
        boomerangstyle = options.read_i16();
        boomeranglife = options.read_i16();
        boomeranglimit = options.read_i16();
        featherjumps = options.read_i16();
        featherlimit = options.read_i16();
        leaflimit = options.read_i16();
        pwingslimit = options.read_i16();
        tanookilimit = options.read_i16();
        bombslimit = options.read_i16();
        wandfreezetime = options.read_i16();
        wandlimit = options.read_i16();
        shellttl = options.read_i16();
        blueblockttl = options.read_i16();
        redblockttl = options.read_i16();
        grayblockttl = options.read_i16();
        storedpowerupdelay = options.read_i16();
        warplockstyle = options.read_i16();
        warplocktime = options.read_i16();
        suicidetime = options.read_i16();

        poweruppreset = options.read_i16();
        options.read_i16_array((int16_t *)g_iCurrentPowerupPresets, NUM_POWERUP_PRESETS * NUM_POWERUPS);

#ifndef _XBOX
        fullscreen = options.read_bool();
#endif

        for (short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++)
            gamemodes[iGameMode]->goal = options.read_i16();

        options.read_raw(&gamemodemenusettings, sizeof(GameModeSettings));

        // FIXME: See WriteConfig
        //fread_or_exception(&teamcounts, sizeof(short), 4, fp);
        //fread_or_exception(&teamids, sizeof(short), 12, fp);
        options.read_raw(&skinids, sizeof(short) * 4);
        options.read_raw(&randomskin, sizeof(bool) * 4);
        options.read_raw(&playercontrol, sizeof(short) * 4);

        //Load skin/team settings
        for (short iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
            if (skinids[iPlayer] >= skinlist->GetCount() || skinids[iPlayer] < 0)
                skinids[iPlayer] = 0;
        }

        announcerlist->SetCurrent(options.read_u8());
        musiclist->SetCurrent(options.read_u8());
        worldmusiclist->SetCurrent(options.read_u8());
        soundpacklist->SetCurrent(options.read_u8());
        menugraphicspacklist->SetCurrent(options.read_u8());
        worldgraphicspacklist->SetCurrent(options.read_u8());
        gamegraphicspacklist->SetCurrent(options.read_u8());

        sfx_setmusicvolume(musicvolume);
        sfx_setsoundvolume(soundvolume);
    }
    catch (std::exception const& error)
    {
        perror(error.what());
        return;
    }

    try {
#ifdef USE_SDL2
        std::string controls_path(GetHomeDirectory() + "controls.sdl2.bin");
#else
        std::string controls_path(GetHomeDirectory() + "controls.sdl.bin");
#endif
        BinaryFile controls(controls_path.c_str(), "rb");
        if (!controls.is_open())
            throw std::runtime_error("Could not open " + controls_path);

        //TODO: Need to test what happens when you unplug some controllers from the xbox
        //and then start up (device index will probably point to a gamepad that isn't in the list)
        //and this will cause a crash
        controls.read_raw(inputConfiguration, sizeof(CInputPlayerControl) * 8);

        //setup player input controls for game
        for (short iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
            short iDevice = controls.read_i16();

#ifdef _XBOX
            playerInput.inputControls[iPlayer] = &inputConfiguration[iPlayer][1]; //Always use gamepads as input devices on xbox
#else
            if (iDevice >= joystickcount)
                iDevice = DEVICE_KEYBOARD;

            playerInput.inputControls[iPlayer] = &inputConfiguration[iPlayer][iDevice == DEVICE_KEYBOARD ? 0 : 1];
#endif
        }
    }
    catch (std::exception const& error)
    {
        perror(error.what());
        return;
    }
}

void CGameValues::WriteConfig()
{
    try {
        std::string options_path(GetHomeDirectory() + "options.bin");
        BinaryFile options(options_path.c_str(), "wb");
        if (!options.is_open())
            throw std::runtime_error("Could not open " + options_path);

        options.write_raw(g_iVersion, sizeof(int) * 4);

#ifdef _XBOX
        options.write_i16(flickerfilter);
        options.write_i16(hardwarefilter);
        options.write_i16(softfilter);
        options.write_bool(aspectratio10x11);

        options.write_float(screenResizeX);
        options.write_float(screenResizeY);
        options.write_float(screenResizeW);
        options.write_float(screenResizeH);
#endif

        options.write_u8(spawnstyle);
        options.write_u8(awardstyle);
        options.write_u8(teamcollision);
        options.write_u8(screencrunch);
        options.write_u8(toplayer);
        options.write_u8(scoreboardstyle);
        options.write_u8(teamcolors);
        options.write_u8(sound);
        options.write_u8(music);
        options.write_u8(musicvolume);
        options.write_u8(soundvolume);
        options.write_u8(respawn);
        options.write_u8(outofboundstime);
        options.write_u8(cpudifficulty);
        options.write_u8(framelimiter);
        options.write_u8(bonuswheel);
        options.write_u8(keeppowerup);
        options.write_u8(showwinningcrown);
        options.write_u8(playnextmusic);
        options.write_u8(pointspeed);
        options.write_u8(swapstyle);
        options.write_u8(overridepowerupsettings);
        options.write_u8(minigameunlocked);
        options.write_u8(startgamecountdown);
        options.write_u8(deadteamnotice);
        options.write_u8(tournamentcontrolstyle);
        options.write_u8(startmodedisplay);

        options.write_i16(shieldtime);
        options.write_i16(shieldstyle);
        options.write_i16(itemrespawntime);
        options.write_i16(hiddenblockrespawn);
        options.write_i16(fireballttl);
        options.write_i16(fireballlimit);
        options.write_i16(hammerdelay);
        options.write_i16(hammerttl);
        options.write_i16(hammerpower);
        options.write_i16(hammerlimit);
        options.write_i16(boomerangstyle);
        options.write_i16(boomeranglife);
        options.write_i16(boomeranglimit);
        options.write_i16(featherjumps);
        options.write_i16(featherlimit);
        options.write_i16(leaflimit);
        options.write_i16(pwingslimit);
        options.write_i16(tanookilimit);
        options.write_i16(bombslimit);
        options.write_i16(wandfreezetime);
        options.write_i16(wandlimit);
        options.write_i16(shellttl);
        options.write_i16(blueblockttl);
        options.write_i16(redblockttl);
        options.write_i16(grayblockttl);
        options.write_i16(storedpowerupdelay);
        options.write_i16(warplockstyle);
        options.write_i16(warplocktime);
        options.write_i16(suicidetime);

        options.write_i16(poweruppreset);
        options.write_raw(&g_iCurrentPowerupPresets, sizeof(short) * NUM_POWERUP_PRESETS * NUM_POWERUPS);

#ifndef _XBOX
        options.write_bool(fullscreen);
#endif
        //Write out game mode goals
        for (short k = 0; k < GAMEMODE_LAST; k++)
            options.write_i16(gamemodes[k]->goal);

        options.write_raw(&gamemodemenusettings, sizeof(GameModeSettings));

        // FIXME
        //fwrite_or_exception(&miTeamSelect->iTeamCounts, sizeof(short), 4, fp);
        //fwrite_or_exception(&miTeamSelect->iTeamIDs, sizeof(short), 12, fp);

        options.write_raw(&skinids, sizeof(short) * 4);
        options.write_raw(&randomskin, sizeof(bool) * 4);
        options.write_raw(&playercontrol, sizeof(short) * 4);

        options.write_u8(announcerlist->GetCurrentIndex());
        options.write_u8(musiclist->GetCurrentIndex());
        options.write_u8(worldmusiclist->GetCurrentIndex());
        options.write_u8(soundpacklist->GetCurrentIndex());
        options.write_u8(menugraphicspacklist->GetCurrentIndex());
        options.write_u8(worldgraphicspacklist->GetCurrentIndex());
        options.write_u8(gamegraphicspacklist->GetCurrentIndex());
    }
    catch (std::exception const& error)
    {
        perror(error.what());
        return;
    }

    try {
#ifdef USE_SDL2
        std::string controls_path(GetHomeDirectory() + "controls.sdl2.bin");
#else
        std::string controls_path(GetHomeDirectory() + "controls.sdl.bin");
#endif
        BinaryFile controls(controls_path.c_str(), "wb");
        if (!controls.is_open())
            throw std::runtime_error("Could not open " + controls_path);


        controls.write_raw(inputConfiguration, sizeof(CInputPlayerControl) * 8);

        for (int iPlayer = 0; iPlayer < 4; iPlayer++)
            controls.write_i16(playerInput.inputControls[iPlayer]->iDevice);
    }
    catch (std::exception const& error)
    {
        perror(error.what());
        return;
    }

    maplist->WriteFilters();
    maplist->WriteMapSummaryCache();
}
