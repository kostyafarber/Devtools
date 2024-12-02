#include "ipc/unix_socket.h"
#include "messages.pb.h"
namespace ipc {
class Transport
{
public:
  Transport(UnixSocket socket) : m_socket(std ::move(socket)) {};

  bool try_send(const synth::SynthMessage &msg);
  bool try_recv(synth::SynthMessage &msg);

private:
  UnixSocket m_socket;
};
} // namespace ipc