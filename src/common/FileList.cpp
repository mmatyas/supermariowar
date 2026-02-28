#include "FileList.h"

#include "linfunc.h"
#include "path.h"
#include "RandomNumberGenerator.h"
#include "util/ContainerHelpers.h"
#include "util/DirIterator.h"

#include <algorithm>
#include <fstream>
#include <variant>

namespace fs = std::filesystem;

#define MAXCATEGORYTRACKS 64


void UpdateMusicWithOverrides(MusicList& musiclist, WorldMusicList& worldmusiclist)
{
    std::vector<MapMusicOverride> mapmusicoverrides;
    std::vector<WorldMusicOverride> worldmusicoverrides;

    enum class Section : unsigned char {
        None,
        Maps,
        Worlds,
    };

    std::ifstream file(convertPath("music/Overrides.txt"));
    if (!file)
        return;

    Section current_section = Section::None;
    std::string line;
    while (std::getline(file, line)) {
        //Ignore comment lines
        if (line.empty() || line[0] == '#' || line[0] == '\n' || line[0] == '\r' || line[0] == ' ' || line[0] == '\t')
            continue;

        //Chop off line ending
        if (line.ends_with('\r'))
            line.pop_back();

        //If we found a category header
        if (line[0] == '[') {
            std::transform(line.begin(), line.end(), line.begin(), ::tolower);

            if (line == "[maps]")
                current_section = Section::Maps;
            else if (line == "[worlds]")
                current_section = Section::Worlds;

            continue;
        }

        //If we're not in a category, ignore this line
        if (current_section == Section::None)
            continue;

        std::list<std::string_view> tokens = tokenize(line, ',');
        if (tokens.empty())
            continue;

        if (current_section == Section::Maps) {
            MapMusicOverride override;

            override.mapname = std::move(tokens.front());
            tokens.pop_front();

            for (std::string_view token : tokens) {
                fs::path path = convertPath(std::string(token));
                if (fs::exists(path))
                    override.songs.emplace_back(std::move(path));
            }
            //Don't add overrides that have no songs
            if (!override.songs.empty())
                mapmusicoverrides.emplace_back(std::move(override));
        }
        else if (current_section == Section::Worlds) {
            WorldMusicOverride override;

            override.worldname = std::move(tokens.front());
            tokens.pop_front();

            fs::path path = convertPath(std::string(tokens.front()));
            if (fs::exists(path)) {
                override.song = std::move(path);
                worldmusicoverrides.emplace_back(std::move(override));
            }
        }
    }

    musiclist.updateEntriesWithOverrides(mapmusicoverrides);
    worldmusiclist.updateEntriesWithOverrides(worldmusicoverrides);
}

///////////// SimpleFileList ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SimpleFileList::SimpleFileList(const fs::path& dirpath, const std::string& extension, bool fAlphabetize)
{
    FilesIterator dir(dirpath, {extension});
    while (auto path = dir.next()) {
        m_filelist.emplace_back(path->string());
    }

    if (m_filelist.empty()) {
        printf("WARNING: The directory `%s` is empty\n", dirpath.c_str());
        m_index = -1;
        return;
    }

    m_index = 0;

    //Alphabetize the list, ignoring author
    if (fAlphabetize) {
        std::vector<std::string> names;
        names.reserve(m_filelist.size());

        //Get only the names of the files, no author information
        for (const fs::path& filepath : m_filelist) {
            std::string name = stripPathAndExtension(filepath.filename().string());
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            names.emplace_back(std::move(name));
        }

        //Now bubblesort them
        bool fDone = false;
        while (!fDone) {
            fDone = true;
            for (size_t i = 0; i < m_filelist.size() - 1; i++) {
                if (names[i].compare(names[i + 1]) > 0) {
                    fDone = false;
                    std::swap(names[i], names[i + 1]);
                    std::swap(m_filelist[i], m_filelist[i + 1]);
                }
            }
        }
    }
}

void SimpleFileList::setCurrentIndex(size_t index)
{
    if (index < m_filelist.size())
        m_index = index;
}

void SimpleFileList::setCurrentPath(const fs::path& name)
{
    auto it = std::find(m_filelist.cbegin(), m_filelist.cend(), name);
    if (it != m_filelist.cend())
        m_index = it - m_filelist.cbegin();
}

void SimpleFileList::next()
{
    if (m_filelist.empty())
        return;

    m_index++;
    if (m_index >= m_filelist.size())
        m_index = 0;
}

void SimpleFileList::prev()
{
    if (m_filelist.empty())
        return;

    m_index--;
    if (m_index >= m_filelist.size())
        m_index = m_filelist.size() - 1;
}

