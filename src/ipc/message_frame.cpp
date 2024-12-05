#include "message_frame.h"
#include "message_header.h"
#include "messages.pb.h"

bool ipc::MessageFrame::pack(const synth::SynthMessage &msg)
{

  auto size = msg.ByteSizeLong();
  m_header.set_size(size);
  m_payload.resize(size);

  return msg.SerializeToArray(m_payload.data(), size);
}

bool ipc::MessageFrame::unpack(synth::SynthMessage &msg)
{
  return msg.ParseFromArray(m_payload.data(), m_header.payload_size());
}