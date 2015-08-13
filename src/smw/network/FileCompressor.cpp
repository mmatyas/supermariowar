#include "network/FileCompressor.h"

#include "lz4.h"

#include <cassert>
#include <cstdio>
#include <cstring>

#define COMPRESSION_SIZE_LIMIT 20000

/*
    This compresses the file <input_path>
    to a self-allocated space pointed by <output_buffer>,
    and puts the compressed size into <compressed_size>.
    The size of the buffer will be <output_header_offset> + 4 + the compressed size.
*/

bool FileCompressor::compress(const std::string& input_path, unsigned char*& output_buffer,
                              size_t& compressed_size, const size_t output_header_offset)
{
    assert(output_buffer == NULL);
    assert(input_path.length() >= 5);
    assert(sizeof(unsigned short) == 2);

    if (input_path.length() < 5)
        return false;

    FILE* input_file = fopen(input_path.c_str(), "rb");
    if (!input_file) {
        printf("[error] File %s not found", input_path.c_str());
        return false;
    }

    char* input_buffer = NULL;

    try {
        // Get uncompressed file size

        fseek(input_file, 0, SEEK_END);
        long input_size = ftell(input_file);
        fseek(input_file, 0, SEEK_SET);

        if (input_size > COMPRESSION_SIZE_LIMIT || input_size < 10) { // max map size
            printf("[error] File %s has irregular size\n", input_path.c_str());
            throw std::exception();
        }

        // Read uncompressed data to buffer

        char* input_buffer = (char*) malloc(input_size);
        if (!input_buffer) {
            printf("[error] Out of memory\n");
            throw std::exception();
        }

        size_t frret = fread(input_buffer, 1, input_size, input_file);
        if (input_size != frret) {
            printf("[error] File reading error (%s) %ld != %lu\n", input_path.c_str(), input_size, frret);
            throw std::exception();
        }

        fclose(input_file);
        input_file = NULL;

        // Compress

        int max_output_size = LZ4_compressBound(input_size);
        if (max_output_size <= 0) {
            printf("[error] File %s is too big\n", input_path.c_str());
            throw std::exception();
        }

        // package headers + uncompressed size (2B) + compressed size (2B) + data
        output_buffer = (unsigned char*) malloc(output_header_offset + 4 + max_output_size);
        if (!output_buffer) {
            printf("[error] Out of memory\n");
            throw std::exception();
        }

        int return_value = LZ4_compress_default(input_buffer, (char*)(output_buffer + output_header_offset + 4), input_size, max_output_size);
        if (return_value <= 0)
            throw std::exception();

        compressed_size = return_value;

        unsigned short stored_full_size = input_size;
        unsigned short stored_compressed_size = compressed_size;
        memcpy(output_buffer + output_header_offset, &stored_full_size, 2);
        memcpy(output_buffer + output_header_offset + 2, &stored_compressed_size, 2);
    }
    catch (std::exception&) {
        if (input_buffer) free(input_buffer);
        if (input_file) fclose(input_file);
        printf("[error] Compression failed\n");
        return false;
    }

    if (input_buffer) free(input_buffer);
    if (input_file) fclose(input_file);
    return true;
}

/*
    This decompresses an <input_buffer>
    and writes the result into a file called <output_path>.
*/

bool FileCompressor::decompress(const unsigned char* input_buffer, const std::string& output_path)
{
    assert(input_buffer != NULL);
    assert(output_path.length() >= 5);
    assert(sizeof(unsigned short) == 2);

    FILE* output_file = fopen(output_path.c_str(), "wb");
    if (!output_file) {
        printf("[error] Could not create %s", output_path.c_str());
        return false;
    }

    char* output_buffer = NULL;

    try {
        // Read file sizes stored in first 4 bytes

        unsigned short stored_full_size = 0;
        unsigned short stored_compressed_size = 0;
        memcpy(&stored_full_size, input_buffer, 2);
        memcpy(&stored_compressed_size, input_buffer + 2, 2);

        if (stored_full_size > COMPRESSION_SIZE_LIMIT || stored_compressed_size > COMPRESSION_SIZE_LIMIT) {
            printf("[error] Irregular size stored, possibly corrupted?\n");
            throw std::exception();
        }

        output_buffer = (char*) malloc(stored_full_size);
        if (!output_buffer) {
            printf("[error] Out of memory\n");
            throw std::exception();
        }

        // Decompress

        int return_value = LZ4_decompress_safe((const char*) input_buffer + 4, output_buffer, stored_compressed_size, stored_full_size);
        if (return_value <= 0)
            throw std::exception();

        // TODO: is this check necessary?
        if (return_value != stored_full_size)
            throw std::exception();

        // Save

        // FIXME
        // FIXME   Do some security checking here!
        // FIXME

        if (stored_full_size != fwrite(output_buffer, 1, stored_full_size, output_file)) {
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
