#include <algorithm>

namespace base {
template <typename T>
struct Clamped {
  T m_min;
  T m_max;

  Clamped(T min, T max) : m_min(min), m_max(max) {};

  T value(T value) { return std::clamp(value, m_min, m_max); }
};

} // namespace base