#pragma once

class IO_MovingObject;


enum class PowerupType : unsigned char {
    PoisonMushroom,
    ExtraLife1,
    ExtraLife2,
    ExtraLife3,
    ExtraLife5,
    Fire,
    Star,
    Clock,
    Bobomb,
    Pow,
    BulletBill,
    Hammer,
    ShellGreen,
    ShellRed,
    ShellSpiny,
    ShellBuzzy,
    Mod,
    Feather,
    MysteryMushroom,
    Boomerang,
    Tanooki,
    IceWand,
    Podobo,
    Bomb,
    Leaf,
    PWings,
    JailKey,
};


IO_MovingObject* createpowerup(short iType, short ix, short iy, bool side, bool spawn);
void CheckSecret(short id);
