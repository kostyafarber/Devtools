#include "ipc/command_server.h"
#include "base/ring_buffer.h"
#include "ipc/synth_message.h"
#include "ipc/transport_listener.h"
#include "logging.h"
#include "messages.pb.h"
#include <atomic>
#include <sys/event.h>

namespace ipc {
CommandServer &CommandServer::operator=(CommandServer &&other) noexcept
{
  if (this == &other)
    return *this;

  m_kqueue_fd = other.m_kqueue_fd;
  m_listener = std::move(m_listener);
  m_running.store(other.m_running, std::memory_order_release);

  other.m_kqueue_fd = -1;
  other.m_running = false;

  return *this;
}

base::ErrorOr<CommandServer>
CommandServer::create(const std::string &socket_path,
                      base::RingBuffer<SynthMessage> &command_queue)
{

  auto queue_fd = kqueue();
  if (queue_fd == -1)
    return base::Error::from_errno("failed to create kqueue");

  auto maybe_tl = ipc::TransportListener::create(socket_path);
  if (maybe_tl.is_error()) {
    LOG_AUDIO(Error, "failed to create transport listener: {}",
              maybe_tl.error().message());
    return maybe_tl.error();
  }

  CommandServer server(queue_fd, std::move(maybe_tl.value()), command_queue);

  if (auto err = server.register_socket_event(server.m_listener.fd(), true);
      err.is_error()) {
    LOG_AUDIO(Error, "failed to register socket event: {}",
              err.error().message());
    return err.error();
  }

  return server;
}

base::ErrorOr<void> CommandServer::register_socket_event(int fd,
                                                         bool is_read) noexcept
{
  struct kevent event;
  EV_SET(&event, fd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_EOF, 0, 0, nullptr);

  if (kevent(m_kqueue_fd, &event, 1, nullptr, 0, nullptr) == -1)
    return base::Error::from_errno("failed to register the event");

  return {};
}

void CommandServer::handle_events() noexcept
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
      if (events[i].ident == m_listener.fd()) {
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
        auto maybe_client = m_listener.accept();
        if (maybe_client.is_error()) {
          LOG_AUDIO(Error, "error accepting client");
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
          LOG_AUDIO(Warn, "client not found");
          continue;
        }
        LOG_AUDIO(Info, "found client");

        if (events[i].flags & EV_EOF) {
          LOG_AUDIO(Error, "EOF client socket");
          m_clients.erase(events[i].ident);
          continue;
        }

        synth::SynthMessage msg;
        if (!client->second.try_recv(msg)) {
          LOG_AUDIO(Error, "Error receiving message");
          continue;
        }

        ipc::SynthMessage fixed_msg; // fixed struct
        fixed_msg.m_message = static_cast<ipc::SynthCommand>(msg.command());

        if (msg.has_frequency()) {
          fixed_msg.data.frequency = msg.frequency();
        } else if (msg.has_volume()) {
          fixed_msg.data.volume = msg.volume();
        } else if (msg.has_duty_cycle()) {
          fixed_msg.data.duty_cycle = msg.duty_cycle();
        }

        LOG_AUDIO(Info, "received message");
        if (!m_command_queue.write(&fixed_msg))
          LOG_AUDIO(Warn, "error writing message");

        LOG_AUDIO(Info, "Wrote message");
      }
    }
  }
}

void CommandServer::start() noexcept
{
  LOG_AUDIO(Info, "Starting socket server");
  if (m_running)
    return;

  m_running.store(true, std::memory_order_release);
  m_event_thread = std::thread([this]() { handle_events(); });
}

void CommandServer::stop() noexcept
{
  m_running.store(false, std::memory_order_release);

  LOG_AUDIO(Info, "attempting to join socket server thread");
  if (m_event_thread.joinable())
    m_event_thread.join();
}

} // namespace ipc