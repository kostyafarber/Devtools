
#include "base/error.h"
#include <vector>

namespace ru
{

class RingBuffer
{
public:
  RingBuffer(size_t size) : m_size(size) {};
  ~RingBuffer() = default;

  // delete copy constructors
  RingBuffer(RingBuffer &) = delete;
  RingBuffer &operator=(RingBuffer &) = delete;

  base::ErrorOr<float> read(size_t pos);
  base::ErrorOr<float> write(size_t pos);

private:
  std::vector<float> m_samples{};
  size_t m_size;

  bool wait();
  bool is_empty();
};
} // namespace ru
