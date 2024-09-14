#include "GSMenu.h"

#include "FileList.h"
#include "Game.h"
#include "GameValues.h"
#include "GSGameplay.h"
#include "net.h"
#include "map.h"
#include "MapList.h"
#include "path.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "Score.h"
#include "gamemodes/BonusHouse.h"
#include "gamemodes/MiniBoss.h"
#include "gamemodes/MiniBoxes.h"
#include "gamemodes/MiniPipe.h"
#include "ui/MI_BonusWheel.h"
#include "ui/MI_MapBrowser.h"
#include "ui/MI_MapField.h"
#include "ui/MI_MapFilterScroll.h"
#include "ui/MI_SelectField.h"
#include "ui/MI_TournamentScoreboard.h"
#include "ui/MI_TourStop.h"
#include "ui/MI_World.h"
#include "Version.h"
#include "world.h"
#include "WorldTourStop.h"

#include "menu/BonusWheelMenu.h"
#include "menu/GameSettingsMenu.h"
#include "menu/MainMenu.h"
#include "menu/MapFilterEditMenu.h"
#include "menu/MatchSelectionMenu.h"
#include "menu/ModeOptionsMenu.h"
#include "menu/OptionsMenu.h"
#include "menu/PlayerControlsMenu.h"
#include "menu/PlayerControlsSelectMenu.h"
#include "menu/TeamSelectMenu.h"
#include "menu/TournamentScoreboardMenu.h"
#include "menu/TourStopMenu.h"
#include "menu/WorldMenu.h"

#include "menu/options/EyeCandyOptionsMenu.h"
#include "menu/options/GameplayOptionsMenu.h"
#include "menu/options/GraphicsOptionsMenu.h"
#include "menu/options/PowerupDropRatesMenu.h"
#include "menu/options/PowerupSettingsMenu.h"
#include "menu/options/ProjectileLimitsMenu.h"
#include "menu/options/ProjectileOptionsMenu.h"
#include "menu/options/SoundOptionsMenu.h"
#include "menu/options/TeamOptionsMenu.h"

#include "menu/network/NetEditServersMenu.h"
#include "menu/network/NetLobbyMenu.h"
#include "menu/network/NetNewRoomMenu.h"
#include "menu/network/NetNewRoomSettingsMenu.h"
#include "menu/network/NetRoomMenu.h"
#include "menu/network/NetServersMenu.h"

#include <cstdlib> // atoi()
#include <cassert>
#include <sstream>


bool LoadStartGraphics();

extern SDL_Surface* screen;
extern SDL_Surface* blitdest;

#ifdef _DEBUG
extern bool	g_fAutoTest;
extern bool g_fRecordTest;
#endif

extern void SetGameModeSettingsFromMenu();
extern void LoadMapObjects(bool fPreview);
extern bool LoadGameSounds();

extern void UpdateScoreBoard();

extern short LookupTeamID(short id);

extern WorldMap g_worldmap;

extern void LoadCurrentMapBackground();

extern CMap* g_map;

extern CScore *score[4];
extern short score_cnt;

extern std::string stripPathAndExtension(const std::string &path);

extern CGameMode * gamemodes[GAMEMODE_LAST];
extern CGM_Bonus * bonushousemode;
extern CGM_Pipe_MiniGame * pipegamemode;
extern CGM_Boss_MiniGame * bossgamemode;
extern CGM_Boxes_MiniGame * boxesgamemode;
extern short currentgamemode;

extern CResourceManager* rm;
extern CGameValues game_values;

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
extern TourList *tourlist;
extern WorldList *worldlist;

// std::uniue_ptr requires that the special member function have access
// to the complete types.
MenuState::MenuState() = default;
MenuState::~MenuState() = default;

MenuState& MenuState::instance()
{
    static MenuState menu;
    return menu;
}

bool MenuState::init()
{
    mMainMenu = std::make_unique<UI_MainMenu>();

    // Options menu
    mOptionsMenu = std::make_unique<UI_OptionsMenu>();
    mGameplayOptionsMenu = std::make_unique<UI_GameplayOptionsMenu>();
    mTeamOptionsMenu = std::make_unique<UI_TeamOptionsMenu>();
    mPowerupDropRatesMenu = std::make_unique<UI_PowerupDropRatesMenu>();
    mPowerupSettingsMenu = std::make_unique<UI_PowerupSettingsMenu>();
    mProjectileLimitsMenu = std::make_unique<UI_ProjectileLimitsMenu>();
    mProjectileOptionsMenu = std::make_unique<UI_ProjectileOptionsMenu>();
    mGraphicsOptionsMenu = std::make_unique<UI_GraphicsOptionsMenu>();
    mEyeCandyOptionsMenu = std::make_unique<UI_EyeCandyOptionsMenu>();
    mSoundOptionsMenu = std::make_unique<UI_SoundOptionsMenu>();

    // Controls menu
    mPlayerControlsSelectMenu = std::make_unique<UI_PlayerControlsSelectMenu>();
    mPlayerControlsMenu = std::make_unique<UI_PlayerControlsMenu>();

    // Gameplay menu
    mModeOptionsMenu = std::make_unique<UI_ModeOptionsMenu>();
    mMatchSelectionMenu = std::make_unique<UI_MatchSelectionMenu>();
    mGameSettingsMenu = std::make_unique<UI_GameSettingsMenu>();
    mMapFilterEditMenu = std::make_unique<UI_MapFilterEditMenu>();
    mTourStopMenu = std::make_unique<UI_TourStopMenu>();
    mWorldMenu = std::make_unique<UI_WorldMenu>();
    mTeamSelectMenu = std::make_unique<UI_TeamSelectMenu>();
    mTournamentScoreboardMenu = std::make_unique<UI_TournamentScoreboardMenu>();
    mBonusWheelMenu = std::make_unique<UI_BonusWheelMenu>();

    // Multiplayer menu
    mNetServersMenu = std::make_unique<UI_NetServersMenu>();
    mNetEditServersMenu = std::make_unique<UI_NetEditServersMenu>();
    mNetLobbyMenu = std::make_unique<UI_NetLobbyMenu>();
    mNetNewRoomMenu = std::make_unique<UI_NetNewRoomMenu>();
    mNetNewRoomSettingsMenu = std::make_unique<UI_NetNewRoomSettingsMenu>(mGameSettingsMenu.get());
    mNetRoomMenu = std::make_unique<UI_NetRoomMenu>();

    mCurrentMenu = mMainMenu.get();
    szCurrentMapName = mGameSettingsMenu->miMapField->GetMapName();

    return true;
}

//---------------------------------------------------------------
// RUN THE MENU
//---------------------------------------------------------------

