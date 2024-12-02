#include "message_frame.h"
#include "logging.h"
#include "message_header.h"
#include "messages.pb.h"

bool ipc::MessageFrame::pack(const synth::SynthMessage &msg)
{
  auto data = msg.SerializeAsString();
  if (data.size() > ipc::MessageHeader::MAX_MESSAGE_SIZE) {
    LOG_AUDIO(Error, "data is larger than the maximum size");
    return false;
  }

  m_header.size = data.size();
  m_payload = std::move(data);

  return true;
}

bool ipc::MessageFrame::unpack(synth::SynthMessage &msg)
{
  return msg.ParseFromString(m_payload);
}