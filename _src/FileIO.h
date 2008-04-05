#ifndef __FILEIO_H_
#define __FILEIO_H_

#include <stdio.h>

FILE * OpenFile(const char * filename, const char * options);
void WriteInt(int out, FILE * outFile);
int ReadInt(FILE * inFile);
void ReadIntChunk(int * mem, size_t iQuantity, FILE * inFile);
void WriteFloat(float out, FILE * outFile);
float ReadFloat(FILE * inFile);
void WriteString(char * szString, FILE * outFile);
void ReadString(char * szString, short size, FILE * outFile);
void WriteByteFromShort(short out, FILE * outFile);
short ReadByteAsShort(FILE * inFile);
void WriteBool(bool out, FILE * outFile);
bool ReadBool(FILE * inFile);
void WriteByte(Uint8 out, FILE * outFile);
Uint8 ReadByte(FILE * inFile);

#endif //__FILEIO_H_

