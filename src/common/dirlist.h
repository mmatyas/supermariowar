/*
DirectoryList is a C++ class for enumerating the contents of directories.
Copyright (C) 2005 Donny Viszneki <smirk@thebuicksix.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef LIST_DIRECTORIES_HEADER
#define LIST_DIRECTORIES_HEADER

#ifdef _XBOX
#  include <xtl.h>
#else
#  ifdef _WIN32
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#include <cstdio>
#  else
#    include <sys/types.h>
#    include <sys/stat.h>
#    include <unistd.h>
#    include <dirent.h>
#  endif
#endif

#include <string>

class DirectoryListing
{
    public:
        /* Constructor accepts path, optional filename extension */
        DirectoryListing(std::string path, std::string file_ext="");

        /* Ask the listing whether or not it successfully got access to a folder */
    bool GetSuccess() {
        return Success;
    };

        /* Get next filename */
        bool operator()(std::string &s);
		bool NextDirectory (std::string &s);

		std::string fullName(const std::string &s);

        /* Destructor */
        ~DirectoryListing();

    private:
        bool Success;
		std::string path;
        std::string Filename_Extension;

        #ifdef _WIN32
        WIN32_FIND_DATA	finddata;
        HANDLE			findhandle;
        std::string Stored_Filename;

        #else
        DIR * dhandle;
        struct dirent * current;

        #endif
};

#endif // LIST_DIRECTORIES_HEADER
