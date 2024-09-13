#ifndef MAPLIST_H
#define MAPLIST_H

#include <map>
#include <string>
#include <vector>

class MapListNode {
public:
    MapListNode(std::string fullName);

    std::vector<bool> pfFilters;
    std::string filename;
    int iShortNameLength = 0;

    short iIndex = 0;
    short iFilteredIndex = 0;

    bool fInCurrentFilterSet = false;
    bool fReadFromCache = false;
    bool fValid = true;
};

//announcerlist and musiclist are still a screwed up (a vector accessed like a vector and a list), but way better than before
class MapList
{
    public:
        MapList(bool fWorldEditor);
        ~MapList();

		//Adds maps in all world map directories to the map list so they can be edited in the map editor
		void addWorldMaps();

        void add(const char * name);
        bool find(const char * name);
		bool findexact(const char * name, bool fWorld);
        bool startswith(char letter);
		bool startswith(std::string match);

    const std::string& currentFilename() const {
        return (*outercurrent).second->filename;
    }
    const std::string& currentShortmapname() const {
        return (*outercurrent).first;
    }
    int currentShortMapNameLen() const {
        return (*outercurrent).second->iShortNameLength;
    }

		void prev(bool fUseFilters);
        void next(bool fUseFilters);
		void random(bool fUseFilters);

    std::string randomFilename() const;

    /// Sets whether the current map is valid and can be loaded
    void setValid(bool fValid) {
        (*current).second->fValid = fValid;
    }
    /// The current map is valid and can be loaded
    bool isValid() const {
        return (*current).second->fValid;
    }

    bool isEmpty() const {
        return maps.empty();
    }
    size_t filteredCount() const {
        return iFilteredMapCount;
    }
    size_t count() const {
        return maps.size();
    }

    std::multimap<std::string, MapListNode*>::iterator GetCurrent() {
        return current;
    }
    void SetCurrent(std::multimap<std::string, MapListNode*>::iterator itr) {
        outercurrent = current = itr;
    }

		void WriteFilters();
		void ReadFilters();

    bool GetFilter(size_t iFilter) const {
        return (*current).second->pfFilters[iFilter];
    }
    void ToggleFilter(size_t iFilter) {
        (*current).second->pfFilters[iFilter] = !(*current).second->pfFilters[iFilter];
    }

		bool FindFilteredMap();
		void ApplyFilters(const std::vector<bool>& pfFilters);
		bool MapInFilteredSet();

		std::multimap<std::string, MapListNode*>::iterator GetIteratorAt(unsigned short iIndex, bool fUseFilters);

    void SaveCurrent() {
        savedcurrent = current;
    }
    void ResumeCurrent() {
        current = savedcurrent;
    }

		void ReloadMapAutoFilters();
		void WriteMapSummaryCache();

		const char * GetUnknownMapName();

    private:

        std::multimap<std::string, MapListNode*> maps;
		std::multimap<std::string, MapListNode*> worldmaps;

        std::multimap<std::string, MapListNode*>::iterator current;
		std::multimap<std::string, MapListNode*>::iterator savedcurrent;

		std::multimap<std::string, MapListNode*>::iterator outercurrent;

		size_t iFilteredMapCount;

		std::multimap<std::string, MapListNode*>::iterator * mlnFilteredMaps;
		std::multimap<std::string, MapListNode*>::iterator * mlnMaps;

		char szUnknownMapString[2];
};

#endif // MAPLIST_H
