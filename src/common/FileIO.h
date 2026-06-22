#pragma once

#include <stdio.h>
#include <stdint.h>
#include <string>
#include <filesystem>


class BinaryFile {
public:
    BinaryFile(const char* path, const char* options);
    BinaryFile(const std::string& path, const char* options);
    BinaryFile(const std::filesystem::path& path, const char* options);
    ~BinaryFile();

    bool is_open() const { return fp; };
    void rewind();
    long pos() const;

    void write_i8(int8_t);
    void write_u8(uint8_t);
    void write_i16(int16_t);
    void write_i32(int32_t);
    void write_bool(bool);
    void write_float(float);
    void write_string_long(const std::string&);
    void write_raw(const void*, size_t);

    int8_t read_i8();
    uint8_t read_u8();
    int16_t read_i16();
    int32_t read_i32();
    bool read_bool();
    float read_float();
    void read_i16_array(int16_t*, size_t);
    void read_i32_array(int32_t*, size_t);
    std::string read_string_long(size_t);
    void read_raw(void*, size_t);

private:
    const char* m_path = nullptr;
    FILE* fp = nullptr;

    void fread_or_exception(void*, size_t, size_t);
    void fwrite_or_exception(const void*, size_t, size_t);
};
