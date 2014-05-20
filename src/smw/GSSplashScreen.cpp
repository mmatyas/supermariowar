#ifdef _XBOX
#include <xtl.h>
#endif

#ifdef _WIN32
#ifndef _XBOX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#endif

#include "GSSplashScreen.h"

#include "global.h"
#include "time.h"
#include "menu.h"
#include "GSGameplay.h"

#include <string>
#include <iostream>
using std::cout;
using std::endl;
using std::string;

extern bool g_fLoadMessages;

extern bool gfx_createmenuskin(gfxSprite ** gSprites, const std::string& filename, Uint8 r, Uint8 g, Uint8 b, short colorScheme, bool fLoadBothDirections);
extern bool gfx_createfullskin(gfxSprite ** gSprites, const std::string& filename, Uint8 r, Uint8 g, Uint8 b, short colorScheme);

extern CEyecandyContainer eyecandy[3];

//-----------------------------------------------------------------------------
// THE LOAD UP SEQUENCE + SPLASH SCREEN
//-----------------------------------------------------------------------------
//that's a bunch of ugly code, maybe i'll throw it out again

SplashScreenState::SplashScreenState()
{
    alpha = 255;
    state = 7;
    timer = 120;
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

    if(!loadok) {
        _load_drawmsg("ERROR: error loading the fonts!\n");
        _load_waitforkey();
        return false;
    }

    //load basic stuff
    loadok &= gfx_loadimagenocolorkey(&rm->menu_backdrop, convertPath("gfx/packs/menu/menu_background.png", graphicspack));
    loadok &= gfx_loadimage(&rm->menu_smw, convertPath("gfx/packs/menu/menu_smw.png", graphicspack), false);
    loadok &= gfx_loadimage(&rm->menu_version, convertPath("gfx/packs/menu/menu_version.png", graphicspack), false);

    if(!loadok) {
        _load_drawmsg("ERROR: error loading the start graphics!\n");
        _load_waitforkey();
        return false;
    }

    return true;
}