void MenuState::onEnterState()
{
    iUnlockMinigameOptionIndex = 0;

    iTournamentAIStep = 0;
    iTournamentAITimer = 0;

    //Reset the keys each time we switch from menu to game and back
    game_values.playerInput.ResetKeys();

    fNeedMenuMusicReset = false;

    if (game_values.gamemode->winningteam > -1 && game_values.tournamentwinner == -1 &&
            (((game_values.matchtype == MatchType::SingleGame || game_values.matchtype == MatchType::QuickGame || game_values.matchtype == MatchType::MiniGame || game_values.matchtype == MatchType::Tournament || game_values.matchtype == MatchType::NetGame) && game_values.bonuswheel == 2) || (game_values.matchtype == MatchType::Tour && game_values.tourstops[game_values.tourstopcurrent - 1]->iBonusType))) {
        mBonusWheelMenu->miBonusWheel->Reset(false);
        mCurrentMenu = mBonusWheelMenu.get();
    } else if (game_values.matchtype != MatchType::SingleGame && game_values.matchtype != MatchType::QuickGame && game_values.matchtype != MatchType::MiniGame && game_values.matchtype != MatchType::NetGame) {
        mCurrentMenu = mTournamentScoreboardMenu.get();
    } else if (game_values.matchtype == MatchType::QuickGame) {
        //Reset back to main menu after quick game
        mCurrentMenu = mMainMenu.get();
        mCurrentMenu->ResetMenu();
    } else if (game_values.matchtype == MatchType::NetGame) {
        mCurrentMenu = mNetLobbyMenu.get();
        mCurrentMenu->ResetMenu();
        netplay.joinSuccessful = false;
        netplay.gameRunning = false;
    }

    if (game_values.matchtype == MatchType::World) {
        if (game_values.gamemode->winningteam > -1 && game_values.tournamentwinner == -1) { //Stage is completed
            mWorldMenu->miWorld->SetControllingTeam(game_values.gamemode->winningteam);
            mWorldMenu->miWorld->SetCurrentStageToCompleted(game_values.gamemode->winningteam);

            mWorldMenu->CloseStageStart();

            //Clear out the stored powerups after a game that had a winner
            if (!game_values.worldskipscoreboard) {
                //Only clear out the stored powerup if we were allowed to use it in the game
                if (game_values.gamemode->HasStoredPowerups()) {
                    for (short iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
                        game_values.storedpowerups[iPlayer] = -1;
                }
            }
        }
        /*else if (game_values.tournamentwinner > -1) //World is completed
        {
            miBonusWheel->Reset(true);
            mCurrentMenu = mBonusWheelMenu;
        }*/

        UpdateScoreBoard();

        //If we're suposed to skip the scoreboard, then reset back to the world map
        if (game_values.worldskipscoreboard) {
            mCurrentMenu = mWorldMenu.get();
            mCurrentMenu->ResetMenu();

            game_values.worldskipscoreboard = false;
        } else {
            mTournamentScoreboardMenu->miTournamentScoreboard->RefreshWorldScores(game_values.gamemode->winningteam);
        }
    } else if (game_values.matchtype == MatchType::Tour) {
        mTournamentScoreboardMenu->miTournamentScoreboard->RefreshTourScores();

        if (game_values.tourstopcurrent < game_values.tourstoptotal)
            mTourStopMenu->miTourStop->Refresh(game_values.tourstopcurrent);
    } else if (game_values.matchtype == MatchType::Tournament) {
        mTournamentScoreboardMenu->miTournamentScoreboard->StopSwirl();
        if (game_values.gamemode->winningteam > -1) {
            mTournamentScoreboardMenu->miTournamentScoreboard->RefreshTournamentScores(game_values.gamemode->winningteam);

            //Set the next controlling team
            switch (game_values.tournamentcontrolstyle) {
            case TournamentControlStyle::GameWinner: { //Winning Team
                game_values.tournamentcontrolteam = game_values.gamemode->winningteam;
                break;
            }

            case TournamentControlStyle::GameLoser: { //Losing Team
                short iNumInPlace = 0;
                short iInPlace[4];
                short iLowestPlace = 0;
                for (short iScore = 0; iScore < score_cnt; iScore++) {
                    if (score[iScore]->place == iLowestPlace) {
                        iInPlace[iNumInPlace++] = iScore;
                    } else if (score[iScore]->place > iLowestPlace) {
                        iNumInPlace = 1;
                        iInPlace[0] = iScore;
                        iLowestPlace = score[iScore]->place;
                    }
                }

                game_values.tournamentcontrolteam = iInPlace[RANDOM_INT(iNumInPlace)];
                break;
            }

            case TournamentControlStyle::LeadingTeams: { //Tournament Ahead Teams
                short iNumInPlace = 0;
                short iInPlace[4];
                short iMostWins = 0;
                for (short iTeam = 0; iTeam < score_cnt; iTeam++) {
                    if (game_values.tournament_scores[iTeam].wins == iMostWins) {
                        iInPlace[iNumInPlace++] = iTeam;
                    } else if (game_values.tournament_scores[iTeam].wins > iMostWins) {
                        iNumInPlace = 1;
                        iInPlace[0] = iTeam;
                        iMostWins = game_values.tournament_scores[iTeam].wins;
                    }
                }

                game_values.tournamentcontrolteam = iInPlace[RANDOM_INT(iNumInPlace)];
                break;
            }

            case TournamentControlStyle::TrailingTeams: { //Tournament Behind Teams
                short iNumInPlace = 0;
                short iInPlace[4] = {0,0,0,0};
                short iLeastWins = 20; //Most possible wins are 10

                for (short iTeam = 0; iTeam < score_cnt; iTeam++) {
                    if (game_values.tournament_scores[iTeam].wins == iLeastWins) {
                        iInPlace[iNumInPlace++] = iTeam;
                    } else if (game_values.tournament_scores[iTeam].wins < iLeastWins) {
                        iNumInPlace = 1;
                        iInPlace[0] = iTeam;
                        iLeastWins = game_values.tournament_scores[iTeam].wins;
                    }
                }

                game_values.tournamentcontrolteam = iInPlace[RANDOM_INT(iNumInPlace)];
                break;
            }

            case TournamentControlStyle::Random: { //Random
                game_values.tournamentcontrolteam = RANDOM_INT( score_cnt);
                break;
            }

            case TournamentControlStyle::RandomLoser: { //Random Losing Team
                short iNumInPlace = 0;
                short iInPlace[4] = {0, 0, 0, 0};
                short iWinner = 0;

                for (short iTeam = 0; iTeam < score_cnt; iTeam++) {
                    if (score[iTeam]->place == 0) {
                        iWinner = iTeam;
                        break;
                    }
                }

                for (short iTeam = 0; iTeam < score_cnt; iTeam++) {
                    if (iTeam == iWinner)
                        continue;

                    iInPlace[iNumInPlace++] = iTeam;
                }

                game_values.tournamentcontrolteam = iInPlace[RANDOM_INT(iNumInPlace)];
                break;
            }

            case TournamentControlStyle::RoundRobin: { //Round Robin
                game_values.tournamentcontrolteam = game_values.tournamentnextcontrol;

                if (++game_values.tournamentnextcontrol >= score_cnt)
                    game_values.tournamentnextcontrol = 0;

                break;
            }

            default: {
                game_values.tournamentcontrolteam = -1;
                break;
            }
            }
        }
    }

    //Reset game mode back to the current game mode in case we came from boss mode
    game_values.gamemode = gamemodes[currentgamemode];

    //Keep track if we entered the menu loop as part of a tournament, if we exit the tournament
    //we need to reset the menu music back to normal
    if (game_values.matchtype != MatchType::SingleGame && game_values.matchtype != MatchType::QuickGame && game_values.matchtype != MatchType::MiniGame && game_values.matchtype != MatchType::NetGame)
        fNeedMenuMusicReset = true;

    if (game_values.music) {
        if (game_values.tournamentwinner < 0) {
            if (game_values.matchtype == MatchType::SingleGame || game_values.matchtype == MatchType::QuickGame || game_values.matchtype == MatchType::MiniGame || game_values.matchtype == MatchType::NetGame)
                rm->backgroundmusic[2].play(false, false);
            else if (game_values.matchtype == MatchType::World)
                rm->backgroundmusic[5].play(false, false);
            else
                rm->backgroundmusic[3].play(false, false);
        }
    }

    if (game_values.matchtype != MatchType::World && game_values.matchtype != MatchType::QuickGame && game_values.matchtype != MatchType::NetGame) {
        if (mCurrentMenu == mGameSettingsMenu.get() || mCurrentMenu == mTournamentScoreboardMenu.get())
            mGameSettingsMenu->miMapField->LoadCurrentMap();
    }

    // On return from a game, refresh room list
    if (netplay.active) {
        if (netplay.currentRoom.roomID) {
            netplay.client.sendLeaveRoomMessage();
            netplay.currentRoom.roomID = 0;
            netplay.joinSuccessful = false;
            netplay.gameRunning = false;
        }
        netplay.client.requestRoomList();
    }
}

void MenuState::update()
{
    if (netplay.active)
        netplay.client.update();

    //Reset the keys that were down the last frame
    game_values.playerInput.ClearPressedKeys(1);

    //handle messages
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            Exit();
            return;
            break;

        case SDL_KEYDOWN: {
            if (event.key.keysym.sym == SDLK_F1) {
                game_values.showfps = !game_values.showfps;
            }

#ifdef _DEBUG
            if (event.key.keysym.sym == SDLK_F2) {
                game_values.frameadvance = !game_values.frameadvance;
            }
#endif
            if (event.key.keysym.mod & (KMOD_LALT | KMOD_RALT)) {
                //ALT + F4 = close window
                if (event.key.keysym.sym == SDLK_F4) {
                    Exit();
                    return;
                } //ALT + Enter = fullscreen/windowed toggle
                else if (event.key.keysym.sym == SDLK_RETURN) {
                    game_values.fullscreen = !game_values.fullscreen;
					gfx_changefullscreen(game_values.fullscreen);
                    blitdest = screen;

                    continue;
                }
            }

            if (event.key.keysym.sym == SDLK_INSERT) {
                gfx_take_screenshot();
            }
#ifdef _DEBUG
            //Pressing insert in debug mode turns on automated testing
            if (event.key.keysym.sym == SDLK_F8) {
                g_fAutoTest = !g_fAutoTest;

                if (g_fAutoTest) {
                    mCurrentMenu = mMainMenu.get();
                    mCurrentMenu->ResetMenu();

                    LoadScript("Scripts/StartMenuAutomation.txt");
                }
            }

            if (event.key.keysym.sym == SDLK_HOME) {
                g_fRecordTest = !g_fRecordTest;

                if (g_fRecordTest) {
                    StartRecordScript();
                } else {
                    SaveScript("Scripts/SavedScript.txt");
                }
            }

            if (event.key.keysym.sym == SDLK_END) {
                AddEmtpyLineToScript();
            }

#endif
            break;

        }

        default:
            break;
        }

        game_values.playerInput.Update(event, 1);
    }

    //If AI is controlling the tournament menu, select the options
    if (game_values.matchtype == MatchType::Tournament && iTournamentAITimer > 0 && mCurrentMenu == mGameSettingsMenu.get() && mGameSettingsMenu->IsOnStartBtn()) {
        if (--iTournamentAITimer == 0) {
            iTournamentAIStep++;

            if (iTournamentAIStep == 1) {
                mGameSettingsMenu->miMapField->ChooseRandomMap();

                iTournamentAITimer = 60;
            } else if (iTournamentAIStep == 2) {
                currentgamemode = RANDOM_INT(GAMEMODE_LAST);
                game_values.gamemode = gamemodes[currentgamemode];
                mGameSettingsMenu->GameModeChanged(currentgamemode);
                game_values.gamemode = gamemodes[mGameSettingsMenu->GetCurrentGameModeID()];

                iTournamentAITimer = 60;
            } else if (iTournamentAIStep == 3) {
                SModeOption * options = game_values.gamemode->GetOptions();

                //Choose a goal from the lower values for a quicker game
                short iRandOption = (RANDOM_INT(6)) + 1;
                game_values.gamemode->goal  = options[iRandOption].iValue;

                mGameSettingsMenu->miGoalField[currentgamemode]->setCurrentValue(gamemodes[currentgamemode]->goal);

                mModeOptionsMenu->SetRandomGameModeSettings(game_values.gamemode->gamemode);

                iTournamentAITimer = 60;
            } else if (iTournamentAIStep == 4) {
                iTournamentAIStep = 0;
                StartGame();
            }
        }
    }

    //Watch for the konami code to unlock the minigames match type
    /*
    if (!game_values.minigameunlocked && mCurrentMenu == mMatchSelectionMenu) {
        if (!mCurrentMenu->GetCurrentControl()->IsModifying()) {
            int keymask =
                (game_values.playerInput.outputControls[0].menu_up.fPressed?1:0) |
                (game_values.playerInput.outputControls[0].menu_down.fPressed?2:0) |
                (game_values.playerInput.outputControls[0].menu_left.fPressed?4:0) |
                (game_values.playerInput.outputControls[0].menu_right.fPressed?8:0) |
                (game_values.playerInput.outputControls[0].menu_random.fPressed?16:0);

            if (iUnlockMinigameOptionIndex < 11) {
                static const int konami_code[11] = {1,1,2,2,4,8,4,8,16,16,16};

                if (keymask & konami_code[iUnlockMinigameOptionIndex])
                    iUnlockMinigameOptionIndex++;
                else if (keymask & ~konami_code[iUnlockMinigameOptionIndex]) {
                    iUnlockMinigameOptionIndex = 0;

                    if (keymask & konami_code[iUnlockMinigameOptionIndex])
                        iUnlockMinigameOptionIndex++;
                }

                if (iUnlockMinigameOptionIndex == 11) {
                    ifSoundOnPlay(rm->sfx_transform);
                    game_values.minigameunlocked = true;

                    mMatchSelectionMenu->ActivateMinigameField();
                }
            }
        }
    }
    */

