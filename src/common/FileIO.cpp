#include "FileIO.h"

#include "SDL_endian.h"

#include <cassert>
#include <cstring>
#include <format>
#include <stdexcept>

BinaryFile::BinaryFile(const char* path, const char* options)
    : m_path(path)
{
    fp = fopen(path, options);
}

BinaryFile::BinaryFile(const std::string& path, const char* options)
    : BinaryFile(path.c_str(), options)
{}

BinaryFile::BinaryFile(const std::filesystem::path& path, const char* options)
    : BinaryFile(path.string().c_str(), options)
{}

BinaryFile::~BinaryFile()
{
    if (fp)
        fclose(fp);
}

void BinaryFile::fread_or_exception(void* ptr, size_t size, size_t count)
{
    long pos = ftell(fp);
    if (fread(ptr, size, count, fp) != count) {
        std::string msg = std::format(
            "File read error in {}\n"
            "Tried to read {} bytes at position {}, but failed\n"
            "The file might be damaged, or it's not in the expected format",
            m_path, size * count, pos);
        if (std::ferror(fp)) {
            msg += "\nSystem message: ";
            msg += std::strerror(errno);
        }
        throw std::runtime_error(std::move(msg));
    }
}

void BinaryFile::fwrite_or_exception(const void* ptr, size_t size, size_t count)
{
    long pos = ftell(fp);
    if (fwrite(ptr, size, count, fp) != count) {
        std::string msg = std::format(
            "File write error in {}\n"
            "Tried to write {} bytes at position {}, but failed",
            m_path, size * count, pos);
        if (std::ferror(fp)) {
            msg += "\nSystem message: ";
            msg += std::strerror(errno);
        }
        throw std::runtime_error(std::move(msg));
    }
}

void BinaryFile::rewind()
{
    if (fp)
        ::rewind(fp);
}

long BinaryFile::pos() const
{
    return fp ? ::ftell(fp) : 0;
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

// Writes an i32 that tells the byte length of the string data *including*
// a terminating null byte, then the text data itself, with a null terminating byte
void BinaryFile::write_string_long(const std::string& string)
{
    if (string.length() > 255) {
        std::string msg = std::format(
            "File write error in {}\n"
            "Tried to write a text that would take {} bytes, which is too long",
            m_path, string.length());
        throw std::runtime_error(std::move(msg));
    }

    // NOTE: `size()` doesn't include the terminating null byte
    write_i32(string.size() + 1);
    fwrite_or_exception(string.data(), sizeof(char), string.size() + 1);
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

// Uses 32 bits to store the length of the string, then the text data,
// including a terminating null byte
std::string BinaryFile::read_string_long(size_t maxlen)
{
    const int stored_len = read_i32();
    if (stored_len <= 0)
        return {};

    const size_t data_len = std::min<size_t>(stored_len, maxlen);
    if (data_len == 0)
        return {};

    std::string text(data_len, '\0');
    fread_or_exception(text.data(), sizeof(char), data_len);

    // NOTE: The stored text always includes a terminating null byte
    text.pop_back();

    return text;
}

void BinaryFile::read_raw(void* target, size_t size)
{
    fread_or_exception(target, size, 1);
}
