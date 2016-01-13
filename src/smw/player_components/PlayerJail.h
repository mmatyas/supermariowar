#ifndef PLAYER_JAIL_STATUS
#define PLAYER_JAIL_STATUS

#include <stdint.h>

class CPlayer;

class PlayerJail
{
public:
    PlayerJail();
    void update(CPlayer& player);
    void draw(CPlayer& player);

    bool isActive() const;

    int8_t getColor() const;
    void setColor(int8_t color);

    void escape(CPlayer& player);
    void free_by_teammate(CPlayer& player);
    void lockInBy(CPlayer& inflictor);

private:
    int8_t owner_teamID;
    int8_t color;
    short timer;

friend class CGM_Jail;
};


#endif // PLAYER_JAIL_STATUS