#ifdef _DEBUG
    if (g_fAutoTest)
        GetNextScriptOperation();

    if (g_fRecordTest)
        SetNextScriptOperation();
#endif

    bool fGenerateMapThumbs = false;
    if (AppState::Menu == game_values.appstate) {
        MenuCodeEnum code = mCurrentMenu->SendInput(&game_values.playerInput);

        // Shortcut to game start
        if (netplay.active) {
            uint8_t lastSendType = netplay.client.lastSentMessage.packageType;
            uint8_t lastRecvType = netplay.client.lastReceivedMessage.packageType;

            if (lastSendType == NET_P2G_SYNC_OK
                && lastRecvType == NET_G2E_GAME_START)
            code = MENU_CODE_NET_ROOM_GO;
        }

        if (MENU_CODE_EXIT_APPLICATION == code) {
            Exit();
            return;
        } else if (MENU_CODE_TO_MAIN_MENU == code) {
            iDisplayError = DISPLAY_ERROR_NONE;
            iDisplayErrorTimer = 0;
            net_endSession();

            mCurrentMenu = mMainMenu.get();
        } else if (MENU_CODE_BACK_TO_MATCH_SELECTION_MENU == code) {
            mCurrentMenu = mMatchSelectionMenu.get();
            iUnlockMinigameOptionIndex = 0;
        } else if (MENU_CODE_TO_MATCH_SELECTION_MENU == code) {
            mMatchSelectionMenu->WorldMapChanged();

            mCurrentMenu = mMatchSelectionMenu.get();
            mCurrentMenu->ResetMenu();
            iUnlockMinigameOptionIndex = 0;
        } else if (MENU_CODE_MATCH_SELECTION_START == code || MENU_CODE_QUICK_GAME_START == code) {
            printf("itt vagyok\n");

            if (MENU_CODE_QUICK_GAME_START == code)
                game_values.matchtype = MatchType::QuickGame;
            else
                game_values.matchtype = mMatchSelectionMenu->GetSelectedMatchType();

            mTeamSelectMenu->ResetTeamSelect();
            mCurrentMenu = mTeamSelectMenu.get();
            mCurrentMenu->ResetMenu();
            printf("Hello\n");

            if (game_values.matchtype != MatchType::Tournament) {
                game_values.tournamentcontrolteam = -1;
                SetControllingTeamForSettingsMenu(game_values.tournamentcontrolteam, false);
            }
        } else if (MENU_CODE_MATCH_SELECTION_MATCH_CHANGED == code) {
            mMatchSelectionMenu->SelectionChanged();
        } else if (MENU_CODE_WORLD_MAP_CHANGED == code) {
            mMatchSelectionMenu->WorldMapChanged();
        } else if (MENU_CODE_TO_OPTIONS_MENU == code) {
            mCurrentMenu = mOptionsMenu.get();
            mCurrentMenu->ResetMenu();
        } else if (MENU_CODE_BACK_TO_OPTIONS_MENU == code) {
            mCurrentMenu = mOptionsMenu.get();
        } else if (MENU_CODE_TO_NET_SERVERS_MENU == code) {
            mCurrentMenu = mNetServersMenu.get();
            mCurrentMenu->ResetMenu();
            net_startSession();
        } else if (MENU_CODE_BACK_TO_GRAPHIC_OPTIONS_MENU == code) {
            mCurrentMenu = mGraphicsOptionsMenu.get();
        } else if (MENU_CODE_TO_CONTROLS_MENU == code) {
            mCurrentMenu = mPlayerControlsSelectMenu.get();
            mCurrentMenu->ResetMenu();
        } else if (MENU_CODE_BACK_TO_CONTROLS_MENU == code) {
            mCurrentMenu = mPlayerControlsSelectMenu.get();
        } else if (MENU_CODE_TO_PLAYER_1_CONTROLS == code) {
            mPlayerControlsMenu->SetPlayer(0);
            mCurrentMenu = mPlayerControlsMenu.get();
            mCurrentMenu->ResetMenu();
        } else if (MENU_CODE_TO_PLAYER_2_CONTROLS == code) {
            mPlayerControlsMenu->SetPlayer(1);
            mCurrentMenu = mPlayerControlsMenu.get();
            mCurrentMenu->ResetMenu();
        } else if (MENU_CODE_TO_PLAYER_3_CONTROLS == code) {
            mPlayerControlsMenu->SetPlayer(2);
            mCurrentMenu = mPlayerControlsMenu.get();
            mCurrentMenu->ResetMenu();
        } else if (MENU_CODE_TO_PLAYER_4_CONTROLS == code) {
            mPlayerControlsMenu->SetPlayer(3);
            mCurrentMenu = mPlayerControlsMenu.get();
            mCurrentMenu->ResetMenu();
        }
        else if (MENU_CODE_TOGGLE_FULLSCREEN == code) {
			gfx_changefullscreen(game_values.fullscreen);
            blitdest = screen;
        }
        else if (MENU_CODE_TO_GAME_SETUP_MENU == code) {
            printf("MENU_CODE_TO_GAME_SETUP_MENU\n");
            //Moves teams to the first arrays in the list and counts the number of teams
            score_cnt = mTeamSelectMenu->GetTeamCount();
            iDisplayError = DISPLAY_ERROR_NONE;
            iDisplayErrorTimer = 0;
            bool fErrorReadingTourFile = false;

            if (MatchType::MiniGame == game_values.matchtype) {
                printf(" Match type: Minigame\n");
                const Minigame minigame = mMatchSelectionMenu->GetMinigame();
                switch (minigame) {
                    case Minigame::PipeCoin:
                        pipegamemode->goal = 50;
                        game_values.gamemode = pipegamemode;
                        break;
                    case Minigame::HammerBoss:
                    case Minigame::BombBoss:
                    case Minigame::FireBoss:
                        game_values.gamemodemenusettings.boss.difficulty = 2;
                        game_values.gamemodemenusettings.boss.hitpoints = 5;

                        bossgamemode->goal = 5;
                        game_values.gamemode = bossgamemode;
                        break;
                    case Minigame::Boxes:
                        boxesgamemode->goal = 10;
                        game_values.gamemode = boxesgamemode;
                        break;
                }
                switch (minigame) {
                    case Minigame::HammerBoss:
                        game_values.gamemodemenusettings.boss.bosstype = Boss::Hammer;
                        break;
                    case Minigame::BombBoss:
                        game_values.gamemodemenusettings.boss.bosstype = Boss::Bomb;
                        break;
                    case Minigame::FireBoss:
                        game_values.gamemodemenusettings.boss.bosstype = Boss::Fire;
                        break;
                    default:
                        break;
                }
                StartGame();
            } else if (MatchType::QuickGame == game_values.matchtype) {
                printf(" Match type: Quick game\n");
                short iRandomMode = RANDOM_INT( GAMEMODE_LAST);
                game_values.gamemode = gamemodes[iRandomMode];

                SModeOption * options = game_values.gamemode->GetOptions();

                //Choose a goal from the lower values for a quicker game
                short iRandOption = (RANDOM_INT(6)) + 1;
                game_values.gamemode->goal  = options[iRandOption].iValue;

                game_values.tournamentwinner = -1;

                StartGame();
            } else {

                //Load the tour here if one was selected
                if (game_values.matchtype == MatchType::Tour) {
                    printf("  Match type: Tour\n");
                    if (!ReadTourFile()) {
                        iDisplayError = DISPLAY_ERROR_READ_TOUR_FILE;
                        iDisplayErrorTimer = 120;
                        fErrorReadingTourFile = true;
                    } else {
                        mTournamentScoreboardMenu->miTournamentScoreboard->CreateScoreboard(score_cnt, game_values.tourstoptotal, &rm->spr_tour_markers);
                    }
                } else if (game_values.matchtype == MatchType::Tournament) {
                    printf("  Match type: Tournament\n");
                    //Set who is controlling the tournament menu
                    if (game_values.tournamentcontrolstyle == TournamentControlStyle::Random || game_values.tournamentcontrolstyle == TournamentControlStyle::RandomLoser) //Random
                        game_values.tournamentcontrolteam = RANDOM_INT( score_cnt);
                    else if (game_values.tournamentcontrolstyle == TournamentControlStyle::RoundRobin) //Round robin
                        game_values.tournamentcontrolteam = 0;
                    else //The first game of the tournament is controlled by all players
                        game_values.tournamentcontrolteam = -1;

                    game_values.tournamentnextcontrol = 1;  //if round robin is selected, choose the next team next

                    mTournamentScoreboardMenu->miTournamentScoreboard->CreateScoreboard(score_cnt, game_values.tournamentgames, &rm->menu_mode_large);
                } else if (game_values.matchtype == MatchType::World) {
                    printf("  Match type: World\n");
                    if (!g_worldmap.Load(TILESIZE)) {
                        iDisplayError = DISPLAY_ERROR_READ_WORLD_FILE;
                        iDisplayErrorTimer = 120;
                        fErrorReadingTourFile = true;
                    } else {
                        mTournamentScoreboardMenu->miTournamentScoreboard->CreateScoreboard(score_cnt, 0, &rm->spr_tour_markers);

                        g_worldmap.SetInitialPowerups();

                        //If a player had a stored powerup from another game, add it to their inventory
                        for (short iPlayer = 0; iPlayer < 4; iPlayer++) {
                            if (game_values.storedpowerups[iPlayer] != -1) {
                                short iTeamId = LookupTeamID(iPlayer);
                                if (game_values.worldpowerupcount[iTeamId] < 32)
                                    game_values.worldpowerups[iTeamId][game_values.worldpowerupcount[iTeamId]++] = game_values.storedpowerups[iPlayer];

                                game_values.storedpowerups[iPlayer] = -1;
                            }
                        }

                        mWorldMenu->miWorld->Init();
                        mWorldMenu->miWorld->SetControllingTeam(RANDOM_INT( score_cnt));
                    }
                }

                if (!fErrorReadingTourFile) {
                    printf("  !fErrorReadingTourFile\n");
                    mTournamentScoreboardMenu->ClearEyeCandy();

                    //Initialize tournament values
                    game_values.tournamentwinner = -1;

                    //Setup wins counters for tournament/tour
                    for (int k = 0; k < 4; k++) {
                        game_values.tournament_scores[k].wins = 0;
                        game_values.tournament_scores[k].total = 0;
                    }

                    if (MatchType::SingleGame == game_values.matchtype || MatchType::Tournament == game_values.matchtype || MatchType::NetGame == game_values.matchtype) {
                        printf("  MatchType::SingleGame || MatchType::Tournament\n");
                        printf("current map: %s\n", szCurrentMapName);
                        maplist->findexact(szCurrentMapName, false);
                        mGameSettingsMenu->miMapField->LoadCurrentMap();

                        game_values.gamemode = gamemodes[mGameSettingsMenu->GetCurrentGameModeID()];

                        for (short iMode = 0; iMode < GAMEMODE_LAST; iMode++) {
                            gamemodes[iMode]->goal = mGameSettingsMenu->miGoalField[iMode]->currentValue();
                        }

                        if (mGameSettingsMenu->GetCurrentGameModeID() == game_mode_owned)
                            mGameSettingsMenu->HideGMSettingsBtn();

                        mCurrentMenu = mGameSettingsMenu.get();
                        mCurrentMenu->ResetMenu();

                        //If it is a tournament, then set the controlling team
                        if (MatchType::Tournament == game_values.matchtype)
                            SetControllingTeamForSettingsMenu(game_values.tournamentcontrolteam, true);
                        } else if (MatchType::Tour == game_values.matchtype) {
                            mCurrentMenu = mTourStopMenu.get();
                            mCurrentMenu->ResetMenu();
                        } else if (MatchType::World == game_values.matchtype) {
                            game_values.screenfadespeed = 8;
                            game_values.screenfade = 8;
                            game_values.appstate = AppState::StartWorld;

                            //Play enter world sound
                            ifsoundonandreadyplay(rm->sfx_enterstage);
                        }

                    //Setup items on next menu
                    for (short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++) {
                        mGameSettingsMenu->miGoalField[iGameMode]->hideItem(-1, game_values.matchtype == MatchType::Tournament);
                    }

                    if (game_values.matchtype == MatchType::World)
                        mGameSettingsMenu->SetHeaderText("World Game Menu");
                    else if (game_values.matchtype == MatchType::Tour)
                        mGameSettingsMenu->SetHeaderText("Tour Game Menu");
                    else if (game_values.matchtype == MatchType::Tournament)
                        mGameSettingsMenu->SetHeaderText("Tournament Game Menu");
                    else
                        mGameSettingsMenu->SetHeaderText("Single Game Menu");

                }
            }
        } else if (MENU_CODE_BACK_TO_GAME_SETUP_MENU == code) {
            bool fNeedTeamAnnouncement = false;
            if (game_values.matchtype == MatchType::World) {
                if (game_values.tournamentwinner == -2 || (game_values.tournamentwinner >= 0 && game_values.bonuswheel == 0)) {
                    ResetTournamentBackToMainMenu();
                } else if (game_values.tournamentwinner >= 0) {
                    mBonusWheelMenu->miBonusWheel->Reset(true);
                    mCurrentMenu = mBonusWheelMenu.get();
                } else {
                    mCurrentMenu = mWorldMenu.get();
                    mWorldMenu->miWorldStop->Refresh(game_values.tourstopcurrent);

                    fNeedTeamAnnouncement = true;
                }
            } else {
                if (game_values.tournamentwinner == -2) { //Tied Tour Result
                    ResetTournamentBackToMainMenu();
                } else if (game_values.tournamentwinner >= 0) { //Tournament/Tour Won and Bonus Wheel will be spun
                    if (game_values.bonuswheel == 0 || (game_values.matchtype == MatchType::Tour && !game_values.tourstops[game_values.tourstopcurrent - 1]->iBonusType)) {
                        ResetTournamentBackToMainMenu();
                    } else {
                        mBonusWheelMenu->miBonusWheel->Reset(true);
                        mCurrentMenu = mBonusWheelMenu.get();
                    }
                } else { //Next Tour/Tourament Game
                    if (game_values.matchtype == MatchType::Tour) {
                        mCurrentMenu = mTourStopMenu.get();
                    } else {
                        mCurrentMenu = mGameSettingsMenu.get();
                    }
                }
            }

            mCurrentMenu->ResetMenu();

            //Set the controlling team for tournament mode
            if (MatchType::Tournament == game_values.matchtype && game_values.tournamentwinner == -1)
                SetControllingTeamForSettingsMenu(game_values.tournamentcontrolteam, true);

            if (fNeedTeamAnnouncement)
                mWorldMenu->miWorld->DisplayTeamControlAnnouncement();
        } else if (MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS == code) {
            if (netplay.active)
                mCurrentMenu = mNetNewRoomSettingsMenu.get();
            else
                mCurrentMenu = mGameSettingsMenu.get();
        } else if (MENU_CODE_MODE_CHANGED == code) {
            game_values.gamemode = gamemodes[mGameSettingsMenu->GetCurrentGameModeID()];
            mGameSettingsMenu->RefreshGameModeButtons();
            mNetNewRoomSettingsMenu->RefreshGameModeButtons();
        } else if (MENU_CODE_BACK_TEAM_SELECT_MENU == code) {
            if (game_values.matchtype == MatchType::World) {
                mWorldMenu->OpenExitDialog();
            } else if (game_values.matchtype == MatchType::Tour) {
                mTourStopMenu->OpenExitDialog();
            } else if (game_values.matchtype == MatchType::Tournament) {
                mGameSettingsMenu->OpenExitDialog();

                if (iTournamentAITimer > 0)
                    SetControllingTeamForSettingsMenu(-1, false);
            } else {
                mCurrentMenu = mTeamSelectMenu.get();
                mCurrentMenu->ResetMenu();
            }
        } else if (MENU_CODE_SOUND_VOLUME_CHANGED == code) {
            game_values.sound = game_values.soundvolume > 0;
            sfx_setsoundvolume(game_values.soundvolume);
            ifSoundOnPlay(rm->sfx_coin);
        } else if (MENU_CODE_MUSIC_VOLUME_CHANGED == code) {
            sfx_setmusicvolume(game_values.musicvolume);

            if (game_values.musicvolume == 0)
                rm->backgroundmusic[2].stop();
            else if (game_values.musicvolume > 0 && !game_values.music)
                rm->backgroundmusic[2].play(false, false);

            game_values.music = game_values.musicvolume > 0;
        } else if (MENU_CODE_START_GAME == code) {
            StartGame();
        } else if (MENU_CODE_EXIT_TOURNAMENT_YES == code || MENU_CODE_EXIT_TOURNAMENT_NO == code) {
            mGameSettingsMenu->CloseExitDialog();

            if (MENU_CODE_EXIT_TOURNAMENT_YES == code)
                ResetTournamentBackToMainMenu();
            else
                SetControllingTeamForSettingsMenu(game_values.tournamentcontrolteam, false);
        } else if (MENU_CODE_EXIT_TOUR_YES == code || MENU_CODE_EXIT_TOUR_NO == code) {
            mTourStopMenu->CloseExitDialog();

            if (MENU_CODE_EXIT_TOUR_YES == code)
                ResetTournamentBackToMainMenu();
        } else if (MENU_CODE_EXIT_WORLD_YES == code || MENU_CODE_EXIT_WORLD_NO == code) {
            mWorldMenu->CloseExitDialog();

            if (MENU_CODE_EXIT_WORLD_YES == code) {
                //Clear out any stored items a player might have
                for (short iPlayer = 0; iPlayer < 4; iPlayer++)
                    game_values.storedpowerups[iPlayer] = -1;

                ResetTournamentBackToMainMenu();
                mWorldMenu->miWorldStop->setVisible(false);
            }
        } else if (MENU_CODE_BONUS_DONE == code) {
            if (mBonusWheelMenu->miBonusWheel->GetPowerupSelectionDone()) {
                if (game_values.matchtype == MatchType::World) {
                    if (game_values.tournamentwinner == -1)
                        mCurrentMenu = mTournamentScoreboardMenu.get();
                    else
                        ResetTournamentBackToMainMenu();
                } else {
                    if ((game_values.matchtype == MatchType::Tour || game_values.matchtype == MatchType::Tournament) && game_values.tournamentwinner == -1 &&
                            ((game_values.matchtype != MatchType::Tour && game_values.bonuswheel == 2) || (game_values.matchtype == MatchType::Tour && game_values.tourstops[game_values.tourstopcurrent - 1]->iBonusType))) {
                        mCurrentMenu = mTournamentScoreboardMenu.get();
                    } else if (game_values.matchtype == MatchType::SingleGame) {
                        mCurrentMenu = mGameSettingsMenu.get();
                        mCurrentMenu->ResetMenu();
                        mGameSettingsMenu->miMapField->LoadCurrentMap();
                    } else {
                        ResetTournamentBackToMainMenu();
                    }
                }
            }
        } else if (MENU_CODE_TO_POWERUP_SELECTION_MENU == code) {
            mCurrentMenu = mPowerupDropRatesMenu.get();
            mCurrentMenu->ResetMenu();
        } else if (MENU_CODE_TO_POWERUP_SETTINGS_MENU == code) {
            mCurrentMenu = mPowerupSettingsMenu.get();
            mCurrentMenu->ResetMenu();
        } else if (MENU_CODE_TO_GRAPHICS_OPTIONS_MENU == code) {
            mCurrentMenu = mGraphicsOptionsMenu.get();
            mCurrentMenu->ResetMenu();
        } else if (MENU_CODE_TO_EYECANDY_OPTIONS_MENU == code) {
            mCurrentMenu = mEyeCandyOptionsMenu.get();
            mCurrentMenu->ResetMenu();
        } else if (MENU_CODE_TO_SOUND_OPTIONS_MENU == code) {
            mCurrentMenu = mSoundOptionsMenu.get();
            mCurrentMenu->ResetMenu();
        } else if (MENU_CODE_TO_GAMEPLAY_OPTIONS_MENU == code) {
            mCurrentMenu = mGameplayOptionsMenu.get();
            mCurrentMenu->ResetMenu();
        } else if (MENU_CODE_TO_TEAM_OPTIONS_MENU == code) {
            mCurrentMenu = mTeamOptionsMenu.get();
            mCurrentMenu->ResetMenu();
        } else if (MENU_CODE_TO_PROJECTILES_OPTIONS_MENU == code) {
            mCurrentMenu = mProjectileOptionsMenu.get();
            mCurrentMenu->ResetMenu();
        } else if (MENU_CODE_TO_PROJECTILES_LIMITS_MENU == code) {
            mCurrentMenu = mProjectileLimitsMenu.get();
            mCurrentMenu->ResetMenu();
        } else if (MENU_CODE_TO_MODE_SETTINGS_MENU == code) {
            if (netplay.active) {
                mCurrentMenu = mModeOptionsMenu->GetOptionsMenu(mNetNewRoomSettingsMenu->getSelectedGameModeID());
                mCurrentMenu->ResetMenu();
            }
            else {
                for (short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++) {
                    if (mGameSettingsMenu->miGoalField[iGameMode]->IsVisible()) {
                        mCurrentMenu = mModeOptionsMenu->GetOptionsMenu(iGameMode);
                        mCurrentMenu->ResetMenu();
                        break;
                    }
                }
            }
        } else if (MENU_CODE_MENU_GRAPHICS_PACK_CHANGED == code) {
            LoadStartGraphics();
            rm->LoadMenuGraphics();

            blitdest = rm->menu_backdrop.getSurface();
            rm->menu_shade.setalpha(App::menuTransparency);
            rm->menu_shade.draw(0, 0);
            blitdest = screen;
        } else if (MENU_CODE_WORLD_GRAPHICS_PACK_CHANGED == code) {
            rm->LoadWorldGraphics();
        } else if (MENU_CODE_GAME_GRAPHICS_PACK_CHANGED == code) {
            const bool pngPalette = gfx_loadpalette(convertPath("gfx/packs/palette.png", gamegraphicspacklist->currentPath()));
            if (!pngPalette) {
                gfx_loadpalette(convertPath("gfx/packs/palette.bmp", gamegraphicspacklist->currentPath()));
            }
            rm->LoadGameGraphics();
        } else if (MENU_CODE_SOUND_PACK_CHANGED == code) {
            rm->LoadGameSounds();

            if (!game_values.soundcapable) {
                game_values.sound = false;
                game_values.music = false;
                game_values.soundvolume = 0;
                game_values.musicvolume = 0;
            }
        } else if (MENU_CODE_WORLD_STAGE_START == code) {
            mWorldMenu->OpenStageStart();
        } else if (MENU_CODE_WORLD_STAGE_NO_START == code) {
            mWorldMenu->CloseStageStart();
        } else if (MENU_CODE_WORLD_MUSIC_CHANGED == code) {
            worldmusiclist->setCurrent(mSoundOptionsMenu->GetCurrentWorldMusicID());
        } else if (MENU_CODE_TOUR_STOP_CONTINUE == code || MENU_CODE_TOUR_STOP_CONTINUE_FORCED == code) {
            //If this tour stop is forced, we need to load the map first
            if (MENU_CODE_TOUR_STOP_CONTINUE_FORCED == code)
                mWorldMenu->miWorldStop->Refresh(game_values.tourstopcurrent);

            mWorldMenu->miWorld->ClearCloud();

            //Tour bonus house
            if (game_values.matchtype == MatchType::World && game_values.tourstops[game_values.tourstopcurrent]->iStageType == 1) {
                bonushousemode->goal = 0;
                game_values.gamemode = bonushousemode;
            } else {
                short iGameMode = game_values.tourstops[game_values.tourstopcurrent]->iMode;

                if (iGameMode == game_mode_pipe_minigame)
                    game_values.gamemode = pipegamemode;
                else if (iGameMode == game_mode_boss_minigame)
                    game_values.gamemode = bossgamemode;
                else if (iGameMode == game_mode_boxes_minigame)
                    game_values.gamemode = boxesgamemode;
                else
                    game_values.gamemode = gamemodes[iGameMode];

                game_values.gamemode->goal = game_values.tourstops[game_values.tourstopcurrent]->iGoal;
            }

            StartGame();
        } else if (MENU_CODE_RESET_STORED_POWERUPS == code) {
            for (short iPlayer = 0; iPlayer < 4; iPlayer++)
                game_values.storedpowerups[iPlayer] = -1;
        } else if (MENU_CODE_MAP_CHANGED == code) {
            if (mCurrentMenu == mNetNewRoomSettingsMenu.get()) {
                assert(netplay.active);
                netplay.mapfilepath = mNetNewRoomSettingsMenu->getCurrentMapPath();
                printf("[net] Selected map: %s\n", netplay.mapfilepath.c_str());
                mNetRoomMenu->SetPreviewMapPath(netplay.mapfilepath);
            }
            else if (game_values.matchtype != MatchType::Tour)
                szCurrentMapName = mGameSettingsMenu->miMapField->GetMapName();
        } else if (MENU_CODE_MAP_FILTER_EXIT == code) {
            maplist->ApplyFilters(game_values.pfFilters);

            //If the filtered map list has at least 1 map in it, then allow exiting the filter menu
            if (maplist->MapInFilteredSet()) {
                mGameSettingsMenu->miMapField->LoadCurrentMap();
                szCurrentMapName = mGameSettingsMenu->miMapField->GetMapName();

                mGameSettingsMenu->CloseMapFilters();

                iDisplayError = DISPLAY_ERROR_NONE;
            } else { //otherwise display a message
                iDisplayError = DISPLAY_ERROR_MAP_FILTER;
                iDisplayErrorTimer = 120;
            }
        } else if (MENU_CODE_TO_MAP_FILTERS == code) {
            mGameSettingsMenu->OpenMapFilters();
        } else if (MENU_CODE_TO_MAP_FILTER_EDIT == code) {
            mMapFilterEditMenu->miMapBrowser->Reset(0);

            mCurrentMenu = mMapFilterEditMenu.get();
            mCurrentMenu->ResetMenu();
        } else if (MENU_CODE_MAP_BROWSER_EXIT == code) {
            mGameSettingsMenu->miMapField->LoadCurrentMap();
            szCurrentMapName = mGameSettingsMenu->miMapField->GetMapName();

            mCurrentMenu = mGameSettingsMenu.get();
            //mCurrentMenu->ResetMenu();
        } else if (MENU_CODE_TO_MAP_BROWSER_THUMBNAILS == code) {
            mMapFilterEditMenu->miMapBrowser->Reset(1);

            mCurrentMenu = mMapFilterEditMenu.get();
            mCurrentMenu->ResetMenu();
        } else if (MENU_CODE_SAVE_ALL_MAP_THUMBNAILS == code) {
            ((UI_OptionsMenu*) mCurrentMenu)->showThumbnailsPopup();
            mCurrentMenu->ResetMenu();
        } else if (MENU_CODE_GENERATE_THUMBS_RESET_YES == code || MENU_CODE_GENERATE_THUMBS_RESET_NO == code) {
            ((UI_OptionsMenu*) mCurrentMenu)->hideThumbnailsPopup();

            if (MENU_CODE_GENERATE_THUMBS_RESET_YES == code) {
                fGenerateMapThumbs = true;
            }
        } else if (MENU_CODE_HEALTH_MODE_START_LIFE_CHANGED == code) {
            mModeOptionsMenu->HealthModeStartLifeChanged();
        } else if (MENU_CODE_HEALTH_MODE_MAX_LIFE_CHANGED == code) {
            mModeOptionsMenu->HealthModeMaxLifeChanged();
        }

        if (netplay.active) {

            LastMessage lastSent = netplay.client.lastSentMessage;
            LastMessage lastRecv = netplay.client.lastReceivedMessage;

            // Override menu code if response has arrived
            if (netplay.operationInProgress)
            {
                MenuCodeEnum previousCode = code;

                //printf("\rlastSendType: %d, lastRecvType: %d", lastSent.packageType, lastRecv.packageType);

                // if sent connect request,
                // received connect_ok
                // then sent skin change
                if (lastSent.packageType == NET_NOTICE_SKIN_CHANGE
                        && lastRecv.packageType == NET_RESPONSE_CONNECT_OK
                        && lastSent.timestamp >= lastRecv.timestamp)
                    code = MENU_CODE_TO_NET_LOBBY_MENU;

                else if (lastSent.packageType == NET_REQUEST_JOIN_ROOM
                        && lastRecv.packageType == NET_RESPONSE_JOIN_OK
                        && lastSent.timestamp < lastRecv.timestamp) // ensure that the incoming message arrived after the request
                    code = MENU_CODE_TO_NET_ROOM_MENU;

                else if (lastSent.packageType == NET_NOTICE_GAMEMODESETTINGS
                        && lastRecv.packageType == NET_RESPONSE_CREATE_OK)
                    code = MENU_CODE_TO_NET_ROOM_MENU;

                // ide a hibakezelést
                // if lastmessage ez és lastresponse emez
                // control.settext...

                // If we have finished the waiting
                if (code != previousCode)
                    netplay.operationInProgress = false;
            }

            if (MENU_CODE_TO_NET_SERVERLIST == code) {
                mNetServersMenu->OpenServerList();
            } else if (MENU_CODE_TO_NET_ADDREMOVE_SERVER_MENU == code) {
                mCurrentMenu = mNetEditServersMenu.get();
                mCurrentMenu->ResetMenu();
                mNetEditServersMenu->Restore();
            } else if (MENU_CODE_NET_ADDREMOVE_SERVER_ON_ADD_BTN == code) {
                mNetEditServersMenu->onPressAdd();
            } else if (MENU_CODE_NET_ADDREMOVE_SERVER_ON_EDIT_BTN == code) {
                mNetEditServersMenu->onPressEdit();
            } else if (MENU_CODE_NET_ADDREMOVE_SERVER_ON_DELETE_BTN == code) {
                mNetEditServersMenu->onPressDelete();
            } else if (MENU_CODE_NET_ADDREMOVE_SERVER_ON_SELECT == code) {
                mNetEditServersMenu->onEntrySelect();
                mNetServersMenu->RefreshScroll();
            } else if (MENU_CODE_NET_ADDREMOVE_SERVER_ON_DIALOG_OK_BTN == code) {
                mNetEditServersMenu->onDialogOk();
                mNetServersMenu->RefreshScroll();
            } else if (MENU_CODE_NET_SERVERLIST_EXIT == code || MENU_CODE_NET_CONNECT_ABORT == code) {
                if (MENU_CODE_NET_SERVERLIST_EXIT == code)
                    netplay.currentMenuChanged = true;
                else {
                    net_startSession(); // release & restart socket
                    netplay.operationInProgress = false;
                }

                mNetServersMenu->Restore();
                iDisplayError = DISPLAY_ERROR_NONE;
            } else if (MENU_CODE_NET_CONNECT_IN_PROGRESS == code) {
                mNetServersMenu->ConnectInProgress();
            } else if (MENU_CODE_TO_NET_LOBBY_MENU == code) {
                // If we are leaving a room
                if (netplay.currentRoom.roomID) {
                    netplay.client.sendLeaveRoomMessage();
                    netplay.currentRoom.roomID = 0;
                    netplay.joinSuccessful = false;
                    netplay.gameRunning = false;
                }
                netplay.client.requestRoomList();

                // Restore layouts
                    mNetServersMenu->Restore();
                    mNetLobbyMenu->Restore();
                    mNetNewRoomMenu->Restore();

                mCurrentMenu = mNetLobbyMenu.get();
                mCurrentMenu->ResetMenu();
            } else if (MENU_CODE_TO_NET_NEW_ROOM_LEVEL_SELECT_MENU == code) {
                mCurrentMenu = mNetNewRoomSettingsMenu.get();
                mCurrentMenu->ResetMenu();
            } else if (MENU_CODE_TO_NET_NEW_ROOM_SETTINGS_MENU == code) {
                mCurrentMenu = mNetNewRoomMenu.get();
                mCurrentMenu->ResetMenu();
            } else if (MENU_CODE_NET_CHAT_SEND == code) {
                if (strlen(netplay.mychatmessage) > 0)
                    netplay.client.sendChatMessage(netplay.mychatmessage);
                else
                    code = MENU_CODE_TO_NET_ROOM_MENU;
            } else if (MENU_CODE_TO_NET_ROOM_MENU == code) {
                mCurrentMenu = mNetRoomMenu.get();
                mCurrentMenu->ResetMenu();
                netplay.currentMenuChanged = true;
                netplay.operationInProgress = false;

                mNetRoomMenu->Restore(); // Restore Room layout
                mNetLobbyMenu->Restore(); // Restore Lobby layout
                mNetNewRoomMenu->Restore(); // remove 'in progress' dialog
            } else if (MENU_CODE_NET_JOIN_ROOM_IN_PROGRESS == code) {
                mNetLobbyMenu->JoinInProgress();
            } else if (MENU_CODE_NET_JOIN_ROOM_ABORT == code) {
                mNetLobbyMenu->AbortJoin();
                iDisplayError = DISPLAY_ERROR_NONE;
            } else if (MENU_CODE_TO_NET_NEW_ROOM_CREATE_IN_PROGRESS == code) {
                mNetNewRoomMenu->CreateInProgress();
            } else if (MENU_CODE_TO_NET_NEW_ROOM_CREATE_ABORT == code) {
                mNetNewRoomMenu->AbortCreate();
                iDisplayError = DISPLAY_ERROR_NONE;
            } else if (MENU_CODE_TO_NET_ROOM_START_IN_PROGRESS == code) {
                mNetRoomMenu->StartInProgress();
            } else if (MENU_CODE_NET_ROOM_GO == code) {
                netplay.operationInProgress = false;
                game_values.matchtype = MatchType::NetGame;

                mTeamSelectMenu->ResetTeamSelect();
                mTeamSelectMenu->ResetMenu();
                score_cnt = mTeamSelectMenu->GetTeamCount();

                game_values.tournamentcontrolteam = -1;
                game_values.tournamentwinner = -1;

                iDisplayError = DISPLAY_ERROR_NONE;
                iDisplayErrorTimer = 0;

                //game_values.noexit = true;
                StartGame();
            }

            // on room change
            if (netplay.currentMenuChanged) {
                //printf("menuChanged\n");

                mNetServersMenu->Refresh();
                mNetRoomMenu->Refresh();

                netplay.currentMenuChanged = false;
            }
        }

        /*if (code != MENU_CODE_NONE)
            printf("Code: %d\n", code);*/
    }

    //--------------- draw everything ----------------------

    //Don't draw backdrop for world
    if (mCurrentMenu != mWorldMenu.get())
        rm->menu_backdrop.draw(0, 0);
    else
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

    mCurrentMenu->Update();
    mCurrentMenu->Draw();

    if (iDisplayError > DISPLAY_ERROR_NONE) {
        rm->spr_selectfield.draw(70, 400, 0, 0, 484, 32);
        rm->spr_selectfield.draw(554, 400, 496, 0, 16, 32);

        if (iDisplayError == DISPLAY_ERROR_READ_TOUR_FILE)
            rm->menu_font_large.drawCentered(320, 405, "Error Reading Tour File!");
        if (iDisplayError == DISPLAY_ERROR_READ_WORLD_FILE)
            rm->menu_font_large.drawCentered(320, 405, "Error Reading World File!");
        else if (iDisplayError == DISPLAY_ERROR_MAP_FILTER)
            rm->menu_font_large.drawCentered(320, 405, "No Maps Meet All Filter Conditions!");

        if (--iDisplayErrorTimer == 0)
            iDisplayError = DISPLAY_ERROR_NONE;
    }

    if (game_values.screenfadespeed != 0) {
        game_values.screenfade += game_values.screenfadespeed;

        if (game_values.screenfade <= 0) {
            game_values.screenfadespeed = 0;
            game_values.screenfade = 0;
        } else if (game_values.screenfade >= 255) {
            game_values.screenfadespeed = 0;
            game_values.screenfade = 255;
        }
    }

    if (game_values.screenfade > 0) {
        rm->menu_shade.setalpha((Uint8)game_values.screenfade);
        rm->menu_shade.draw(0, 0);
    }

    if (game_values.screenfade == 255) {
        if (AppState::StartGame == game_values.appstate) {
            if (game_values.matchtype == MatchType::QuickGame)
                mModeOptionsMenu->SetRandomGameModeSettings(game_values.gamemode->gamemode);
            else if (game_values.matchtype == MatchType::NetGame)
                // TODO: set from network
                mModeOptionsMenu->SetRandomGameModeSettings(game_values.gamemode->gamemode);
            else
                SetGameModeSettingsFromMenu();

            if (game_values.matchtype == MatchType::World && game_values.tourstops[game_values.tourstopcurrent]->iStageType == 1) {
                g_map->loadMap(convertPath("maps/special/two52_special_bonushouse.map"), read_type_full);
                LoadCurrentMapBackground();

                if (game_values.music) {
                    rm->backgroundmusic[0].load(worldmusiclist->currentMusic(WORLDMUSICBONUS, ""));
                    rm->backgroundmusic[0].play(false, false);
                }
            } else {
                std::string sShortMapName = "";

                bool fMiniGameMapFound = false;

                if (game_values.matchtype == MatchType::World) {
                    if (game_values.gamemode->gamemode == game_mode_pipe_minigame ||
                            game_values.gamemode->gamemode == game_mode_boss_minigame ||
                            game_values.gamemode->gamemode == game_mode_boxes_minigame) {
                        fMiniGameMapFound = maplist->findexact(game_values.tourstops[game_values.tourstopcurrent]->pszMapFile.c_str(), true);

                        if (fMiniGameMapFound) {
                            g_map->loadMap(maplist->currentFilename(), read_type_full);
                            sShortMapName = maplist->currentShortmapname();
                        }
                    }
                }

                if (game_values.gamemode->gamemode == game_mode_pipe_minigame) {
                    if (!fMiniGameMapFound) {
                        g_map->loadMap(convertPath("maps/special/two52_special_pipe_minigame.map"), read_type_full);
                        sShortMapName = "minigamepipe";
                    }
                } else if (game_values.gamemode->gamemode == game_mode_boss_minigame) {
                    if (!fMiniGameMapFound) {
                        Boss bossType = game_values.gamemodesettings.boss.bosstype;
                        bossgamemode->SetBossType(bossType);
                        switch (bossType) {
                            case Boss::Hammer:
                                g_map->loadMap(convertPath("maps/special/two52_special_hammerboss_minigame.map"), read_type_full);
                                break;
                            case Boss::Bomb:
                                g_map->loadMap(convertPath("maps/special/two52_special_bombboss_minigame.map"), read_type_full);
                                break;
                            case Boss::Fire:
                                g_map->loadMap(convertPath("maps/special/two52_special_fireboss_minigame.map"), read_type_full);
                                break;
                        }
                        sShortMapName = "minigameboss";
                    }
                } else if (game_values.gamemode->gamemode == game_mode_boxes_minigame) {
                    if (!fMiniGameMapFound) {
                        g_map->loadMap(convertPath("maps/special/two52_special_boxes_minigame.map"), read_type_full);
                        sShortMapName = "minigameboxes";
                    }
                } else if (game_values.matchtype == MatchType::QuickGame) {
                    //Load a random map for the quick game
                    std::string szMapName = maplist->randomFilename();
                    g_map->loadMap(szMapName, read_type_full);
                    sShortMapName = stripPathAndExtension(szMapName);

                    printf("  State: GS_START_GAME, Match type: MatchType::QuickGame\n");
                } else if (netplay.active) {
                    // NOTE: for the host, netplay.mapfilepath will be ./data/something
                    // while for the other players, it's ~/.smw/net_last.map
                    g_map->loadMap(netplay.mapfilepath, read_type_full);
                    // TODO: this is used for setting background music, but
                    // since the filename is always the same, this will fall
                    // back to a random music in the loaded music category
                    sShortMapName = stripPathAndExtension(netplay.mapfilepath);
                } else {
                    g_map->loadMap(maplist->currentFilename(), read_type_full);
                    sShortMapName = maplist->currentShortmapname();
                }

                LoadCurrentMapBackground();

                //Allows all players to start the game
                game_values.singleplayermode = -1;

                if (game_values.music) {
                    musiclist->setRandomMusic(g_map->musicCategoryID, sShortMapName.c_str(), g_map->szBackgroundFile);
                    rm->backgroundmusic[0].load(musiclist->currentMusic());
                    rm->backgroundmusic[0].play(game_values.playnextmusic, false);
                }
            }

            game_values.appstate = AppState::Game;
            printf("  GS_GAME\n");

            g_map->predrawbackground(rm->spr_background, rm->spr_backmap[0]);
            g_map->predrawforeground(rm->spr_frontmap[0]);

            g_map->predrawbackground(rm->spr_background, rm->spr_backmap[1]);
            g_map->predrawforeground(rm->spr_frontmap[1]);

            g_map->SetupAnimatedTiles();
            LoadMapObjects(false);

            GameStateManager::instance().changeStateTo(&GameplayState::instance());
            return;
        } else if (AppState::StartWorld == game_values.appstate) { //Fade to world match type
            game_values.screenfadespeed = -8;

            mCurrentMenu = mWorldMenu.get();
            mCurrentMenu->ResetMenu();

            rm->backgroundmusic[2].stop();
            rm->backgroundmusic[5].load(worldmusiclist->currentMusic(g_worldmap.GetMusicCategory(), g_worldmap.GetWorldName()));
            rm->backgroundmusic[5].play(false, false);
            fNeedMenuMusicReset = true;

            mWorldMenu->miWorld->DisplayTeamControlAnnouncement();

            game_values.appstate = AppState::Menu;
        }
    }

    if (fGenerateMapThumbs) {
        rm->menu_dialog.draw(160, 176, 0, 0, 160, 64);
		rm->menu_dialog.draw(App::screenWidth/2, 176, 352, 0, 160, 64);
        rm->menu_dialog.draw(160, 240, 0, 416, 160, 64);
		rm->menu_dialog.draw(App::screenWidth/2, App::screenHeight/2, 352, 416, 160, 64);
		rm->menu_font_large.drawCentered(App::screenWidth/2, 215, "Refreshing Map Thumbnails");
		rm->menu_font_large.drawCentered(App::screenWidth/2, 245, "Please Wait...");
    }

