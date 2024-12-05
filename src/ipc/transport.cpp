#include "transport.h"
#include "logging.h"
#include "message_frame.h"
#include "messages.pb.h"

bool ipc::Transport::try_send(const synth::SynthMessage &msg)
{
  ipc::MessageFrame frame;
  if (!frame.pack(msg)) {
    LOG_AUDIO(Error, "failed to pack message");
    return false;
  }

  LOG_AUDIO(Info, "header size send: {}", frame.header().size());
  if (!m_socket.send(frame.header().data(), frame.header().size())) {
    LOG_AUDIO(Error, "failed to send header");
    return false;
  }

  if (!m_socket.send(frame.payload().data(), frame.payload().size())) {
    LOG_AUDIO(Error, "failed to send message");
    return false;
  }

  return true;
}

bool ipc::Transport::try_recv(synth::SynthMessage &msg)
{
  ipc::MessageFrame frame;
  if (!m_socket.recv(frame.header().data(), frame.header().size())) {
    LOG_AUDIO(Error, "error recv frame");
    return false;
  }

  if (auto err = frame.validate_header(); err.is_error()) {
    LOG_AUDIO(Error, "error validating header: {}", err.error().message());
    return false;
  }

  frame.payload().resize(frame.payload_size());

  if (!m_socket.recv(frame.payload().data(), frame.payload_size())) {
    LOG_AUDIO(Error, "failed to read message");
    return false;
  }

  if (!frame.unpack(msg)) {
    LOG_AUDIO(Error, "failed to unpack message");
    return false;
  }

  return true;
}