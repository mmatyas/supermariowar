#ifndef PLAYER_CAPE
#define PLAYER_CAPE

class CPlayer;

class PlayerCape
{
public:
    void reset();
    void restart_animation();
    void spin(CPlayer& player);
    void draw(CPlayer& player);

private:
    unsigned char iCapeTimer;
    unsigned short iCapeFrameX;
    unsigned short iCapeFrameY;
    bool fCapeUp;
    unsigned short iCapeYOffset;
};


#endif // PLAYER_CAPE
