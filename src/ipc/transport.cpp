#include "transport.h"
#include "ipc/message_header.h"
#include "logging.h"
#include "message_frame.h"
#include "messages.pb.h"

bool ipc::Transport::try_send(const synth::SynthMessage &msg)
{
  ipc::MessageFrame frame;
  if (!frame.pack(msg)) {
    LOG_AUDIO(Error, "error packing msg into frame");
    return false;
  }

  if (!m_socket.send(&frame.header(), sizeof(frame.header()))) {
    LOG_AUDIO(Error, "failed to send header");
    return false;
  }

  if (!m_socket.send(&frame.payload(), frame.header().size)) {
    LOG_AUDIO(Error, "failed to send message");
    return false;
  }

  return true;
}

bool ipc::Transport::try_recv(synth::SynthMessage &msg)
{
  ipc::MessageFrame frame;
  if (!m_socket.recv(&frame.header(), sizeof(frame.header()))) {
    LOG_AUDIO(Error, "error recv frame");
    return false;
  }

  if (ipc::MessageHeader::validate(frame.header()) !=
      ipc::MessageHeader::ValidationResult::Valid) {
    LOG_AUDIO(Error, "error validating header");
    return false;
  }

  if (!m_socket.recv(&frame.payload(), frame.size())) {
    LOG_AUDIO(Error, "failed to read message");
    return false;
  }

  return frame.unpack(msg);
}