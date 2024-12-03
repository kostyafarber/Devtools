
#include "error.h"
#include "ipc/message_header.h"
#include "messages.pb.h"
#include <cstddef>

namespace ipc {
class MessageFrame
{
public:
  void pack(const synth::SynthMessage &msg);
  bool unpack(synth::SynthMessage &msg);

  const base::ErrorOr<void> validate_header() { return m_header.validate(); }
  const std::span<std::byte> header() { return m_header.data(); }
  const std::string &payload() const { return m_payload; }
  std::string &payload() { return m_payload; }

private:
  ipc::MessageHeader m_header;
  std::string m_payload;
};

} // namespace ipc