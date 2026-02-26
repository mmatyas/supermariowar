#pragma once

#include <algorithm>


template<typename Container, typename T>
bool contains(Container&& list, T&& item) {
    return std::find(list.cbegin(), list.cend(), item) != list.cend();
}
