#include "global.h"

FILE * OpenFile(const char * filename, const char * options)
{
#ifdef _XBOX
	std::string optionsbin = std::string("D:\\") + std::string(filename);
#else
	#ifdef PREFIXPATH
		char * folder=getenv("HOME");
		#ifdef __MACOSX__
			std::string optionsbin=std::string(folder) + std::string("/Library/Preferences/smw.") + std::string(filename);
		#else
			std::string optionsbin=std::string(folder) + std::string("/.smw.") + std::string(filename);
		#endif
	#else
		std::string optionsbin = std::string(filename);
	#endif
#endif

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

	fwrite(&out, sizeof(Uint32), 1, outFile);
}

int ReadInt(FILE * inFile)
{
	int in;
	fread(&in, sizeof(Uint32), 1, inFile);

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
	for(unsigned int i = 0; i < iQuantity; i++)
	{
		fread(&mem[i], sizeof(Uint32), 1, inFile);

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
	fread(mem, sizeof(Uint32), iQuantity, inFile);
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

	fwrite(&out, sizeof(float), 1, outFile);
}

float ReadFloat(FILE * inFile)
{
	float in;
	fread(&in, sizeof(float), 1, inFile);

#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
	float t = in;

    ((char*)&in)[0] = ((char*)&t)[3];
    ((char*)&in)[1] = ((char*)&t)[2];
    ((char*)&in)[2] = ((char*)&t)[1];
    ((char*)&in)[3] = ((char*)&t)[0];
#endif

	return in;
}

void WriteString(char * szString, FILE * outFile)
{
	int iLen = strlen(szString) + 1;
	WriteInt(iLen, outFile);
	fwrite(szString, sizeof(Uint8), iLen, outFile);
}

void ReadString(char * szString, short size, FILE * inFile)
{
	int iLen = ReadInt(inFile);

	char * szReadString = new char[iLen];
	fread(szReadString, sizeof(Uint8), iLen, inFile);
	szReadString[iLen - 1] = 0;

	//Prevent buffer overflow  5253784 5253928
	strncpy(szString, szReadString, size - 1);
	szString[size - 1] = 0;

	delete [] szReadString;
}


void WriteByteFromShort(short out, FILE * outFile)
{
	char b = (char)out;
	fwrite(&b, sizeof(Uint8), 1, outFile);
}

short ReadByteAsShort(FILE * inFile)
{
	char b;
	fread(&b, sizeof(Uint8), 1, inFile);

	return (short)b;
}

void WriteBool(bool out, FILE * outFile)
{
	fwrite(&out, sizeof(Uint8), 1, outFile);
}

bool ReadBool(FILE * inFile)
{
	bool b;
	fread(&b, sizeof(Uint8), 1, inFile);

	return b;
}

void WriteByte(Uint8 out, FILE * outFile)
{
	fwrite(&out, sizeof(Uint8), 1, outFile);
}

Uint8 ReadByte(FILE * inFile)
{
	Uint8 b;
	fread(&b, sizeof(Uint8), 1, inFile);

	return b;
}