// TODO: FIX
/*#if	!_DEBUG
    if (game_values.showfps)
#endif
        rm->menu_font_large.drawf(0, App::screenHeight - rm->menu_font_large.getHeight(), "Actual:%.1f/%.1f, Flip:%.1f, Potential:%.1f", realfps, 1000.0f / (float)WAITTIME, flipfps, 1000.0f / (float)ticks);
*/

#ifdef _DEBUG
    if (g_fAutoTest)
        rm->menu_font_small.drawRightJustified(635, 5, "Auto");

    if (g_fRecordTest)
        rm->menu_font_small.drawRightJustified(635, 5, "Recording...");
#endif

    if (fGenerateMapThumbs) {
        fGenerateMapThumbs = false;
        rm->backgroundmusic[2].togglePause();

        //Reload map auto filters from live map files (don't use the cache)
        maplist->ReloadMapAutoFilters();

        //Write out all the map thumbnails for the map browser and filter editor
        std::map<std::string, MapListNode>::iterator itr = maplist->GetIteratorAt(0, false);

        short iMapCount = maplist->count();
        for (short iMap = 0; iMap < iMapCount; iMap++) {
            std::string szThumbnail("maps/cache/");
            szThumbnail += GetNameFromFileName(itr->second.filename);

#ifdef PNG_SAVE_FORMAT
            szThumbnail += ".png";
#else
            szThumbnail += ".bmp";
#endif

            g_map->loadMap((*itr).second.filename, read_type_preview);
            g_map->saveThumbnail(convertPath(szThumbnail), false);

            itr++;
        }

        rm->backgroundmusic[2].togglePause();
    }
}

