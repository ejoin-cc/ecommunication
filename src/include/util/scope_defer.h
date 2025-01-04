#ifndef ECOMM_SCOPE_DEFER_H
#define ECOMM_SCOPE_DEFER_H

#include <utility>

namespace ecomm {

template <typename T>
class ScopeGuard;

template <typename T>
ScopeGuard<T> BuildScopeGuard(T &&callback) noexcept;

struct ScopeGuardHelper {};

template <typename T>
class ScopeGuard {
public:
  friend ScopeGuard BuildScopeGuard<T>(T &&callback) noexcept;

  ~ScopeGuard() noexcept {
    if (!cancel_) {
      callback_();
    }
  }

  void Cancel() noexcept { cancel_ = true; }

  ScopeGuard() = delete;
  ScopeGuard(const ScopeGuard &) = delete;
  ScopeGuard &operator=(const ScopeGuard &) = delete;
  ScopeGuard &operator=(ScopeGuard &&) = delete;

private:
  explicit ScopeGuard(T &&callback) noexcept
      : callback_(callback), cancel_(false) {}

  T callback_;
  bool cancel_;
};

template <typename T>
ScopeGuard<T> BuildScopeGuard(T &&callback) noexcept {
  return ScopeGuard<T>(std::forward<T>(callback));
}

template <typename T>
ScopeGuard<T> operator+(ScopeGuardHelper, T &&callback) {
  return BuildScopeGuard(std::forward<T>(callback));
}

#define INNER_SCOPE_DEFER(LINE) defer_object_##LINE
#define SCOPE_DEFER(LINE) INNER_SCOPE_DEFER(LINE)
#define scope_defer \
  auto SCOPE_DEFER(__LINE__) = ecomm::ScopeGuardHelper() + [&]() noexcept

}  // namespace ecomm

#endif  // ECOMM_SCOPE_DEFER_H