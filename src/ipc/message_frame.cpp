#include "message_frame.h"
#include "message_header.h"
#include "messages.pb.h"

void ipc::MessageFrame::pack(const synth::SynthMessage &msg)
{
  auto data = msg.SerializeAsString();

  m_header.set_size(data.size());
  m_payload = std::move(data);
}

bool ipc::MessageFrame::unpack(synth::SynthMessage &msg)
{
  auto unpacked = msg.ParseFromString(m_payload);

  return unpacked;
}