#include "doctest.h"

#include "AndroidControllerMapping.h"

TEST_CASE("Android pad maps menu, gameplay, and concurrent direction sources") {
    AndroidControllerSnapshot source;
    source.buttons[SDL_CONTROLLER_BUTTON_DPAD_LEFT] = true;
    source.axes[SDL_CONTROLLER_AXIS_LEFTX] = -26000;
    source.buttons[SDL_CONTROLLER_BUTTON_A] = true;
    source.buttons[SDL_CONTROLLER_BUTTON_B] = true;
    source.buttons[SDL_CONTROLLER_BUTTON_X] = true;
    source.buttons[SDL_CONTROLLER_BUTTON_START] = true;

    auto menu = AndroidDesiredControls(source, 1, true);
    CHECK(menu[2]); // left
    CHECK(menu[4]); // select
    CHECK(menu[5]); // cancel
    CHECK(menu[6]); // random

    auto game = AndroidDesiredControls(source, 0, true);
    CHECK(game[0]); // move left
    CHECK(game[2]); // jump
    CHECK(game[4]); // run
    CHECK(game[5]); // powerup
    CHECK(game[6]); // pause

    source.buttons[SDL_CONTROLLER_BUTTON_DPAD_LEFT] = false;
    CHECK(AndroidDesiredControls(source, 0, true)[0]); // stick still holds left
    source.axes[SDL_CONTROLLER_AXIS_LEFTX] = 0;
    CHECK_FALSE(AndroidDesiredControls(source, 0, true)[0]);
}

TEST_CASE("Android keyboard and pad sources release independently and clear on lifecycle reset") {
    AndroidControllerSnapshot source;
    source.keyboard[1][2] = true;
    source.buttons[SDL_CONTROLLER_BUTTON_DPAD_LEFT] = true;
    CHECK(AndroidDesiredControls(source, 1, true)[2]);
    source.buttons[SDL_CONTROLLER_BUTTON_DPAD_LEFT] = false;
    CHECK(AndroidDesiredControls(source, 1, true)[2]); // held keyboard still moves
    source.keyboard[1][2] = false;
    CHECK_FALSE(AndroidDesiredControls(source, 1, true)[2]);

    source.buttons[SDL_CONTROLLER_BUTTON_A] = true;
    CHECK(AndroidDesiredControls(source, 0, true)[2]);
    source.clear(); // same operation used on background, removal, and ResetKeys
    CHECK_FALSE(AndroidDesiredControls(source, 0, true)[2]);
}

TEST_CASE("Android CPU player does not receive gameplay movement") {
    AndroidControllerSnapshot source;
    source.buttons[SDL_CONTROLLER_BUTTON_DPAD_RIGHT] = true;
    CHECK_FALSE(AndroidDesiredControls(source, 0, false)[1]);
    CHECK(AndroidDesiredControls(source, 1, false)[3]); // menu still works
}
