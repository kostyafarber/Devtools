#include "ipc/transport_listener.h"
#include "error.h"
#include "ipc/unix_socket.h"
#include "logging.h"

ipc::TransportListener &
ipc::TransportListener::operator=(ipc::TransportListener &&other)
{
  if (this == &other) {
    return *this;
  }

  m_socket = std::move(other.m_socket);

  return *this;
};

base::ErrorOr<ipc::TransportListener>
ipc::TransportListener::create(const std::string &path)
{
  auto maybe_socket = ipc::UnixSocket::create(path);

  if (maybe_socket.is_error()) {
    LOG_AUDIO(Error, "failed to create unix socket: {}",
              maybe_socket.error().message());
    return maybe_socket.error();
  }

  auto socket = std::move(maybe_socket.value());
  LOG_AUDIO(Info, "socket path: {}, socket fd: {}", socket.socket_path(),
            socket.fd());

  ipc::TransportListener tl(std::move(socket));
  LOG_AUDIO(Info, "created transport listener");

  if (auto err = tl.m_socket.listen(); err.is_error()) {
    LOG_AUDIO(Error, "failed to listen on socket: {}", err.error().message());
    return err.error();
  }

  return tl;
}

base::ErrorOr<ipc::Transport> ipc::TransportListener::accept()
{
  auto maybe_client = m_socket.accept();
  if (maybe_client.is_error())
    return maybe_client.error();

  ipc::Transport t(std::move(maybe_client.value()));

  return t;
}