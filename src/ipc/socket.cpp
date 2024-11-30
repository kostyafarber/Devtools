#include "socket.h"
#include "ipc/messages/message_header.h"
#include "messages.pb.h"
#include <cstring>
#include <filesystem>
#include <sys/_types/_ssize_t.h>
#include <sys/socket.h>
#include <sys/un.h>

namespace ipc {

UnixSocket &UnixSocket::operator=(UnixSocket &&other) noexcept
{
  if (this == &other)
    return *this;

  if (m_socket_fd >= 0)
    close(m_socket_fd);

  m_socket_fd = other.m_socket_fd;
  m_path = std::move(other.m_path);
  m_running = other.m_running;

  other.m_socket_fd = -1;
  other.m_running = false;

  return *this;
}

base::ErrorOr<UnixSocket> UnixSocket::create(const std::string &path)
{
  int fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd == 1)
    return base::Error::from_errno("failed socket creation");

  return UnixSocket(fd, path);
}

base::ErrorOr<void> UnixSocket::listen()
{
  struct sockaddr_un addr;
  addr.sun_family = AF_UNIX;
  std::strncpy(addr.sun_path, m_path.c_str(), sizeof(addr.sun_path) - 1);

  if (std::filesystem::exists(m_path))
    std::filesystem::remove(m_path);

  if (bind(m_socket_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    return base::Error::from_errno("failed to bind socket");

  if (::listen(m_socket_fd, 1) == -1)
    return base::Error::from_errno("failed to listen to socket");

  return {};
}

base::ErrorOr<void> UnixSocket::connect()
{
  struct sockaddr_un addr;
  addr.sun_family = AF_UNIX;
  std::strncpy(addr.sun_path, m_path.c_str(), sizeof(addr.sun_path) - 1);

  if (::connect(m_socket_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    return base::Error::from_errno("failed to connect to socket");

  return {};
}

base::ErrorOr<UnixSocket> UnixSocket::accept()
{
  struct sockaddr_un client_addr;
  socklen_t client_len = sizeof(client_addr);

  int client_fd =
      ::accept(m_socket_fd, (struct sockaddr *)&client_addr, &client_len);
  if (client_fd == -1)
    return base::Error::from_errno("failed to accept connection");

  return UnixSocket(client_fd, m_path);
}

bool UnixSocket::try_send(ipc::MessageHeader::Raw &header,
                          const synth::SynthMessage &message) noexcept
{
  if (!recv_exactly(&header, sizeof(header))) {
    LOG_AUDIO(Warn, "could not read header");
    return false;
  }

  auto validate = ipc::MessageHeader::validate(header);
  if (validate != ipc::MessageHeader::ValidationResult::Valid) {
    LOG_AUDIO(Error, "error validating bytes");
    return false;
  }

  if (!recv_exactly(&message, header.size)) {
  }

  return true;
}

bool UnixSocket::try_recv(ipc::MessageHeader &header,
                          synth::SynthMessage &message) noexcept
{
  ipc::MessageHeader::Raw raw_header;

  if (!recv_exactly(&raw_header, sizeof(header)))
    return true;

  ssize_t bytes_received =
      ::recv(m_socket_fd, &message, sizeof(message), MSG_DONTWAIT);

  return bytes_received == sizeof(message);
}
} // namespace ipc