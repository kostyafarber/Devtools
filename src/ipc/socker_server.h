#include "ipc/socket.h"
#include <sys/event.h>

class SocketServer
{
public:
  base::ErrorOr<SocketServer> create();

private:
  SocketServer();
  ~SocketServer();
  int m_kqueue;
  ipc::UnixSocket m_listening_socket;
};