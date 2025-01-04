#include "util/time.h"

#include <gtest/gtest.h>

TEST (Time, time_point) {
  const auto time_point_1 = ecomm::GetNowTimePoint();
  const auto time_point_2 = ecomm::GetTimePointFrom(time_point_1, std::chrono::milliseconds(5));
  ASSERT_TRUE(time_point_2 > time_point_1);
  const auto span = ecomm::GetTimeSpan(time_point_1, time_point_2);
  ASSERT_TRUE(span.count() == 5);
}

