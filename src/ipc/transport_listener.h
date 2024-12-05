#pragma once

#include "error.h"
#include "ipc/transport.h"
#include "ipc/unix_socket.h"
#include <cstddef>

namespace ipc {
class TransportListener
{
public:
  static base::ErrorOr<TransportListener> create(const std::string &path);

  TransportListener(const TransportListener &) = delete;
  TransportListener &operator=(TransportListener &) = delete;

  TransportListener(TransportListener &&other)
      : m_socket(std::move(other.m_socket)) {};

  TransportListener &operator=(TransportListener &&other);

  size_t fd() { return m_socket.fd(); }
  std::string const path() { return m_socket.socket_path(); }

  base::ErrorOr<ipc::Transport> accept();

private:
  TransportListener(ipc::UnixSocket &&socket) : m_socket(std::move(socket)) {};
  ipc::UnixSocket m_socket;
};
} // namespace ipc