#ifndef LINFUNC_CPP
#define LINFUNC_CPP

#ifdef	_WIN32

#define strCiCompare	_stricmp
#define	inPlaceLowerCase	_strlwr

#else

char *inPlaceLowerCase(char *str);

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

#endif // LINFUNC_CPP
