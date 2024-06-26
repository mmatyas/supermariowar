#pragma once

enum class MatchType : unsigned char {
    SingleGame,
    Tournament,
    Tour,
    MiniGame,
    World,
    QuickGame,
    NetGame,
};


enum class Minigame : unsigned char {
    PipeCoin,
    HammerBoss,
    BombBoss,
    FireBoss,
    Boxes,
};


enum class Boss : unsigned char {
    Hammer,
    Bomb,
    Fire,
};
