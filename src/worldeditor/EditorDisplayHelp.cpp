#include "EditorDisplayHelp.h"

#include "ResourceManager.h"

#include <array>


namespace {
constexpr std::array<const char* const, 24> COLUMN1 {
    "Modes:",
    "[1] - Water Mode",
    "[2] - Land Mode",
    "[3] - Stage Objects Mode",
    "[4] - Path Mode",
    "[5] - Objects Mode",
    "[6] - Bridges Mode",
    "[p] - Connection Mode",
    "[w] - Warp Mode",
    "[v] - Vehicle",
    "      [c] - Copy Vehicle",
    "[t] - Start and Doors",
    "[b] - Vehicle Boundaries",
    "[i] - Initial Powerups",
    "[e] - Edit Stages",
    "",
    "File:",
    "[n] - New World",
    "[s] - Save World",
    "[shift] + [s] - Save As",
    "[f] - Find World",
    "[shift] + [f] - New Search",
    "[pageup] - Go To Previous World",
    "[pagedown] - Go To Next World",
};

constexpr std::array<const char* const, 13> COLUMN2 {
    "Place Tiles:",
    "[Left Mouse Button] - Place Item",
    "[Right Mouse Button] - Remove Item",
    "",
    // "Move Mode:",
    // "[Right Mouse Button] - Select Area",
    // "[Left Mouse Button] - Unselect Area",
    // "Select And Drag - Move Selections",
    // "Hold [shift] - Multiple Selections",
    // "Hold [ctrl] - Freehand Selections",
    // "[delete] - Delete Selection",
    // "[c] - Copy Selection",
    // "",
    "Miscellaneous:",
    "[r] - Change Music Category",
    "[Arrow Keys] - Navigate World",
    "[a] - Automatic Path/Land",
    "[k] - Resize World",
    "[ctrl] + [delete] - Clear All",
    "[insert] - Screenshot",
    "[alt] + [enter] - Full Screen/Window",
    "[space] - Toggle Stage Previews",
};
}  // namespace


void EditorDisplayHelp::renderSetup(CResourceManager& rm)
{
    rm.menu_font_large.drawCentered(320, 15, "Help");

    int offsety = 55;
    int offsetx = 30;
    for (const char* const line : COLUMN1) {
        rm.menu_font_small.drawC(offsetx, offsety, line);
        offsety += rm.menu_font_small.getHeight() + 2;
    }

    offsetx = 300;
    offsety = 55;
    for (const char* const line : COLUMN2) {
        rm.menu_font_small.draw(offsetx, offsety, line);
        offsety += rm.menu_font_small.getHeight() + 2;
    }
}


void EditorDisplayHelp::onSetupKeypress(const SDL_KeyboardEvent& event)
{
    if (event.state == SDL_PRESSED)
        newlyEntered = false;
}
