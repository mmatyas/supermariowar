#ifndef FILEIO_H
#define FILEIO_H

#include <stdio.h>
#include <stdint.h>

class BinaryFile {
public:
    BinaryFile(const char* filename, const char* options);
    ~BinaryFile();

    bool is_open();
    void rewind();

    void write_i8(int8_t);
    void write_u8(uint8_t);
    void write_i16(int16_t);
    void write_i32(int32_t);
    void write_bool(bool);
    void write_float(float);
    void write_string(const char*);
    void write_string_long(const char*);
    void write_raw(const void*, size_t);

    int8_t read_i8();
    uint8_t read_u8();
    int16_t read_i16();
    int32_t read_i32();
    bool read_bool();
    float read_float();
    void read_i16_array(int16_t*, size_t);
    void read_i32_array(int32_t*, size_t);
    void read_string(char*, size_t);
    void read_string_long(char*, size_t);
    void read_raw(void*, size_t);

private:
    FILE* fp;

    void fread_or_exception(void*, size_t, size_t);
    void fwrite_or_exception(const void*, size_t, size_t);
};

#endif // FILEIO_H
