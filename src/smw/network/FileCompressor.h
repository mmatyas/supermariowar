#ifndef SMW_NET_COMPRESSOR_H
#define SMW_NET_COMPRESSOR_H

#include <string>
#include <stdint.h>

struct CompressedData {
	uint8_t* data;
	size_t size;

	CompressedData(uint8_t*, size_t);
	~CompressedData();

	bool is_valid();
};

class FileCompressor {
public:
    static CompressedData compress(const std::string& input_path, const size_t output_header_offset = 0);
    static bool decompress(const uint8_t* input_buffer, const std::string& output_path);
};

#endif // SMW_NET_COMPRESSOR_H
