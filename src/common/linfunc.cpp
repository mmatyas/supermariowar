#include "linfunc.h"

#include <algorithm>
#include <string>
#include <ctype.h>
#include <sstream>

//_DEBUG
#if 0
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

bool CopyFile(const char *src, const char *dest, bool dontOverwrite)
{
    struct stat fileinfo;
    FILE *s = NULL, *d = NULL;
    char buf[BUFSIZ];
    size_t actual_bufsiz;

    if ((dontOverwrite && stat(dest, &fileinfo) != -1) ||
            (s = fopen(src, "rb")) == NULL || (d = fopen(dest, "wb")) == NULL) {
        if (s != NULL)
             fclose(s);
        if (d != NULL)
             fclose(d);
        return false;
    }

    while ((actual_bufsiz = fread(buf, sizeof(char), BUFSIZ, s)) > 0 &&
        fwrite(buf, sizeof(char), actual_bufsiz, d) == actual_bufsiz);

    fclose(s);
    fclose(d);

    return (ferror(s) != 0 || ferror(d) != 0);
}
#endif

// apply lowercase without duplicating, used only internally
char *inPlaceLowerCase(char *str)
{
    char *p = str;

    for (; *p; p++)
        *p = tolower(*p);

    return str;
}

void inPlaceLowerCase(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

bool cstr_ci_equals(const char* const a, const char* const b)
{
    if (a == b)
        return true;

    for (const char *pa = a, *pb = b; *pa; pa++, pb++) {
        if (::tolower(*pa) != ::tolower(*pb))
            return false;
    }

    return true;
}

std::list<std::string_view> tokenize(std::string_view text, char delim, size_t maxsplit)
{
    if (text.empty()) {
        return { text };
    }

    std::list<std::string_view> tokens;
    size_t start = 0;

    while (start < text.size()) {
        if (maxsplit == 0) {
            tokens.emplace_back(text.substr(start));
            break;
        }

        const size_t end = text.find(delim, start);
        if (end == std::string_view::npos) {
            tokens.emplace_back(text.substr(start));
            break;
        }

        tokens.emplace_back(text.substr(start, end - start));
        start = end + 1;
        maxsplit--;
    }

    return tokens;
}