void SimpleFileList::random()
{
    if (!m_filelist.empty())
        m_index = RANDOM_INT(m_filelist.size());
}

void SimpleFileList::add(fs::path path)
{
    m_filelist.emplace_back(std::move(path));
}

bool SimpleFileList::find(const std::string& name)
{
    if (m_filelist.empty())
        return false;

    const size_t start = m_index;
    do {
        next(); //sets us to the beginning if we hit the end -> loop through the maps

        const fs::path& path = m_filelist[m_index];
        if (path.filename().string().starts_with(name))
            return true;
    }
    while (m_index != start);

    return false;
}


AnnouncerList::AnnouncerList()
    : SimpleFileList(convertPath("sfx/announcer/"), ".txt")
{}

GraphicsList::GraphicsList()
    : SimpleDirectoryList(convertPath("gfx/packs/"))
{}

SoundsList::SoundsList()
    : SimpleDirectoryList(convertPath("sfx/packs/"))
{}

TourList::TourList()
    : SimpleFileList(convertPath("tours/"), ".txt")
{}

WorldList::WorldList()
    : SimpleFileList(convertPath("worlds/"), ".txt", true)
{}

BackgroundList::BackgroundList()
    : SimpleFileList(convertPath("gfx/packs/Classic/backgrounds/"), ".png")
{}

FiltersList::FiltersList()
    : SimpleFileList(convertPath("filters/"), ".txt")
{}


///////////// SkinList ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkinList::SkinList()
{
    FilesIterator dir(convertPath("gfx/skins/"), {".bmp", ".png"});
    while (auto path = dir.next()) {
        m_skins.emplace_back(SkinListNode {
            stripCreatorAndExt(path->filename().string()),
            path->string(),
        });
    }
    utils::sort(m_skins, [](const SkinListNode& lhs, const SkinListNode& rhs) {
        return lhs.name < rhs.name;
    });
}


///////////// SimpleDirectoryList ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SimpleDirectoryList::SimpleDirectoryList(const std::string &path)
{
    SubdirsIterator dir(path);
    while (auto path = dir.next()) {
        m_filelist.emplace_back(path->string());
    }
    if (m_filelist.empty()) {
        printf("ERROR: Empty directory.  %s\n", path.c_str());
        //exit(0);
    }

    m_index = 0;
}


///////////// MusicList ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MusicList::MusicList()
{
    SubdirsIterator dir(convertPath("music/game/"));
    while (auto path = dir.next()) {
        if (auto pack = MusicPack::load(path->string()))
            m_entries.emplace_back(std::move(*pack));
    }

    if (m_entries.empty()) {
        throw "Could not load any map music!";
    }
}

void MusicList::setRandomMusic(MusicCategory musicCategory, const std::string& mapName, const std::string& background)
{
    m_currentMusic = m_entries[m_currentIndex].randomMusic(musicCategory, mapName, background);
}

void MusicList::setNextMusic(MusicCategory musicCategory, const std::string& mapName, const std::string& background)
{
    m_currentMusic = m_entries[m_currentIndex].nextMusic(musicCategory, mapName, background);
}

void MusicList::next() {
    m_currentIndex = (m_currentIndex + 1) % m_entries.size();
}

void MusicList::prev() {
    m_currentIndex = (m_currentIndex == 0 ? m_entries.size() : m_currentIndex) - 1;
}

void MusicList::random() {
    m_currentIndex = RANDOM_INT(m_entries.size());
}

void MusicList::updateEntriesWithOverrides(const std::vector<MapMusicOverride>& overrides)
{
    for (MusicPack& entry : m_entries) {
        entry.updateWithOverrides(overrides);
    }
}


///////////// MusicPack///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MusicPack::MusicPack(std::string name)
    : m_name(std::move(name))
{}