bool MenuState::ReadTourFile()
{
    ResetTourStops();

    FILE * fp = fopen(tourlist->at(game_values.tourindex).c_str(), "r");
    const char* const ignorable_leads = " #\n\r\t";

    char buffer[256];
    bool fReadVersion = false;
    Version version;
    while (fgets(buffer, 256, fp) && game_values.tourstoptotal < 10) {
        if (strchr(ignorable_leads, buffer[0]))
            continue;

        if (!fReadVersion) {
            fReadVersion = true;

            char * psz = strtok(buffer, ".\n");
            if (psz)
                version.major = atoi(psz);

            psz = strtok(NULL, ".\n");
            if (psz)
                version.minor = atoi(psz);

            psz = strtok(NULL, ".\n");
            if (psz)
                version.patch = atoi(psz);

            psz = strtok(NULL, ".\n");
            if (psz)
                version.build = atoi(psz);

            continue;
        }

        TourStop* ts = new TourStop();
        *ts = ParseTourStopLine(buffer, version, false);

        game_values.tourstops.push_back(ts);
        game_values.tourstoptotal++;
    }

    if (game_values.tourstoptotal != 0) {
        mTourStopMenu->miTourStop->Refresh(game_values.tourstopcurrent);

        //For old tours, turn on the bonus wheel at the end
        if (version.major == 1 && version.minor == 7 && version.patch == 0 && version.build <= 1)
            game_values.tourstops[game_values.tourstoptotal - 1]->iBonusType = 1;
    }

    fclose(fp);

    return game_values.tourstoptotal != 0;
}

