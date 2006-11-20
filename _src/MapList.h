#include <string>

class MapListNode
{
	public:
		MapListNode(std::string fullName);
		~MapListNode();

		bool * pfFilters;
		bool fInCurrentFilterSet;
		std::string filename;
		short iIndex;
		short iFilteredIndex;

		bool fReadFromCache;

		bool fValid;
};

//announcerlist and musiclist are still a screwed up (a vector accessed like a vector and a list), but way better than before
class MapList
{
    public:
        MapList();
        ~MapList();

        void add(const char * name);
        bool find(const char * name);
		bool findexact(const char * name);
        bool startswith(char letter);

		const char* currentFilename(){return (*current).second->filename.c_str();};
        const char* currentShortmapname(){return (*current).first.c_str();};
        
		void prev(bool fUseFilters);
        void next(bool fUseFilters);
		void random(bool fUseFilters);

		//Sets/Gets if a map at the current map node is valid and can be loaded
		void SetValid(bool fValid) {(*current).second->fValid = fValid;}
		bool GetValid() {return (*current).second->fValid;}
        
		int GetFilteredCount() {return iFilteredMapCount;}
		int GetCount() {return maps.size();}
		
		std::map<std::string, MapListNode*>::iterator GetCurrent() {return current;}
		void SetCurrent(std::map<std::string, MapListNode*>::iterator itr) {current = itr;}

		void WriteFilters();
		void ReadFilters();
		
		bool GetFilter(short iFilter) {return (*current).second->pfFilters[iFilter];}
		bool * GetFilters() {return (*current).second->pfFilters;}
		void ToggleFilter(short iFilter) {(*current).second->pfFilters[iFilter] = !(*current).second->pfFilters[iFilter];}

		bool FindFilteredMap();
		void ApplyFilters(bool * pfFilters);
		bool MapInFilteredSet();

		std::map<std::string, MapListNode*>::iterator GetIteratorAt(unsigned short iIndex, bool fUseFilters);

		void SaveCurrent() {savedcurrent = current;}
		void ResumeCurrent() {current = savedcurrent;}

		void ReloadMapAutoFilters();
		void WriteMapSummaryCache();

    private:

        std::map<std::string, MapListNode*> maps;
        std::map<std::string, MapListNode*>::iterator current;
		std::map<std::string, MapListNode*>::iterator savedcurrent;

		short iFilteredMapCount;

		std::map<std::string, MapListNode*>::iterator * mlnFilteredMaps;
		std::map<std::string, MapListNode*>::iterator * mlnMaps;
};

//it was kinda a bad idea to have skinlist and announcer list based on this, because both are accessed in different ways (skinlist like an vector and announcer list like a list). grrrr
class SimpleFileList
{
    public:
		SimpleFileList() {};
        SimpleFileList(const std::string &path, const std::string &extension);
        virtual ~SimpleFileList(){;};	//i have no idea how to do that virtual destructor crap right. i guess this might do it.
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


    private:
        std::string CurrentMusic;
        std::vector<MusicEntry*> entries;
        int currentIndex;
};

