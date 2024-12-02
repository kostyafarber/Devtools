#pragma once

#include "base/ring_buffer.h"
#include "ipc/messages/synth_message.h"
#include "ipc/unix_socket.h"
#include <atomic>
#include <sys/event.h>
#include <thread>

namespace ipc {
class SocketServer
{
public:
  static base::ErrorOr<SocketServer>
  create(const std::string &socket_path,
         base::RingBuffer<SynthMessage> &command_queue);

  ~SocketServer()
  {
    stop();
    if (m_kqueue_fd >= 0)
      close(m_kqueue_fd);

    std::remove(m_listening_socket.socket_path().c_str());
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
               base::RingBuffer<ipc::SynthMessage> &command_queue)
      : m_kqueue_fd(kqueue_fd), m_listening_socket(std::move(socket)),
        m_command_queue(command_queue) {};

  void handle_events() noexcept;
  std::map<size_t, ipc::UnixSocket> m_clients;

  base::RingBuffer<ipc::SynthMessage> &m_command_queue;

  ipc::UnixSocket m_listening_socket;
  int m_kqueue_fd;
  std::atomic<bool> m_running{false};

  std::thread m_event_thread;
};

} // namespace ipc