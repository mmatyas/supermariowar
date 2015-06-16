#ifndef PLAYER_SUICIDE_TIMER
#define PLAYER_SUICIDE_TIMER

class CPlayer;

class PlayerSuicideTimer
{
public:
    void reset();
    void update(CPlayer& player);
    void draw(CPlayer& player);

private:
    short timer;
    short counttimer;
    short displaytimer;
};

#endif // PLAYER_SUICIDE_TIMER
