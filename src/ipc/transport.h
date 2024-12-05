#pragma once

#include "ipc/unix_socket.h"
#include "messages.pb.h"
#include <cstddef>
namespace ipc {
class Transport
{
public:
  Transport(UnixSocket socket) : m_socket(std ::move(socket)) {};

  size_t fd() const { return m_socket.fd(); }
  bool try_send(const synth::SynthMessage &msg);
  bool try_recv(synth::SynthMessage &msg);

private:
  UnixSocket m_socket;
};
} // namespace ipc