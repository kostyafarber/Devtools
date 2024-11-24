#include "ru/ring_buffer.h"
#include <cstdint>
namespace ipc {
template <typename Message>
class Queue
{
  Queue() {};
  ~Queue() {};

  Queue(const Queue &) = delete;
  const Queue &operator=(const Queue &) = delete;

  void try_send(Message m) noexcept;
  void try_receive(Message &m) noexcept;

private:
  size_t m_size;
  ru::RingBuffer<Message> m_buffer;
  uint32_t m_sequence{0};
};
} // namespace ipc