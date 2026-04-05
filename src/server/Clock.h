#ifndef CLOCK_DEFINITIONS_H
#define CLOCK_DEFINITIONS_H

#include <chrono>

typedef std::chrono::steady_clock::time_point TimePoint;
inline TimePoint time_now() noexcept { return std::chrono::steady_clock::now(); }

#endif // CLOCK_DEFINITIONS_H
