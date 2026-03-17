/*----------------------------------------------+
| gfx							Florian Hufsky	|
|												|
| start:		14.12.2003						|
| last changes:	22.12.2003						|
+----------------------------------------------*/
/*--------------------------------------------------------------+
| gfx															|
|																|
| a stripped down version of tfree/free  for a pixelate article	|
|																|
| gfx is a mini graphics library containing						|
|  + initialisation of SDL										|
|  + a class for simple Sprites									|
|  + a class for fonts											|
|																|
| have a lot of fun!											|
|		(C) 2003-2008 Florian Hufsky <florian.hufsky@gmail.com>	|
+--------------------------------------------------------------*/

#ifndef GFX_H
#define GFX_H

#include "GlobalConstants.h"
#include "gfx/Color.h"
#include "gfx/gfxSprite.h"
#include "gfx/gfxFont.h"
#include "gfx/gfxSDL.h"

#include "SDL.h"
#include <filesystem>
#include <optional>

using SpriteStrip = std::array<gfxSprite, PGFX_LAST>;


bool gfx_loadpalette(const std::filesystem::path& palette_path);

void gfx_cliprect(SDL_Rect * srcRect, SDL_Rect * dstRect, short x, short y, short w, short h);

/// Clips a source and destination area pair, so that the destination area doesn't go past
/// a certain threshold in a given direction.
/// For example, if the clip edge is `ClipEdge::Right`, then `dstRect` is cut at `x = threshold`,
/// and the width of `srcRect` shrinks accordingly.
/// Returns true if the destination area is fully hidden.
[[nodiscard]] bool gfx_adjusthiddenrects(SDL_Rect& srcRect, SDL_Rect& dstRect, ClipEdge edge, int threshold);

void gfx_drawpreview(SDL_Surface * surface,
    short dstX, short dstY,
    short srcX, short srcY, short iw, short ih,
    short clipX, short clipY, short clipW, short clipH,
    bool wrap,
    std::optional<std::pair<ClipEdge, int>> clip = std::nullopt);

SpriteStrip gfx_loadfullskin(const std::filesystem::path& path, short colorScheme);
SpriteStrip gfx_loadmenuskin(const std::filesystem::path& path, short colorScheme, bool fLoadBothDirections);

void gfx_setjoystickteamcolor(SDL_Joystick * joystick, short team, float brightness);


RGB getRgb(SDL_Surface* surf, int x, int y);

#endif // GFX_H
