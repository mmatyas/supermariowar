#include "Game.h"

#include "path.h"

#include <SDL3/SDL_init.h>
#include <array>
#include <cstdio>
#include <format>

#if _WIN32
#include <windows.h>
#endif

#if defined(unix) || defined(__unix__) || defined(__unix) || defined(__APPLE__)
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif


namespace {
struct SdlProperty {
    const char* key;
    const char* val;
};
} // namespace


void ensureSettingsDir()
{
    const std::string smwHome = GetHomeDirectory();

#if	_WIN32
    const bool success = CreateDirectory(smwHome.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError();
    if (!success)
        perror("[error] Could not create settings directory");

#else
    struct stat st;
    if (stat(smwHome.c_str(), &st) != 0) { // inode does not exist
        if (mkdir(smwHome.c_str(), 0775) != 0)
            perror("[error] Could not create settings directory");
    }
    else if (!S_ISDIR(st.st_mode)) {  // inode exist, but not a directory
        perror("[error] Could not access settings directory");
    }

#endif
}


void App::registerSdlMetadata(std::string_view app_title)
{
    const std::string version = std::format("{}, {}", GIT_REVISION, GIT_DATE);
    const std::array<SdlProperty, 6> metadata {
        SdlProperty { SDL_PROP_APP_METADATA_NAME_STRING, app_title.data() },
        SdlProperty { SDL_PROP_APP_METADATA_VERSION_STRING, version.c_str() },
        SdlProperty { SDL_PROP_APP_METADATA_IDENTIFIER_STRING, "net.smwstuff.SuperMarioWar" },
        SdlProperty { SDL_PROP_APP_METADATA_CREATOR_STRING, "Super Mario War Developers" },
        SdlProperty { SDL_PROP_APP_METADATA_URL_STRING, "https://github.com/mmatyas/supermariowar" },
        SdlProperty { SDL_PROP_APP_METADATA_TYPE_STRING, "game" },
    };
    for (const SdlProperty& prop : metadata) {
        if (!SDL_SetAppMetadataProperty(prop.key, prop.val)) {
            printf("[init] Failed to set %s\n", prop.key);
        }
    }
}
