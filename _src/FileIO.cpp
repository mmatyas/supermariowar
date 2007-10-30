#include "global.h"

void WriteInt(int out, FILE * outFile)
{
#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
	int t = out;

    ((char*)&out)[0] = ((char*)&t)[3];
    ((char*)&out)[1] = ((char*)&t)[2];
    ((char*)&out)[2] = ((char*)&t)[1];
    ((char*)&out)[3] = ((char*)&t)[0];
#endif

	fwrite(&out, sizeof(int), 1, outFile);
}

int ReadInt(FILE * inFile)
{
	int in;
	fread(&in, sizeof(int), 1, inFile);

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
		fread(&mem[i], sizeof(int), 1, inFile);

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
	fread(mem, sizeof(int), iQuantity, inFile);
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
	fwrite(szString, sizeof(char), iLen, outFile);
}

void ReadString(char * szString, short size, FILE * outFile)
{
	int iLen = ReadInt(outFile);

	char * szReadString = new char[iLen];
	fread(szReadString, sizeof(char), iLen, outFile);
	szReadString[iLen - 1] = 0;

	//Prevent buffer overflow  5253784 5253928
	strncpy(szString, szReadString, size - 1);
	szString[size - 1] = 0;

	delete [] szReadString;
}

