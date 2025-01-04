#ifndef ECOMM_TIME_H
#define ECOMM_TIME_H

#include <chrono>

namespace ecomm {

inline std::chrono::steady_clock::time_point GetNowTimePoint() {
  return std::chrono::steady_clock::now();
}

inline std::chrono::steady_clock::time_point GetTimePointFrom(
    const std::chrono::steady_clock::time_point start_time,
    const std::chrono::milliseconds span) {
  return start_time + span;
}

inline std::chrono::steady_clock::time_point GetTimePointFromNow(
    const std::chrono::milliseconds span) {
  return GetTimePointFrom(GetNowTimePoint(), span);
}

inline std::chrono::milliseconds GetTimeSpan(
    const std::chrono::steady_clock::time_point before,
    const std::chrono::steady_clock::time_point after) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
}

}  // namespace ecomm

#endif  // ECOMM_TIME_H