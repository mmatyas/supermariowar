#include "GSSplashScreen.h"

#include "FileList.h"
#include "MapList.h"
#include "Game.h"
#include "GameValues.h"
#include "GSMenu.h"
#include "GSGameplay.h"
#include "eyecandy.h"
#include "ResourceManager.h"

#include <string>
#include <iostream>

#ifdef _XBOX
#include <xtl.h>
#endif

#ifdef _WIN32
#ifndef _XBOX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#endif

using std::cout;
using std::endl;
using std::string;

extern SDL_Surface* screen;
extern SDL_Surface* blitdest;

extern bool g_fLoadMessages;

extern CEyecandyContainer eyecandy[3];

extern MapList *maplist;
extern MusicList *musiclist;
extern GraphicsList *menugraphicspacklist;

extern CGameValues game_values;
extern CResourceManager* rm;
extern CGame* smw;

extern void _load_drawmsg(const std::string& f);
extern void _load_waitforkey();

//-----------------------------------------------------------------------------
// THE LOAD UP SEQUENCE + SPLASH SCREEN
//-----------------------------------------------------------------------------
//that's a bunch of ugly code, maybe i'll throw it out again

SplashScreenState::SplashScreenState()
{
    alpha = 255;
    state = 7;
    timer = 120;
    firstFrame = true;
}

SplashScreenState& SplashScreenState::instance() {
    static SplashScreenState ss;
    return ss;
}

void SplashScreenState::onLeaveState()
{
    delete menu_credits;
}

bool LoadStartGraphics()
{
    const char * graphicspack = menugraphicspacklist->current_name();

    bool loadok = true;

    loadok &= rm->menu_font_small.init(convertPath("gfx/packs/menu/menu_font_small.png", graphicspack));
    loadok &= rm->menu_font_large.init(convertPath("gfx/packs/menu/menu_font_large.png", graphicspack));

    if (!loadok) {
        _load_drawmsg("ERROR: error loading the fonts!\n");
        _load_waitforkey();
        return false;
    }

    //load basic stuff
    loadok &= gfx_loadimagenocolorkey(&rm->menu_backdrop, convertPath("gfx/packs/menu/menu_background.png", graphicspack));
    loadok &= gfx_loadimage(&rm->menu_smw, convertPath("gfx/packs/menu/menu_smw.png", graphicspack), false);
    loadok &= gfx_loadimage(&rm->menu_version, convertPath("gfx/packs/menu/menu_version.png", graphicspack), false);

    if (!loadok) {
        _load_drawmsg("ERROR: error loading the start graphics!\n");
        _load_waitforkey();
        return false;
    }

    return true;
}

