#pragma once

#include "base/logging.h"
#include "ipc/socket.h"
#include "ru/ring_buffer.h"
#include <map>
#include <sys/event.h>

namespace ipc {
class SocketServer
{
public:
  static base::ErrorOr<SocketServer> create(const std::string &socket_path);

  ~SocketServer()
  {
    if (m_kqueue_fd >= 0)
      close(m_kqueue_fd);
  }

  SocketServer(const SocketServer &) = delete;
  SocketServer &operator=(const SocketServer &) = delete;

  SocketServer(SocketServer &&other) noexcept
      : m_kqueue_fd(other.m_kqueue_fd),
        m_listening_socket(std::move(other.m_listening_socket))
  {
    other.m_kqueue_fd = -1;
  };
  SocketServer &operator=(SocketServer &&) noexcept;

  base::ErrorOr<void> register_socket_event(int fd, bool is_read) noexcept;
  void handle_events() noexcept;
  void stop() noexcept { m_running = false; }

private:
  SocketServer(int kqueue_fd, ipc::UnixSocket &&socket)
      : m_kqueue_fd(kqueue_fd), m_listening_socket(std::move(socket)){};

  std::map<size_t, ipc::UnixSocket> m_clients;

  ru::RingBuffer<ipc::SynthMessage> m_command_queue;

  ipc::UnixSocket m_listening_socket;
  int m_kqueue_fd;
  bool m_running{false};
};

} // namespace ipc