void MenuState::StartGame()
{
    printf("> StartGame\n");
#ifdef _DEBUG
    iScriptState = 2;
    fScriptRunPreGameOptions = false;
#endif

    game_values.appstate = AppState::StartGame;
    //backgroundonly = false;
    //fastmap = false;

    game_values.WriteConfig();

    //Load skins for players
    printf("Loading player skins...\n");
    for (int k = 0; k < 4; k++) {
        if (game_values.playercontrol[k] > 0) {
            if (netplay.active) {
                if (k == netplay.remotePlayerNumber) { // local player uses local skin
                    printf("  player %d -> local\n", k);
                    rm->LoadFullSkin(rm->spr_player[k], game_values.skinids[k], game_values.colorids[k]);
                }
                else {
                    std::ostringstream path;
                    path << GetHomeDirectory() << "net_skin" << k << ".bmp";
                    printf("  player %d -> %s\n", k, path.str().c_str());

                    if (!rm->LoadFullSkin(rm->spr_player[k], path.str(), k)) {
                        printf("[warning] Could not load netplay skin of player %d, using default\n", k);
                        rm->LoadFullSkin(rm->spr_player[k], game_values.skinids[k], game_values.colorids[k]);
                    }
                }
            }
            else if (game_values.randomskin[k]) {
                do {
                    game_values.skinids[k] = RANDOM_INT( skinlist->count());
                } while (!rm->LoadFullSkin(rm->spr_player[k], game_values.skinids[k], game_values.colorids[k]));
            }
            else {
                rm->LoadFullSkin(rm->spr_player[k], game_values.skinids[k], game_values.colorids[k]);
            }
        }
    }

    //Load announcer sounds if changed
    if (game_values.loadedannouncer != announcerlist->currentIndex()) {
        game_values.loadedannouncer = announcerlist->currentIndex();

        //Delete the old sounds
        for (int k = 0; k < PANNOUNCER_SOUND_LAST; k++)
            rm->sfx_announcer[k].reset();

        FILE * announcerfile = fopen(announcerlist->currentPath().c_str(), "r");

        char szBuffer[256];

        int announcerIndex = 0;

        while (fgets(szBuffer, 256, announcerfile) && announcerIndex < PANNOUNCER_SOUND_LAST) {
            //Ignore comment lines
            if (szBuffer[0] == '#' || szBuffer[0] == ' ' || szBuffer[0] == '\t' || szBuffer[0] == '\n' || szBuffer[0] == '\r')
                continue;

            //Clean off carriage returns
            for (int k = 0; k < (int)strlen(szBuffer); k++) {
                if (szBuffer[k] == '\r' || szBuffer[k] == '\n') {
                    szBuffer[k] = '\0';
                    break;
                }
            }

            //If it is not "[none]", then add this announcer sound
            if (strcmp(szBuffer, "[none]"))
                rm->sfx_announcer[announcerIndex].init(convertPath(szBuffer));

            announcerIndex++;
        }

        fclose(announcerfile);
    }

    //Load soundtrack music if changed
    if (game_values.loadedmusic != musiclist->currentIndex()) {
        game_values.loadedmusic = (short)musiclist->currentIndex();
        rm->backgroundmusic[1].load(musiclist->music(0)); //Stage Clear
        rm->backgroundmusic[3].load(musiclist->music(2)); //Tournament Menu
        rm->backgroundmusic[4].load(musiclist->music(3)); //Tournament Over
    }

    rm->backgroundmusic[2].stop();
    ifsoundonandreadyplay(rm->sfx_announcer[11]);

    game_values.screenfade = 8;
    game_values.screenfadespeed = 8;
    printf("< StartGame\n");
}

