#ifndef PLAYER_WINGS
#define PLAYER_WINGS

class CPlayer;

class PlayerWings
{
public:
    void reset();
    void draw(CPlayer& player);

private:
    unsigned char iWingsTimer;
    unsigned short iWingsFrame;
};

#endif // PLAYER_WINGS
