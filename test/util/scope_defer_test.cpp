#include "util/scope_defer.h"

#include <gtest/gtest.h>

TEST (ScopedDeferTest, defer) {
  bool flag = false;
  {
    auto guard = ecomm::BuildScopeGuard([&flag] {
      flag = true;
    });
  }
  ASSERT_TRUE(flag);

  flag = false;
  {
    scope_defer {
      flag = true;
    };
  }
  ASSERT_TRUE(flag);

  flag = false;
  {
    scope_defer {
      flag = true;
    };
    scope_defer {
      flag = false;
    };
  }
  ASSERT_TRUE(flag);

  flag = false;
  {
    auto guard = ecomm::BuildScopeGuard([&flag] {
      flag = true;
    });
    guard.Cancel();
  }
  ASSERT_FALSE(flag);
}