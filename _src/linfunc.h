#ifndef LINFUNC_CPP
#define LINFUNC_CPP
//The purpose of this file is to create any compatibility functions needed for linux.
//do NOT include it for non-linux builds!
#define _strdup strdup
#define _stricmp strcasecmp
#define _unlink unlink
#define Sleep SDL_Delay
#ifdef _DEBUG
bool CopyFile(const char *src, const char *dest, bool dontOverwrite);
#endif
char *_strlwr(char *str);
#endif
