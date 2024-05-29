#include "GameValues.h"

#include "FileIO.h"
#include "FileList.h"
#include "GameMode.h"
#include "GlobalConstants.h"
#include "MapList.h" // req. only by WriteConfig
#include "Version.h"
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

namespace {
/*
0 == poison mushroom
1 == 1up
2 == 2up
3 == 3up
4 == 5up
5 == flower
6 == star
7 == clock
8 == bobomb
9 == pow
10 == bulletbill
11 == hammer
12 == green shell
13 == red shell
14 == spike shell
15 == buzzy shell
16 == mod
17 == feather
18 == mystery mushroom
19 == boomerang
20 == tanooki
21 == ice wand
22 == podoboo
23 == bombs
24 == leaf
25 == pwings
*/
constexpr short DEFAULT_POWERUP_PRESETS[NUM_POWERUP_PRESETS][NUM_POWERUPS] {
//   0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25
    {5,10, 4, 2, 1,10, 8, 4, 4, 2, 2, 4, 8, 4, 2, 4, 2, 4, 5, 6, 6, 3, 4, 4, 5, 3}, //Custom 1
    {5,10, 4, 2, 1,10, 8, 4, 4, 2, 2, 4, 8, 4, 2, 4, 2, 4, 5, 6, 6, 3, 4, 4, 5, 3}, //Custom 2
    {5,10, 4, 2, 1,10, 8, 4, 4, 2, 2, 4, 8, 4, 2, 4, 2, 4, 5, 6, 6, 3, 4, 4, 5, 3}, //Custom 3
    {5,10, 4, 2, 1,10, 8, 4, 4, 2, 2, 4, 8, 4, 2, 4, 2, 4, 5, 6, 6, 3, 4, 4, 5, 3}, //Custom 4
    {5,10, 4, 2, 1,10, 8, 4, 4, 2, 2, 4, 8, 4, 2, 4, 2, 4, 5, 6, 6, 3, 4, 4, 5, 3}, //Custom 5
    {5,10, 5, 3, 1,10, 2, 3, 4, 3, 3, 4, 9, 6, 2, 4, 4, 7, 5, 6, 6, 3, 2, 2, 5, 5}, //Balanced
    {5, 0, 0, 0, 0,10, 0, 0, 0, 0, 0, 7, 9, 6, 3, 4, 0, 0, 0, 4, 0, 2, 0, 2, 0, 0}, //Weapons Only
    {0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0}, //Koopa Bros Weapons
    {5,10, 7, 5, 2, 0, 6, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 5, 0, 3, 0, 0, 0, 8, 6}, //Support Items
    {3, 3, 1, 0, 0, 0, 0, 0, 4, 2, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 3, 3, 0, 0}, //Booms and Shakes
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 2, 0, 0, 0, 8, 3}, //Fly and Glide
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 4, 2, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //Shells
    {5, 8, 4, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0}, //Mushrooms Only
    {3, 5, 0, 0, 0, 5, 2, 0, 0, 0, 3, 0, 6, 4, 1, 3, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}, //Super Mario Bros 1
    {0, 5, 0, 0, 0, 0, 2, 4, 3, 2, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, //Super Mario Bros 2
    {0, 3, 0, 0, 0, 8, 4, 0, 0, 0, 5, 2,10, 9, 4, 5, 0, 0, 0, 4, 3, 0, 4, 0, 8, 5}, //Super Mario Bros 3
    {0,10, 0, 0, 0,10, 6, 0, 0, 0, 2, 0, 8, 4, 2, 4, 0, 4, 0, 0, 0, 0, 5, 0, 0, 0}, //Super Mario World
};
} // namespace

extern CGameValues game_values;
void ifSoundOnPlay(sfxSound& sfx)
{
    if (game_values.sound)
        sfx.play();
}