std::optional<MusicPack> MusicPack::load(const fs::path& musicdirectory)
{
    MusicPack self(musicdirectory.filename().string());

    const fs::path file_path = musicdirectory / "Music.txt";
    std::ifstream file(file_path);
    if (!file) {
        printf("Error: Could not open: %s\n", file_path.c_str());
        return {};
    }

    struct HeaderSection {};
    struct CategorySection { MusicCategory category; };
    struct MapSpecificSection {};
    struct BackgroundSpecificSection {};
    using Section = std::variant<HeaderSection, CategorySection, MapSpecificSection, BackgroundSpecificSection>;

    // The file starts with four special songs
    Section current_section = HeaderSection {};

    std::string line;
    while (std::getline(file, line)) {
        //Ignore comment lines
        if (line.empty() || line[0] == '#' || line[0] == '\n' || line[0] == '\r' || line[0] == ' ' || line[0] == '\t')
            continue;

        //Chop off line ending
        if (line.ends_with('\r'))
            line.pop_back();

        //If we found a category header
        if (line[0] == '[') {
            std::transform(line.begin(), line.end(), line.begin(), ::tolower);
            current_section = [current_section, &line]() -> Section {
                if (line == "[land]") return CategorySection { MusicCategory::Land };
                if (line == "[underground]") return CategorySection { MusicCategory::Underground };
                if (line == "[underwater]") return CategorySection { MusicCategory::Underwater };
                if (line == "[castle]") return CategorySection { MusicCategory::Castle };
                if (line == "[platforms]") return CategorySection { MusicCategory::Platforms };
                if (line == "[ghost]") return CategorySection { MusicCategory::Ghost };
                if (line == "[bonus]") return CategorySection { MusicCategory::Bonus };
                if (line == "[battle]") return CategorySection { MusicCategory::Battle };
                if (line == "[desert]") return CategorySection { MusicCategory::Desert };
                if (line == "[clouds]") return CategorySection { MusicCategory::Clouds };
                if (line == "[snow]") return CategorySection { MusicCategory::Snow };
                if (line == "[maps]") return MapSpecificSection {};
                if (line == "[backgrounds]") return BackgroundSpecificSection {};
                return current_section;
            }();
            continue;
        }

        std::visit([&line, &self, &musicdirectory](auto const& section) {
            using T = std::decay_t<decltype(section)>;

            if constexpr (std::is_same_v<T, CategorySection>) {
                //Cap the number of songs at MAXCATEGORYTRACKS for a category
                if (self.m_category_songs[section.category].size() >= MAXCATEGORYTRACKS)
                    return;

                fs::path path = musicdirectory / line;
                if (!fs::exists(path))
                    return;

                self.m_all_songs.emplace_back(std::move(path));
                self.m_category_songs[section.category].emplace_back(self.m_all_songs.size() - 1);
                return;
            }
            if constexpr (std::is_same_v<T, MapSpecificSection> || std::is_same_v<T, BackgroundSpecificSection>) {
                std::list<std::string_view> tokens = tokenize(line, ',');
                if (tokens.size() < 2)
                    return;

                const auto mapname = std::string(tokens.front());
                tokens.pop_front();

                std::vector<size_t>& target_index_list = std::is_same_v<T, MapSpecificSection>
                    ? self.m_map_overrides[mapname]
                    : self.m_background_overrides[mapname];

                for (std::string_view filename : tokens) {
                    fs::path path = musicdirectory / filename;
                    if (fs::exists(path)) {
                        self.m_all_songs.emplace_back(std::move(path));
                        target_index_list.emplace_back(self.m_all_songs.size() - 1);
                    }
                }
                return;
            }
        }, current_section);
    }

    file.close();

    //Now read labeled subdirectories like "Land", "Underground", "Castle", etc for more songs
    for (size_t idx = 0; idx < static_cast<size_t>(MusicCategory::COUNT); idx++) {
        const auto category = static_cast<MusicCategory>(idx);
        const fs::path category_dir = musicdirectory / to_string(category);
        if (!fs::is_directory(category_dir))
            continue;

        FilesIterator iter(category_dir, {".ogg"});
        while (auto path = iter.next()) {
            if (self.m_category_songs[category].size() < MAXCATEGORYTRACKS) {
                self.m_all_songs.emplace_back(std::move(*path));
                self.m_category_songs[category].emplace_back(self.m_all_songs.size() - 1);
            }
        }
    }

    if (self.m_all_songs.empty()) {
        printf("Error: No songs found in: %s\n", musicdirectory.c_str());
        return {};
    }

    //Verify we have at least one track for each category
    const auto get_fallback_category = [](MusicCategory category) constexpr -> std::optional<MusicCategory> {
        switch (category) {
            case MusicCategory::Platforms: return MusicCategory::Land;
            case MusicCategory::Ghost: return MusicCategory::Underground;
            case MusicCategory::Bonus: return MusicCategory::Underwater;
            case MusicCategory::Battle: return MusicCategory::Castle;
            case MusicCategory::Desert: return MusicCategory::Land;
            case MusicCategory::Clouds: return MusicCategory::Land;
            case MusicCategory::Snow: return MusicCategory::Land;
            default: return std::nullopt;
        }
    };
    for (size_t idx = 0; idx < static_cast<size_t>(MusicCategory::COUNT); idx++) {
        const auto category = static_cast<MusicCategory>(idx);
        std::vector<size_t>& songlist = self.m_category_songs[category];
        if (songlist.empty()) {
            if (auto fallback = get_fallback_category(category)) {
                songlist = self.m_category_songs[*fallback];
            } else {
                printf("Error: Missing track definition for music category: %s\n", to_string(category).data());
                return {};
            }
        }
    }

#ifndef NDEBUG
    printf("------ Songlist for %s ------\n", self.m_name.c_str());
    for (size_t i = 0; i < self.m_all_songs.size(); i++) {
        printf("%lu: %s\n", i, self.m_all_songs[i].c_str());
    }
    printf("\n\n");
    for (const auto& [category, songlist] : self.m_category_songs) {
        printf("---- %s ----\n", to_string(category).data());
        for (size_t i = 0; i < songlist.size(); i++) {
            const size_t file_idx = songlist[i];
            printf("%lu: %s\n", i, self.m_all_songs[file_idx].c_str());
        }
    }
#endif

    return self;
}

