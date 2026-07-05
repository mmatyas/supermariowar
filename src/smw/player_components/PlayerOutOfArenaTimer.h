#pragma once

class CPlayer;

class PlayerOutOfArenaTimer
{
public:
    void reset();
    void update(CPlayer&);
    void draw(CPlayer&);

private:
    short timer = 0;
    short displaytimer = 0;
};
