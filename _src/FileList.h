#ifndef __FILELIST_H_
#define __FILELIST_H_

#include <string>

//it was kinda a bad idea to have skinlist and announcer list based on this, because both are accessed in different ways (skinlist like an vector and announcer list like a list). grrrr
class SimpleFileList
{
    public:
		SimpleFileList() {};
        SimpleFileList(const std::string &path, const std::string &extension, bool fAlphabetize = false);
        virtual ~SimpleFileList();
        const char * GetIndex(unsigned int index);
        int GetCount() {return filelist.size();}

        int GetCurrentIndex(){return currentIndex;};
		void SetCurrent(unsigned int index)
		{
			if(filelist.empty())
				return;

			if(index < filelist.size())
				currentIndex = index;
			else
				currentIndex = 0;
		};

        const char * current_name()
		{
			if(currentIndex > -1)
				return filelist[currentIndex].c_str();

			return NULL;
		};

        void next();
        void prev();

        void random()
		{
			if(!filelist.empty())
				currentIndex = rand() % filelist.size();
		};

		void SetCurrentName(const std::string &name);

		void add(const char * name)
		{
			filelist.push_back(name);
		}

		bool find(const char * name)
		{
			char * szLookForName = _strlwr(_strdup(name));
			bool fFound = false;

			int oldCurrent = currentIndex;
			do
			{
				next();	//sets us to the beginning if we hit the end -> loop through the maps

				char * szCurrentName = _strlwr(_strdup(filelist[currentIndex].c_str()));

				if(strstr(szCurrentName, szLookForName))	//compare names after
					fFound = true;

				free(szCurrentName);
			}
			while(currentIndex != oldCurrent && !fFound);

			free(szLookForName);

			return fFound;
		}
		
    protected:
        std::vector<std::string> filelist;
        int currentIndex;
};

class SimpleDirectoryList : public SimpleFileList
{
    public:
        SimpleDirectoryList(const std::string &path);
        virtual ~SimpleDirectoryList(){;};
};

class SkinListNode
{
	public:
		SkinListNode(std::string skinName, std::string skinPath);
		~SkinListNode() {}

		std::string sSkinName;
		std::string sSkinPath;
};

class SkinList
{
    public:
        SkinList();
		int GetCount() {return skins.size();}
		const char * GetIndex(unsigned int index);
		const char * GetSkinName(unsigned int index);

	private:
		std::vector<SkinListNode*> skins;
};

class AnnouncerList : public SimpleFileList
{
    public:
        AnnouncerList() : SimpleFileList(convertPath("sfx/announcer/"), ".txt"){;};
};

class GraphicsList : public SimpleDirectoryList
{
    public:
        GraphicsList() : SimpleDirectoryList(convertPath("gfx/packs/")){;};
};

class SoundsList : public SimpleDirectoryList
{
    public:
        SoundsList() : SimpleDirectoryList(convertPath("sfx/packs/")){;};
};

class TourList : public SimpleFileList
{
    public:
        TourList() : SimpleFileList(convertPath("tours/"), ".txt"){;};
};

class WorldList : public SimpleFileList
{
    public:
        WorldList() : SimpleFileList(convertPath("worlds/"), ".txt", true){;};
};

class BackgroundList : public SimpleFileList
{
    public:
        BackgroundList() : SimpleFileList(convertPath("gfx/packs/Classic/backgrounds/"), ".png"){;};
};

class FiltersList : public SimpleFileList
{
    public:
        FiltersList() : SimpleFileList(convertPath("filters/"), ".txt"){;};
};

class MusicOverride
{
	public:
		std::vector<int> songs;
};

class MusicEntry
{
    public:
        MusicEntry(const std::string & musicdirectory);
		~MusicEntry() {}

		void UpdateWithOverrides();

        std::string GetMusic(unsigned int musicID);
        std::string GetRandomMusic(int iCategoryID, const char * szMapName, const char * szBackground);
		std::string GetNextMusic(int iCategoryID, const char * szMapName, const char * szBackground);

        int		numsongsforcategory[MAXMUSICCATEGORY];
        int		songsforcategory[MAXMUSICCATEGORY][MAXCATEGORYTRACKS];
        std::vector<std::string> songFileNames;

		std::map<std::string, MusicOverride*> mapoverride;
		std::map<std::string, MusicOverride*> backgroundoverride;

        std::string name;
		unsigned short iCurrentMusic;

        bool fError;

		bool fUsesMapOverrides;
		bool fUsesBackgroundOverrides;
};

class MusicList
{
    public:
        MusicList();
        ~MusicList();

        std::string GetMusic(int musicID);
        void SetRandomMusic(int iCategoryID, const char * szMapName, const char * szBackground);
		void SetNextMusic(int iCategoryID, const char * szMapName, const char * szBackground);
        std::string GetCurrentMusic();

        int GetCurrentIndex(){return currentIndex;};
        void SetCurrent(unsigned int index)
		{
			if(index < entries.size())
				currentIndex = index;
			else
				currentIndex = 0;
		};

        const char * current_name(){return entries[currentIndex]->name.c_str();};
        void next();
        void prev();
        void random(){currentIndex = rand()%entries.size();};

		void UpdateEntriesWithOverrides();

    private:
        std::string CurrentMusic;
        std::vector<MusicEntry*> entries;
        int currentIndex;
};

class WorldMusicEntry
{
    public:
        WorldMusicEntry(const std::string & musicdirectory);
		~WorldMusicEntry() {}

		void UpdateWithOverrides();

		std::string GetMusic(unsigned int musicID, const char * szWorldName);

        std::string songFileNames[MAXWORLDMUSICCATEGORY + 2];
        std::string name;

		std::map<std::string, std::string> worldoverride;

        bool fError;

		bool fUsesWorldOverrides;
};

class WorldMusicList
{
    public:
		WorldMusicList();
		~WorldMusicList();

		std::string GetMusic(int musicID, const char * szWorldName);
		std::string GetCurrentMusic();

		int GetCurrentIndex(){return currentIndex;}
		void SetCurrent(unsigned int index)
		{
			if(index < entries.size())
				currentIndex = index;
			else
				currentIndex = 0;
		};

		const char * current_name(){return entries[currentIndex]->name.c_str();}
		void next();
		void prev();
		void random() {currentIndex = rand()%entries.size();}

		int GetCount() {return entries.size();}

		void UpdateEntriesWithOverrides();

    private:
        std::string CurrentMusic;
        std::vector<WorldMusicEntry*> entries;
        int currentIndex;
};

#endif //__FILELIST_H_

