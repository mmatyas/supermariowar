#pragma once

#include "GlobalConstants.h"

/// Layout of an in-game team scoreboard.
/// The player icons are on the left, followed by the score digits;
/// the stored powerup slots are below the digits.
namespace scoreboard {

/// Horizontal offset of the icon of the `slot`th player of a team
constexpr short playerOffsetX(short slot) {
    return 2 + 34 * slot;
}

/// Horizontal offset of the stored powerup of the `slot`th player of a team of `teamSize` players
constexpr short powerupOffsetX(short teamSize, short slot) {
    return 3 + 34 * teamSize + 18 * slot;
}

/// Horizontal offset of the score digits
constexpr short scoreTextOffsetX(short teamSize) {
    return 34 * teamSize + 1;
}

/// Total width of the scoreboard of a team
constexpr short boxWidth(short teamSize) {
    // Up to 3 players the powerup slots fit under the score digits,
    // for larger teams they are wider than the digits
    return teamSize <= 3 ? 56 + 34 * teamSize : 2 + 52 * teamSize;
}

static_assert(powerupOffsetX(1, 0) == 37);
static_assert(powerupOffsetX(2, 1) == 89);
static_assert(powerupOffsetX(3, 2) == 141);
static_assert(boxWidth(3) == 158);

} // namespace scoreboard
