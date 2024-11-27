#pragma once

#include "base/logging.h"
#include "ipc/socket.h"
#include "ru/ring_buffer.h"
#include <map>
#include <sys/event.h>
#include <thread>

namespace ipc {
class SocketServer
{
public:
  static base::ErrorOr<SocketServer>
  create(const std::string &socket_path,
         ru::RingBuffer<SynthMessage> &command_queue);

  ~SocketServer()
  {
    if (m_kqueue_fd >= 0)
      close(m_kqueue_fd);

    stop();
  }

  SocketServer(const SocketServer &) = delete;
  SocketServer &operator=(const SocketServer &) = delete;

  SocketServer(SocketServer &&other) noexcept
      : m_kqueue_fd(other.m_kqueue_fd),
        m_listening_socket(std::move(other.m_listening_socket)),
        m_command_queue(other.m_command_queue)
  {
    other.m_kqueue_fd = -1;
  };
  SocketServer &operator=(SocketServer &&) noexcept;

  base::ErrorOr<void> register_socket_event(int fd, bool is_read) noexcept;
  void start() noexcept;
  void stop() noexcept;

private:
  SocketServer(int kqueue_fd, ipc::UnixSocket &&socket,
               ru::RingBuffer<SynthMessage> &command_queue)
      : m_kqueue_fd(kqueue_fd), m_listening_socket(std::move(socket)),
        m_command_queue(command_queue) {};

  void handle_events() noexcept;
  std::map<size_t, ipc::UnixSocket> m_clients;

  ru::RingBuffer<ipc::SynthMessage> &m_command_queue;

  ipc::UnixSocket m_listening_socket;
  int m_kqueue_fd;
  std::atomic<bool> m_running{false};

  std::thread m_event_thread;
};

} // namespace ipc