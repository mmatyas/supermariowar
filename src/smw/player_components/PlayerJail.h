#pragma once

#include <stdint.h>

class CPlayer;

class PlayerJail {
public:
    void update(CPlayer& player);
    void draw(CPlayer& player);

    bool isActive() const;

    int8_t getColor() const;
    void setColor(int8_t color);

    void escape(CPlayer& player);
    void free_by_teammate(CPlayer& player);
    void lockInBy(CPlayer& inflictor);

private:
    int8_t owner_teamID = -1;
    int8_t color = 0;
    short timer = 0;

friend class CGM_Jail;
};
