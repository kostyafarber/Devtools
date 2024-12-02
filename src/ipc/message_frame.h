
#include "ipc/message_header.h"
#include "messages.pb.h"

namespace ipc {
class MessageFrame
{
public:
  MessageFrame()
      : m_header(ipc::MessageHeader::Raw{.magic = ipc::MessageHeader::MAGIC,
                                         .size = 0}) {};

  bool pack(const synth::SynthMessage &msg);
  bool unpack(synth::SynthMessage &msg);

  const MessageHeader::Raw &header() { return m_header; }
  const std::string &payload() { return m_payload; }
  const size_t size() const { return m_header.size; }

private:
  ipc::MessageHeader::Raw m_header;
  std::string m_payload;
};

} // namespace ipc