#ifndef __FILEIO_H_
#define __FILEIO_H_

#include <stdio.h>

void WriteInt(int out, FILE * outFile);
int ReadInt(FILE * inFile);
void ReadIntChunk(int * mem, size_t iQuantity, FILE * inFile);
void WriteFloat(float out, FILE * outFile);
float ReadFloat(FILE * inFile);
void WriteString(char * szString, FILE * outFile);
void ReadString(char * szString, short size, FILE * outFile);

#endif //__FILEIO_H_

