#include "FileIO.h"
#include "path.h"

#include <stdint.h>
#include <stdexcept>

void fread_or_exception(void* ptr, size_t size, size_t count, FILE* stream)
{
    if (fread(ptr, size, count, stream) != count) {
        perror("File read error");
        throw std::runtime_error("File read error");
    }
}

void fwrite_or_exception(const void* ptr, size_t size, size_t count, FILE* stream)
{
    if (fwrite(ptr, size, count, stream) != count) {
        perror("File write error");
        throw std::runtime_error("File write error");
    }
}

FILE * OpenFile(const char * filename, const char * options)
{
	std::string homeDirectory = GetHomeDirectory();

	std::string optionsbin = homeDirectory + std::string(filename);

	return fopen(optionsbin.c_str(), options);
}

void WriteInt(int out, FILE * outFile)
{
#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
	int t = out;

    ((char*)&out)[0] = ((char*)&t)[3];
    ((char*)&out)[1] = ((char*)&t)[2];
    ((char*)&out)[2] = ((char*)&t)[1];
    ((char*)&out)[3] = ((char*)&t)[0];
#endif

	fwrite_or_exception(&out, sizeof(Uint32), 1, outFile);
}

int ReadInt(FILE * inFile)
{
	int in;
	fread_or_exception(&in, sizeof(Uint32), 1, inFile);

#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
	int t = in;

    ((char*)&in)[0] = ((char*)&t)[3];
    ((char*)&in)[1] = ((char*)&t)[2];
    ((char*)&in)[2] = ((char*)&t)[1];
    ((char*)&in)[3] = ((char*)&t)[0];
#endif

	return in;
}

#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
void ReadIntChunk(int * mem, size_t iQuantity, FILE * inFile)
{
    for (unsigned int i = 0; i < iQuantity; i++) {
		fread_or_exception(&mem[i], sizeof(Uint32), 1, inFile);

		int t = mem[i];

		((char*)&mem[i])[0] = ((char*)&t)[3];
		((char*)&mem[i])[1] = ((char*)&t)[2];
		((char*)&mem[i])[2] = ((char*)&t)[1];
		((char*)&mem[i])[3] = ((char*)&t)[0];
	}
}
#else
void ReadIntChunk(int * mem, size_t iQuantity, FILE * inFile)
{
	fread_or_exception(mem, sizeof(Uint32), iQuantity, inFile);
}
#endif


void WriteFloat(float out, FILE * outFile)
{
#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
	float t = out;

    ((char*)&out)[0] = ((char*)&t)[3];
    ((char*)&out)[1] = ((char*)&t)[2];
    ((char*)&out)[2] = ((char*)&t)[1];
    ((char*)&out)[3] = ((char*)&t)[0];
#endif

	fwrite_or_exception(&out, sizeof(float), 1, outFile);
}

float ReadFloat(FILE * inFile)
{
	float in;
	fread_or_exception(&in, sizeof(float), 1, inFile);

#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
	float t = in;

    ((char*)&in)[0] = ((char*)&t)[3];
    ((char*)&in)[1] = ((char*)&t)[2];
    ((char*)&in)[2] = ((char*)&t)[1];
    ((char*)&in)[3] = ((char*)&t)[0];
#endif

	return in;
}

void WriteString(const char * szString, FILE * outFile)
{
	int iLen = strlen(szString) + 1;
	WriteInt(iLen, outFile);
	fwrite_or_exception(szString, sizeof(Uint8), iLen, outFile);
}

void ReadString(char * szString, short size, FILE * inFile)
{
	int iLen = ReadInt(inFile);

	char * szReadString = new char[iLen];
	fread_or_exception(szReadString, sizeof(Uint8), iLen, inFile);
	szReadString[iLen - 1] = 0;

	//Prevent buffer overflow  5253784 5253928
	strncpy(szString, szReadString, size - 1);
	szString[size - 1] = 0;

	delete [] szReadString;
}


void WriteByteFromShort(short out, FILE * outFile)
{
	int8_t b = (int8_t)out;
	fwrite_or_exception(&b, sizeof(Uint8), 1, outFile);
}

short ReadByteAsShort(FILE * inFile)
{
	int8_t b;
	fread_or_exception(&b, sizeof(Uint8), 1, inFile);

	return (short)b;
}

void WriteBool(bool out, FILE * outFile)
{
	fwrite_or_exception(&out, sizeof(Uint8), 1, outFile);
}

bool ReadBool(FILE * inFile)
{
	bool b;
	fread_or_exception(&b, sizeof(Uint8), 1, inFile);

	return b;
}

void WriteByte(Uint8 out, FILE * outFile)
{
	fwrite_or_exception(&out, sizeof(Uint8), 1, outFile);
}

Uint8 ReadByte(FILE * inFile)
{
	Uint8 b;
	fread_or_exception(&b, sizeof(Uint8), 1, inFile);

	return b;
}