const fs::path& MusicPack::music(size_t idx) const
{
    idx = std::min(idx, m_all_songs.size() - 1);
    return m_all_songs[idx];
}

const fs::path& MusicPack::randomMusic(MusicCategory musicCategory, const std::string& mapName, const std::string& background)
{
    //First check if there is specific map music
    const auto mapIter = m_map_overrides.find(mapName);
    if (mapIter != m_map_overrides.end() && !mapIter->second.empty()) {
        m_currentMusic = RANDOM_INT(mapIter->second.size());
        const size_t path_idx = mapIter->second[m_currentMusic];
        return m_all_songs[path_idx];
    }

    //Then check if there is specific background music
    const auto bgIter = m_background_overrides.find(background);
    if (bgIter != m_background_overrides.end() && !bgIter->second.empty()) {
        m_currentMusic = RANDOM_INT(bgIter->second.size());
        const size_t path_idx = bgIter->second[m_currentMusic];
        return m_all_songs[path_idx];
    }

    //Then default to the music category
    if (!m_category_songs[musicCategory].empty()) {
        m_currentMusic = RANDOM_INT(m_category_songs[musicCategory].size());
        const size_t path_idx = m_category_songs[musicCategory][m_currentMusic];
        return m_all_songs[path_idx];
    }

    return m_all_songs.at(4);
}

const fs::path& MusicPack::nextMusic(MusicCategory musicCategory, const std::string& mapName, const std::string& background)
{
    //First check if there is specific map music
    const auto mapIter = m_map_overrides.find(mapName);
    if (mapIter != m_map_overrides.end() && !mapIter->second.empty()) {
        if (++m_currentMusic >= mapIter->second.size())
            m_currentMusic = 0;

        return m_all_songs[mapIter->second[m_currentMusic]];
    }
    //Then check if there is specific background music
    const auto bgIter = m_background_overrides.find(background);
    if (bgIter != m_background_overrides.end() && !bgIter->second.empty()) {
        if (++m_currentMusic >= bgIter->second.size())
            m_currentMusic = 0;

        return m_all_songs[bgIter->second[m_currentMusic]];
    }
    //Then default to the music category
    if (!m_category_songs[musicCategory].empty()) {
        if (++m_currentMusic >= m_category_songs[musicCategory].size())
            m_currentMusic = 0;

        return m_all_songs[m_category_songs[musicCategory][m_currentMusic]];
    }

    return m_all_songs.at(4);
}

void MusicPack::updateWithOverrides(const std::vector<MapMusicOverride>& overrides)
{
    for (const MapMusicOverride& override : overrides) {
        std::vector<size_t>& songlist = m_map_overrides[override.mapname];
        songlist.reserve(songlist.size() + override.songs.size());
        for (const fs::path& path : override.songs) {
            m_all_songs.emplace_back(path);
            songlist.emplace_back(m_all_songs.size() - 1);
        }
    }
}


///////////// MusicList ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WorldMusicList::WorldMusicList()
{
    SubdirsIterator dir(convertPath("music/world/"));
    while (auto path = dir.next()) {
        if (auto pack = WorldMusicPack::load(path->string())) {
            m_entries.emplace_back(std::move(*pack));
        }
    }
    if (m_entries.empty()) {
        throw "Could not load any world music!";
    }
}

void WorldMusicList::next() {
    m_currentIndex = (m_currentIndex + 1) % m_entries.size();
}

void WorldMusicList::prev() {
    m_currentIndex = (m_currentIndex == 0 ? m_entries.size() : m_currentIndex) - 1;
}