bool SplashScreenState::init()
{
    if (!LoadStartGraphics())
        return false;

//  gfx_loadimagenocolorkey(&rm->menu_dpi_logo, convertPath("gfx/packs/menu/splash_72dpi.png", menugraphicspacklist->current_name()));
//  gfx_loadimagenocolorkey(&rm->menu_contest_winners, convertPath("gfx/packs/menu/splash_contest_winners.png", menugraphicspacklist->current_name()));

    menu_credits = new gfxSprite();
    gfx_loadimage(menu_credits, convertPath("gfx/packs/menu/splash_credits.png", menugraphicspacklist->current_name()), false);

//	const char * contributors[] = {
//	"no_shorty", "redfalcon", "no_human", "dschingis", "funvill",
//	"matsche", "aeroflare", "Tymoe", "David Olofson", "scoti",
//	"affeOHNEwaffe", "mademan", "mario piuk", "yvoo", "DNightmare",
//	"Armen", "vvill", "zio tiok", "Donny Viszneki","alex-rus07", "JacobTheHero",
//	"Stephan Peijnik", "ventuz", "Stefan Schury", "riahc3",
//	"jinkies.uk", "devguy", "Alexis Morrissette", "Bidbood", "Quinn Storm",
//	"Flexserve", "Xijar", "j rok", "nitsuja", "4matsy", "leftyfb",
//	"Brian Porter", "Brandon Stansbury", "Brian Smith", "Caesar Cypher",
//	"Chris Schager", "Alex Arnot", "Jason Lumbert", "Kevin King",
//	"Fernando Marquez", "DrJones", "SleepyP", "UltimateNinja9",
//	"Alex Brown", "GKi", "mpinger", "neonater", "NMcCoy", "Rogultgot", "Arima",
//	"Toadeeboy", "Crapcom", "Tonberry2k", "Jum2004", "Bacon", "Tetra Vega",
//	"Young Link", "Blordow", "Gozinzolo", "Ragey", "SaturnEchidna", "Link901",
//	"Techokami", "dolorous", "SMW Fan", "Timonator", "YoshiMonarch7", "Tiptup300",
//	"Dude", "Justinio", "Matthew Callis", "VenomousNinja",
//	"Discrosh", "Ikill4you", "Helix Snake", "tubesteak", "wasabimario", "Lee",
//	"Pikawil", "Marioman64", "Peardian", "Bob Ippolito", "Viper Snake", "neavea",
//	"Mr.Bob-omb", "milua", "bobmanperson", "DrTek", "somestrangeflea", "nes6502",
//	"XPort", "Naphistim", "Chaos", "NiGHTS", "Kutter", "Maximilian",
//	"Felix the Ghost", "Water Kirby"};
//
//#define NUM_CONTRIBUTORS ((int)(sizeof(contributors)/sizeof(char*)))
//
//	bool contributorUsed[NUM_CONTRIBUTORS];
//	int contributorOrder[NUM_CONTRIBUTORS];
//
//	for (int k = 0; k < NUM_CONTRIBUTORS; k++)
//	{
//		contributorUsed[k] = false;
//	}
//
//	for (int k = 0; k < NUM_CONTRIBUTORS; k++)
//	{
//		int index = RANDOM_INT(NUM_CONTRIBUTORS);
//
//		while (contributorUsed[index])
//		{
//			if (++index >= NUM_CONTRIBUTORS)
//				index = 0;
//		}
//
//		contributorUsed[index] = true;
//		contributorOrder[k] = index;
//	}
//

    return true;
}

