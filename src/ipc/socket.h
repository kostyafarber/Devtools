#pragma once

#include "base/error.h"
#include "base/logging.h"
#include "ipc/messages/message_header.h"
#include "messages.pb.h"
#include <cstdio>
#include <sys/socket.h>
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
  template <typename T>
  bool send_exactly(T *msg, size_t size)
  {
    size_t total_bytes_sent = 0;
    auto buffer = reinterpret_cast<uint8_t *>(msg);

    while (total_bytes_sent < size) {
      ssize_t bytes_sent = ::send(m_socket_fd, buffer + total_bytes_sent,
                                  size - total_bytes_sent, MSG_NOSIGNAL);

      if (bytes_sent == -1) {
        LOG_AUDIO(Error, "error trying to send bytes");
        return false;
      }
    }
    return size == total_bytes_sent;
  }

  template <typename T>
  bool recv_exactly(T *msg, size_t size)
  {

    size_t total_bytes_read = 0;
    auto buffer = reinterpret_cast<uint8_t *>(msg);

    while (total_bytes_read < size) {
      ssize_t bytes_read = ::recv(m_socket_fd, buffer + total_bytes_read,
                                  size - total_bytes_read, MSG_DONTWAIT);

      if (bytes_read == -1) {
        LOG_AUDIO(Error, "error trying to read bytes");
        return false;
      }

      total_bytes_read = total_bytes_read + bytes_read;
    }

    return size == total_bytes_read;
  }

  std::string m_path;
  UnixSocket(int fd, std::string path) : m_socket_fd(fd), m_path(path) {};
  int m_socket_fd{-1};

  bool m_running{false};
};
} // namespace ipc