void WorldMusicList::random() {
    m_currentIndex = RANDOM_INT(m_entries.size());
}

void WorldMusicList::updateEntriesWithOverrides(const std::vector<WorldMusicOverride>& overrides)
{
    for (WorldMusicPack& entry : m_entries) {
        entry.updateWithOverrides(overrides);
    }
}


///////////// WorldMusicPack ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WorldMusicPack::WorldMusicPack(std::string name)
    : m_name(std::move(name))
{}

std::optional<WorldMusicPack> WorldMusicPack::load(const fs::path& musicdirectory)
{
    WorldMusicPack self(musicdirectory.filename().string());

    const fs::path file_path = musicdirectory / "Music.txt";
    std::ifstream file(file_path);
    if (!file) {
        printf("Error: Could not open: %s\n", file_path.c_str());
        return {};
    }

    struct HeaderSection {};
    struct CategorySection { WorldMusicCategory category; };
    struct WorldSpecificSection {};
    struct BackgroundSpecificSection {};
    using Section = std::variant<HeaderSection, CategorySection, WorldSpecificSection>;

    // The file starts with a version header
    Section current_section = HeaderSection {};

    std::string line;
    while (std::getline(file, line)) {
        //Ignore comment lines
        if (line.empty() || line[0] == '#' || line[0] == '\n' || line[0] == '\r' || line[0] == ' ' || line[0] == '\t')
            continue;

        //Chop off line ending
        if (line.ends_with('\r'))
            line.pop_back();

        //If we found a category header
        if (line[0] == '[') {
            std::transform(line.begin(), line.end(), line.begin(), ::tolower);
            current_section = [current_section, &line]() -> Section {
                if (line == "[grass]") return CategorySection { WorldMusicCategory::Grass };
                if (line == "[desert]") return CategorySection { WorldMusicCategory::Desert };
                if (line == "[water]") return CategorySection { WorldMusicCategory::Water };
                if (line == "[giant]") return CategorySection { WorldMusicCategory::Giant };
                if (line == "[sky]") return CategorySection { WorldMusicCategory::Sky };
                if (line == "[ice]") return CategorySection { WorldMusicCategory::Ice };
                if (line == "[pipe]") return CategorySection { WorldMusicCategory::Pipe };
                if (line == "[dark]") return CategorySection { WorldMusicCategory::Dark };
                if (line == "[space]") return CategorySection { WorldMusicCategory::Space };
                if (line == "[bonus]") return CategorySection { WorldMusicCategory::Bonus };
                if (line == "[sleep]") return CategorySection { WorldMusicCategory::Sleep };
                if (line == "[worlds]") return WorldSpecificSection {};
                return current_section;
            }();
            continue;
        }

        std::visit([&line, &self, &musicdirectory](auto const& section) {
            using T = std::decay_t<decltype(section)>;

            if constexpr (std::is_same_v<T, CategorySection>) {
                fs::path path = musicdirectory / line;
                if (fs::exists(path)) {
                    self.m_category_song[section.category] = std::move(path);
                }
                return;
            }
            if constexpr (std::is_same_v<T, WorldSpecificSection>) {
                std::list<std::string_view> tokens = tokenize(line, ',');
                if (tokens.size() < 2)
                    return;

                const auto worldname = std::string(tokens.front());
                tokens.pop_front();
                const std::string_view filename = tokens.front();

                fs::path path = musicdirectory / filename;
                if (!fs::exists(path))
                    return;

                self.m_world_overrides[worldname] = std::move(path);
                return;
            }
        }, current_section);
    }

    file.close();

#ifndef NDEBUG
    printf("------ World Songlist for %s ------\n", self.m_name.c_str());
    for (const auto& [category, song] : self.m_category_song) {
        printf("%s: %s\n", to_string(category).data(), song.c_str());
    }
#endif
    return self;
}

const fs::path& WorldMusicPack::music(WorldMusicCategory category, const std::string& worldName) const
{
    //First check if there is specific map music
    const auto iter = m_world_overrides.find(worldName);
    if (iter != m_world_overrides.cend()) {
        return iter->second;
    }

    if (auto it = m_category_song.find(category); it != m_category_song.cend()) {
        return it->second;
    }
    return m_category_song.at(WorldMusicCategory::Grass);
}

void WorldMusicPack::updateWithOverrides(const std::vector<WorldMusicOverride>& overrides)
{
    for (const WorldMusicOverride& override : overrides) {
        m_world_overrides[override.worldname] = override.song;
    }
}
