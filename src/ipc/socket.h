#pragma once

#include "base/error.h"
#include "ipc/messages/message_header.h"
#include "messages.pb.h"
#include <unistd.h>

namespace ipc {

class UnixSocket
{
public:
  static base::ErrorOr<UnixSocket> create(const std::string &path);

  ~UnixSocket()
  {
    if (m_socket_fd >= 0)
      close(m_socket_fd);
  }

  UnixSocket(const UnixSocket &) = delete;
  const UnixSocket &operator=(const UnixSocket &) = delete;

  UnixSocket(UnixSocket &&other)
      : m_socket_fd(other.m_socket_fd), m_path(std::move(other.m_path)),
        m_running(other.m_running)
  {
    other.m_socket_fd = -1;
    other.m_running = false;
  }
  UnixSocket &operator=(UnixSocket &&) noexcept;

  bool try_send(ipc::MessageHeader &header,
                const synth::SynthMessage &message) noexcept;
  bool try_recv(ipc::MessageHeader &header,
                synth::SynthMessage &message) noexcept;

  int fd() const { return m_socket_fd; }
  std::string socket_path() const { return m_path; }

  base::ErrorOr<void> connect();
  base::ErrorOr<void> listen();
  base::ErrorOr<UnixSocket> accept();

private:
  bool send_exactly(synth::SynthMessage &msg, size_t);
  bool recv_exactly(synth::SynthMessage &msg, size_t);

  std::string m_path;
  UnixSocket(int fd, std::string path) : m_socket_fd(fd), m_path(path) {};
  int m_socket_fd{-1};

  bool m_running{false};
};
} // namespace ipc