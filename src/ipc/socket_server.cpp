#include "ipc/socket_server.h"
#include "ipc/messages/message_header.h"
#include "ru/ring_buffer.h"
#include <atomic>
#include <sys/event.h>

namespace ipc {
SocketServer &SocketServer::operator=(SocketServer &&other) noexcept
{
  if (this == &other)
    return *this;

  m_kqueue_fd = other.m_kqueue_fd;
  m_listening_socket = std::move(m_listening_socket);
  m_running.store(other.m_running, std::memory_order_release);

  other.m_kqueue_fd = -1;
  other.m_running = false;

  return *this;
}

base::ErrorOr<SocketServer>
SocketServer::create(const std::string &socket_path,
                     ru::RingBuffer<SynthMessage> &command_queue)
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

  SocketServer server(queue_fd, std::move(socket), command_queue);

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
  EV_SET(&event, fd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_EOF, 0, 0, nullptr);

  if (kevent(m_kqueue_fd, &event, 1, nullptr, 0, nullptr) == -1)
    return base::Error::from_errno("failed to register the event");

  return {};
}

void SocketServer::handle_events() noexcept
{
  const int MAX_EVENTS = 32;
  struct kevent events[MAX_EVENTS];

  struct timespec timeout{
      .tv_sec = 0,
      .tv_nsec = 100000000 // 100ms timeout
  };

  while (auto running = m_running.load(std::memory_order_acquire)) {
    int n = kevent(m_kqueue_fd, nullptr, 0, events, MAX_EVENTS, &timeout);

    if (n == -1) {
      LOG_AUDIO(Error, "kevent error in handle_events: {}", strerror(errno));
      continue;
    }

    for (int i = 0; i < n; i++) {
      LOG_AUDIO(Info, "processing {} events", n);
      if (events[i].ident == m_listening_socket.fd()) {
        if (events[i].flags & EV_EOF) {
          LOG_AUDIO(Error, "EOF on listening socket");
          stop();
          break;
        }

        if (events[i].flags & EV_ERROR) {
          LOG_AUDIO(Error, "Error on listening socket");
          stop();
          break;
        }

        LOG_AUDIO(Info, "accepting client");
        auto maybe_client = m_listening_socket.accept();
        if (maybe_client.is_error()) {
          LOG_AUDIO(Error, "Error accepting client");
          continue;
        }

        auto client = std::move(maybe_client.value());

        LOG_AUDIO(Info, "registering socket event with fd: {}", client.fd());
        if (auto err = register_socket_event(client.fd(), true);
            err.is_error()) {
          LOG_AUDIO(Error, "Error registering client event");
          continue;
        }

        m_clients.emplace(client.fd(), std::move(client));
      } else {
        auto client = m_clients.find(events[i].ident);
        if (client == m_clients.end()) {
          LOG_AUDIO(Warn, "Client not found");
          continue;
        }

        ipc::MessageHeader header;
        synth::SynthMessage msg;
        if (!client->second.try_recv(header, msg)) {
          LOG_AUDIO(Error, "Error receiving message");
          continue;
        }

        // add conversion step

        LOG_AUDIO(Info, "received message");
        if (!m_command_queue.write(&msg))
          LOG_AUDIO(Warn, "error writing message");

        LOG_AUDIO(Info, "Wrote message");
      }
    }
  }
}

void SocketServer::start() noexcept
{
  LOG_AUDIO(Info, "Starting socket server");
  if (m_running)
    return;

  m_running.store(true, std::memory_order_release);
  m_event_thread = std::thread([this]() { handle_events(); });
}

void SocketServer::stop() noexcept
{
  m_running.store(false, std::memory_order_release);

  LOG_AUDIO(Info, "attempting to join socket server thread");
  if (m_event_thread.joinable())
    m_event_thread.join();
}

} // namespace ipc