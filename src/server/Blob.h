#ifndef BLOB_H
#define BLOB_H

#include <stddef.h>
#include <stdint.h>

class Blob {
public:
    Blob();
    virtual ~Blob();

    bool empty() const;
    void replace_with(const void*, size_t);
    void free();

    const uint8_t* get_data() const;
    size_t get_size() const;

protected:
    uint8_t* data;
    size_t size;
};

#endif // BLOB_H
