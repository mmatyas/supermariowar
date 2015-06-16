#ifndef PLAYER_OUT_OF_ARENA_TIMER
#define PLAYER_OUT_OF_ARENA_TIMER

class CPlayer;

class PlayerOutOfArenaTimer
{
public:
    void reset();
    void update(CPlayer&);
    void draw(CPlayer&);

private:
    short timer;
    short displaytimer;
};

#endif // PLAYER_OUT_OF_ARENA_TIMER
