#include "ipc/socket.h"
#include <sys/event.h>
#include <thread>

class SocketServer
{
public:
  base::ErrorOr<SocketServer> create(const std::string &socket_path);

  SocketServer(const SocketServer &) = delete;
  SocketServer &operator=(const SocketServer &) = delete;

  SocketServer(SocketServer &&) noexcept;
  SocketServer &operator=(SocketServer &&) noexcept;

  void handle_events() noexcept;
  void stop() noexcept { m_running = false; }

private:
  SocketServer(int kqueue_fd, ipc::UnixSocket &&socket)
      : m_kqueue_fd(kqueue_fd), m_listening_socket(std::move(socket)) {};
  ~SocketServer();
  ipc::UnixSocket m_listening_socket;

  int m_kqueue_fd;
  base::ErrorOr<void> register_socket_event(int fd, bool is_read) noexcept;

  bool m_running{false};
};