#include "base/error.h"

namespace ipc {

class UnixSocket
{
public:
  static base::ErrorOr<UnixSocket> create(const std::string &path);

  ~UnixSocket();

  UnixSocket(const UnixSocket &) = delete;
  const UnixSocket &operator=(const UnixSocket &) = delete;

  UnixSocket(UnixSocket &&);
  UnixSocket &operator=(const UnixSocket &&);

  bool try_send() noexcept;
  bool try_recv() noexcept;

  base::ErrorOr<void> connect();
  base::ErrorOr<void> listen();
  base::ErrorOr<void> accept();

private:
  UnixSocket(int fd) : m_socket_fd(fd) {};
  int m_socket_fd{-1};

  bool m_running{false};
};
} // namespace ipc