#ifndef MAPLIST_H
#define MAPLIST_H

#include "path.h"

#include <map>
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

		int iShortNameLength;

		bool fReadFromCache;

		bool fValid;
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

    const char* currentFilename() {
        return (*outercurrent).second->filename.c_str();
    }
    const char* currentShortmapname() {
        return stripCreatorAndDotMap(outercurrent->first).c_str();
    }
    int currentShortMapNameLen() {
        return (*outercurrent).second->iShortNameLength;
    }

		void prev(bool fUseFilters);
        void next(bool fUseFilters);
		void random(bool fUseFilters);

		const char* randomFilename();

		//Sets/Gets if a map at the current map node is valid and can be loaded
    void SetValid(bool fValid) {
        (*current).second->fValid = fValid;
    }
    bool GetValid() {
        return (*current).second->fValid;
    }

		//TODO: use size?
    bool IsEmpty() {
        return maps.empty();
    }

    int GetFilteredCount() {
        return iFilteredMapCount;
    }
    int GetCount() {
        return maps.size();
    }

    std::map<std::string, MapListNode*>::iterator GetCurrent() {
        return current;
    }
    void SetCurrent(std::map<std::string, MapListNode*>::iterator itr) {
        outercurrent = current = itr;
    }

		void WriteFilters();
		void ReadFilters();

    bool GetFilter(short iFilter) {
        return (*current).second->pfFilters[iFilter];
    }
    bool * GetFilters() {
        return (*current).second->pfFilters;
    }
    void ToggleFilter(short iFilter) {
        (*current).second->pfFilters[iFilter] = !(*current).second->pfFilters[iFilter];
    }

		bool FindFilteredMap();
		void ApplyFilters(bool * pfFilters);
		bool MapInFilteredSet();

		std::map<std::string, MapListNode*>::iterator GetIteratorAt(unsigned short iIndex, bool fUseFilters);

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
        struct CompareShortName {
            bool operator()(const std::string& a, const std::string& b) const {
                const std::string a_short = stripCreatorAndDotMap(a);
                const std::string b_short = stripCreatorAndDotMap(b);
                if (a_short != b_short)
                    return a_short < b_short;

                return a < b;
            }
        };


        std::map<std::string, MapListNode*, CompareShortName> maps;
		std::map<std::string, MapListNode*, CompareShortName> worldmaps;

        std::map<std::string, MapListNode*, CompareShortName>::iterator current;
		std::map<std::string, MapListNode*, CompareShortName>::iterator savedcurrent;

		std::map<std::string, MapListNode*, CompareShortName>::iterator outercurrent;

		short iFilteredMapCount;

		std::map<std::string, MapListNode*, CompareShortName>::iterator * mlnFilteredMaps;
		std::map<std::string, MapListNode*, CompareShortName>::iterator * mlnMaps;

		char szUnknownMapString[2];
};

#endif // MAPLIST_H
