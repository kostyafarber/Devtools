
#pragma once

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <vector>

namespace base {

template <typename T>
class RingBuffer
{
public:
  RingBuffer() = default;
  RingBuffer(size_t size) : m_size(size), m_samples(size){};
  ~RingBuffer() = default;

  // delete copy constructors
  RingBuffer(const RingBuffer &) = delete;
  const RingBuffer &operator=(const RingBuffer &) = delete;

  RingBuffer(RingBuffer &&) = default;
  RingBuffer &operator=(RingBuffer &&) = default;

  bool read(T *, size_t n = 1) noexcept;
  bool write(T *, size_t n = 1) noexcept;

private:
  std::vector<T> m_samples{};
  std::atomic<size_t> m_write_pos = 0;
  std::atomic<size_t> m_read_pos = 0;

  size_t m_size;
  std::atomic<size_t> m_current_size{0};

  bool is_empty() const noexcept { return m_current_size == 0; }
  bool is_full() const noexcept { return m_size == m_current_size; }

  bool available_space(size_t n) const noexcept
  {
    return m_current_size + n <= m_size;
  }
  bool available_samples(size_t n) const noexcept
  {
    return m_current_size >= n;
  }

  bool wait();
};

template <typename T>
bool RingBuffer<T>::read(T *out_buffer, size_t n) noexcept
{

  if (is_empty() || (!available_samples(n)))
    return false;

  auto read_pos = m_read_pos.load(std::memory_order_acquire);

  if (read_pos + n > m_size) {
    auto first_chunk = m_size - read_pos;
    std::copy_n(&m_samples[read_pos], first_chunk, out_buffer);

    auto second_chunk = n - first_chunk;
    std::copy_n(&m_samples[0], second_chunk, out_buffer + first_chunk);
  } else {
    std::copy_n(&m_samples[read_pos], n, out_buffer);
  }

  m_read_pos.store((m_read_pos + n) % m_size, std::memory_order_release);
  m_current_size -= n;
  return true;
}

template <typename T>
bool RingBuffer<T>::write(T *out_buffer, size_t n) noexcept
{

  auto write_pos = m_write_pos.load(std::memory_order_acquire);

  if (write_pos + n > m_size) {
    auto first_chunk = m_size - write_pos;
    std::copy_n(out_buffer, first_chunk, &m_samples[write_pos]);

    auto second_chunk = n - first_chunk;
    std::copy_n(out_buffer + first_chunk, second_chunk, &m_samples[0]);
  } else {
    std::copy_n(out_buffer, n, &m_samples[m_write_pos]);
  }
  m_write_pos.store((m_write_pos + n) % m_size, std::memory_order_release);
  m_current_size += n;

  return true;
}

} // namespace ru
