#pragma once

#include <cassert>
#include <cstddef>
#include <vector>


template<typename T>
class Grid {
private:
    size_t width = 0;
    size_t height = 0;
    std::vector<T> data;

public:
    Grid(size_t w, size_t h)
        : width(w)
        , height(h)
        , data(w * h)
    {}
    Grid() : Grid(0, 0) {};

    size_t cols() const { return width; }
    size_t rows() const { return height; }
    size_t empty() const { return data.empty(); }
    size_t size() const { return data.size(); }

    void clear() {
        data.clear();
        height = 0;
        width = 0;
    }

    void fill(const T& val) {
        data = decltype(data)(width * height, val);
    }

    void swap(Grid<T>& other) noexcept {
        std::swap(width, other.width);
        std::swap(height, other.height);
        data.swap(other.data);
    }

    T& at(size_t x, size_t y) {
        assert(y * width + x < data.size());
        return data[y * width + x];
    }
    const T& at(size_t x, size_t y) const {
        assert(y * width + x < data.size());
        return data[y * width + x];
    }

    auto begin() noexcept { return data.begin(); }
    auto begin() const noexcept { return data.begin(); }
    auto cbegin() const noexcept { return data.cbegin(); }

    auto end() noexcept { return data.end(); }
    auto end() const noexcept { return data.end(); }
    auto cend() const noexcept { return data.cend(); }
};
