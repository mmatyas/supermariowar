#pragma once

class CPlayer;

class PlayerCape {
public:
    void reset();
    void restart_animation();
    void spin(CPlayer& player);
    void draw(CPlayer& player);

private:
    unsigned char iCapeTimer = 0;
    unsigned short iCapeFrameX = 0;
    unsigned short iCapeFrameY = 0;
    bool fCapeUp = false;
    short iCapeYOffset = 0;
};