bool LoadGameSounds()
{
    game_values.soundcapable = false;

#if !defined(_XBOX) && !defined(__EMSCRIPTEN__)  //xbox and emscripten has sound capabilities
    int frequency, channels;
    Uint16 format;

    if(0 == Mix_QuerySpec(&frequency, &format, &channels))
        return false;
#endif

    const char * soundpack = soundpacklist->current_name();

    sfx_mip.init(convertPath("sfx/packs/mip.wav", soundpack));
    sfx_deathsound.init(convertPath("sfx/packs/death.wav", soundpack));
    sfx_jump.init(convertPath("sfx/packs/jump.wav", soundpack));
    sfx_skid.init(convertPath("sfx/packs/skid.wav", soundpack));
    sfx_capejump.init(convertPath("sfx/packs/capejump.wav", soundpack));
    sfx_invinciblemusic.init(convertPath("sfx/packs/invincible.wav", soundpack));
    sfx_extraguysound.init(convertPath("sfx/packs/1up.wav", soundpack));
    sfx_sprout.init(convertPath("sfx/packs/sprout.wav", soundpack));
    sfx_collectpowerup.init(convertPath("sfx/packs/collectpowerup.wav", soundpack));
    sfx_collectfeather.init(convertPath("sfx/packs/feather.wav", soundpack));
    sfx_tailspin.init(convertPath("sfx/packs/tail.wav", soundpack));
    sfx_storepowerup.init(convertPath("sfx/packs/storeitem.wav", soundpack));
    sfx_breakblock.init(convertPath("sfx/packs/breakblock.wav", soundpack));
    sfx_bump.init(convertPath("sfx/packs/bump.wav", soundpack));
    sfx_coin.init(convertPath("sfx/packs/coin.wav", soundpack));
    sfx_fireball.init(convertPath("sfx/packs/fireball.wav", soundpack));
    sfx_springjump.init(convertPath("sfx/packs/springjump.wav", soundpack));
    sfx_timewarning.init(convertPath("sfx/packs/timewarning.wav", soundpack));
    sfx_hit.init(convertPath("sfx/packs/hit.wav", soundpack));
    sfx_chicken.init(convertPath("sfx/packs/chicken.wav", soundpack));
    sfx_transform.init(convertPath("sfx/packs/transform.wav", soundpack));
    sfx_yoshi.init(convertPath("sfx/packs/yoshi.wav", soundpack));
    sfx_pause.init(convertPath("sfx/packs/pause.wav", soundpack));
    sfx_bobombsound.init(convertPath("sfx/packs/bob-omb.wav", soundpack));
    sfx_areatag.init(convertPath("sfx/packs/dcoin.wav", soundpack));
    sfx_cannon.init(convertPath("sfx/packs/cannon.wav", soundpack));
    sfx_burnup.init(convertPath("sfx/packs/burnup.wav", soundpack));
    sfx_pipe.init(convertPath("sfx/packs/warp.wav", soundpack));
    sfx_thunder.init(convertPath("sfx/packs/thunder.wav", soundpack));
    sfx_slowdownmusic.init(convertPath("sfx/packs/clock.wav", soundpack));
    sfx_flyingsound.init(convertPath("sfx/packs/slowdown.wav", soundpack));
    sfx_storedpowerupsound.init(convertPath("sfx/packs/storedpowerup.wav", soundpack));
    sfx_kicksound.init(convertPath("sfx/packs/kick.wav", soundpack));
    sfx_racesound.init(convertPath("sfx/packs/race.wav", soundpack));
    sfx_bulletbillsound.init(convertPath("sfx/packs/bulletbill.wav", soundpack));
    sfx_boomerang.init(convertPath("sfx/packs/boomerang.wav", soundpack));
    sfx_spit.init(convertPath("sfx/packs/spit.wav", soundpack));
    sfx_starwarning.init(convertPath("sfx/packs/starwarning.wav", soundpack));
    sfx_powerdown.init(convertPath("sfx/packs/powerdown.wav", soundpack));
    sfx_switchpress.init(convertPath("sfx/packs/switchpress.wav", soundpack));
    sfx_superspring.init(convertPath("sfx/packs/superspring.wav", soundpack));
    sfx_stun.init(convertPath("sfx/packs/stun.wav", soundpack));
    sfx_inventory.init(convertPath("sfx/packs/inventory.wav", soundpack));
    sfx_worldmove.init(convertPath("sfx/packs/mapmove.wav", soundpack));
    sfx_treasurechest.init(convertPath("sfx/packs/treasurechest.wav", soundpack));
    sfx_flamecannon.init(convertPath("sfx/packs/flamecannon.wav", soundpack));
    sfx_wand.init(convertPath("sfx/packs/wand.wav", soundpack));
    sfx_enterstage.init(convertPath("sfx/packs/enter-stage.wav", soundpack));
    sfx_gameover.init(convertPath("sfx/packs/gameover.wav", soundpack));
    sfx_pickup.init(convertPath("sfx/packs/pickup.wav", soundpack));

    game_values.soundcapable = true;
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
//	for(int k = 0; k < NUM_CONTRIBUTORS; k++)
//	{
//		contributorUsed[k] = false;
//	}
//
//	for(int k = 0; k < NUM_CONTRIBUTORS; k++)
//	{
//		int index = RNGMAX(NUM_CONTRIBUTORS);
//
//		while(contributorUsed[index])
//		{
//			if(++index >= NUM_CONTRIBUTORS)
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

    while(SDL_PollEvent(&loop_event)) {
        switch(loop_event.type) {

#ifndef _XBOX
        case SDL_QUIT: {
            game_values.gamestate = GS_QUIT;
            return;
        }
        break;
#endif
        case SDL_KEYDOWN: {
            switch(loop_event.key.keysym.sym) {
            case SDLK_RETURN:
                if(loop_event.key.keysym.mod & (KMOD_LALT | KMOD_RALT)) {
#ifndef _XBOX
                    game_values.fullscreen = !game_values.fullscreen;
                    gfx_setresolution(smw->ScreenWidth, smw->ScreenHeight, game_values.fullscreen);
                    blitdest = screen;
#endif
                }
                break;

#ifndef _XBOX
            case SDLK_F4:
                if(loop_event.key.keysym.mod & (KMOD_LALT | KMOD_RALT))
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

    for(int iPlayer = 0; iPlayer < 4; iPlayer++) {
        if (game_values.playerInput.outputControls[iPlayer].menu_select.fPressed ||
            game_values.playerInput.outputControls[iPlayer].menu_cancel.fPressed ||
            game_values.playerInput.outputControls[iPlayer].menu_random.fPressed) {
            //if(state <= 6)
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

                Menu::instance().init();
                GameplayState::instance().init();
                GameStateManager::instance().changeStateTo(&Menu::instance());
                return;
            }
        }
    }

    //if(state == 0 || state == 3)
    //{
    //  alpha += 4;
    //  if(alpha >= 255)
    //  {
    //      alpha = 255;
    //      state++;
    //  }
    //}
    //else if(state == 1 || state == 4)
    //{
    //  if(--timer <= 0)
    //  {
    //      timer = 120;
    //      state++;
    //  }
    //}
    //else if(state == 2 || state == 5)
    //{
    //  alpha -= 4;
    //  if(alpha <= 0)
    //  {
    //      alpha = 0;
    //      state++;
    //  }
    //}
    //else if(state == 6)
    //{
    //  alpha += 5;
    //  if(alpha >= 255)
    //  {
    //      alpha = 255;
    //      state++;
    //  }
    //}

    SDL_FillRect(screen, NULL, 0x0);

    //if(state == 0 || state == 1 || state == 2)
    //{
    //  menu_dpi_logo.setalpha((Uint8)alpha);
    //  menu_dpi_logo.draw(195, 186);
    //}
    //else if(state == 3 || state == 4 || state == 5)
    //{
    //  menu_contest_winners.setalpha((Uint8)alpha);
    //  menu_contest_winners.draw(0, 0);
    //}
    //else
    if(state == 6 || state == 7 || state == 8) {
        rm->menu_backdrop.setalpha((Uint8)alpha);
        rm->menu_backdrop.draw(0, 0);

        rm->menu_smw.setalpha((Uint8)alpha);
        rm->menu_smw.draw(smw->ScreenWidth/2 - ((short)rm->menu_smw.getWidth() >> 1), 30);  //smw logo

        rm->menu_version.setalpha((Uint8)alpha);
        rm->menu_version.draw(570, 10); //smw logo

        rm->menu_font_large.setalpha((Uint8)alpha);
        rm->menu_font_large.drawRightJustified(smw->ScreenWidth * 0.98f, 45, "WIP");

        menu_credits->setalpha((Uint8)alpha);
        menu_credits->draw(227, 200);
    }

    if(state == 7) {
//            _load_drawmsg("Loading...");
        rm->menu_font_large.drawCentered(smw->ScreenWidth/2, smw->ScreenHeight * 0.875f, "Loading...");
    } else if(state == 8) {
//            _load_drawmsg("Press Any Key To Continue");
        rm->menu_font_large.drawCentered(smw->ScreenWidth/2, smw->ScreenHeight * 0.875f, "Press Any Key To Continue");

        eyecandy[2].cleandeadobjects();
        eyecandy[2].update();
        eyecandy[2].draw();

        /*          static int timer = 60;
                    static int index = 0;
                    if(++timer >= 60)
                    {
                        eyecandy[2].add(new EC_GravText(&rm->menu_font_large, smw->ScreenWidth/2, smw->ScreenHeight, contributors[contributorOrder[index]], -8.2f));
                        timer = 0;

                        if(++index >= NUM_CONTRIBUTORS)
                            index = 0;
                    } */
    }

    if(state == 7) {
        // load initial coin sound
        backgroundmusic[2].load(musiclist->GetMusic(1));

        rm->LoadAllGraphics();

        LoadGameSounds();

        if(!game_values.soundcapable) {
            game_values.sound = false;
            game_values.music = false;
            game_values.soundvolume = 0;
            game_values.musicvolume = 0;
        }

        //Read the map filter lists
        maplist->ReadFilters();
        maplist->ApplyFilters(game_values.pfFilters);

        ifSoundOnPlay(sfx_coin);


        state++;
    }
}
