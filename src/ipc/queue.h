#include <cstdint>
namespace ipc {
template <typename Message>
class Queue
{
  Queue() {};
  ~Queue() {};

  Queue(const Queue &) = delete;
  const Queue &operator=(const Queue &) = delete;

  // want to delete copy
  // not sure when we'd ever want to move this thing?

  void try_send(Message m) noexcept;
  void try_receive(Message &m) noexcept;

private:
  uint32_t m_sequence{0};
};
} // namespace ipc