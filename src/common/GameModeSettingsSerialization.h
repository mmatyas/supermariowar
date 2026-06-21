#pragma once

#include <vector>

struct GameModeSettings;


/// Turns the selected game mode settings into a list of integers
std::vector<short> serializeGMS(short gamemodeId, const GameModeSettings& gmsSettings);

/// Parses the active tokenizer of `strtok` (a list of integers) into the selected game mode settings
std::vector<bool> deserializeGMS(short gamemodeId, GameModeSettings& out, const GameModeSettings& current);
