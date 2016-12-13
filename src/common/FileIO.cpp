#include "FileIO.h"

#include "SDL_endian.h"

#include <cassert>
#include <cstring>
#include <stdexcept>
#include <vector>

BinaryFile::BinaryFile(const char* filename, const char* options)
    : fp(NULL)
{
    fp = fopen(filename, options);
}

BinaryFile::~BinaryFile()
{
    if (fp)
        fclose(fp);
}

void BinaryFile::fread_or_exception(void* ptr, size_t size, size_t count)
{
    if (fread(ptr, size, count, fp) != count)
        throw std::runtime_error("File read error");
}

void BinaryFile::fwrite_or_exception(const void* ptr, size_t size, size_t count)
{
    if (fwrite(ptr, size, count, fp) != count)
        throw std::runtime_error("File write error");
}

bool BinaryFile::is_open()
{
    return fp;
}

void BinaryFile::rewind()
{
    if (fp)
        ::rewind(fp);
}

void BinaryFile::write_i8(int8_t value)
{
    fwrite_or_exception(&value, sizeof(int8_t), 1);
}

void BinaryFile::write_u8(uint8_t value)
{
    fwrite_or_exception(&value, sizeof(uint8_t), 1);
}

void BinaryFile::write_i16(int16_t value)
{
#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
    int16_t tmp = value;

    ((char*)&value)[0] = ((char*)&tmp)[1];
    ((char*)&value)[1] = ((char*)&tmp)[0];
#endif

    fwrite_or_exception(&value, sizeof(int16_t), 1);
}

void BinaryFile::write_i32(int32_t value)
{
#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
    int32_t tmp = value;

    ((char*)&value)[0] = ((char*)&tmp)[3];
    ((char*)&value)[1] = ((char*)&tmp)[2];
    ((char*)&value)[2] = ((char*)&tmp)[1];
    ((char*)&value)[3] = ((char*)&tmp)[0];
#endif

    fwrite_or_exception(&value, sizeof(int32_t), 1);
}

void BinaryFile::write_bool(bool value)
{
    write_u8(value);
}

void BinaryFile::write_float(float value)
{
#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
    float tmp = value;

    ((char*)&value)[0] = ((char*)&tmp)[3];
    ((char*)&value)[1] = ((char*)&tmp)[2];
    ((char*)&value)[2] = ((char*)&tmp)[1];
    ((char*)&value)[3] = ((char*)&tmp)[0];
#endif

    fwrite_or_exception(&value, sizeof(float), 1);
}

void BinaryFile::write_string(const char* string)
{
    assert(string);
    assert(strlen(string) < 254);

    int len = strlen(string) + 1;
    if (len > 255) {
        len = 255;
    }

    write_u8(len);
    fwrite_or_exception(string, sizeof(char), len);
}

void BinaryFile::write_string_long(const char* string)
{
    assert(string);
    assert(strlen(string) < 254);

    int len = strlen(string) + 1;
    if (len > 255) {
        len = 255;
    }

    write_i32(len);
    fwrite_or_exception(string, sizeof(char), len);
}

void BinaryFile::write_raw(const void* source, size_t size)
{
    fwrite_or_exception(source, size, 1);
}

uint8_t BinaryFile::read_u8()
{
    uint8_t b;
    fread_or_exception(&b, sizeof(uint8_t), 1);
    return b;
}

int8_t BinaryFile::read_i8()
{
    int8_t b;
    fread_or_exception(&b, sizeof(int8_t), 1);
    return b;
}

bool BinaryFile::read_bool()
{
    return read_u8();
}

int16_t BinaryFile::read_i16()
{
    int16_t in;
    fread_or_exception(&in, sizeof(int16_t), 1);

#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
    int16_t tmp = in;

    ((char*)&in)[0] = ((char*)&tmp)[1];
    ((char*)&in)[1] = ((char*)&tmp)[0];
#endif

    return in;
}

void BinaryFile::read_i16_array(int16_t* target, size_t size)
{
    assert(target);
    assert(size > 0);

#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
    for (unsigned int i = 0; i < size; i++) {
        fread_or_exception(&target[i], sizeof(int16_t), 1);

        int16_t tmp = target[i];

        ((char*)&target[i])[0] = ((char*)&tmp)[1];
        ((char*)&target[i])[1] = ((char*)&tmp)[0];
    }
#else
    fread_or_exception(target, sizeof(int16_t), size);
#endif
}

int32_t BinaryFile::read_i32()
{
    int32_t in;
    fread_or_exception(&in, sizeof(int32_t), 1);

#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
    int32_t tmp = in;

    ((char*)&in)[0] = ((char*)&tmp)[3];
    ((char*)&in)[1] = ((char*)&tmp)[2];
    ((char*)&in)[2] = ((char*)&tmp)[1];
    ((char*)&in)[3] = ((char*)&tmp)[0];
#endif

    return in;
}

void BinaryFile::read_i32_array(int32_t* target, size_t size)
{
    assert(target);
    assert(size > 0);

#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
    for (unsigned int i = 0; i < size; i++) {
        fread_or_exception(&target[i], sizeof(int32_t), 1);

        int32_t tmp = target[i];

        ((char*)&target[i])[0] = ((char*)&tmp)[3];
        ((char*)&target[i])[1] = ((char*)&tmp)[2];
        ((char*)&target[i])[2] = ((char*)&tmp)[1];
        ((char*)&target[i])[3] = ((char*)&tmp)[0];
    }
#else
    fread_or_exception(target, sizeof(int32_t), size);
#endif
}

float BinaryFile::read_float()
{
    float in;
    fread_or_exception(&in, sizeof(float), 1);

#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
    float tmp = in;

    ((char*)&in)[0] = ((char*)&tmp)[3];
    ((char*)&in)[1] = ((char*)&tmp)[2];
    ((char*)&in)[2] = ((char*)&tmp)[1];
    ((char*)&in)[3] = ((char*)&tmp)[0];
#endif

    return in;
}

void BinaryFile::read_string(char* target, size_t size)
{
    assert(target);
    assert(size > 0);

    const uint8_t len = read_u8();
    if (len <= 0) {
        target[0] = '\0';
        return;
    }

    std::vector<char> string(len, '\0');

    fread_or_exception(string.data(), sizeof(char), len);
    string[len - 1] = '\0';

    // if len < N, fills the rest with 0
    // if len > N, copies the first N characters
    strncpy(target, string.data(), size - 1);
    target[size - 1] = 0;
}

// This it a variant of read_string, which uses
// 32 bits to store the length of the string.
void BinaryFile::read_string_long(char* target, size_t size)
{
    assert(target);
    assert(size > 0);

    const int len = read_i32();
    if (len <= 0) {
        target[0] = '\0';
        return;
    }

    std::vector<char> string(len, '\0');

    fread_or_exception(string.data(), sizeof(char), len);
    string[len - 1] = '\0';

    // if len < N, fills the rest with 0
    // if len > N, copies the first N characters
    strncpy(target, string.data(), size - 1);
    target[size - 1] = 0;
}

void BinaryFile::read_raw(void* target, size_t size)
{
    fread_or_exception(target, size, 1);
}
