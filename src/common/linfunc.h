#ifndef LINFUNC_H
#define LINFUNC_H

#if defined(_WIN32) && defined(_MSC_VER) // MSVC only

#define strCiCompare	_stricmp
#define	inPlaceLowerCase	_strlwr

#else

char *inPlaceLowerCase(char *str);
char *lowercaseDup(const char *name);

//#define _strdup strdup
//#define _stricmp strcasecmp
//#define strlwr	lowercase
//#define _unlink unlink
//#define Sleep SDL_Delay

#define strCiCompare	strcasecmp

//_DEBUG
#if 0
bool CopyFile(const char *src, const char *dest, bool dontOverwrite);
#endif

#endif

#endif // LINFUNC_H
