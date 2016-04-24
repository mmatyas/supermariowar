#include "Blob.h"

#include <string.h>

Blob::Blob(): data(0), size(0) {}

Blob::~Blob() {
	free();
}

void Blob::free() {
	delete data;
	data = nullptr;
	size = 0;
}

void Blob::replace_with(const void* src, size_t src_size) {
	free();
	data = new uint8_t[src_size];
	memcpy(data, src, src_size);
    size = src_size;
}

bool Blob::empty() const {
	return size == 0;
}

const uint8_t* Blob::get_data() const {
	return data;
}

size_t Blob::get_size() const {
	return size;
}
