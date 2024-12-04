#pragma once

#include "base/ring_buffer.h"
#include "ipc/synth_message.h"
#include "ipc/transport.h"
#include "ipc/transport_listener.h"
#include <atomic>
#include <map>
#include <sys/event.h>
#include <thread>

namespace ipc {
class CommandServer
{
public:
  static base::ErrorOr<CommandServer>
  create(const std::string &socket_path,
         base::RingBuffer<SynthMessage> &command_queue);

  ~CommandServer()
  {
    stop();
    if (m_kqueue_fd >= 0)
      close(m_kqueue_fd);

    std::remove(m_listener.path().c_str());
  }

  CommandServer(const CommandServer &) = delete;
  CommandServer &operator=(const CommandServer &) = delete;

  CommandServer(CommandServer &&other) noexcept
      : m_kqueue_fd(other.m_kqueue_fd), m_listener(std::move(other.m_listener)),
        m_command_queue(other.m_command_queue)
  {
    other.m_kqueue_fd = -1;
  };
  CommandServer &operator=(CommandServer &&) noexcept;

  void start() noexcept;
  void stop() noexcept;

private:
  CommandServer(int kqueue_fd, ipc::TransportListener &&listener,
                base::RingBuffer<ipc::SynthMessage> &command_queue)
      : m_kqueue_fd(kqueue_fd), m_listener(std::move(listener)),
        m_command_queue(command_queue) {};

  base::ErrorOr<void> register_socket_event(int fd, bool is_read) noexcept;
  void handle_events() noexcept;
  std::map<size_t, ipc::Transport> m_clients;

  base::RingBuffer<ipc::SynthMessage> &m_command_queue;

  std::string m_socket_path;
  ipc::TransportListener m_listener;
  int m_kqueue_fd;
  std::atomic<bool> m_running{false};

  std::thread m_event_thread;
};

} // namespace ipc