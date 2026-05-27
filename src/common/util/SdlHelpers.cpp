#include "SdlHelpers.h"


void SdlDeleter::operator()(SDL_Window* ptr) const noexcept { SDL_DestroyWindow(ptr); }
void SdlDeleter::operator()(SDL_Renderer* ptr) const noexcept { SDL_DestroyRenderer(ptr); }
void SdlDeleter::operator()(SDL_Texture* ptr) const noexcept { SDL_DestroyTexture(ptr); }
void SdlDeleter::operator()(SDL_Surface* ptr) const noexcept { SDL_DestroySurface(ptr); }