short defaultPowerupSetting(size_t presetIdx, size_t powerupIdx)
{
    return DEFAULT_POWERUP_PRESETS[presetIdx][powerupIdx];
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
    appstate     = AppState::Splash;
#ifdef _DEBUG
    fullscreen      = false;
#else
    fullscreen      = false;
#endif

    awardstyle      = AwardStyle::Fireworks;
    spawnstyle      = SpawnStyle::Swirl;
    tournamentgames   = 2;
    tournamentwinner  = -1;
    selectedminigame = Minigame::PipeCoin;
    matchtype = MatchType::SingleGame;
    tourindex     = 0;
    tourstopcurrent   = 0;
    tourstoptotal   = 0;
    worldindex      = 0;
    teamcollision = TeamCollisionStyle::Off;
    screencrunch    = true;
    toplayer      = true;
    loadedannouncer   = -1;
    loadedmusic     = -1;
    scoreboardstyle = ScoreboardStyle::Top;
    teamcolors          = true;
    cputurn       = -1;
    shieldtime      = 62;
    shieldstyle     = ShieldStyle::SoftWithStomp;
    musicvolume     = 128;
    soundvolume     = 128;
    respawn       = 2;
    itemrespawntime   = 1860;  //default item respawn is 30 seconds (30 * 62 fps)
    hiddenblockrespawn  = 1860;  //default item respawn is 30 seconds
    outofboundstime   = 5;
    warplockstyle   = WarpLockStyle::ExitOnly;  // Lock Warp Exit Only
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
    boomerangstyle = BoomerangStyle::SMB3;  //SMB3 style
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
    secretsenabled  = false;
    poweruppreset   = 0;
    tournamentcontrolstyle = TournamentControlStyle::All;


    pfFilters     = new bool[NUM_AUTO_FILTERS + filterslist->count()];
    piFilterIcons   = new short[NUM_AUTO_FILTERS + filterslist->count()];
    fNeedWriteFilters = false;

    for (size_t iFilter = 0; iFilter < NUM_AUTO_FILTERS + filterslist->count(); iFilter++) {
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
        playerInput.inputControls[iPlayer] = &inputConfiguration[iPlayer][0];
    }

    //Set the default powerup weights for bonus wheel and [?] boxes
    for (short iPreset = 0; iPreset < NUM_POWERUP_PRESETS; iPreset++) {
        for (short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++) {
            allPowerupPresets[iPreset][iPowerup] = defaultPowerupSetting(iPreset, iPowerup);
        }
    }
}

void CGameValues::resetGameplaySettings()
{
    flags = GameplayFlags();

    screenfade = 255;
    screenfadespeed = -8;

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
void CGameConfig::ReadBinaryConfig() {
    try {
        std::string options_path(GetHomeDirectory() + "options.bin");
        BinaryFile options(options_path.c_str(), "rb");
        if (!options.is_open())
            throw std::runtime_error("Could not open " + options_path);

        Version version;
        version.major = options.read_i32();
        version.minor = options.read_i32();
        version.patch = options.read_i32();
        version.build = options.read_i32();

        if (GAME_VERSION != version) {
            printf("Old options.bin detected. Skipped reading it.\n");
            return;
        }

        spawnstyle = static_cast<SpawnStyle>(options.read_u8());
        awardstyle = static_cast<AwardStyle>(options.read_u8());
        teamcollision = static_cast<TeamCollisionStyle>(options.read_u8());
        screencrunch = options.read_u8();
        toplayer = options.read_u8();
        scoreboardstyle = static_cast<ScoreboardStyle>(options.read_u8());
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
        secretsenabled = options.read_u8();
        startgamecountdown = options.read_u8();
        deadteamnotice = options.read_u8();
        tournamentcontrolstyle = static_cast<TournamentControlStyle>(options.read_u8());
        startmodedisplay = options.read_u8();

        shieldtime = options.read_i16();
        shieldstyle = static_cast<ShieldStyle>(options.read_i16());
        itemrespawntime = options.read_i16();
        hiddenblockrespawn = options.read_i16();
        fireballttl = options.read_i16();
        fireballlimit = options.read_i16();
        hammerdelay = options.read_i16();
        hammerttl = options.read_i16();
        hammerpower = options.read_i16();
        hammerlimit = options.read_i16();
        boomerangstyle = static_cast<BoomerangStyle>(options.read_i16());
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
        warplockstyle = static_cast<WarpLockStyle>(options.read_i16());
        warplocktime = options.read_i16();
        suicidetime = options.read_i16();

        poweruppreset = options.read_i16();
        options.read_i16_array((int16_t *)allPowerupPresets, NUM_POWERUP_PRESETS * NUM_POWERUPS);

        fullscreen = options.read_bool();

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
            if (skinids[iPlayer] >= skinlist->count() || skinids[iPlayer] < 0)
                skinids[iPlayer] = 0;
        }

        announcerlist->setCurrentIndex(options.read_u8());
        musiclist->SetCurrent(options.read_u8());
        worldmusiclist->SetCurrent(options.read_u8());
        soundpacklist->setCurrentIndex(options.read_u8());
        menugraphicspacklist->setCurrentIndex(options.read_u8());
        worldgraphicspacklist->setCurrentIndex(options.read_u8());
        gamegraphicspacklist->setCurrentIndex(options.read_u8());

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

            if (iDevice >= joystickcount)
                iDevice = DEVICE_KEYBOARD;

            playerInput.inputControls[iPlayer] = &inputConfiguration[iPlayer][iDevice == DEVICE_KEYBOARD ? 0 : 1];
        }
    }
    catch (std::exception const& error)
    {
        perror(error.what());
        return;
    }
}

