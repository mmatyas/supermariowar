#ifndef SMW_NET_COMPRESSOR_H
#define SMW_NET_COMPRESSOR_H

#include <string>

class FileCompressor {
public:
    static bool compress(const std::string& input_path, unsigned char*& output_buffer,
                         size_t& compressed_size, const size_t output_header_offset = 0);
    static bool decompress(const unsigned char* input_buffer, const std::string& output_path);
};

#endif // SMW_NET_COMPRESSOR_H
