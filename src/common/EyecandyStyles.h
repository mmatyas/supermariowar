#pragma once


enum class AwardStyle: unsigned char {
    None,
    Fireworks,
    Swirl,
    Halo,
    Souls,
    Text,
};


enum class ScoreboardStyle : unsigned char {
    Top,
    Bottom,
    Corners,
};


enum class SpawnStyle : unsigned char {
    Instant,
    Door,
    Swirl,
};
