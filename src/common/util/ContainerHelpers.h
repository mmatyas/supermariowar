#pragma once

#include <algorithm>


namespace utils {
template<typename Container, typename T>
bool contains(Container&& list, T&& item) {
    return std::find(list.cbegin(), list.cend(), item) != list.cend();
}

template<typename Container, typename Compare>
void sort(Container&& list, Compare&& compare) {
    std::sort(list.begin(), list.end(), compare);
}
} // namespace utils
