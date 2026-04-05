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

template<typename Container, typename Pred>
auto find_if(Container&& list, Pred&& pred) {
    return std::find_if(list.cbegin(), list.cend(), pred);
}
} // namespace utils
