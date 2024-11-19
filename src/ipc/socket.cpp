#include "socket.h"
#include <sys/socket.h>
#include <sys/un.h>

namespace ipc {

base::ErrorOr<UnixSocket> UnixSocket::create(const std::string &path)
{
  int fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd == 1)
    return base::Error::from_errno("failed socket creation");

  return UnixSocket(fd);
}
} // namespace ipc