void CGameConfig::WriteConfig() const
{
    try {
        std::string options_path(GetHomeDirectory() + "options.bin");
        BinaryFile options(options_path.c_str(), "wb");
        if (!options.is_open())
            throw std::runtime_error("Could not open " + options_path);

        options.write_i32(GAME_VERSION.major);
        options.write_i32(GAME_VERSION.minor);
        options.write_i32(GAME_VERSION.patch);
        options.write_i32(GAME_VERSION.build);

        options.write_u8(static_cast<uint8_t>(spawnstyle));
        options.write_u8(static_cast<uint8_t>(awardstyle));
        options.write_u8(static_cast<uint8_t>(teamcollision));
        options.write_u8(screencrunch);
        options.write_u8(toplayer);
        options.write_u8(static_cast<uint8_t>(scoreboardstyle));
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
        options.write_u8(secretsenabled);
        options.write_u8(startgamecountdown);
        options.write_u8(deadteamnotice);
        options.write_u8(static_cast<uint8_t>(tournamentcontrolstyle));
        options.write_u8(startmodedisplay);

        options.write_i16(shieldtime);
        options.write_i16(static_cast<short>(shieldstyle));
        options.write_i16(itemrespawntime);
        options.write_i16(hiddenblockrespawn);
        options.write_i16(fireballttl);
        options.write_i16(fireballlimit);
        options.write_i16(hammerdelay);
        options.write_i16(hammerttl);
        options.write_i16(hammerpower);
        options.write_i16(hammerlimit);
        options.write_i16(static_cast<short>(boomerangstyle));
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
        options.write_i16(static_cast<short>(warplockstyle));
        options.write_i16(warplocktime);
        options.write_i16(suicidetime);

        options.write_i16(poweruppreset);
        options.write_raw(&allPowerupPresets, sizeof(short) * NUM_POWERUP_PRESETS * NUM_POWERUPS);

        options.write_bool(fullscreen);

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

        options.write_u8(announcerlist->currentIndex());
        options.write_u8(musiclist->GetCurrentIndex());
        options.write_u8(worldmusiclist->GetCurrentIndex());
        options.write_u8(soundpacklist->currentIndex());
        options.write_u8(menugraphicspacklist->currentIndex());
        options.write_u8(worldgraphicspacklist->currentIndex());
        options.write_u8(gamegraphicspacklist->currentIndex());
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
