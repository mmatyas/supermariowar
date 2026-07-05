#pragma once

class CPlayer;

class PlayerSuicideTimer {
public:
    void reset();
    void update(CPlayer& player);
    void draw(CPlayer& player);

private:
    short timer = 0;
    short counttimer = 0;
    short displaytimer = 2;
};
