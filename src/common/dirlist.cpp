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

#include "dirlist.h"

#include <string>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;


/* Use references to avoid copying */
inline bool endsWith(string& s, string& q)
{
    /* Cache these results because we used references*/
    int size_s = s.size();
    int size_q = q.size();

    /* s cannot contain q */
    if (size_s < size_q) return false;

    /* Look for it */
    return s.substr(size_s-size_q, size_q) == q;
}

DirectoryListing::DirectoryListing(string dlPath, string file_ext)
{
    /* Store optional filename extension and search path */
    Filename_Extension = file_ext;
    this->path = dlPath;

    /* Windows Directory Enumeration*/
    #ifdef _WIN32

		string search = path+"*";
        findhandle = FindFirstFile(search.c_str(), &finddata);
        Success = findhandle != INVALID_HANDLE_VALUE;
        Stored_Filename = finddata.cFileName;

    /* POSIX Directory Enumeration */
    #else
        dhandle = opendir(path.c_str());
        Success = dhandle != NULL;
    #endif
}

DirectoryListing::~DirectoryListing()
{
    if (!Success) return;

    #ifdef _WIN32
        FindClose(findhandle);
    #else
        closedir(dhandle);
    #endif
}


std::string DirectoryListing :: fullName(const std::string &s)
{
    return path + s;
}

bool DirectoryListing :: operator() (string &s)
{
    if (!Success) return false;

    bool retval;

    do
    {
        /* Microsoft directory enumeration - here we retval = a stored filename first,
         * and then fetch the next filename from microsoft */
        #ifdef _WIN32
            /* If we have a filename stored.. */
            if (Stored_Filename.size())
            {
                /* Return this filename */
                s = Stored_Filename;

                /* Ask for another filename */
                if (FindNextFile(findhandle, &finddata))
                    Stored_Filename = finddata.cFileName;

                else Stored_Filename = "";

                retval = true;
            }

            /* No files remain */
            else retval = false;


        /* POSIX directory enumeration - more straightforward */
        #else

            /* Try to get next file in list */
            current = readdir(dhandle);
        if (current) {
                /* Return the filename */
                s = current->d_name;
                retval = true;
            }

            /* There are no more files in the list*/
            else retval = false;

        #endif

    } while (retval == true && endsWith(s, Filename_Extension) == false);

    return retval;
}

bool DirectoryListing :: NextDirectory (string &s)
{
	if (!Success) return false;

    bool retval;
        /* Microsoft directory enumeration - here we retval = a stored filename first,
         * and then fetch the next directory from microsoft */
#ifdef _WIN32

	struct stat fileinfo;
    do {
        /* If we have a filename stored.. */
        if (Stored_Filename.size()) {
            /* Return this filename */
            s = Stored_Filename;

            /* Ask for another filename */
            if (FindNextFile(findhandle, &finddata))
                Stored_Filename = finddata.cFileName;

            else Stored_Filename = "";

            retval = true;
        }

        /* No files remain */
        else
            retval = false;
	}
    //while (retval == true && ((GetFileAttributes(s.c_str()) & FILE_ATTRIBUTE_DIRECTORY) == 0 || s == "." || s == ".." || s == "CVS" || s == ".svn"));
	while (retval == true && (stat(fullName(s).c_str(), &fileinfo) == -1 || (fileinfo.st_mode & _S_IFDIR) == 0 || s == "." || s == ".." || s == "CVS" || s == ".svn"));

#else /* POSIX directory enumeration - more straightforward */

    struct stat fileinfo;
    do {
        /* Try to get next file in list */
        current = readdir(dhandle);
        if (current) {
            /* Return the filename */
            s = current->d_name;
            retval = true;
        }

        /* There are no more files in the list*/
        else retval = false;
    } while (retval == true && (stat(fullName(s).c_str(), &fileinfo) == -1 || !S_ISDIR(fileinfo.st_mode) || s == "." || s == ".." || s == "CVS" || s == ".svn"));

#endif

    return retval;
}
