#pragma once

class CPlayer;

class PlayerWings {
public:
    void reset();
    void draw(CPlayer& player);

private:
    unsigned char iWingsTimer = 0;
    unsigned short iWingsFrame = 0;
};
