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
