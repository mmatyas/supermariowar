#include "options.h"
#include "SDL_endian.h"
#include <fstream>
using namespace std;

#define Reading (!Writing)

Options :: Options(const string& filename) :
    FileName(filename),
    Opened(false),
    Writing(false),
    NativeSex(true)
{
    Ropen();
    Transfer();
    Close();
}

bool Options :: Ropen()
{
    // Set whether we are reading or writing
    Writing = false;
    
    // Open the file
    File.open(FileName, ios::binary | ios::read);

    // Fail
    if (!Opened()) return false;

    // Determine the file's bytesex
    int add_boobs;
    File.read(&add_boobs, sizeof(add_boobs));
    if (add_boobs == 0xADDB00B5) NativeSex = true;
    else if (add_boobs == 0xB500DBAD) NativeSex = false;
    else
    {
        // This file has no bytesex signature
        cout<< "The file \""<< FileName<< "\" has no bytesex signature"<< endl;
        File.close();
        return false;
    }

    return true;
}

bool Options :: Wopen()
{
    // Set whether we are reading or writing
    Writing = true;

    // Enable our native bytesex mode
    NativeSex = true;

    // Open file
    File.open(FileName, ios::binary | ios:write | ios::trunc);

    // Fail
    if (!Opened()) return false;

    // Leave our bytesex signature
    int add_boobs = 0xADB00B5;
    File.write(&add_boobs, sizeof(add_boobs));
}

bool Options :: Opened()
{
    return File.is_open()
}

// Read or write strings of data. 
bool Options :: Wread(void *n, size_t m)
{
    if (Writing)
    {
        File.write(n, m);
        return true;
    }

    else
    {
        File.read(n, m);
        // TODO: Check for errors
        return true;
    }
}

bool Options :: Do32(int *n)
{
    if (Writing)
    {
        if (NativeSex)
            File.write(n, 4);

        else
        {
            int m;
            ((char*)&m)[0] = ((char*)n)[3];
            ((char*)&m)[1] = ((char*)n)[2];
            ((char*)&m)[2] = ((char*)n)[1];
            ((char*)&m)[3] = ((char*)n)[0];
            
            File.write(&m, 4);
        }

        return true;
    }

    // Reading
    else
    {
        if (NativeSex)
            File.read(n, 4);  // TODO: Error checking

        else
        {
            int m;
            File.read(&m, 4); // TODO: Error checking

            ((char*)n)[0] = ((char*)&m)[3];
            ((char*)n)[1] = ((char*)&m)[2];
            ((char*)n)[2] = ((char*)&m)[1];
            ((char*)n)[3] = ((char*)&m)[0];
        }

        return true;
    }
}

bool Options :: Do16(short *n)
{
    if (Writing)
    {
        if (NativeSex)
            File.write(n, 2);

        else
        {
            short m;
            ((char*)&m)[0] = ((char*)n)[1];
            ((char*)&m)[1] = ((char*)n)[0];
            
            File.write(&m, 2);
        }

        return true;
    }

    // Reading
    else
    {
        if (NativeSex)
            File.read(n, 2);  // TODO: Error checking

        else
        {
            short m;
            File.read(&m, 2); // TODO: Error checking

            ((char*)n)[0] = ((char*)&m)[1];
            ((char*)n)[1] = ((char*)&m)[0];
        }

        return true;
    }
}

bool Options :: Do8(char *n)
{
    if (Writing)
    {
        File.write(n, 1);

        return true;
    }

    // Reading
    else
    {
        File.read(n, 1);  // TODO: Error checking

        return true;
    }
}