void SplashScreenState::update()
{
    game_values.playerInput.ClearPressedKeys(1);

    // TODO: move this out of this method maybe

    while (SDL_PollEvent(&loop_event)) {
        switch (loop_event.type) {

#ifndef _XBOX
        case SDL_QUIT: {
            game_values.gamestate = GS_QUIT;
            return;
        }
        break;
#endif
        case SDL_KEYDOWN: {
            switch (loop_event.key.keysym.sym) {
            case SDLK_RETURN:
                if (loop_event.key.keysym.mod & (KMOD_LALT | KMOD_RALT)) {
#ifndef _XBOX
                    game_values.fullscreen = !game_values.fullscreen;
                    gfx_changefullscreen(game_values.fullscreen);
                    blitdest = screen;
#endif
                }
                break;

#ifndef _XBOX
            case SDLK_F4:
                if (loop_event.key.keysym.mod & (KMOD_LALT | KMOD_RALT))
                    game_values.gamestate = GS_QUIT;
                    return;
                break;
#endif

            default:
                break;
            }
            break;
        }

        default:
            break;
        }

        game_values.playerInput.Update(loop_event, 1);
    }

    for (int iPlayer = 0; iPlayer < 4; iPlayer++) {
        if (game_values.playerInput.outputControls[iPlayer].menu_select.fPressed ||
            game_values.playerInput.outputControls[iPlayer].menu_cancel.fPressed ||
            game_values.playerInput.outputControls[iPlayer].menu_random.fPressed) {
            //if (state <= 6)
            //{
            //  state = 6;
            //  alpha = 255;
            //}
            //else
            {
                blitdest = rm->menu_backdrop.getSurface();
                rm->menu_shade.setalpha(smw->MenuTransparency);
                rm->menu_shade.draw(0, 0);
                blitdest = screen;

                g_fLoadMessages = false;
                eyecandy[2].clean();

                game_values.playerInput.ResetKeys();
                game_values.gamestate = GS_MENU;

                MenuState::instance().init();
                GameplayState::instance().init();
                GameStateManager::instance().changeStateTo(&MenuState::instance());
                return;
            }
        }
    }

    //if (state == 0 || state == 3)
    //{
    //  alpha += 4;
    //  if (alpha >= 255)
    //  {
    //      alpha = 255;
    //      state++;
    //  }
    //}
    //else if (state == 1 || state == 4)
    //{
    //  if (--timer <= 0)
    //  {
    //      timer = 120;
    //      state++;
    //  }
    //}
    //else if (state == 2 || state == 5)
    //{
    //  alpha -= 4;
    //  if (alpha <= 0)
    //  {
    //      alpha = 0;
    //      state++;
    //  }
    //}
    //else if (state == 6)
    //{
    //  alpha += 5;
    //  if (alpha >= 255)
    //  {
    //      alpha = 255;
    //      state++;
    //  }
    //}

    SDL_FillRect(screen, NULL, 0x0);

    //if (state == 0 || state == 1 || state == 2)
    //{
    //  menu_dpi_logo.setalpha((Uint8)alpha);
    //  menu_dpi_logo.draw(195, 186);
    //}
    //else if (state == 3 || state == 4 || state == 5)
    //{
    //  menu_contest_winners.setalpha((Uint8)alpha);
    //  menu_contest_winners.draw(0, 0);
    //}
    //else
    if (state == 6 || state == 7 || state == 8) {
        rm->menu_backdrop.setalpha((Uint8)alpha);
        rm->menu_backdrop.draw(0, 0);

        rm->menu_smw.setalpha((Uint8)alpha);
        rm->menu_smw.draw(smw->ScreenWidth/2 - ((short)rm->menu_smw.getWidth() >> 1), 30);  //smw logo

        rm->menu_version.setalpha((Uint8)alpha);
        rm->menu_version.draw(628 - rm->menu_version.getWidth(), 10); //smw logo

        rm->menu_font_large.setalpha((Uint8)alpha);
        rm->menu_font_large.drawRightJustified(smw->ScreenWidth * 0.98f, 45, "WIP");

        menu_credits->setalpha((Uint8)alpha);
        menu_credits->draw(227, 200);
    }

    if (state == 7) {
//            _load_drawmsg("Loading...");
        rm->menu_font_large.drawCentered(smw->ScreenWidth/2, smw->ScreenHeight * 0.875f, "Loading...");
    } else if (state == 8) {
//            _load_drawmsg("Press Any Key To Continue");
        rm->menu_font_large.drawCentered(smw->ScreenWidth/2, smw->ScreenHeight * 0.875f, "Press Any Key To Continue");

        eyecandy[2].cleandeadobjects();
        eyecandy[2].update();
        eyecandy[2].draw();

        /*          static int timer = 60;
                    static int index = 0;
                    if (++timer >= 60)
                    {
                        eyecandy[2].add(new EC_GravText(&rm->menu_font_large, smw->ScreenWidth/2, smw->ScreenHeight, contributors[contributorOrder[index]], -8.2f));
                        timer = 0;

                        if (++index >= NUM_CONTRIBUTORS)
                            index = 0;
                    } */
    }

    // Only start loading after we displayed something, in the 2nd frame
    if (firstFrame) {
        firstFrame = false;
        return;
    }

    if (state == 7) {
        // load initial coin sound
        rm->backgroundmusic[2].load(musiclist->GetMusic(1));

        rm->LoadAllGraphics();
        rm->LoadGameSounds();

        if (!game_values.soundcapable) {
            game_values.sound = false;
            game_values.music = false;
            game_values.soundvolume = 0;
            game_values.musicvolume = 0;
        }

        //Read the map filter lists
        maplist->ReadFilters();
        maplist->ApplyFilters(game_values.pfFilters);

        ifSoundOnPlay(rm->sfx_coin);


        state++;
    }
}