void MenuState::SetControllingTeamForSettingsMenu(short iControlTeam, bool fDisplayMessage)
{
    mGameSettingsMenu->SetControllingTeam(iControlTeam);

    mModeOptionsMenu->SetControllingTeam(iControlTeam);

    mGameSettingsMenu->miMapFilterScroll->SetControllingTeam(iControlTeam);
    mMapFilterEditMenu->miMapBrowser->SetControllingTeam(iControlTeam);

    if (fDisplayMessage)
        DisplayControllingTeamMessage(iControlTeam);

    //Scan controlling team members and if they are only bots, then let the cpu control the selection
    iTournamentAITimer = 0;
    iTournamentAIStep = 0;
    mGameSettingsMenu->SetAllowExit(false);

    if (iControlTeam >= 0) {
        bool fNeedAI = true;
        for (short iPlayer = 0; iPlayer < game_values.teamcounts[iControlTeam]; iPlayer++) {
            if (game_values.playercontrol[game_values.teamids[iControlTeam][iPlayer]] == 1) {
                fNeedAI = false;
                break;
            }
        }

        if (fNeedAI) {
            iTournamentAITimer = 60;
            mGameSettingsMenu->SetAllowExit(true);
        }
    }
}

void MenuState::DisplayControllingTeamMessage(short iControlTeam)
{
    //Display the team that is in control of selecting the next game
    char szMessage[128];
    if (iControlTeam < 0)
        sprintf(szMessage, "All Teams Are In Control");
    else if (game_values.teamcounts[iControlTeam] <= 1)
        sprintf(szMessage, "Player %d Is In Control", game_values.teamids[iControlTeam][0] + 1);
    else
        sprintf(szMessage, "Team %d Is In Control", iControlTeam + 1);

    mCurrentMenu->AddEyeCandy(new EC_Announcement(&rm->menu_font_large, &rm->spr_announcementicons, szMessage, iControlTeam < 0 ? 4 : game_values.colorids[game_values.teamids[iControlTeam][0]], 120, 100));
}

void MenuState::Exit()
{
#ifdef __EMSCRIPTEN__
    return;
#endif
    game_values.appstate = AppState::Quit;
    game_values.WriteConfig();
}

