
#include "error.h"
#include "ipc/message_header.h"
#include "messages.pb.h"
#include <cstddef>

namespace ipc {
class MessageFrame
{
public:
  using ByteArray = std::vector<std::byte>;

  bool pack(const synth::SynthMessage &msg);
  bool unpack(synth::SynthMessage &msg);

  const base::ErrorOr<void> validate_header() { return m_header.validate(); }
  std::span<std::byte> const header() { return m_header.data(); }
  size_t payload_size() const { return m_header.payload_size(); }

  const ByteArray &payload() const { return m_payload; }
  ByteArray &payload() { return m_payload; }

private:
  ipc::MessageHeader m_header;
  ByteArray m_payload;
};

} // namespace ipc