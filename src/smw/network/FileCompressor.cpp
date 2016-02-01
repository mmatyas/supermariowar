#include "network/FileCompressor.h"

#include "zlib.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define COMPRESSION_SIZE_LIMIT 20000

CompressedData::CompressedData(uint8_t* data, size_t size)
    : data(data)
    , size(size)
{}

CompressedData::~CompressedData() {
    if (data)
        free(data);

    size = 0;
}

bool CompressedData::is_valid() {
    if (!data || !size)
        return false;
    return true;
}

/*
    This compresses the file <input_path>
    to a self-allocated space pointed by <output_buffer>,
    and puts the compressed size into <compressed_size>.
    The size of the buffer will be <output_header_offset> + 4 + the compressed size.
*/

CompressedData FileCompressor::compress(const std::string& input_path, const size_t output_header_offset)
{
    FILE* input_file = NULL;
    uint8_t* input_buffer = NULL;
    uint8_t* compressed_buffer = NULL;

    try {
        // Open the file

        assert(input_path.length() >= 5);
        if (input_path.length() < 5)
            throw std::exception();

        input_file = fopen(input_path.c_str(), "rb");
        if (!input_file) {
            printf("[error] Could not open %s\n", input_path.c_str());
            throw std::exception();
        }

        // Get uncompressed file size

        fseek(input_file, 0, SEEK_END);
        long input_size = ftell(input_file);
        if (input_size < 0) {
            perror("[error] File read error");
            throw std::exception();
        }
        fseek(input_file, 0, SEEK_SET);

        if (input_size > COMPRESSION_SIZE_LIMIT || input_size < 10) { // max map size
            printf("[error] File %s has irregular size\n", input_path.c_str());
            throw std::exception();
        }

        // Read uncompressed data to buffer

        input_buffer = (uint8_t*) calloc(input_size, 1);
        if (!input_buffer) {
            printf("[error] Not enough memory for reading file %s\n", input_path.c_str());
            throw std::exception();
        }

        size_t frret = fread(input_buffer, 1, input_size, input_file);
        if (input_size != (long)frret) {
            printf("[error] File reading error (%s) %ld != %lu\n", input_path.c_str(), input_size, frret);
            throw std::exception();
        }

        fclose(input_file);
        input_file = NULL;

        // Compress

        // Zlib requires ulong, but the size of that depends on the platform
        unsigned long compressed_size_ulong = compressBound(input_size);
        if (compressed_size_ulong > COMPRESSION_SIZE_LIMIT) {
            printf("[error] File is too big: %s\n", input_path.c_str());
            throw std::exception();
        }

        // package headers + uncompressed size (2B) + compressed size (2B) + data
        compressed_buffer = (uint8_t*) calloc(output_header_offset + 4 + compressed_size_ulong, 1);
        if (!compressed_buffer) {
            printf("[error] Not enough memory to compress %s\n", input_path.c_str());
            throw std::exception();
        }

        int return_value = ::compress((uint8_t*)(compressed_buffer + output_header_offset + 4), &compressed_size_ulong, input_buffer, input_size);
        if (return_value != Z_OK) {
            printf("[error] Out of memory, could not compress %s\n", input_path.c_str());
            throw std::exception();
        }

        free(input_buffer);

        uint16_t stored_full_size = input_size;
        uint16_t stored_compressed_size = compressed_size_ulong;
        memcpy(compressed_buffer + output_header_offset, &stored_full_size, 2);
        memcpy(compressed_buffer + output_header_offset + 2, &stored_compressed_size, 2);

        return CompressedData(compressed_buffer, compressed_size_ulong + output_header_offset + 4);
    }
    catch (std::exception&) {
        printf("[error] Compression failed\n");
    }

    if (input_file) fclose(input_file);
    if (input_buffer) free(input_buffer);
    if (compressed_buffer) free(compressed_buffer);
    return CompressedData(NULL, 0);
}

/*
    This decompresses an <input_buffer>
    and writes the result into a file called <output_path>.
*/

bool FileCompressor::decompress(const uint8_t* input_buffer, const std::string& output_path)
{
    assert(input_buffer != NULL);
    assert(output_path.length() >= 5);

    FILE* output_file = fopen(output_path.c_str(), "wb");
    if (!output_file) {
        printf("[error] Could not create %s", output_path.c_str());
        return false;
    }

    uint8_t* output_buffer = NULL;

    try {
        // Read file sizes stored in first 4 bytes

        uint16_t stored_full_size = 0;
        uint16_t stored_compressed_size = 0;
        memcpy(&stored_full_size, input_buffer, 2);
        memcpy(&stored_compressed_size, input_buffer + 2, 2);

        if (stored_full_size > COMPRESSION_SIZE_LIMIT || stored_compressed_size > COMPRESSION_SIZE_LIMIT) {
            printf("[error] Irregular size stored, possibly corrupted?\n");
            throw std::exception();
        }

        // Zlib requires ulong, but the size of that depends on the platform
        unsigned long output_size_ulong = stored_full_size;
        output_buffer = (uint8_t*) malloc(stored_full_size);
        if (!output_buffer) {
            printf("[error] Out of memory\n");
            throw std::exception();
        }

        // Decompress

        int return_value = ::uncompress(output_buffer, &output_size_ulong, (const uint8_t*) input_buffer + 4, stored_compressed_size);
        if (return_value != Z_OK) {
            printf("[error] Out of memory, could not decompress\n");
            throw std::exception();
        }

        // Zlib note: after uncrompress(), output_size may decrease
        // But if we have set the header values correctly, that shouldn't happen to us
        assert(stored_full_size == output_size_ulong);

        // Save

        // FIXME
        // FIXME   Do some security checking here!
        // FIXME

        if (stored_full_size != fwrite(output_buffer, 1, output_size_ulong, output_file)) {
            printf("[error] File writing error (%s)\n", output_path.c_str());
            throw std::exception();
        }
    }
    catch (std::exception&) {
        if (output_buffer) free(output_buffer);
        if (output_file) fclose(output_file);
        printf("[error] Decompression failed\n");
        return false;
    }

    if (output_buffer) free(output_buffer);
    if (output_file) fclose(output_file);
    return true;
}