void MenuState::ResetTournamentBackToMainMenu()
{
    mCurrentMenu = mMainMenu.get();
    mCurrentMenu->ResetMenu();

    if (game_values.matchtype != MatchType::SingleGame && game_values.matchtype != MatchType::QuickGame && game_values.matchtype != MatchType::MiniGame && game_values.matchtype != MatchType::NetGame) {
        if (fNeedMenuMusicReset) {
            rm->backgroundmusic[3].stop();
            rm->backgroundmusic[2].play(false, false);
            fNeedMenuMusicReset = false;
        }
    }
}

//Debugging code for writing and playing test scripts
#ifdef _DEBUG

void MenuState::LoadScript(const char * szScriptFile)
{
    operations.clear();

    FILE * fp = fopen(szScriptFile, "r");

    if (fp != NULL) {
        char buffer[256];
        while (fgets(buffer, 256, fp) != NULL) {
            if (buffer[0] == '#' || buffer[0] == '\n' || buffer[0] == ' ' || buffer[0] == '\t')
                continue;

            ScriptOperation * op = new ScriptOperation();

            //Get controller to use
            op->iController = atoi(strtok(buffer, ",\n"));

            //Get iterations on this operation
            op->iIterations = atoi(strtok(NULL, ",\n"));

            //Get input operations to perform
            for (short i = 0; i < 8; i++) {
                op->fInput[i] = atoi(strtok(NULL, ",\n")) != 0;
            }

            operations.push_back(op);
        }

        fclose(fp);
    }

    current = operations.begin();
    iScriptState = 0;
}

void MenuState::ResetScript()
{
    std::vector<ScriptOperation*>::iterator itr = operations.begin();

    while (itr != operations.end()) {
        (*itr)->iTimesExecuted = 0;
        itr++;
    }

    current = operations.begin();
    iScriptState = 0;
}

void MenuState::GetNextScriptOperation()
{
    if (AppState::Menu != game_values.appstate)
        return;

    //static short slowdowntimer = 0;
    //if (++slowdowntimer < 30)
    //return;
    //slowdowntimer = 0;

    if (iScriptState == 1) {
        if (mCurrentMenu == mGameSettingsMenu.get()) {
            if (!fScriptRunPreGameOptions) {
                fScriptRunPreGameOptions = true;
                LoadScript("Scripts/GameModeOptionsOnly.txt");
                iScriptState = 0;
                return;
            } else if (game_values.gamemode->gamemode == game_mode_frag ||
                      game_values.gamemode->gamemode == game_mode_timelimit ||
                      game_values.gamemode->gamemode == game_mode_coins ||
                      game_values.gamemode->gamemode == game_mode_classic ||
                      game_values.gamemode->gamemode == game_mode_eggs ||
                      game_values.gamemode->gamemode == game_mode_owned) {
                LoadScript("Scripts/NoModeOptions.txt");
            } else if (game_values.gamemode->gamemode == game_mode_jail) {
                LoadScript("Scripts/JailModeOptions.txt");
            } else if (game_values.gamemode->gamemode == game_mode_stomp) {
                LoadScript("Scripts/StompModeOptions.txt");
            } else if (game_values.gamemode->gamemode == game_mode_ctf) {
                LoadScript("Scripts/CaptureTheFlagModeOptions.txt");
            } else if (game_values.gamemode->gamemode == game_mode_chicken) {
                LoadScript("Scripts/ChickenModeOptions.txt");
            } else if (game_values.gamemode->gamemode == game_mode_tag) {
                LoadScript("Scripts/TagModeOptions.txt");
            } else if (game_values.gamemode->gamemode == game_mode_star) {
                LoadScript("Scripts/StarModeOptions.txt");
            } else if (game_values.gamemode->gamemode == game_mode_domination) {
                LoadScript("Scripts/DominationModeOptions.txt");
            } else if (game_values.gamemode->gamemode == game_mode_koth) {
                LoadScript("Scripts/KingOfTheHillModeOptions.txt");
            } else if (game_values.gamemode->gamemode == game_mode_race) {
                LoadScript("Scripts/RaceModeOptions.txt");
            } else if (game_values.gamemode->gamemode == game_mode_frenzy) {
                LoadScript("Scripts/FrenzyModeOptions.txt");
            } else if (game_values.gamemode->gamemode == game_mode_survival) {
                LoadScript("Scripts/SurvivalModeOptions.txt");
            }

            game_values.gamemode->setdebuggoal();
        } else if (mCurrentMenu == mTourStopMenu.get()) {
            static int tournext = RANDOM_INT(200);

            if (--tournext < 0) {
                game_values.playerInput.outputControls[0].menu_select.fPressed = true;
                tournext = RANDOM_INT(200);
                game_values.gamemode->setdebuggoal();
            }
            return;
        }

        return;
    } else if (iScriptState == 2) {
        if (mCurrentMenu == mTournamentScoreboardMenu.get()) {
            static int scoreboardnext = RANDOM_INT(200);

            if (--scoreboardnext < 0) {
                game_values.playerInput.outputControls[0].menu_select.fPressed = true;
                iScriptState = 3;
                scoreboardnext = RANDOM_INT(200);
            }

            return;
        } else if (mCurrentMenu == mGameSettingsMenu.get()) {
            iScriptState = 0;
            mCurrentMenu = mMainMenu.get();
            mCurrentMenu->ResetMenu();
            LoadScript("Scripts/StartMenuAutomation.txt");
            fScriptRunPreGameOptions = true;
            return;
        } else if (mCurrentMenu == mBonusWheelMenu.get()) {
            static int bonuswheelnext = RANDOM_INT(200) + 300;

            if (--bonuswheelnext < 0) {
                game_values.playerInput.outputControls[0].menu_select.fPressed = true;
                //iScriptState = 3;
                bonuswheelnext = RANDOM_INT(200) + 300;
            }
            return;
        } else if (mCurrentMenu == mTourStopMenu.get()) {
            static int tournext = RANDOM_INT(200);

            if (--tournext < 0) {
                game_values.playerInput.outputControls[0].menu_select.fPressed = true;
                tournext = RANDOM_INT(200);
                game_values.gamemode->setdebuggoal();
            }
            return;
        }

        return;
    } else if (iScriptState == 3) {
        if (mCurrentMenu == mBonusWheelMenu.get()) {
            game_values.playerInput.outputControls[0].menu_select.fPressed = true;
            return;
        } else if (mCurrentMenu == mGameSettingsMenu.get()) {
            iScriptState = 0;
            LoadScript("Scripts/GameModeOptionsOnly.txt");
            fScriptRunPreGameOptions = true;
            return;
        } else if (mCurrentMenu == mMainMenu.get()) {
            iScriptState = 0;
            LoadScript("Scripts/StartMenuAutomation.txt");
            fScriptRunPreGameOptions = true;
            return;
        } else if (mCurrentMenu == mTourStopMenu.get()) {
            static int tournext = RANDOM_INT(200);

            if (--tournext < 0) {
                game_values.playerInput.outputControls[0].menu_select.fPressed = true;
                tournext = RANDOM_INT(200);
                game_values.gamemode->setdebuggoal();
            }
            return;
        }

        return;
    }

    // here bad reference when pressing Ins!
    ScriptOperation * op = *current;

    while (op->iTimesExecuted >= op->iIterations) {
        if (++current == operations.end()) {
            iScriptState = 1;
            return;
        }

        op = *current;
    }

    op->iTimesExecuted++;

    short iController = op->iController;

    if (iController == 5)
        iController = RANDOM_INT(4);

    short iKeys[8];
    short iNumKeys = 0;

    for (short i = 0; i < 8; i++) {
        if (op->fInput[i])
            iKeys[iNumKeys++] = i;
    }

    if (iController == MAX_PLAYERS) { //All controllers
        game_values.playerInput.outputControls[0].keys[iKeys[RANDOM_INT(iNumKeys)]].fPressed = true;
        game_values.playerInput.outputControls[1].keys[iKeys[RANDOM_INT(iNumKeys)]].fPressed = true;
        game_values.playerInput.outputControls[2].keys[iKeys[RANDOM_INT(iNumKeys)]].fPressed = true;
        game_values.playerInput.outputControls[3].keys[iKeys[RANDOM_INT(iNumKeys)]].fPressed = true;
    } else {
        game_values.playerInput.outputControls[iController].keys[iKeys[RANDOM_INT(iNumKeys)]].fPressed = true;
    }
}

void MenuState::StartRecordScript()
{
    operations.clear();
}

void MenuState::SaveScript(const char * szScriptFile)
{
    FILE * fp = fopen(szScriptFile, "w");

    if (fp) {
        std::vector<ScriptOperation*>::iterator itr = operations.begin();

        while (itr != operations.end()) {
            ScriptOperation * op = *itr;

            //Add line breaks
            if (op->iController == -1) {
                fprintf(fp, "\n");
                itr++;
                continue;
            }

            fprintf(fp, "%d,%d", op->iController, op->iIterations);

            for (short i = 0; i < 8; i++) {
                fprintf(fp, ",%d", op->fInput[i]);
            }

            fprintf(fp, "\n");

            itr++;
        }

        fclose(fp);

#if defined(__APPLE__)
        chmod(szScriptFile, S_IRWXU | S_IRWXG | S_IROTH);
#endif
    }
}

void MenuState::AddEmtpyLineToScript()
{
    ScriptOperation * op = new ScriptOperation();
    op->iController = -1;
    operations.push_back(op);
}

void MenuState::SetNextScriptOperation()
{
    for (short i = 0; i < 8; i++) {
        if (game_values.playerInput.outputControls[0].keys[i].fPressed) {
            ScriptOperation * op = new ScriptOperation();

            op->iController = 0;
            op->iIterations = 1;

            for (short iKey = 0; iKey < 8; iKey++) {
                op->fInput[iKey] = false;
            }

            op->fInput[i] = true;

            operations.push_back(op);
        }
    }
}


/*
void EnterBossMode(short type)
{
    if (game_values.gamestate == GS_GAME && game_values.gamemode->gamemode != game_mode_boss_minigame)
    {
        bossgamemode->SetBossType(type);

        game_values.screenfade = 2;
        game_values.screenfadespeed = 2;

        rm->backgroundmusic[0].stop();
        ifsoundonstop(rm->sfx_invinciblemusic);
        ifsoundonstop(rm->sfx_timewarning);
        ifsoundonstop(rm->sfx_slowdownmusic);

        game_values.gamestate = GS_START_GAME;
    }
}*/

#endif
