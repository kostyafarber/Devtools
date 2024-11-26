#include "ipc/socket_server.h"
#include <sys/event.h>

namespace ipc {
SocketServer &SocketServer::operator=(SocketServer &&other) noexcept
{
  if (this == &other)
    return *this;

  m_kqueue_fd = other.m_kqueue_fd;
  m_listening_socket = std::move(m_listening_socket);
  m_running = other.m_running;

  other.m_kqueue_fd = -1;
  other.m_running = false;

  return *this;
}

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

  if (auto err =
          server.register_socket_event(server.m_listening_socket.fd(), true);
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
      LOG_AUDIO(Error, "kevent error in handle_events");
      continue;
    }

    for (int i = 0; i < n; i++) {
      if (events[i].ident == m_listening_socket.fd()) {
        if (events[i].flags & EV_EOF) {
          LOG_AUDIO(Error, "EOF on listening socket");
          m_running = false;
          break;
        }

        if (events[i].flags & EV_ERROR) {
          LOG_AUDIO(Error, "Error on listening socket");
          m_running = false;
          break;
        }

        auto maybe_client = m_listening_socket.accept();
        if (maybe_client.is_error()) {
          LOG_AUDIO(Error, "Error accepting client");
          continue;
        }

        auto client = std::move(maybe_client.value());
        if (auto err = register_socket_event(client.fd(), true);
            err.is_error()) {
          LOG_AUDIO(Error, "Error registering client event");
          continue;
        }

        m_clients.emplace(client.fd(), std::move(client));
      } else {
        auto client = m_clients.find(events[i].ident);
        if (client == m_clients.end()) {
          LOG_AUDIO(Error, "Client not found");
          continue;
        }

        ipc::SynthMessage msg;
        if (!client->second.try_recv(msg)) {
          LOG_AUDIO(Error, "Error receiving message");
          continue;
        }

        m_command_queue.write(&msg);
      }
    }
  }
}

} // namespace ipc