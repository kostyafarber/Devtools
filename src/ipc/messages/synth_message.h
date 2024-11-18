#include "synth_command.h"
#include <cstdint>

namespace ipc {

class SynthMessage
{
  SynthCommand m_message;

  union PayloadData {
    float frequency;
    float volume;
    float duty_cycle;
    std::uint8_t padding[16];
  } data;
};
} // namespace ipc