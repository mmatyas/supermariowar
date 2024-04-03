#pragma once


enum class TeamCollisionStyle : unsigned char {
    Off,
    Assist,
    On,
};


enum class TournamentControlStyle : unsigned char {
    All,
    GameWinner,
    GameLoser,
    LeadingTeams,
    TrailingTeams,
    Random,
    RandomLoser,
    RoundRobin,
};


enum class DeathStyle : unsigned char {
    Respawn,
    Shield,
};


enum class JailStyle : unsigned char {
    Classic,
    Owned,
    FreeForAll,
};


enum class StarStyle : unsigned char {
    Ztar,
    Shine,
    Multi,
    Random,
};
