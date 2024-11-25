#include "ipc/socker_server.h"
#include <sys/event.h>

base::ErrorOr<SocketServer> SocketServer::create(const std::string &socket_path)
{
  auto queue_fd = kqueue();
  if (queue_fd == -1)
    return base::Error::from_errno("failed to create kqueue");

  auto maybe_socket = ipc::UnixSocket::create(socket_path);
  if (maybe_socket.is_error()) {
    close(queue_fd);
    return maybe_socket.error();
  }

  auto socket = std::move(maybe_socket.value());

  if (auto err = socket.listen(); err.is_error()) {
    close(queue_fd);
    return err.error();
  }

  SocketServer server(queue_fd, std::move(socket));

  if (auto err = register_socket_event(server.m_listening_socket.fd(), true);
      err.is_error())
    return err.error();

  return server;
}

base::ErrorOr<void> SocketServer::register_socket_event(int fd,
                                                        bool is_read) noexcept
{
  struct kevent event;
  EV_SET(&event, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);

  if (kevent(m_kqueue_fd, &event, 1, nullptr, 0, nullptr) == -1)
    return base::Error::from_errno("failed to register the event");

  return {};
}

void SocketServer::handle_events() noexcept
{
  const int MAX_EVENTS = 32;
  struct kevent events[MAX_EVENTS];

  m_running = true;

  while (m_running) {
    int n = kevent(m_kqueue_fd, nullptr, 0, events, MAX_EVENTS, nullptr);

    if (n == -1) {
      // LOG_AUDIO(Error, "kevent error in handle_events");
      continue;
    }
  }
}
